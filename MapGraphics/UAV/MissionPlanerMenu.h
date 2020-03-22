#ifndef MISSIONPLANERMENU_H
#define MISSIONPLANERMENU_H

#include <QtMath>
#include <QDebug>
#include <QWidget>
#include <QTimer>
#include <QByteArray>
#include <QMessageBox>
#include "Data.h"

namespace Ui {
class MissionPlanerMenu;
}

class MissionPlanerMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MissionPlanerMenu(QWidget *parent = nullptr);
    ~MissionPlanerMenu();


signals:

 void connection ();

 void Show_PID (QByteArray data);

 void writeserver (QByteArray _data);

 void find_to_map (QPointF point);

 void cocking_ (bool cocking);

 void plane (QPointF point);

private:

 QPointF plane_marker;

 Telemetry tele;

 ROUT rout;

 PID pid;

public slots:

    void reciverdata (QByteArray point);

    void MoveToPoint (QPointF  point);

    void MoveToRoute (QPointF  points);

    void removePoints ();

    void removeRoute ();

private slots:

    void loadind_rout ();

    void rout_time_out ();

    void on_pushButton_find_gps_clicked();

    void on_pushButton_connection_clicked();

    void on_pushButton_load_rout_clicked();

    void on_pushButton_delete_rout_clicked();

    void on_pushButton_return_rout_clicked();

    void on_pushButton_reserved_clicked();

    void on_checkBox_cocking_clicked(bool checked);

    void on_pushButton_recalibration_clicked();

    void on_pushButton_save_calibration_clicked();

    void on_horizontalSlider_Truster_valueChanged(int value);

    void on_checkBox_Manul_mode_clicked(bool checked);

    void on_pushButton_del_calib_clicked();

public:

    int counter = 0;

    int timer_counter = 0;

    bool time_out = false;

    QVector <ROUT> array_rout;

    bool cocking = false;

    bool alt_cocking = true;

private:
    Ui::MissionPlanerMenu *ui;
};

#endif // MISSIONPLANERMENU_H
