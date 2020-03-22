#ifndef MISSIONPLANER_H
#define MISSIONPLANER_H
#include "MapGraphicsView.h"
#include "MissionPlanerMenu.h"

#include <QObject>

class MissionPlaner : public QObject
{
    Q_OBJECT

public:
    MissionPlaner();


signals:

 void parametres ();

public slots:

 void MoveToPoint (QPointF  point);

 void MoveToRoute (QPointF  points);

 void removePoints ();

 void removeRoute ();

public:

 unsigned int counter = 0;

 float lon [64];

 float lat [64];

 int alt[64], clb[64], indx[64];

 QByteArray Transmit_route;

 MissionPlanerMenu menu;

};

#endif // MISSIONPLANER_H
