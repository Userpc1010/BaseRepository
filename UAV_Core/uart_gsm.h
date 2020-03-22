#ifndef UART_GSM_H
#define UART_GSM_H

#include "GPS.h"
#include <QObject>
#include <QTimerEvent>


class UART_GSM : public QObject
{

    Q_OBJECT


public:

    UART_GSM();

private:

int fd4;

int move = 0;

int send = 0;

bool ini;

QByteArray data;

public:

protected:

     void timerEvent(QTimerEvent*);


public:

     void StartGSM ();

     void GSM();

     void Initializing ();

     void send_cordinat ();



public slots:


signals:

  void GSM_process ();


};

#endif // UART_GSM_H
