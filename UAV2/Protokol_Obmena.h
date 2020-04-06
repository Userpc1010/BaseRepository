

const char sp_startMarker [5] = "<@#>";    // Переменная, содержащая маркер начала пакета
const char sp_stopMarker  [5] = "<&^>";    // Переменная, содержащая маркер конца пакета
const char sp_telemetry  [5] = "TELE";     // Отправка телеметрии
const char sp_PID [4] = "PID";             // Отправка 
const char sp_Lim [4] = "LIM";             // Отправка
const char sp_Trimm [5] = "TRIM";             // Отправка  
const uint8_t sp_MarkerLength = 4;         // Переменная длинны маркера -1
unsigned char sp_dataString [255];            // Здесь будут храниться принимаемые данные 
uint8_t sp_startMarkerStatus;                 // Флаг состояния маркера начала пакета
uint8_t sp_stopMarkerStatus;                  // Флаг состояния маркера конца пакета
uint8_t sp_dataLength;                        // Флаг состояния принимаемых данных
uint8_t sp_count;                             // Длинна входящего сообщения
uint8_t data_length;                          // Длинна структуры данных
bool sp_packetAvailable;                      // Флаг завершения приема пакета
                      
bool Key_interapt = false;   //Запуск метода кнопок
bool Key_W = false;          //тонгаж отрицательный, вниз
bool Key_S = false;          //тонгаж положительный, вверх
bool Key_A = false;          //крен в лево
bool Key_D = false;          //крен в право 
bool Key_Q = false;          //курс вправо
bool Key_E = false;          //курс влево

uint8_t Trottle = 0;       // Тяга двигателя
         
struct Move
{
 int16_t azimut = 0; //Приходит в виде 180/-180 град
 int16_t alt = 0;
 uint8_t climb = 0;
 uint8_t type_point = 0;
 bool circle = false; 

}Move;

struct telemetry
{
 float altitude = 0.0f;
 int8_t pitch = 0;
 int16_t roll = 0;
 int16_t yaw = 0;
 uint8_t cal_system = 0;
 uint8_t cal_gyro = 0;
 uint8_t cal_accel = 0;
 uint8_t cal_mag = 0;
  
}telemetry; 


struct PID
{
  
 float P_Alt = 0.0f; //0.5f;
 float D_Alt = 0.0f; //1.0f;

 float P_Bank = 0.0f; //0.5f;
 float D_Bank = 0.0f; //1.0f;

 float P_Pitch = 0.0f; //3.0f; 
 float P_Roll =  0.0f; //2.0f; 
 float P_Yaw = 0.0f; //2.0f; 

 float D_Pitch = 0.0f; //2.0f; 
 float D_Roll = 0.0f; //2.0f; 
 float D_Yaw = 0.0f; //2.0f; 
  
}PID; 

struct Trimm
{
  
 float Left_V_Tail = 0.0f; //0.5f;
 float Right_V_Tail = 0.0f; //1.0f;

 float Left_Eleron = 0.0f; //0.5f;
 float Right_Eleron = 0.0f; //1.0f;

 float Magnetick_decclination = 0.0f;
 float Offset_pitch = 0.0f;
 float Offset_roll = 0.0f;
  
}Trimm;

struct Limit
{
  
 float Pitch_Max = 0.0f;
 float Pitch_Min = 0.0f;

 float Roll_Max = 0.0f;
 float Roll_Min = 0.0f;

 float Yaw_Max = 0.0f;
 float Yaw_Min = 0.0f;

 float Bank_Max = 0.0f;
 float Bank_Min = 0.0f;

 float Error_Pitch_Max = 0.0f;
 float Error_Pitch_Min = 0.0f;

 float Error_Roll_Max = 0.0f;
 float Error_Roll_Min = 0.0f;

 float Error_Yaw_Max = 0.0f;
 float Error_Yaw_Min = 0.0f;
 
}Limit;  

void sp_Reset()
{
  sp_count = 0;                   // Сброс длинны пакета
  sp_startMarkerStatus = 0;      // Сброс флага маркера начала пакета
  sp_stopMarkerStatus = 0;      // Сброс флага маркера конца пакета
  sp_dataLength = 0;           // Сброс флага принимаемых данных
}

