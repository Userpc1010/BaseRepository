#include "MissionPlaner.h"


MissionPlaner::MissionPlaner()
{


}


void MissionPlaner::MoveToPoint(QPointF  point)
{
  qDebug()<<point;


}

void MissionPlaner::MoveToRoute(QPointF  points)
{
  parametres();

  counter++;

  lon [counter] = points.x(); //Долгота

  lat [counter] = points.y(); //Широта

  alt [counter] = menu._alt;  //Высота

  clb [counter] = menu._clb;  //Угол подьёма

  indx [counter] = menu._indx; // счётчик тоже нужно предать что-бы избежать пустых точек

  qDebug()<< "lon " << lon[counter] << "lat " << lat[counter]<<"Эшелон "<<alt[counter] <<"Климб "<<clb[counter] <<"Тип "<<indx[counter]<<"№ "<<counter;
}

void MissionPlaner::removePoints()
{
  qDebug()<<"remove_point";

  counter--;

  lon [counter] = 0;

  lat [counter] = 0;

  alt [counter] = 0;

  clb [counter] = 0;

  indx [counter] = 0;
}

void MissionPlaner::removeRoute()
{
    counter = 0;

    for (int i = 0; i < 64; i++ )
    {
        lon [i] = 0;

        lat [i] = 0;

        alt [i] = 0;

        clb [i] = 0;

        indx [i] = 0;
    }

    qDebug()<<"remove_all_points";
}

