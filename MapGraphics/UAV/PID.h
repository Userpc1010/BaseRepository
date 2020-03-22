#ifndef PID_H
#define PID_H

#include <QWidget>

namespace Ui {
class PID;
}

class PID : public QWidget
{
    Q_OBJECT

public:
    explicit PID(QWidget *parent = nullptr);
    ~PID();

private:
    Ui::PID *ui;
};

#endif // PID_H
