  
#include <Servo.h>
#include "Protokol_Obmena.h"
#include "PID.h"
#include "Sonar.h"
#include "BMP280.h"
#include "BNO_055.h"
#include <EEPROM_I2C_DUE.h>

#define EE24LC16MAXBYTES 16384/8 //Кол-во банок памяти

// Chips 16Kbit (2048KB) or smaller only have one-word addresses.
// Also try to guess page size from device size (going by Microchip 24LCXX datasheets here). из библиотеки
#define DEVICEADDRESS (0x50) 

EEPROM_I2C EEPROM;

adafruit_bno055_offsets_t Calib_data;

// Проверка времени отправки телеметрии и опрос датчика
uint16_t bno_count = 0, telemetry_count = 0;

float pitch, roll, yaw, last_pitch, last_roll, last_yaw; bool circle = false; int16_t mag_dec = 0;

sensors_event_t event;             //Событие получения точки 

Servo A3_LEFT_VTAIL;              //Инициализация класса серво  
Servo A4_RIGHT_VTAIL;                
Servo A5_Left_roll;                
Servo A6_Right_roll;
Servo A2_Trottle;               //Тяга двигателя               


void setup()
{
 
 Wire.begin();
  
 Serial3.begin(115200);  // Инициализируем последовательный интерфейс с TinkerBoard
 
 Serial.begin(115200);   //ПК для отладки

 I2Cscan();// look for I2C devices on the bus

 sp_SetUp();         //Протокол обмена

 EEPROM.begin(DEVICEADDRESS, EE24LC16MAXBYTES);

  //Пины идут последовательно 2,3 4 и т.д.
  A2_Trottle.attach     (2);              // Тяга двигателя 
  A3_LEFT_VTAIL.attach  (3);              // Управление хвостом
  A4_RIGHT_VTAIL.attach (4);              // Управление хвостом
  A5_Left_roll.attach   (5);              // Управление элеронами
  A6_Right_roll.attach  (6);              // Управление элеронами
  

  pinMode(trigPin, OUTPUT);                           // Trigger pin set to output
  pinMode(echoPin, INPUT);                            // Echo pin set to input

  _initISR( TC2, 0, ID_TC6, TC_CMR_TCCLKS_TIMER_CLOCK2, 525-1, TC6_IRQn);

  attachInterrupt(echoPin, echo_interrupt, CHANGE);  // Attach interrupt to the sensor echo input

  A2_Trottle.writeMicroseconds(2200);

//==========================================================================================================================================================================//
//------------------------------------------------------------------------------BNO_055------------------------------------------------------------------------------------//
//=========================================================================================================================================================================//


 if (!bno.begin())
    {
      /* There was a problem detecting the BNO055 ... check your connections */
      Serial.print("BNO055 no detected ... Check your wiring or I2C ADDR!");
      while (1)
      { delay(500); sp_Send("ER1"); };
    }
  

 long bnoID;
 bool foundCalib = false;
 uint8_t dist = 0;

 EEPROM.get( 0 , bnoID);
 sensor_t sensor;

    /*
    *  Look for the sensor's unique ID at the beginning oF EEPROM.
    *  This isn't foolproof, but it's better than nothing.
    */
    bno.getSensor(&sensor);
    if (bnoID != sensor.sensor_id)
    {
        Serial.println("\nNo Calibration Data for this sensor exists in EEPROM");
        delay(500);
    }
    else
    {
        Serial.println("\nFound Calibration for this sensor in EEPROM.");
        
        EEPROM.get( 8 , Calib_data );

        displaySensorOffsets( Calib_data );

        Serial.println("\n\nRestoring Calibration data to the BNO055...");
        bno.setSensorOffsets( Calib_data );

        Serial.println("\n\nCalibration data loaded into BNO055");
        foundCalib = true;
    }

    delay(1000);

    /* Display some basic information on this sensor */
    displaySensorDetails();

    /* Optional: Display current status */
    displaySensorStatus();

   /* Crystal must be configured AFTER loading calibration data into BNO055. */
    bno.setExtCrystalUse(true);

    sensors_event_t event;
    bno.getEvent(&event);
    
    /* always recal the mag as It goes out of calibration very often */
    if (foundCalib){

        Serial.println("Move sensor slightly to calibrate magnetometers");

        while (!bno.isFullyCalibrated())
        {
          bno.getEvent(&event);
            
          if (dist == 1 ) send_telemetry ();

          if (dist == 2 ) sp_Send("AC1");
            
          delay(BNO055_SAMPLERATE_DELAY_MS);

          if ( dist >= 3 ) dist = 0;

          dist++;
           
        }

        sp_Send("AC0");
        
    }
    else
    { 
        Serial.println("Please Calibrate Sensor: ");

        while (!bno.isFullyCalibrated())
        {
            bno.getEvent(&event);
        
         /* Optional: Display calibration status */
          send_telemetry ();
            
         /* Wait the specified delay before requesting new data */
          if (dist == 1 ) send_telemetry ();

          if (dist == 2 ) sp_Send("MG1");
            
          delay(BNO055_SAMPLERATE_DELAY_MS);

          if ( dist >= 3 ) dist = 0;

          dist++;
          
        }
        
         Serial.println("Calibration Results: ");
         adafruit_bno055_offsets_t newCalib;
         
         if(bno.getSensorOffsets(newCalib))
         {
         
         displaySensorOffsets(newCalib);

         Serial.println("\n\nStoring calibration data to EEPROM...");
         
         bno.getSensor(&sensor);
         bnoID = sensor.sensor_id;

         EEPROM.put( 0 , bnoID);

         EEPROM.put( 8 , newCalib);
         
         sp_Send("MG0");
        
         Serial.println("Data stored to EEPROM.");
         Serial.println("\n--------------------------------\n");
         
         }        
    }

    delay(500);

    displaySensorStatus();

  //==========================================================================================================================================================================//
  //------------------------------------------------------------------------------BMP_280------------------------------------------------------------------------------------//
  //=========================================================================================================================================================================//

 
  A2_Trottle.writeMicroseconds(800);
    
  delay(1000); // add delay to see results before serial spew of data    
  
  // Read the WHO_AM_I register of the BMP280 this is a good test of communication
  byte f = readByte(BMP280_ADDRESS, BMP280_ID);  // Read WHO_AM_I register for BMP280
  Serial.print("BMP280 "); 
  Serial.print("по адресу "); 
  Serial.print(f, HEX); 
  Serial.print(" а должен быть"); 
  Serial.println(0x58, HEX);
  Serial.println(" ");

  delay(1000); 

  writeByte(BMP280_ADDRESS, BMP280_RESET, 0xB6); // reset BMP280 before initilization
  delay(100);

  BMP280Init(); // Initialize BMP280 altimeter 
  Serial.println("BMP280 Калибровочные коэффиценты:");
  Serial.print("dig_T1 ="); 
  Serial.println(dig_T1);
  Serial.print("dig_T2 ="); 
  Serial.println(dig_T2);
  Serial.print("dig_T3 ="); 
  Serial.println(dig_T3);
  Serial.print("dig_P1 ="); 
  Serial.println(dig_P1);
  Serial.print("dig_P2 ="); 
  Serial.println(dig_P2);
  Serial.print("dig_P3 ="); 
  Serial.println(dig_P3);
  Serial.print("dig_P4 ="); 
  Serial.println(dig_P4);
  Serial.print("dig_P5 ="); 
  Serial.println(dig_P5);
  Serial.print("dig_P6 ="); 
  Serial.println(dig_P6);
  Serial.print("dig_P7 ="); 
  Serial.println(dig_P7);
  Serial.print("dig_P8 ="); 
  Serial.println(dig_P8);
  Serial.print("dig_P9 ="); 
  Serial.println(dig_P9);

  delay(2000);

  EEPROM.get ( 32, PID ); 

  sp_Send("Start");
}

