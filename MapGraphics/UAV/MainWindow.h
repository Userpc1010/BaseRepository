#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <Data.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

    void write_to_server(QByteArray data);

    void Send_Config (QByteArray data);

private slots:

    void on_actionExit_triggered();

    void keyPressEvent(QKeyEvent* e);

    void keyReleaseEvent(QKeyEvent* e);

    void closeEvent(QCloseEvent *event);

    void cocking (bool cocking);

    void on_action_config_triggered();

    void ref_Send_Config (QByteArray data);

    void ref_write_to_server (QByteArray data);

private:
    Ui::MainWindow *ui;

    bool lock;
};

#endif // MAINWINDOW_H
