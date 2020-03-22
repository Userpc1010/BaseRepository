#ifndef MYSERVER_H
#define MYSERVER_H
#include "uart_due.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QString>
#include <QtMath>

class MyServer : public QTcpServer
{
    Q_OBJECT

public:
      MyServer();
      ~MyServer();
public slots:

    void startserver();                                // запуск сервера
    void incomingConnection(int socketDescriptor);     // обработчик входящего подключения имя этого метода привязано к его функции и должно быть толко таким: incomingConnection!
    void read();                                       // обработчик входящих данных
    void disconect();                                  // обработчик разрыва соединения
    void write (QByteArray data);                                     // запись в порт

private:
    QTcpSocket *socket;        // вещатель
    QByteArray arr;            //буфер приёма

    int trust [3];                            //Массив обработки данных от QByteArray
    unsigned int truster = 0;                 //Число тяги
    unsigned int trusterOLD = 0;              //запасное значение на случай ошибки определения длинны массива

    unsigned char convert (int trust [], int length);   //Метод обработки строкового числа тяги в переменную

    bool lock =false;

signals:

   void send_uart (QByteArray data);

   void send_point (QByteArray point);
};
#endif // MYSERVER_H
