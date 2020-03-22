#ifndef MyTcpServer_H
#define MyTcpServer_H

#include <QObject>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
      ~MyTcpServer();

    bool lock;


    QTcpSocket *socket;
    QByteArray Data;


signals:

   void reciver (QByteArray data);

public slots:

    void write(QByteArray data);
    void read();
    void disconnect();
    void connection();

};

#endif // MyTcpServer_H
