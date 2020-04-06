#ifndef Config_WIDGET_H
#define Config_WIDGET_H

#include <Data.h>
#include <QWidget>

namespace Ui {
class Config_Widget;
}

class Config_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Config_Widget(QWidget *parent = nullptr);
    ~Config_Widget();

signals:

  void writeserver(QByteArray data);

public slots:

private slots:

    void Show_config_Widget (QByteArray data);

    void on_pushButton_Close_clicked();

    void on_pushButton_Get_PID_clicked();

    void on_pushButton_Set_PID_clicked();

    void on_pushButton_del_pid_clicked();

    void on_pushButton_Get_limit_clicked();

    void on_pushButton_Set_limit_clicked();

    void on_pushButton_Get_trim_clicked();

    void on_pushButton_Set_trim_clicked();

private:

    PID pid;

    Trimm trimm;

    Limit limit;

    Ui::Config_Widget *ui;
};

#endif // Config_WIDGET_H
