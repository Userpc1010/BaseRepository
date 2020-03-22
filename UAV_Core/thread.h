#ifndef THREAD_H
#define THREAD_H
#include <QThread>
#include <QString>
#include <QDebug>


class Threads : public QThread
{
public:

   explicit Threads(QString ThreadName);

   void run();

private:

   QString name;

};

#endif // THREAD_H