void loop()
{
    telemetry_count++;  if ( telemetry_count >= 52000 ) send_telemetry();

    bno_count++;  if ( bno_count >= 400 )
    {
    bno_count = 0;
      
    bno.getEvent(&event);

    rawTemp =  readBMP280Temperature(); bmp280_compensate_T(rawTemp);  
    
    rawPress =  readBMP280Pressure(); BMP280Pressure = (float) bmp280_compensate_P(rawPress)/25600.; // Prwssure in mbar
    
    altitude = 44307.69f * (1.0f - pow((BMP280Pressure/1013.25f), 0.190284f));
 
    if ( type_point == 1 || type_point == 2 || Sonar ) altitude_offset = altitude - (echo_duration / 5800.0f);
    
    altitude -= altitude_offset;

    pitch = event.orientation.y;
    roll = event.orientation.z;
    yaw = event.orientation.x;
    yaw += mag_dec; // Смещение на угол магнитного склонения
  
    if(yaw < -180) yaw += 360.0f; //Востанавливаем результат смещения по типу +180/-180
    if(yaw >  180) yaw -= 360.0f;  
    }

    turns();//Поворот 
       
    if (Serial3.available())serialUSBEvent();
}
//-----------------------------------------------------------------------------------------------------------------
//----------------------- Отправка телеметрии ---------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
  void send_telemetry ()
  {
    telemetry_count = 0;

    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    
    bno.getCalibration(&system, &gyro, &accel, &mag);
    
    telemetry.altitude = altitude;
    telemetry.pitch = pitch;
    telemetry.roll = roll;
    telemetry.yaw = yaw;
    telemetry.cal_system = system;
    telemetry.cal_gyro = gyro;
    telemetry.cal_accel = accel;
    telemetry.cal_mag = mag;
    
    Send_Telemetry ();
  }
