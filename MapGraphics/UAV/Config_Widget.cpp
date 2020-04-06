#include "Config_Widget.h"
#include "ui_Config_Widget.h"
#include <QDebug>

Config_Widget::Config_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Config_Widget)
{
    ui->setupUi(this);  
}

Config_Widget::~Config_Widget()
{
    delete ui;
}

void Config_Widget::Show_config_Widget(QByteArray data)
{
   if (data [0] == 'P' && data[1] == 'I' && data[2] == 'D') {

   uint8_t ee = 3; uint8_t* p = (uint8_t*)(void*) &pid; for( int count = sizeof(pid); count ; --count ) *p++ = data[ee++];

   this->ui->doubleSpinBox_P_Alt->setValue(pid.P_Alt);
   this->ui->doubleSpinBox_2_D_Alt->setValue(pid.D_Alt);

   this->ui->doubleSpinBox_3_P_Bank->setValue(pid.P_Bank);
   this->ui->doubleSpinBox_4_D_Bank->setValue(pid.D_Bank);

   this->ui->doubleSpinBox_5_P_Pitch->setValue(pid.P_Pitch);
   this->ui->doubleSpinBox_6_P_Roll->setValue(pid.P_Roll);
   this->ui->doubleSpinBox_7_P_Yaw->setValue(pid.P_Yaw);

   this->ui->doubleSpinBox_8_D_Pitch->setValue(pid.D_Pitch);
   this->ui->doubleSpinBox_9_D_Roll->setValue(pid.D_Roll);
   this->ui->doubleSpinBox_10_D_Yaw->setValue(pid.D_Yaw);

   }

   if (data [0] == 'L' && data[1] == 'I' && data[2] == 'M')
   {
     uint8_t ee = 3; uint8_t* p = (uint8_t*)(void*) &limit; for( int count = sizeof(limit); count ; --count ) *p++ = data[ee++];

     this->ui->doubleSpinBox_Max_pitch->setValue(limit.Pitch_Max);
     this->ui->doubleSpinBox_Max_roll->setValue(limit.Roll_Max);
     this->ui->doubleSpinBox_Max_bank->setValue(limit.Bank_Max);
     this->ui->doubleSpinBox_Max_yaw->setValue(limit.Yaw_Max);

     this->ui->doubleSpinBox_Min_pitch->setValue(limit.Pitch_Min);
     this->ui->doubleSpinBox_Min_roll->setValue(limit.Roll_Min);
     this->ui->doubleSpinBox_Min_bank->setValue(limit.Bank_Min);
     this->ui->doubleSpinBox_Min_yaw->setValue(limit.Yaw_Min);

     this->ui->doubleSpinBox_Max_pitch_error_pid->setValue(limit.Error_Pitch_Max);
     this->ui->doubleSpinBox_Max_roll_error_pid->setValue(limit.Error_Roll_Max);
     this->ui->doubleSpinBox_Max_roll_error_pid->setValue(limit.Error_Yaw_Max);

     this->ui->doubleSpinBox_Min_pitch_error_pid->setValue(limit.Error_Pitch_Min);
     this->ui->doubleSpinBox_Min_roll_error_pid->setValue(limit.Error_Roll_Min);
     this->ui->doubleSpinBox_Min_yaw_error_pid->setValue(limit.Error_Yaw_Min);

   }

   if (data [0] == 'T' && data[1] == 'R' && data[2] == 'I' && data[3] == 'M')
   {
     uint8_t ee = 4; uint8_t* p = (uint8_t*)(void*) &trimm; for( int count = sizeof(trimm); count ; --count ) *p++ = data[ee++];

     this->ui->doubleSpinBox_trimm_left_Ruder->setValue(trimm.Left_V_Tail);
     this->ui->doubleSpinBox_trimm_right_Ruder->setValue(trimm.Right_V_Tail);
     this->ui->doubleSpinBox_trimm_left_Eleron->setValue(trimm.Left_Eleron);
     this->ui->doubleSpinBox_trimm_right_Eleron->setValue(trimm.Right_Eleron);

     this->ui->doubleSpinBox_Magnetc_declination->setValue(trimm.Magnetick_decclination);
     this->ui->doubleSpinBox_offset_pitch_axis->setValue(trimm.Offset_pitch);
     this->ui->doubleSpinBox_offset_roll_axis->setValue(trimm.Offset_roll);
   }
}

