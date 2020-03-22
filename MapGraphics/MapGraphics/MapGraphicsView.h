#ifndef MAPGRAPHICSVIEW_H
#define MAPGRAPHICSVIEW_H

#include <QWidget>
#include <QPointer>
#include <QSharedPointer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QList>
#include <QContextMenuEvent>
#include <QVector3D>
#include <QStringBuilder>
#include <QHash>

#include "MapGraphicsScene.h"
#include "MapGraphicsObject.h"
#include "MapTileSource.h"
#include "MapGraphics_global.h"

#include "guts/MapTileGraphicsObject.h"
#include "guts/PrivateQGraphicsInfoSource.h"
#include "CircleObject.h"
#include "LineObject.h"

class MAPGRAPHICSSHARED_EXPORT MapGraphicsView : public QWidget, public PrivateQGraphicsInfoSource
{
    Q_OBJECT
public:
    enum DragMode
    {
        NoDrag,
        ScrollHandDrag,
        RubberBandDrag
    };

    enum ZoomMode
    {
        CenterZoom,
        MouseZoom
    };

public:
    explicit MapGraphicsView(MapGraphicsScene * scene=0, QWidget * parent = 0);
    virtual ~MapGraphicsView();

    QPointF center() const;
    void centerOn(const QPointF& pos);
    void centerOn(qreal longitude, qreal latitude);
    void centerOn(const MapGraphicsObject * item);

    QPointF mapToScene(const QPoint viewPos) const;

    MapGraphicsView::DragMode dragMode() const;
    void setDragMode(MapGraphicsView::DragMode);

    MapGraphicsScene * scene() const;
    void setScene(MapGraphicsScene *);

    //pure-virtual from PrivateQGraphicsInfoSource
    QSharedPointer<MapTileSource> tileSource() const;

    /**
     * @brief Sets the tile source that this view will pull from.
     * MapGraphicsView does NOT take ownership of the tile source.
     *
     * @param tSource
     */
    void setTileSource(QSharedPointer<MapTileSource> tSource);

    //pure-virtual from PrivateQGraphicsInfoSource
    quint8 zoomLevel() const;
    void setZoomLevel(quint8 nZoom, ZoomMode zMode = CenterZoom);

    void zoomIn(ZoomMode zMode = CenterZoom);
    void zoomOut(ZoomMode zMode = CenterZoom);

signals:

    void zoomLevelChanged(quint8 nZoom);
    void mapClicked_Left(QPointF pos);
    void remove ();
    void remove_all ();
    void mapClicked_Right(QPointF pos);

public slots:

    void find_marker (QPointF point);

    void plane_marker (QPointF point);

protected slots:
    virtual void handleChildMouseDoubleClick(QMouseEvent * event);
    virtual void handleChildMouseMove(QMouseEvent * event);
    virtual void handleChildMousePress(QMouseEvent * event);
    virtual void handleChildMouseRelease(QMouseEvent * event);
    virtual void handleChildViewContextMenu(QContextMenuEvent * event);
    virtual void handleChildViewScrollWheel(QWheelEvent * event);    

private slots:
    void renderTiles();

protected:
    void doTileLayout();
    void resetQGSSceneSize();

private:
    QPointer<MapGraphicsScene> _scene;
    QPointer<QGraphicsView> _childView;
    QPointer<QGraphicsScene> _childScene;
    QSharedPointer<MapTileSource> _tileSource;

    QSet<MapTileGraphicsObject *> _tileObjects;
    QPoint _clickPos;

    QList <QGraphicsObject *> objets;

    QPointF sceneLastPos;

    QPointF sceneLastLastPos;

    qint8 counter = 0;

    CircleObject * Marker;

    CircleObject * Marker1;

    CircleObject * Find_marker;

    CircleObject * Plane_marker = new CircleObject( 16, true);

    LineObject * Line;

    quint8 _zoomLevel;

    DragMode _dragMode;

    QString left_click = ":/icons/images/Marker_3.png";

    QString right_click = ":/icons/images/Marker_5.png";

    QString Find_markerr = ":/icons/images/Marker_4.png";

    QString Plane_markerr = ":/icons/images/Plane_1.png";

    bool lock_1 = false;

    bool lock_2 = false;


};

inline uint qHash(const QPointF& key)
{
    const QString temp = QString::number(key.x()) % "," % QString::number(key.y());
    return qHash(temp);
}

#endif // MAPGRAPHICSVIEW_H
