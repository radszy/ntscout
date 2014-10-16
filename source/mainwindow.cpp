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
#include "ui_proceeddialog.h"

#include "gridwidget.h"
#include "loginwidget.h"
#include "progresswidget.h"
#include "summarywidget.h"
#include "settingsdialog.h"

#include "bbapi.h"
#include "country.h"
#include "player.h"
#include "util.h"

#include <QMessageBox>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QAction>
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loginWidget = new LoginWidget;
    gridWidget = new GridWidget;
    progressWidget = new ProgressWidget;
    summaryWidget = new SummaryWidget;
    settingsDialog = new SettingsDialog;

    ui->stackedWidget->addWidget(loginWidget);
    ui->stackedWidget->addWidget(gridWidget);
    ui->stackedWidget->addWidget(progressWidget);
    ui->stackedWidget->addWidget(summaryWidget);

    ui->actionUpdate->setDisabled(true);

    connect(gridWidget, SIGNAL(canProceed(bool)),
            this, SLOT(enableNext(bool)));
    connect(progressWidget, SIGNAL(finished(bool)),
            ui->nextButton, SLOT(setEnabled(bool)));

    QAction* act = new QAction(this);
    act->setShortcut(Qt::Key_Return);
    addAction(act);
    connect(act, SIGNAL(triggered()), this, SLOT(nextClicked()));

    QDesktopWidget desktop;
    QRect screen = desktop.screenGeometry();
    int width = 640;
    int height = 480;
    setGeometry(screen.width() / 2 - width / 2,
                screen.height() / 2 - height / 2,
                width, height);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::proceedToCountryWidget()
{
    loginWidget->setInformation("");
    ui->nextButton->setDisabled(true);

    QString user = loginWidget->getLogin();
    QString pass = loginWidget->getPassword();

    BBApi bb(user, pass);
    QString error = bb.login();
    if (!error.isEmpty()) {
        loginWidget->setInformation(
                    "<html><font color=\"red\">"
                    "Unable to login: " +
                    error + "</color></html>");
        ui->nextButton->setEnabled(true);
        return;
    }

    enableNext(false);

    CountryList clist;
    Util::readCountry(clist);
    gridWidget->setCountryList(clist);
    ui->stackedWidget->setCurrentWidget(gridWidget);
}

void MainWindow::proceedToProgressWidget()
{
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setText("<p align=\"center\" style=\"font-size:14px\">Program is going to start searching now.</p>");
    msgBox.setInformativeText("<p align=\"center\" style=\"font-size:14px\">Do you want to proceed?</p>");
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes) {
        progressWidget->reset();
        ui->stackedWidget->setCurrentWidget(progressWidget);
        ui->backButton->setEnabled(true);
        ui->nextButton->setDisabled(true);
        QList<SearchValues*> values = gridWidget->getSearchValues();
        progressWidget->start(values);
    }
}

void MainWindow::proceedToSummaryWidget()
{
    ui->backButton->setDisabled(true);
    ui->nextButton->setEnabled(true);

    PlayerList players = progressWidget->getResults();
    summaryWidget->setResults(players);
    ui->stackedWidget->setCurrentWidget(summaryWidget);
}

void MainWindow::goBackToCountryWidget()
{
    ui->backButton->setDisabled(true);
    enableNext(false);

    gridWidget->reset();
    ui->stackedWidget->setCurrentWidget(gridWidget);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (ui->stackedWidget->currentIndex() == Progress) {
        progressWidget->stop();
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::nextClicked()
{
    switch (ui->stackedWidget->currentIndex()) {
        case Login:
            proceedToCountryWidget();
            break;
        case Country:
            proceedToProgressWidget();
            break;
        case Progress:
            proceedToSummaryWidget();
            break;
        case Summary:
            // prepare for new search
            goBackToCountryWidget();
            break;
    }
}

void MainWindow::backClicked()
{
    switch (ui->stackedWidget->currentIndex()) {
        case Progress:
            progressWidget->stop();
            ui->stackedWidget->setCurrentWidget(gridWidget);
            ui->nextButton->setEnabled(true);
            ui->backButton->setDisabled(true);
            break;
        default:
            // enabled only in Progress
            break;
    }
}

void MainWindow::updateTriggered()
{
    BBApi bb;
    if (bb.login() != "") {
        QMessageBox::warning(this, "Error", "Please login first", QMessageBox::Ok);
        return;
    }

    CountryList clist;
    bb.countries(clist);
    bb.namesEn(clist);

    Util::writeCountry(clist);

    // download flags

    QMessageBox::information(this, "Done", "You need to restart application to take effect.");
}

void MainWindow::settingsTriggered()
{
    settingsDialog->exec();
}

void MainWindow::reportTriggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/rsxee/NTScout/issues"));
}

void MainWindow::aboutTriggered()
{
    QDesktopServices::openUrl(QUrl("http://www.buzzerbeater.com/community/forum/read.aspx?thread=218184&m=1"));
}

void MainWindow::enableNext(bool enabled)
{
    ui->nextButton->setEnabled(enabled);
    ui->nextButton->setToolTip(
                enabled ? "" : "You need to select at least "
                               "one country and one nationality");
}
