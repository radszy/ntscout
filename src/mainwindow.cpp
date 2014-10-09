#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "loginwidget.h"
#include "countrygridwidget.h"
#include "bbapi.h"
#include "player.h"

#include <QDebug>

extern BBApi* g_bbapi;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loginWidget = new LoginWidget;
    countries = new CountryGridWidget;

//    ui->backButton->setVisible(false);
    ui->buttonLayout->setEnabled(false);

    ui->stackedWidget->addWidget(loginWidget);
//    ui->stackedWidget->addWidget(countries);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::nextClicked()
{
//    loginWidget->setInformation(
//                "<html><font color=\"red\">Unable to login"
//                ": Not Authorized</color></html>");

    BBApi bb;
    bb.login("", "");
    QList<int> d;
    d << 163160;
//    bb.teams(r,d);
    PlayerList r;
    bb.roster(r,d);
}
