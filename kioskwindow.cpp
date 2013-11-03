#include "kioskwindow.h"
#include "ui_kioskwindow.h"

KioskWindow::KioskWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KioskWindow)
{
    ui->setupUi(this);
}

KioskWindow::~KioskWindow()
{
    delete ui;
}
