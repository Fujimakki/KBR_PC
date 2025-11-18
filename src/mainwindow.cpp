#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sThread(new QThread(this))
    , sWorker(new SerialWorker)
{
    ui->setupUi(this);

    sWorker->moveToThread(sThread);
}

MainWindow::~MainWindow()
{
    delete ui;
}
