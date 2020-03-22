#ifndef Signals_H
#define Signals_H
#include <QObject>


class Signals : public QObject
{

 Q_OBJECT

public:
    Signals();


public slots:
   void setValue();
signals:
   void event_uart();

};

#endif // Signals_H
