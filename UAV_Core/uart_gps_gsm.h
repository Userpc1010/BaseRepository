#ifndef UART_GPS_GSM_H
#define UART_GPS_GSM_H
#include <QObject>
#include <QVector>
#include <QTimer>
#include <QtMath>
#include "GPS.h"

class UART_GPS_GSM : public QObject
{

    Q_OBJECT


public:

    UART_GPS_GSM();

private:

bool ini = false;  /* Готовность gsm модема к отправке */ int8_t last = 0; /* Сброс модема */ uint8_t type_client = 0;

bool gps_reset = false; /* Проверка пакета на четность */ uint8_t gps_reset_count = 0; // Счётчик сброса GPS

NAV_PVT pvt; /* UBX NAV_PVT */ MOVE _move; ROUT _rout; /* Полётные точки */ Move _move_; /* Курс для DUE */ SI si; /* Перевод данных UBX в СИ */

Current_pos current; //Текущее положение БПЛА

Telemetry_uart Tele_u; //Телеметрия от DUE

Telemetry_wlan Tele_w; //Телеметрия для MapGraphics

uint8_t fd3, fd4; // UART 3/4

QVector <ROUT> array_rout; //Запись маршрута полёта


    void translit_to_SI ();

    void GoToNavPoint ();

    int UpdateNavPiont (int type);

    float geoDistance();

    float geoBearing();

    void Move_cirle();

    void Move_line();

public:

     void Start_GSM ();

     void GSM();

     void Initializing_GSM (const uint8_t step );

     void reset (bool res);

     void send_cordinat_SMS (const uint8_t step );

     void Start_GPS ();

     void GPS();

     void calcChecksum(unsigned char* CK);


public slots:

     void gps_timer ();

     void gsm_timer ();

     void gsm_update ();

     void reciver_point(QByteArray point);

     void telemetry( QByteArray data );

signals:

  void Tele_process (QByteArray point);

  void azimut ( QByteArray data );


};
#endif // UART_GPS_H
