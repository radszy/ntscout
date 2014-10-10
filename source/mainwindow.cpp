//Copyright (C) <2014>  <RSX>

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "countrygridwidget.h"
#include "loginwidget.h"

#include "bbapi.h"
#include "country.h"
#include "player.h"
#include "util.h"

#include <QDebug>
#include <QFile>

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
            Util::readCountry(clist);
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
