#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "countrygridwidget.h"
#include "loginwidget.h"

#include "bbapi.h"
#include "country.h"
#include "player.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loginWidget = new LoginWidget;
    countryGridWidget = new CountryGridWidget;

    ui->stackedWidget->addWidget(loginWidget);
    ui->stackedWidget->addWidget(countryGridWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::nextClicked()
{
    switch (ui->stackedWidget->currentIndex()) {
        case 0:
        {
            loginWidget->setInformation("");
            ui->nextButton->setDisabled(true);

            QString user = loginWidget->getLogin();
            QString pass = loginWidget->getPassword();

            BBApi bb;
            QString error = bb.login(user, pass);
            if (!error.isEmpty()) {
                loginWidget->setInformation(
                            "<html><font color=\"red\">"
                            "Unable to login: " +
                            error + "</color></html>");
                ui->nextButton->setEnabled(true);
                return;
            }

            CountryList clist;
            bb.countries(clist);
            bb.namesEn(clist);
            countryGridWidget->setCountryList(clist);

            ui->stackedWidget->setCurrentWidget(countryGridWidget);
            ui->backButton->setEnabled(true);

            break;
        }
        case 1:
            break;
    }
}

void MainWindow::backClicked()
{
//    switch (ui->stackedWidget->currentIndex()) {

    //    }
}

void MainWindow::updateTriggered()
{

}

void MainWindow::settingsTriggered()
{

}

void MainWindow::reportTriggered()
{

}

void MainWindow::aboutTriggered()
{

}
