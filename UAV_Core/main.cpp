#include <QCoreApplication>
#include <myserver.h>
#include <uart_due.h>
#include <uart_gps_gsm.h>
#include <uart_gsm.h>
#include <GPS.h>
#include <thread>
#include <QObject>

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    UART_Protokol Serial;

    Serial.StartProtokol();

    UART_GPS_GSM GPMS;

    GPMS.Start_GPS();

    GPMS.Start_GSM();

    MyServer Server;



   // std::thread serial (&UART_Protokol::Event, Serial); // Запуск метода класса в отдельном потоке

   // serial.detach();

   // std::this_thread::sleep_for(std::chrono::milliseconds(50)); //что-бы тратить меньше ресурсов ЦП




    QObject::connect (&a,&QCoreApplication::destroyed,        &Server, &MyServer::deleteLater, Qt::QueuedConnection);

    QObject::connect (&a,&QCoreApplication::destroyed,        &Serial, &UART_Protokol::deleteLater, Qt::QueuedConnection);

    QObject::connect (&a,&QCoreApplication::destroyed,        &GPMS, &UART_GPS_GSM::deleteLater, Qt::QueuedConnection);



    QObject::connect (&Serial,&UART_Protokol::ServerWrite,    &Server,&MyServer::write, Qt::QueuedConnection);

    QObject::connect (&Server,&MyServer::send_uart,           &Serial, &UART_Protokol::sp_Send, Qt::QueuedConnection);

    QObject::connect(&Server,&MyServer::send_point,           &GPMS, &UART_GPS_GSM::reciver_point, Qt::QueuedConnection);

    QObject::connect(&GPMS,&UART_GPS_GSM::Tele_process,       &Server,&MyServer::write, Qt::QueuedConnection );

     QObject::connect(&GPMS,&UART_GPS_GSM::azimut,            &Serial, &UART_Protokol::sp_Send, Qt::QueuedConnection);

    QObject::connect (&Serial,&UART_Protokol::Telemetriy,     &GPMS,&UART_GPS_GSM::telemetry, Qt::QueuedConnection);

    QObject::connect (&a,&QCoreApplication::destroyed,        &Server,&MyServer::deleteLater, Qt::QueuedConnection);


    Server.startserver();

    return a.exec();
}
 /*
  "By default, a QObject lives in the thread in which it is created. " "All QObjects must live in the same thread as their parent." But if i can't use thread for UART_Protokol::Event how call
   read method when data arrive in port? Again use signal, but i don't know what emit signal when data arrived in UART. Maybe flag comport used for notification.
 */
