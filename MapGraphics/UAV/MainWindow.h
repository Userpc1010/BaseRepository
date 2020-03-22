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

    void Show_PID (QByteArray data);

private slots:

    void on_actionExit_triggered();

    void keyPressEvent(QKeyEvent* e);

    void keyReleaseEvent(QKeyEvent* e);

    void cocking (bool cocking);

    void on_action_PID_triggered();

    void ref_Show_PID (QByteArray data);

    void ref_write_to_server (QByteArray data);

private:
    Ui::MainWindow *ui;

    bool lock;
};

#endif // MAINWINDOW_H
