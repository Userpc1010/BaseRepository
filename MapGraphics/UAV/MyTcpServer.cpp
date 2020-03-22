#include "MyTcpServer.h"

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
  socket = new QTcpSocket(this);

  connect(socket, SIGNAL(readyRead()),this,SLOT(read()));       //связываем сигнал прихода данных с чтением

  connect(socket, SIGNAL(disconnected()),this,SLOT(disconnect())); //сигнал потери связи с отключением

}

MyTcpServer::~MyTcpServer()
{
  socket->deleteLater();
}


void MyTcpServer::connection()

{
    socket->connectToHost("192.168.3.2",8981);

    lock = true;
}

void MyTcpServer::read()

{
       Data = socket->readAll();          //Ждём и читаем все данные
       reciver (Data);


}

void MyTcpServer::disconnect()

{
    socket->deleteLater();
    lock = false;
}

void MyTcpServer::write(QByteArray data)
{

    if (lock) {socket->write(data); socket->flush(); socket->waitForBytesWritten(5000); socket->flush(); }

}

