#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "MapGraphicsView.h"
#include "MapGraphicsScene.h"
#include "tileSources/GridTileSource.h"
#include "tileSources/OSMTileSource.h"
#include "tileSources/CompositeTileSource.h"
#include "guts/CompositeTileSourceConfigurationWidget.h"
#include "CircleObject.h"
#include "PolygonObject.h"
#include "MissionPlaner.h"
#include "MissionPlanerMenu.h"
#include "MyTcpServer.h"
#include "PID_Widget.h"

#include <QSharedPointer>
#include <QtDebug>
#include <QImage>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Setup the MapGraphics scene and view
    MapGraphicsScene * scene = new MapGraphicsScene(this);
    MapGraphicsView * view = new MapGraphicsView(scene,this);

    MyTcpServer * server = new MyTcpServer(this);

    //The view will be our central widget
    this->setCentralWidget(view);

    //Setup some tile sources
    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    QSharedPointer<GridTileSource> gridTiles(new GridTileSource(), &QObject::deleteLater);
    QSharedPointer<CompositeTileSource> composite(new CompositeTileSource(), &QObject::deleteLater);
    composite->addSourceBottom(osmTiles);
    composite->addSourceTop(gridTiles);
    view->setTileSource(composite);

    //Create a widget in the dock that lets us configure tile source layers
    CompositeTileSourceConfigurationWidget * tileConfigWidget = new CompositeTileSourceConfigurationWidget(composite.toWeakRef(),
                                                                                         this->ui->dockWidget);
    MissionPlanerMenu * menu = new MissionPlanerMenu (this->ui->dockWidget_2);


    this->ui->dockWidget_2->setWidget(menu);
    this->ui->dockWidget->setWidget(tileConfigWidget);
    delete this->ui->dockWidgetContents;
    delete this->ui->dockWidgetContents_5;

    this->ui->menuWindow->addAction(this->ui->dockWidget->toggleViewAction());
    this->ui->menuWindow->addAction(this->ui->dockWidget_2->toggleViewAction());
    this->ui->dockWidget->toggleViewAction()->setText("&Слои");
    this->ui->dockWidget_2->toggleViewAction()->setText("&Упрвление");

    connect(menu, SIGNAL(connection()),                       server, SLOT(connection()));
    connect(this, SIGNAL(destroyed()),                        server, SLOT(deleteLater()));
    connect(menu, SIGNAL(writeserver(QByteArray)),            server, SLOT(write(QByteArray)));
    connect(server, SIGNAL(reciver(QByteArray)),              menu, SLOT(reciverdata(QByteArray)));
    connect(this, SIGNAL(destroyed()),                        menu, SLOT(deleteLater()));


    connect(view, SIGNAL(mapClicked_Left(QPointF)),           menu, SLOT(MoveToPoint(QPointF)));
    connect(view, SIGNAL(mapClicked_Right(QPointF)),          menu, SLOT(MoveToRoute (QPointF)));
    connect(view, SIGNAL(remove()),                           menu, SLOT(removePoints()));
    connect(view, SIGNAL(remove_all()),                       menu, SLOT(removeRoute()));
    connect(menu, SIGNAL(plane (QPointF)),                    view, SLOT(plane_marker (QPointF)));
    connect(menu, SIGNAL(find_to_map (QPointF)),              view, SLOT(find_marker (QPointF)));
    connect(menu, SIGNAL(Show_PID(QByteArray)),               this, SLOT(ref_Show_PID(QByteArray)));

    connect(this, SIGNAL(write_to_server(QByteArray)),        server, SLOT(write(QByteArray)));
    connect(menu, SIGNAL(cocking_(bool)),                     this, SLOT(cocking(bool)));

    lock = menu->cocking;

    view->setZoomLevel(18);
    view->centerOn( 41.934028, 45.135004);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//private slot
void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::cocking(bool cocking)
{
 lock = cocking;
}


void MainWindow::keyPressEvent(QKeyEvent* e)
{
     if( !e->isAutoRepeat() && lock ) // Без этого постоянно отсылаютсья сигналы
      {
         switch (e->key()) {
        case 1062:
            write_to_server("wu");
            break;
        case 1067:
            write_to_server("su");
            break;
        case 1060:
            write_to_server("au");
            break;
        case 1042:
            write_to_server("du");
            break;
        case 1049:
            write_to_server("qu");
            break;
        case 1059:
            write_to_server("eu");
            break;
           }
}
  e->accept();
}

void MainWindow::keyReleaseEvent(QKeyEvent* e)
{
     if( !e->isAutoRepeat() && lock )        // Без этого постоянно отсылаютсья сигналы
     {

         switch (e->key()) {
        case 1062:
            write_to_server("wd");
            break;
        case 1067:
            write_to_server("sd");
            break;
        case 1060:
            write_to_server("ad");
            break;
        case 1042:
            write_to_server("dd");
            break;
        case 1049:
            write_to_server("qd");
            break;
        case 1059:
            write_to_server("ed");
            break;
           }
}
     e->accept();
}

void MainWindow::on_action_PID_triggered()
{
    PID_Widget * PID_config_widget = new PID_Widget();
    PID_config_widget->setAttribute(Qt::WA_DeleteOnClose);

    connect(this, SIGNAL(destroyed()),      PID_config_widget, SLOT(deleteLater()));

    connect(PID_config_widget,SIGNAL(writeserver(QByteArray)), this, SLOT(ref_write_to_server(QByteArray)));

    connect(this, SIGNAL (Show_PID(QByteArray)),   PID_config_widget, SLOT(Show_config_PID (QByteArray)));

    PID_config_widget->show();
}

void MainWindow::ref_Show_PID(QByteArray data)
{
  emit Show_PID(data);

}

void MainWindow::ref_write_to_server(QByteArray data)
{
   emit write_to_server(data);
}
