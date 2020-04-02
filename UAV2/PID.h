float Pitch_pid_out = 0; //Выходы ПИД регуляторов
float Roll_pid_out = 0;
float Yaw_pid_out = 0;

uint32_t Last_Deviation = 0; //Время после последих вычислений
uint32_t Last_Alt = 0; 
uint32_t Last_Bank = 0;

uint8_t time_charg_Alt = 0;
uint8_t time_charg_Bank = 0;
uint8_t time_charg_Deviation = 0;
  
float LastError_alt = 0; //Ошибка регулирования из предыдущих расчётов 
float LastError_bank = 0;

int16_t set_point_pitch = 0; //Заданное значение ПИД 
int16_t set_point_roll =  0; 
int16_t set_point_yaw =  0; 
int16_t set_point_alt =  0; 
int16_t set_point_bank =  0;

uint8_t climb_angle = 0;  //Угол атаки 
int8_t vifish_angle = 0; //Скрость (угол) поворота в ручном режиме

bool Manual = false;  //Ручной режим упреавления
bool FlyMet = false; //Микширование сервомашинок 

//FIR фильтр производной расчитывается по пяти точкам взят из INAV 1.1
const uint8_t filterLength = 5;

int16_t shiftBuf_pitch [filterLength] = {0, 0, 0, 0, 0};
int16_t shiftBuf_roll [filterLength] = {0, 0, 0, 0, 0};
int16_t shiftBuf_yaw [filterLength] = {0, 0, 0, 0, 0};
int16_t coeffBuf [filterLength] = {5, 2, -8, -2, 3};


void filterUpdateFIR( int16_t * shiftBuf, int16_t newSample)
{
    // Shift history buffer and push new sample
    for (int16_t i = filterLength - 1; i > 0; i--)
    
    shiftBuf[i] = shiftBuf[i - 1];

    shiftBuf[0] = newSample;
}

float filterApplyFIR( int16_t * shiftBuf, float commonMultiplier)
{
    float accum = 0;

    for (int16_t i = 0; i < filterLength; i++)
    
    accum += shiftBuf[i] * coeffBuf[i];
        
    return accum * commonMultiplier;
}


void Bank_pid ( int16_t yaw )
{

 time_charg_Bank = millis() - Last_Bank;  if( time_charg_Bank >= 92 && !Manual ){ 
  
 int16_t Error = set_point_bank - yaw;

 if (Error < -180) Error += 360;// Делаем результат вычетания вида -180 +180
 if (Error >  180) Error -= 360;

 float Delta =  (Error - LastError_bank) / time_charg_Bank;

 LastError_bank = Error;

 set_point_roll = ( PID.P_Bank * Error + PID.D_Bank * Delta);

 if ( set_point_roll > 30 ) set_point_roll = 30;

 if ( set_point_roll < -30 ) set_point_roll = -30;

 Last_Bank = millis();

 } 
}



void Alt_pid ( float alt )
{

 time_charg_Alt = millis() - Last_Alt; if ( time_charg_Alt >= 115 &&  !Manual ){ 
  
 float Error = set_point_alt - alt;

 float Delta =  ( Error - LastError_alt) / time_charg_Alt;

 LastError_alt = Error;

 set_point_pitch =  PID.P_Alt * Error + PID.D_Alt * Delta;

 if ( set_point_pitch > climb_angle) set_point_pitch = climb_angle;

 if ( set_point_pitch < -climb_angle) set_point_pitch = -climb_angle;

 Last_Alt = millis();
 
 }
}


void Deviation ( int16_t pitch, int16_t roll,  int16_t yaw, bool circle )
{
  
 time_charg_Deviation = millis() - Last_Deviation; if ( time_charg_Deviation >= 12 ){ 

 int16_t Error_Pitch = set_point_pitch - pitch;
 
 int16_t Error_Roll = set_point_roll - roll;

 if( Manual){ set_point_yaw = yaw + vifish_angle; //Ручное управление разворот град/дельта т
 
 if (set_point_yaw < -180) set_point_yaw += 360;// Делаем результат вычетания вида -180 +180
 if (set_point_yaw >  180) set_point_yaw -= 360;
 }
 
 int16_t Error_Yaw = set_point_yaw - yaw;

 if (circle && !Manual) Error_Yaw = Error_Yaw + set_point_pitch;// Вращение вокруг точки

 if (Error_Yaw < -180) Error_Yaw += 360;// Делаем результат вычетания вида -180 +180
 if (Error_Yaw >  180) Error_Yaw -= 360;

 if (Error_Yaw < -60) Error_Yaw = -60;
 if (Error_Yaw >  60) Error_Yaw = 60;
 
 
 filterUpdateFIR( shiftBuf_pitch, pitch); float Delta_Pitch = filterApplyFIR ( shiftBuf_pitch, PID.D_Pitch / (8 * time_charg_Deviation));

 filterUpdateFIR( shiftBuf_roll, roll); float Delta_Roll = filterApplyFIR ( shiftBuf_roll, PID.D_Roll / (8 * time_charg_Deviation));

 filterUpdateFIR( shiftBuf_yaw, yaw); float Delta_Yaw = filterApplyFIR ( shiftBuf_yaw, PID.D_Yaw / (8 * time_charg_Deviation));

 
 Pitch_pid_out = PID.P_Pitch * Error_Pitch + Delta_Pitch;
  
 Roll_pid_out =  PID.P_Roll * Error_Roll + Delta_Roll;
  
 Yaw_pid_out =  PID.P_Yaw * Error_Yaw + Delta_Yaw;


 if ( Yaw_pid_out < -60  )  Yaw_pid_out = -60.0f; // Делаем результат вида 0 - 60
 if ( Yaw_pid_out > 60   )  Yaw_pid_out = 60.0f;

 if ( Roll_pid_out > 60  )  Roll_pid_out = 60.0f; 
 if ( Roll_pid_out < -60 )  Roll_pid_out = -60.0f; 

 if ( Pitch_pid_out > 60 )  Pitch_pid_out = 60.0f; 
 if ( Pitch_pid_out < -60)  Pitch_pid_out = -60.0f; 

 FlyMet = true;

 Last_Deviation = millis();
 }
}
