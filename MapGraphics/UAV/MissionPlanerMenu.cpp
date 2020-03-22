#include "MissionPlanerMenu.h"
#include "ui_MissionPlanerMenu.h"


MissionPlanerMenu::MissionPlanerMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MissionPlanerMenu)
{
    ui->setupUi(this);

     QTimer *rout_time = new QTimer(this);

     connect(rout_time, SIGNAL(timeout()),this, SLOT (rout_time_out()));

     rout_time->start(10);
}

MissionPlanerMenu::~MissionPlanerMenu()
{
    delete ui;
}

 void MissionPlanerMenu::reciverdata(QByteArray point)
 {
     if (point == "GSM_load")       ui->label_Table_info_cal_stat->setText("Загрузка GSM модема...");
     if (point == "GSM_init")       ui->label_Table_info_cal_stat->setText("GSM модем загружен");
     if (point == "GSM_send")       ui->label_Table_info_cal_stat->setText("Координаты GPS отправлены");
     if (point == "GSM_no_sim")     ui->label_Table_info_cal_stat->setText("Симкарта не определена");
     if (point == "Un_call")        ui->label_Table_info_cal_stat->setText("Неизвестный вызов... Ошибка инициализации АОН");
     if (point == "Сall")           ui->label_Table_info_cal_stat->setText("Вызов... инициализация отправки данных");
     if (point == "UART_OK_1")      ui->label_Table_info_cal_stat->setText("Самопроверка МК ...");
     if (point == "UART_OK_2")      ui->label_Table_info_cal_stat->setText("Самопроверка МК завершена");
     if (point == "Disconnection")  ui->label_Table_info_cal_stat->setText("Соединение сброшено");
     if (point == "Connection")     ui->label_Table_info_cal_stat->setText("Соединение установлено");
     if (point == "MG1")            ui->label_Table_info_cal_stat->setText("Калибровка запущена...");
     if (point == "MG0")            ui->label_Table_info_cal_stat->setText("Калибровка завершена");
     if (point == "DL1")            ui->label_Table_info_cal_stat->setText("Данные калибровки удалены");
     if (point == "DL2")            ui->label_Table_info_cal_stat->setText("Данные настроки ПИД удалены");
     if (point == "ER1")            ui->label_Table_info_cal_stat->setText("Ошибка инициализации BNO055...");
     if (point == "AC1")            ui->label_Table_info_cal_stat->setText("Самопроверка пошевелите БПЛА...");
     if (point == "AC0")            ui->label_Table_info_cal_stat->setText("Самопроверка завершена успешно");
     if (point == "RC1")            ui->label_Table_info_cal_stat->setText("Перекалибровка завершена");
     if (point == "SV1")            ui->label_Table_info_cal_stat->setText("Данные автокалибровки сохранены");
     if (point == "Reciv")          { counter++; loadind_rout();}

     if (point [0] == 'T' && point[1] == 'E' && point[2] == 'L' && point[3] == 'E' ) {

     uint8_t ee = 4; uint8_t* p = (uint8_t*)(void*)&tele; for( int count = sizeof(tele); count ; --count ) *p++ = point[ee++];

     ui->label_4_altitude->setText( QString("%1").arg(tele.altitude, 0, 'f', 2));
     ui->label_3_gSpeed->setText( QString("%1").arg(tele.g_speed, 0, 'f', 2));
     ui->label_turn_mag->setText( QString("%1").arg(tele.mismatch, 0, 'f', 2));
     ui->label_5_distamce->setText( QString("%1").arg(tele.dist_to_point, 0, 'f', 2));
     ui->label_turn_GNSS->setText( QString("%1").arg(tele.gps_mismatch));
     ui->label_pitch->setText( QString("%1").arg(tele.pitch));
     ui->label_roll->setText( QString("%1").arg(tele.roll));
     ui->label_yaw->setText( QString("%1").arg(tele.yaw));
     ui->label_Sys_cal->setText(QString("%1").arg(tele.cal_system));
     ui->label_Gyro_cal->setText(QString("%1").arg(tele.cal_gyro));
     ui->label_Acc_cal->setText(QString("%1").arg(tele.cal_accel));
     ui->label_Mag_cal->setText(QString("%1").arg(tele.cal_mag));

     plane_marker.setX(tele.lon);

     plane_marker.setY(tele.lat);

     plane(plane_marker);
    }

     if (point [0] == 'P' && point[1] == 'I' && point[2] == 'D') {

     emit Show_PID(point);

     }
 }

void MissionPlanerMenu::on_pushButton_find_gps_clicked()
{
   QPointF point;

   QString lat = ui->lineEdit_Find_to_lat->text();

   QString lon = ui->lineEdit_Find_to_lon->text();

   double latt = lat.toDouble();

   double lonn = lon.toDouble();

   point.setX(lonn);

   point.setY(latt);

   find_to_map(point);
}


