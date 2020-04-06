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
const int8_t coeffBuf [filterLength] = {5, 2, -8, -2, 3};
int16_t shiftBuf_pitch [filterLength] = {0, 0, 0, 0, 0};
int16_t shiftBuf_roll [filterLength] = {0, 0, 0, 0, 0};
int16_t shiftBuf_yaw [filterLength] = {0, 0, 0, 0, 0};


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


void Bank_pid ( float yaw )
{

 time_charg_Bank = millis() - Last_Bank;  if( time_charg_Bank >= 92 && !Manual ){ 
  
 float Error = set_point_bank - yaw;

 if (Error < -180.0f) Error += 360.0f;// Делаем результат вычетания вида -180 +180
 if (Error >  180.0f) Error -= 360.0f;

 float Delta =  (Error - LastError_bank) / time_charg_Bank;

 LastError_bank = Error;

 set_point_roll = ( PID.P_Bank * Error + PID.D_Bank * Delta);

 if ( set_point_roll > Limit.Bank_Max ) set_point_roll = Limit.Bank_Max;

 if ( set_point_roll < Limit.Bank_Min ) set_point_roll = Limit.Bank_Min;

 Last_Bank = millis(); time_charg_Bank = 0;
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

 Last_Alt = millis(); time_charg_Alt = 0;
 }
}


void Deviation ( float pitch, float roll,  float yaw, bool circle )
{
  
 time_charg_Deviation = millis() - Last_Deviation; if ( time_charg_Deviation >= 12 ){ 

 float Error_Pitch = set_point_pitch - pitch;
 
 float Error_Roll = set_point_roll - roll;

 if( Manual){ set_point_yaw = yaw + vifish_angle; //Ручное управление разворот град/дельта т
 
 if (set_point_yaw < -180.0f) set_point_yaw += 360.0f;// Делаем результат вычетания вида -180 +180
 if (set_point_yaw >  180.0f) set_point_yaw -= 360.0f;
 }
 
 float Error_Yaw = set_point_yaw - yaw;

 if (circle && !Manual) Error_Yaw = Error_Yaw + set_point_pitch;// Вращение вокруг точки

 if (Error_Yaw < -180.0f) Error_Yaw += 360.0f;// Делаем результат вычетания вида -180 +180
 if (Error_Yaw >  180.0f) Error_Yaw -= 360.0f;

 if (Error_Yaw < Limit.Error_Yaw_Min) Error_Yaw = Limit.Error_Yaw_Min;
 if (Error_Roll < Limit.Error_Roll_Min) Error_Roll = Limit.Error_Roll_Min;
 if (Error_Pitch <  Limit.Error_Pitch_Min) Error_Pitch =  Limit.Error_Pitch_Min;
 
 if (Error_Yaw >  Limit.Error_Yaw_Max) Error_Yaw =  Limit.Error_Yaw_Max;
 if (Error_Roll >  Limit.Error_Roll_Max) Error_Roll =  Limit.Error_Roll_Max;
 if (Error_Pitch >  Limit.Error_Pitch_Max) Error_Pitch =  Limit.Error_Pitch_Max;
 
 filterUpdateFIR( shiftBuf_pitch, pitch); float Delta_Pitch = filterApplyFIR ( shiftBuf_pitch, PID.D_Pitch / (8 * time_charg_Deviation));

 filterUpdateFIR( shiftBuf_roll, roll); float Delta_Roll = filterApplyFIR ( shiftBuf_roll, PID.D_Roll / (8 * time_charg_Deviation));

 filterUpdateFIR( shiftBuf_yaw, yaw); float Delta_Yaw = filterApplyFIR ( shiftBuf_yaw, PID.D_Yaw / (8 * time_charg_Deviation));

 
 Pitch_pid_out = PID.P_Pitch * Error_Pitch + Delta_Pitch;
  
 Roll_pid_out =  PID.P_Roll * Error_Roll + Delta_Roll;
  
 Yaw_pid_out =  PID.P_Yaw * Error_Yaw + Delta_Yaw;


 if ( Yaw_pid_out > Limit.Yaw_Max )  Yaw_pid_out = Limit.Yaw_Max;
 if ( Roll_pid_out > Limit.Roll_Max )  Roll_pid_out = Limit.Roll_Max;
 if ( Pitch_pid_out > Limit.Pitch_Max )  Pitch_pid_out = Limit.Pitch_Max;
 
 if ( Yaw_pid_out < Limit.Yaw_Min )  Yaw_pid_out = Limit.Yaw_Min;  
 if ( Roll_pid_out < Limit.Roll_Min )  Roll_pid_out = Limit.Roll_Min;  
 if ( Pitch_pid_out < Limit.Pitch_Min )  Pitch_pid_out = Limit.Pitch_Min; 

 FlyMet = true;

 Last_Deviation = millis(); time_charg_Deviation = 0; 
 }
}
