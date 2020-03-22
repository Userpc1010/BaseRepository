#ifndef GETPOSITION_H
#define GETPOSITION_H

#include "MapGraphicsScene.h"

#include <QtDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QObject>

class GetPosition : public MapGraphicsScene
{  
    Q_OBJECT

public:

     explicit GetPosition (QObject *parent = 0);


signals:

public slots:

    void mapClicked(QPointF pos);

protected:

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:

    QPointF _clickPos;

};

#endif // GETPOSITION_H
