#include "PID_Widget.h"
#include "ui_PID_Widget.h"
#include <QDebug>

PID_Widget::PID_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PID_Widget)
{
    ui->setupUi(this);  
}

PID_Widget::~PID_Widget()
{
    delete ui;
}

void PID_Widget::Show_config_PID(QByteArray data)
{
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

void PID_Widget::on_pushButton_Get_PID_clicked()
{
  writeserver("PID0");
}

void PID_Widget::on_pushButton_Set_PID_clicked()
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

void PID_Widget::on_pushButton_Close_clicked()
{
  this->deleteLater();
}

void PID_Widget::on_pushButton_del_pid_clicked()
{
   writeserver("dp");
}
