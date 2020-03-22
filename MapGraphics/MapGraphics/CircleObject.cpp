#include "CircleObject.h"

#include <QtDebug>
#include <QKeyEvent>

CircleObject::CircleObject(qreal radius,bool sizeIsZoomInvariant, MapGraphicsObject *parent) :
    MapGraphicsObject(sizeIsZoomInvariant,parent)
{
    _radius = qMax<qreal>(radius,0.01);

    this->setFlag(MapGraphicsObject::ObjectIsSelectable);
    this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);
}

CircleObject::~CircleObject()
{
}

QRectF CircleObject::boundingRect() const
{
    return QRectF(-1*_radius,
                  -1*_radius,
                  2*_radius,
                  2*_radius);
}
void CircleObject::drawMarkers(bool _draw, bool _marker, QString _markers, QString _index, bool _plane)
{
    draw = _draw; index = _index; markers = _markers; marker = _marker; plane = _plane;

    if (_plane)
    {
        this->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
        this->setFlag(MapGraphicsObject::ObjectIsMovable, false);
        this->setFlag(MapGraphicsObject::ObjectIsFocusable, false);
    }

}

void CircleObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(!marker)
    {
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setBrush(_fillColor);
    painter->drawEllipse(QPointF(0,0),_radius,_radius);
     return;
    }


   painter->rotate(-180);
   painter->setPen(QColor(0,0,0,255));


   if (plane)painter->drawPixmap(QPointF(-16,-15),QPixmap(markers) );
   else painter->drawPixmap(QPointF(-10,-15),QPixmap(markers) );

   if (draw){ //Отрисовка текста

   painter->setFont(QFont("Times", 14));

   const QTransform& t = painter->transform();   // Переворачивает текст
   bool mirror = t.determinant() < 0;
   double dx = t.m11() > 0 ? 1 : -1;
   double dy = mirror ? -dx : dx;
   painter->save();
   QTransform scale = QTransform::fromScale(dx,dy);
   painter->setTransform(scale, true);

   painter->drawText( QPointF(-15,15), index );

   painter->restore();
   }
}

qreal CircleObject::radius() const
{
    return _radius;
}

void CircleObject::setRadius(qreal radius)
{
    _radius = radius;
    this->redrawRequested();
}


//protected
//virtual from MapGraphicsObject
void CircleObject::keyReleaseEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Delete))
    {
        this->deleteLater();
        event->accept();
    }
    else
        event->ignore();
}