void Config_Widget::on_pushButton_Close_clicked()
{
   this->close();
}

void Config_Widget::on_pushButton_Get_PID_clicked()
{
 writeserver("PID0");
}

void Config_Widget::on_pushButton_Set_PID_clicked()
{
    pid.P_Alt = this->ui->doubleSpinBox_P_Alt->value();
    pid.D_Alt = this->ui->doubleSpinBox_2_D_Alt->value();

    pid.P_Bank = this->ui->doubleSpinBox_3_P_Bank->value();
    pid.D_Bank = this->ui->doubleSpinBox_4_D_Bank->value();

    pid.P_Pitch =  this->ui->doubleSpinBox_5_P_Pitch->value();
    pid.P_Roll = this->ui->doubleSpinBox_6_P_Roll->value();
    pid.P_Yaw = this->ui->doubleSpinBox_7_P_Yaw->value();

    pid.D_Pitch = this->ui->doubleSpinBox_8_D_Pitch->value();
    pid.D_Roll = this->ui->doubleSpinBox_9_D_Roll->value();
    pid.D_Yaw = this->ui->doubleSpinBox_10_D_Yaw->value();

    QByteArray data;

    data += "PID1";

    uint8_t* p = (uint8_t*)(void*)&pid; for( int count = sizeof(pid); count ; --count ) data += *p++;

    writeserver(data);
}

void Config_Widget::on_pushButton_del_pid_clicked()
{
  writeserver("dp");
}

void Config_Widget::on_pushButton_Get_limit_clicked()
{
 writeserver("LIM0");
}

void Config_Widget::on_pushButton_Set_limit_clicked()
{
   limit.Pitch_Max = this->ui->doubleSpinBox_Max_pitch->value();
   limit.Bank_Max = this->ui->doubleSpinBox_Max_bank->value();
   limit.Roll_Max = this->ui->doubleSpinBox_Max_roll->value();
   limit.Yaw_Max = this->ui->doubleSpinBox_Max_yaw->value();

   limit.Pitch_Min = this->ui->doubleSpinBox_Min_pitch->value();
   limit.Bank_Min = this->ui->doubleSpinBox_Min_bank->value();
   limit.Roll_Min = this->ui->doubleSpinBox_Min_roll->value();
   limit.Yaw_Min= this->ui->doubleSpinBox_Min_yaw->value();

   limit.Error_Pitch_Max = this->ui->doubleSpinBox_Max_pitch_error_pid->value();
   limit.Error_Roll_Max = this->ui->doubleSpinBox_Max_roll_error_pid->value();
   limit.Error_Yaw_Max = this->ui->doubleSpinBox_Max_yaw_error_pid->value();

   limit.Error_Pitch_Min = this->ui->doubleSpinBox_Min_pitch_error_pid->value();
   limit.Error_Roll_Min = this->ui->doubleSpinBox_Min_roll_error_pid->value();
   limit.Error_Yaw_Min = this->ui->doubleSpinBox_Min_yaw_error_pid->value();

   QByteArray data;

   data += "LIM1";

   uint8_t* p = (uint8_t*)(void*)&limit; for( int count = sizeof(limit); count ; --count ) data += *p++;

   writeserver(data);
}

void Config_Widget::on_pushButton_Get_trim_clicked()
{
  writeserver("TRIM0");
}

void Config_Widget::on_pushButton_Set_trim_clicked()
{
  trimm.Left_V_Tail = this->ui->doubleSpinBox_trimm_left_Ruder->value();
  trimm.Right_V_Tail = this->ui->doubleSpinBox_trimm_right_Ruder->value();

  trimm.Left_Eleron = this->ui->doubleSpinBox_trimm_left_Eleron->value();
  trimm.Right_Eleron = this->ui->doubleSpinBox_trimm_right_Eleron->value();

  trimm.Magnetick_decclination = this->ui->doubleSpinBox_Magnetc_declination->value();
  trimm.Offset_pitch = this->ui->doubleSpinBox_offset_pitch_axis->value();
  trimm.Offset_roll = this->ui->doubleSpinBox_offset_roll_axis->value();

  QByteArray data;

  data += "TRIM1";

  uint8_t* p = (uint8_t*)(void*)&trimm; for( int count = sizeof(trimm); count ; --count ) data += *p++;

  writeserver(data);

}