void MissionPlanerMenu::MoveToPoint(QPointF  point)
{
    MOVE move;

    alt_cocking = false;

    if (cocking) {

    QByteArray data;

    move.lat = point.y();

    move.lon = point.x();

    move.alt = ui->spinBox_alt->value();

    move.climb = ui->spinBox_climb->value();

    move.type_point = ui->comboBox_type_point->currentIndex();

    move.radius_point = ui->spinBox_radius_point->value();

    move.radius_circl = ui->spinBox_radius_circle->value();

    data += "MOVE";

    uint8_t* p = (uint8_t*)(void*)&move; for( int count = sizeof(move); count ; --count ) data += *p++;

    writeserver(data);

    }
}

void MissionPlanerMenu::MoveToRoute(QPointF points)
{

  rout.lon = points.x();

  rout.lat = points.y();

  rout.alt = ui->spinBox_alt->value();

  rout.climb = ui->spinBox_climb->value();

  rout.type_point = ui->comboBox_type_point->currentIndex();

  rout.radius_point = ui->spinBox_radius_point->value();

  rout.radius_circl = ui->spinBox_radius_circle->value();

  array_rout.push_back(rout);

}

void MissionPlanerMenu::removePoints()
{
  qDebug()<<"remove_point";

if (!array_rout.isEmpty()) array_rout.removeLast();
}

void MissionPlanerMenu::removeRoute()
{     
   if (!array_rout.isEmpty()) array_rout.clear();

    qDebug()<<"remove_all_points";
}

void MissionPlanerMenu::on_pushButton_connection_clicked()
{
  connection();
}

void MissionPlanerMenu::on_pushButton_load_rout_clicked()
{
    alt_cocking = false;

if (!array_rout.isEmpty()){ writeserver("dr"); loadind_rout();} /* Удаляем старый маршрут */
else ui->label_Table_info_cal_stat->setText("Сначало проложите маршрут");

/*
    if ( array_rout.length() > 40) {

        array_rout.clear();

       qDebug()<<"remove_all_points_Limit_40_point";

       QMessageBox msgBox;
       msgBox.setWindowTitle("Ошибка");
       msgBox.setText("Превышен лимит на 40 точек все точки удалены");
       msgBox.exec();
   }

  */
}

void MissionPlanerMenu::loadind_rout()
{
   QByteArray data;

   timer_counter = 0;

   data += "ROUT";

   if ( counter < array_rout.length() )
   {
    ui->label_Table_info_cal_stat->setText("Загрузка маршрута: " + QString("%1").arg(counter) + " из " + QString("%1").arg(array_rout.length()) + " ...");

    uint8_t* p = (uint8_t*)(void*)&array_rout[counter]; for( int count = sizeof(rout); count ; --count ) data += *p++;

    writeserver(data); time_out = true;
   }
   else
   {
    ui->label_Table_info_cal_stat->setText("Маршрут загружен: " + QString("%1").arg(counter) + " из " + QString("%1").arg(array_rout.length()));

    time_out = false;  counter = 0;
   }
}

void MissionPlanerMenu::rout_time_out()
{
  if( time_out) timer_counter ++;

  if( time_out && timer_counter > 20 && !array_rout.isEmpty()) { counter = 0; timer_counter = 0; writeserver("dr"); loadind_rout();}
}

void MissionPlanerMenu::on_pushButton_delete_rout_clicked()
{
  writeserver("dr"); ui->label_Table_info_cal_stat->setText("Маршрут удалён");
}

void MissionPlanerMenu::on_pushButton_return_rout_clicked()
{
 writeserver("rt");
}

void MissionPlanerMenu::on_pushButton_reserved_clicked()
{
 writeserver("st");
}

void MissionPlanerMenu::on_checkBox_cocking_clicked(bool checked)
{
  cocking = checked; cocking_(checked);
}

void MissionPlanerMenu::on_pushButton_recalibration_clicked()
{
   writeserver("ca");
}

void MissionPlanerMenu::on_pushButton_save_calibration_clicked()
{
 writeserver("cl");
}

void MissionPlanerMenu::on_checkBox_Manul_mode_clicked(bool checked)
{
    if (checked) writeserver("MN1");
    else writeserver("MN0");
}

void MissionPlanerMenu::on_horizontalSlider_Truster_valueChanged(int value)
{
    QByteArray data = "tr";

    data += (uint8_t)value;

    writeserver(data);
}


void MissionPlanerMenu::on_pushButton_del_calib_clicked()
{
     writeserver("dc");
}
