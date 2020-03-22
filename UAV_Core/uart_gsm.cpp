#include "uart_gsm.h"
#include <wiringSerial.h>
#include <errno.h>
#include <QDebug>

UART_GSM::UART_GSM()
{



}

void UART_GSM::StartGSM()

{
    if ((fd4 = serialOpen ("/dev/ttyS4", 115200)) < 0)
    {
      qDebug()<<"Ошибка определения GPS"<<strerror (errno) ;
    }

    startTimer(500);


}


void UART_GSM::timerEvent(QTimerEvent*){

    if(serialDataAvail(fd4))
   {

      GSM(); // Вызов «читалки» принятых данных


   }

}


void UART_GSM::GSM()
{
    data.clear();

  while ( serialDataAvail(fd4) ) {

     int c =  serialGetchar(fd4);

     if (c != 13 || c!= 10) { data += c;}
  }
    qDebug()<<": "<<data;

    if (data == "\r\nMODEM:STARTUP\r\n") qDebug()<< "Загрузка GSM...";

    if (data == "\r\n+PBREADY\r\n") { move = 1; Initializing(); }

    if (data == "AT+CLIP=1\r\r\nOK\r\n") { move = 2;  Initializing(); }

    if (data == "AT+CMGF=1\r\r\nOK\r\n") {move = 3; Initializing(); }

    if ( data == "AT+CSCS=\"GSM\"\r\r\nOK\r\n") {move = 4; Initializing(); }

    if ( data == "\r\nRING\r\n\r\n+CLIP: \"79187910029\",145,,,\"\",0\r\n" && ini ) { send = 1; send_cordinat(); }// Звонок

    if ( data == "\r\nNO CARRIER\r\n"); // Вызов окончен

    if ( data == "ATH\r\r\nOK\r\n") { send = 2; send_cordinat(); }
    \

}


void UART_GSM::Initializing()
{

    if (move == 1 ) serialPuts(fd4, "AT+CLIP=1\r");

    if ( move == 2 ) serialPuts(fd4, "AT+CMGF=1\r");

    if ( move == 3 ) serialPuts(fd4, "AT+CSCS=\"GSM\"\r" );

    if ( move == 4 ) { ini = true; move = 0; qDebug()<< "Модем в режиме ожидания";}

}

void UART_GSM::send_cordinat()
{


   if (send == 1) serialPuts(fd4, "ATH\r");

   if (send == 2) {

       //double lat = (long)pvt.lat;
      // double lon = (long)pvt.lon;

      // qDebug()<<lon <<lat;

       //QString sms = "AT+CMGS=\"+79187910029\"""lon:"+QString("%1").arg(lon / 10000000, 0, 'f', 7 )+"lat:"+ QString("%1").arg(lat / 10000000, 0, 'f', 7 )+(char) 26;

       //qDebug()<< sms;

       //serialPuts(fd4, sms.toUtf8());

   }

}

