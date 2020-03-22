#include "PID.h"
#include "ui_PID.h"

PID::PID(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PID)
{
    ui->setupUi(this);
}

PID::~PID()
{
    delete ui;
}
