#ifndef PID_WIDGET_H
#define PID_WIDGET_H

#include <Data.h>
#include <QWidget>

namespace Ui {
class PID_Widget;
}

class PID_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit PID_Widget(QWidget *parent = nullptr);
    ~PID_Widget();

signals:

  void writeserver(QByteArray data);

public slots:

private slots:

    void Show_config_PID (QByteArray data);

    void on_pushButton_Get_PID_clicked();

    void on_pushButton_Set_PID_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_del_pid_clicked();

private:

    PID pid;

    Ui::PID_Widget *ui;
};

#endif // PID_WIDGET_H
