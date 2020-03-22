int16_t Pitch_pid_out = 0; //Выходы ПИД регуляторов
int16_t Roll_pid_out = 0;
int16_t Yaw_pid_out = 0;

uint16_t Counter_Deviation = 0; //Время после последих вычислений
uint16_t Counter_Alt = 0; 
uint16_t Counter_Bank = 0;

float LastError_pitch = 0;  //Ошибка регулирования из предыдущих расчётов  
float LastError_roll = 0;
float LastError_yaw = 0;
float LastError_alt = 0;
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


void Bank_pid ( int16_t yaw )
{

<<<<<<< HEAD
 Counter_Bank++; if ( Counter_Bank >= 5000 && !Manual ) 
=======
 Counter_Bank++; if ( Counter_Bank >= 5000 && !Manual )
>>>>>>> 39dc8ac6ed763f6e9cb0dd4cdacc930c622576dc
 { 
  
 int16_t Error = set_point_bank - yaw;

<<<<<<< HEAD
 if (Error < -180) Error += 360;// Делаем результат вычетания вида -180 +180
 if (Error >  180) Error -= 360;
=======
 while (Error < -180) Error += 360;
 while (Error >  180) Error -= 360;
>>>>>>> 39dc8ac6ed763f6e9cb0dd4cdacc930c622576dc

 float Delta =  (Error - LastError_bank) / Counter_Bank;

 LastError_bank = Error;

 set_point_roll = ( PID.P_Bank * Error + PID.D_Bank * Delta);

 if ( set_point_roll > 30 ) set_point_roll = 30;

 if ( set_point_roll < -30 ) set_point_roll = -30;

 Counter_Bank = 0;

 } 
}



void Alt_pid ( float alt )
{

 Counter_Alt++; if ( Counter_Alt >= 9000 &&  !Manual )
 { 
  
 float Error = set_point_alt - alt;

 float Delta =  (Error - LastError_alt) / Counter_Alt;

 LastError_alt = Error;

 set_point_pitch =  PID.P_Alt * Error + PID.D_Alt * Delta;

 if ( set_point_pitch > climb_angle) set_point_pitch = climb_angle;

 if ( set_point_pitch < -climb_angle) set_point_pitch = -climb_angle;

 Counter_Alt = 0;
 
 }
}


void Deviation ( int16_t pitch, int16_t roll,  int16_t yaw, bool circle )
{
  
<<<<<<< HEAD
 Counter_Deviation ++;  if ( Counter_Deviation >= 5100 ) // Решил использовать обычный инкремент т.к. Due работает очень быстро,вызов времени и операции на 32 битными числами стоят дорого но преймущества не дают. 
=======
 Counter_Deviation ++;  if ( Counter_Deviation >= 5100 )
>>>>>>> 39dc8ac6ed763f6e9cb0dd4cdacc930c622576dc
 { 

 int16_t Error_Pitch = set_point_pitch - pitch;
 
 int16_t Error_Roll = set_point_roll - roll;

 if( Manual){ set_point_yaw = yaw + vifish_angle; //Ручное управление разворот град/дельта т
 
<<<<<<< HEAD
 if (set_point_yaw < -180) set_point_yaw += 360;// Делаем результат вычетания вида -180 +180
 if (set_point_yaw >  180) set_point_yaw -= 360;
=======
 if(set_point_yaw < 0) set_point_yaw   += 360.0f; // Делаем результат вычетания вида 0 - 360
 if(set_point_yaw > 360) set_point_yaw  -= 360.0f;
>>>>>>> 39dc8ac6ed763f6e9cb0dd4cdacc930c622576dc
 }
 
 int16_t Error_Yaw = set_point_yaw - yaw;

 if (circle && !Manual) Error_Yaw = Error_Yaw + set_point_pitch;// Вращение вокруг точки

<<<<<<< HEAD
 if (Error_Yaw < -180) Error_Yaw += 360;// Делаем результат вычетания вида -180 +180
 if (Error_Yaw >  180) Error_Yaw -= 360;

 if (Error_Yaw < -60) Error_Yaw = -60;// Что-бы регулятор не рухнул.
 if (Error_Yaw >  60) Error_Yaw = 60;
=======
 while (Error_Yaw < -180) Error_Yaw += 360;// Делаем результат вычетания вида -180 +180
 while (Error_Yaw >  180) Error_Yaw -= 360;
>>>>>>> 39dc8ac6ed763f6e9cb0dd4cdacc930c622576dc

 float Delta_Pitch =  (Error_Pitch - LastError_pitch) / Counter_Deviation;

 float Delta_Roll =  (Error_Roll - LastError_roll) / Counter_Deviation;

 float Delta_Yaw =  (Error_Yaw - LastError_yaw) / Counter_Deviation;

 LastError_pitch = Error_Pitch;

 LastError_roll = Error_Roll;

 LastError_yaw = Error_Yaw;

 
 Pitch_pid_out = PID.P_Pitch * Error_Pitch + Delta_Pitch * PID.D_Pitch;
  
 Roll_pid_out =  PID.P_Roll * Error_Roll + Delta_Roll * PID.D_Roll;
  
 Yaw_pid_out =  PID.P_Yaw * Error_Yaw + Delta_Yaw * PID.D_Yaw;


 if ( Yaw_pid_out < -60  )  Yaw_pid_out = -60; // Делаем результат вида 0 - 60
 if ( Yaw_pid_out > 60   )  Yaw_pid_out = 60;

 if ( Roll_pid_out > 60  )  Roll_pid_out = 60.0f; 
 if ( Roll_pid_out < -60 )  Roll_pid_out = -60.0f; 

 if ( Pitch_pid_out > 60 )  Pitch_pid_out = 60.0f; 
 if ( Pitch_pid_out < -60)  Pitch_pid_out = -60.0f; 

 FlyMet = true;

 Counter_Deviation = 0;
 }
}