void sp_ResetAll()
{
  for (uint16_t i = 0; i < 255; i++)sp_dataString[i] = 0;    // Обнуляем буфер приема данных (баг в цикле for нельзя использовать uint8_t и int8_t) 
  sp_Reset();                                                       // Частичный сброс протокола
  sp_packetAvailable = false;                                      // Сброс флага завершения приема пакета
  data_length = 0;
}


void sp_SetUp()
{
  sp_ResetAll();               // Полный сброс протокола
}



void sp_Read()
{
  while(Serial3.available() && !sp_packetAvailable)            
  {
    unsigned char bufferChar = Serial3.read();                       
    
    if( sp_startMarkerStatus < sp_MarkerLength )        
    {  
     if(sp_startMarker[sp_startMarkerStatus] == bufferChar)   
     {
       sp_startMarkerStatus++;                               
     }
     else
     {
       sp_ResetAll();                                         
     }
    }  
    else
    {
     // Стартовый маркер прочитан полностью
       if(sp_dataLength == 0)                                 
       {    
          sp_dataLength = bufferChar;
       }
       else                                                    
      {
        if(sp_dataLength > sp_count)            
        {
          sp_dataString[sp_count] = bufferChar;                          
          sp_count++;       
        }
        else                                                       
        {
          if( sp_stopMarkerStatus < sp_MarkerLength )        
          {
            if(sp_stopMarker[sp_stopMarkerStatus] == bufferChar)  
            {
              sp_stopMarkerStatus++;

              if( sp_stopMarkerStatus == sp_MarkerLength )
              {
                data_length = sp_dataLength;    
                sp_Reset();                                       
                sp_packetAvailable = true;  
              }
            }
            else
            {
              sp_ResetAll();                                      
            }
          }
        }
      } 
    }    
  }
}

void sp_Send(const char *data)
{
  Serial3.print(sp_startMarker);          // Отправляем маркер начала пакета
  Serial3.write(strlen(data));            // Отправляем длину передаваемых данных
  Serial3.print(data);                    // Отправляем сами данные
  Serial3.print(sp_stopMarker);           // Отправляем маркер конца пакета
}

// передаём структуру
void Send_Telemetry ()
{
  Serial3.print(sp_startMarker);           // Отправляем маркер начала пакета
  Serial3.write(sizeof(telemetry) + 4);    // Отправляем длину передаваемых данных
  Serial3.print(sp_telemetry);             // Отправляем заголовок
  
  byte* b = (byte*) &telemetry; for(size_t i = 0; i < sizeof(telemetry); i++) Serial3.write(*b++); //Отправляем сами данные
  
  Serial3.print(sp_stopMarker);                                      // Отправляем маркер конца пакета
}

// передаём структуру
void Send_PID ()
{
  Serial3.print(sp_startMarker);           // Отправляем маркер начала пакета
  Serial3.write(sizeof(PID) + 3);    // Отправляем длину передаваемых данных
  Serial3.print(sp_PID);             // Отправляем заголовок
  
  byte* b = (byte*) &PID; for(size_t i = 0; i < sizeof(PID); i++) Serial3.write(*b++); //Отправляем сами данные
  
  Serial3.print(sp_stopMarker);                                      // Отправляем маркер конца пакета
}

void Send_Trimm ()
{
  Serial3.print(sp_startMarker);           // Отправляем маркер начала пакета
  Serial3.write(sizeof(Trimm) + 4);    // Отправляем длину передаваемых данных
  Serial3.print(sp_Trimm);             // Отправляем заголовок
  
  byte* b = (byte*) &Trimm; for(size_t i = 0; i < sizeof(Trimm); i++) Serial3.write(*b++); //Отправляем сами данные
  
  Serial3.print(sp_stopMarker);                                      // Отправляем маркер конца пакета
}


void Send_Limit ()
{
  Serial3.print(sp_startMarker);           // Отправляем маркер начала пакета
  Serial3.write(sizeof(Limit) + 3);    // Отправляем длину передаваемых данных
  Serial3.print(sp_Lim);             // Отправляем заголовок
  
  byte* b = (byte*) &Limit; for(size_t i = 0; i < sizeof(Limit); i++) Serial3.write(*b++); //Отправляем сами данные
  
  Serial3.print(sp_stopMarker);                                      // Отправляем маркер конца пакета
}