//------------------------------------------------------------------------------------------------------------------
//----- Функция протокола обмена данными
//------------------------------------------------------------------------------------------------------------------

void serialUSBEvent() 
{
  sp_Read();                         // Вызов «читалки» принятых данных
  
  if(sp_packetAvailable)             // Если после вызова «читалки» пакет полностью принят
  {
  ParseCommand();                   // Обрабатываем принятую информацию
  sp_ResetAll();                    // Полный сброс протокола.
  
  if( Manual && Key_interapt ) Keys();  // Прерывания кнопок
  }  
}
//--------------------------------------------------------------------------------------------------------------------
//---------------------Функция поворота ------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

void turns ()
{  
  
 Alt_pid( altitude );
 
 Bank_pid ( yaw );  

 Deviation ( pitch, roll, yaw, circle ); 

 if ( FlyMet ) Fly_metod (); //Микширование
}
//------------------------------------------------------------------------------------------------------------------
//----- Функция микширования рулей и тяги о граничения углов атаки ------------------------------------------------
//------------------------------------------------------------------------------------------------------------------


void Fly_metod () 

{
  FlyMet = false;
  
  A3_LEFT_VTAIL.write ((Yaw_pid_out + Pitch_pid_out) * 0.5f + 90 );
  
  A4_RIGHT_VTAIL.write ((Yaw_pid_out - Pitch_pid_out) * 0.5f + 90 );

  A5_Left_roll.write (Roll_pid_out  + 90 ); 
  
  A6_Right_roll.write (Roll_pid_out  + 90 ); 

  A2_Trottle.writeMicroseconds( map(Trottle, 0, 255, 800, 2200));  //Тяга двигателя
}

//========================================================================================================================================================================//
//-------------------------------------------------------------- Функции кнопок -----------------------------------------------------------------------------------------//
//======================================================================================================================================================================//

 void Keys ()
 {
 
 if (!Key_W) set_point_pitch = 0; 
 if (!Key_S) set_point_pitch = 0;
 if (!Key_A) set_point_roll = 0;
 if (!Key_D) set_point_roll = 0; 
 if (!Key_Q) vifish_angle  = 0;
 if (!Key_E) vifish_angle  = 0;
 
 if ( Key_S) set_point_pitch = 30;
 if ( Key_W) set_point_pitch = -30; 
 if ( Key_D) set_point_roll = 45;
 if ( Key_A) set_point_roll = -45; 
 if ( Key_Q) vifish_angle  = 30;
 if ( Key_E) vifish_angle  = -30; 

 Key_interapt = false;
 } 
//=========================================================================================================================================================//
//----------------------------------------Функция приёма-обработки команд-----------------------------------------------------------------------------------
//=========================================================================================================================================================//

