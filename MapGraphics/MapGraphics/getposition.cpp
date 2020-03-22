#include "GetPosition.h"

#include <QtDebug>

GetPosition::GetPosition(QObject *parent) { }


void GetPosition::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)

{
  _clickPos = event->pos();

  //mapClicked(_clickPos);

  qDebug()<<_clickPos;
}
