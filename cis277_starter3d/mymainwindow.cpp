#include "mymainwindow.h"
#include "ui_mymainwindow.h"

MyMainWindow::MyMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyMainWindow)
{
    ui->setupUi(this);
    ui->widget->setFocus();
}

MyMainWindow::~MyMainWindow()
{
    delete ui;
}

void MyMainWindow::on_actionQuit_triggered()
{
    close();
}