void ParseCommand()
{
  
     //Собщения в обработчике желательно делать как можно короче также для более быстрой обработки
     //первый байт должен отличаться от других если он принадлежит другой команде
     
     if( sp_dataString[0] == 'S' && sp_dataString[1] == 't' && sp_dataString[2] == 'a' && sp_dataString[3] == 'r' && sp_dataString[4] == 't' ) sp_Send("ready"); //Самопроверка
     

     if( sp_dataString[0] == 'M' && sp_dataString[1] == 'N' && sp_dataString[2] == '0' ) { Manual = false; Sonar = false; } 
      
     if( sp_dataString[0] == 'M' && sp_dataString[1] == 'N' && sp_dataString[2] == '1' ) { Manual = true; Sonar = true; }
  
     
     if ( sp_dataString[0] == 't' && sp_dataString[1] == 'r') Trottle = sp_dataString[2]; // Тяга двигателя
     
       
     if ( sp_dataString[0] == 'w' && sp_dataString[1] == 'u' ){Key_W = true; Key_interapt = true;} //Клавиши управления

     if ( sp_dataString[0] == 's' && sp_dataString[1] == 'u' ){Key_S = true;  Key_interapt = true;}

     if ( sp_dataString[0] == 'a' && sp_dataString[1] == 'u' ){Key_A = true; Key_interapt = true;}

     if ( sp_dataString[0] == 'd' && sp_dataString[1] == 'u' ){Key_D = true; Key_interapt = true;}
    
     if ( sp_dataString[0] == 'q' && sp_dataString[1] == 'u' ){Key_Q = true;  Key_interapt = true;}

     if ( sp_dataString[0] == 'e' && sp_dataString[1] == 'u' ){Key_E = true;  Key_interapt = true;}
   
     if ( sp_dataString[0] == 'w' && sp_dataString[1] == 'd' ){Key_W = false; Key_interapt = true;}

     if ( sp_dataString[0] == 's' && sp_dataString[1] == 'd' ){Key_S = false; Key_interapt = true;}

     if ( sp_dataString[0] == 'a' && sp_dataString[1] == 'd' ){Key_A = false; Key_interapt = true;}

     if ( sp_dataString[0] == 'd' && sp_dataString[1] == 'd' ){Key_D = false; Key_interapt = true;}

     if ( sp_dataString[0] == 'q' && sp_dataString[1] == 'd' ){Key_Q = false; Key_interapt = true;}

     if ( sp_dataString[0] == 'e' && sp_dataString[1] == 'd' ){Key_E = false; Key_interapt = true;}
     
     if ( sp_dataString[0] == 'c' && sp_dataString[1] == 'l' ) Save_Calibration_Data ();      //Сохранение калибровки в ПЗУ
     
     if ( sp_dataString[0] == 'c' && sp_dataString[1] == 'a' ) Recalibration_Sensor ();      //Перекалибровка датчика на ходу

     if ( sp_dataString[0] == 'd' && sp_dataString[1] == 'c' ) Del_Calibration_Data ();

     if ( sp_dataString[0] == 'd' && sp_dataString[1] == 'p' ) Del_PID_Data ();

     if ( sp_dataString[0] == 'M' && sp_dataString[1] == 'o' && sp_dataString[2] == 'v' && sp_dataString[3] == 'e')
     {    
       uint8_t ee = 4; uint8_t* p = (uint8_t*)(void*)&Move; for( int count = data_length - 4; count ; --count ) *p++ = sp_dataString[ee++];

      circle = Move.circle;  type_point = Move.type_point; climb_angle = Move.climb; set_point_alt = Move.alt; set_point_bank = Move.azimut; set_point_yaw = Move.azimut; mag_dec = Move.mag_dec;
     } 

     if ( sp_dataString[0] == 'P' && sp_dataString[1] == 'I' && sp_dataString[2] == 'D' && sp_dataString[3] == '1')
     {    
       uint8_t ee = 4; uint8_t* p = (uint8_t*)(void*)&PID; for( int count = data_length - 4; count ; --count ) *p++ = sp_dataString[ee++]; EEPROM.put( 32, PID );
     } 

     if ( sp_dataString[0] == 'P' && sp_dataString[1] == 'I' && sp_dataString[2] == 'D' && sp_dataString[3] == '0')
     {    
        EEPROM.get( 32, PID ); Send_PID (); 
     } 
}

// I2C Функция поиска

void I2Cscan()
{
// Поиск устройст I2C 
  byte error, address;
  uint16_t nDevices;

  Serial.println("Поиск...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // В i2c_scanner используется возвращаемое значение
    // the Write.endTransmisstion что-бы увидедь,если
    // устройство не обнаружено по адрресу.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C устройство работает по адресу 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Неизвестная ошибка по адресу 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("Необнаружено I2C устройств\n");
  else
    Serial.println("Готово\n");
    
}


void Save_Calibration_Data ()
{
   if(bno.getSensorOffsets(Calib_data))
   {
   EEPROM.put( 8, Calib_data);
   
   sp_Send("SV1");
   }
}

void Recalibration_Sensor ()
{ 
   EEPROM.get( 8, Calib_data);
 
   bno.setSensorOffsets(Calib_data);  

   bno.setExtCrystalUse(true);

   sp_Send("RC1");
}

void Del_Calibration_Data ()
{
   for (int i = 0; i < 32; i++ ) EEPROM.writeByte( i, 0 );
  
   sp_Send("DL1");
}

void Del_PID_Data ()
{
   for (int i = 32; i < 74; i++ ) EEPROM.writeByte( i, 0 );
  
   sp_Send("DL2");
}
