#include "myserver.h"



MyServer::MyServer(){} //коструктор

MyServer::~MyServer(){socket->write("Disconnection"); socket->deleteLater(); }//деструктор



void MyServer::startserver()

{
    if (this->listen(QHostAddress::Any,8981))qDebug()<<"Сервер запущен";
    else{ qDebug()<<"Ошибка при попытки старта сервера";}

}



void MyServer::incomingConnection(int socketDescriptor ) // обработчик подключений название метода обязательно должно быть incomingConnection
{
    socket = new QTcpSocket(this);

    socket->setSocketDescriptor( socketDescriptor );// присовение уникального номера порта

    connect(socket,SIGNAL(readyRead()),this,SLOT(read())); // соединение прерывания порта с методом чтения

    connect(socket,SIGNAL(disconnected()),this,SLOT(disconect())); //соединение прерывания разрыва соединнинения с методом

    qDebug()<<socketDescriptor<<" Клиент подключен";

    socket->write("Connection");

    socket->waitForBytesWritten(5000);

    lock = true;

}
void MyServer::read()
{
     arr = socket->readAll();

      if ( arr == "wu" )send_uart("wu");
      if ( arr == "su" )send_uart("su");
      if ( arr == "au" )send_uart("au");
      if ( arr == "du" )send_uart("du");
      if ( arr == "qu" )send_uart("qu");
      if ( arr == "eu" )send_uart("eu");
      if ( arr == "wd" )send_uart("wd");
      if ( arr == "sd" )send_uart("sd");
      if ( arr == "ad" )send_uart("ad");
      if ( arr == "dd" )send_uart("dd");
      if ( arr == "qd" )send_uart("qd");
      if ( arr == "ed" )send_uart("ed");
      if ( arr == "ca" ) send_uart("ca");
      if ( arr == "cl" ) send_uart("cl");
      if ( arr == "st" )send_point("st");
      if ( arr == "rt" )send_point("rt");
      if ( arr == "dr") send_point("dr");
      if ( arr == "MN0")send_uart("MN0");
      if ( arr == "MN1")send_uart("MN1");
      if ( arr == "PID0")send_uart("PID0");
      if ( arr == "LIM0")send_uart("LIM0");
      if ( arr == "TRIM0")send_uart("TRIM0");
      if ( arr == "dp")send_uart("dp");
      if ( arr == "dc")send_uart("dc");

      if (arr[0] == 'M' && arr[1] == 'O' && arr[2] == 'V' && arr[3] == 'E' ) send_point(arr);

      if (arr[0] == 'R' && arr[1] == 'O' && arr[2] == 'U' && arr[3] == 'T' ) send_point(arr);

      if (arr[0] == 'P' && arr[1] == 'I' && arr[2] == 'D' && arr[3] == '1' ) send_uart(arr);

      if (arr[0] == 'L' && arr[1] == 'I' && arr[2] == 'M' && arr[3] == '1' ) send_uart(arr);

      if (arr[0] == 'T' && arr[1] == 'R' && arr[2] == 'I' && arr[3] == 'M' && arr[4] == '1' ) send_uart(arr);


      if( arr[0] == 't' && arr[1] == 'r') send_uart(arr);


}

void MyServer::disconect()

{ 
    socket->deleteLater();

    lock = false;
}


void MyServer::write (QByteArray data)
{
  if(lock){socket->write(data); socket->flush(); socket->waitForBytesWritten(5000); socket->flush();}
}

