#ifndef MARKERS_H
#define MARKERS_H

#include "MapGraphics_global.h"
#include "MapGraphicsObject.h"

class Markers : public QGraphicsObject
{
  Q_OBJECT

public:

    Markers();
    ~Markers();


  void AddMarker (qreal _radius,  bool _draw, QString _markers, QString _index);




};

#endif // MARKERS_H
