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

#include "gridwidget.h"
#include "loginwidget.h"
#include "progresswidget.h"
#include "summarywidget.h"
#include "settingsdialog.h"

#include "bbapi.h"
#include "country.h"
#include "player.h"
#include "settings.h"
#include "util.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
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

    connect(gridWidget, SIGNAL(canProceed(bool)),
            this, SLOT(enableNextButton(bool)));
    connect(progressWidget, SIGNAL(finished(bool)),
            ui->nextButton, SLOT(setEnabled(bool)));

    int width = 640, height = 480;
    QPoint point = Util::screenCenter(width, height);
    setGeometry(point.x(), point.y(), width, height);

    setWindowTitle("NTScout " + version);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::proceedToCountryWidget()
{
    loginWidget->disableFields();
    loginWidget->setInformation("");
    ui->nextButton->setDisabled(true);

    QString user = loginWidget->getLogin();
    QString pass = loginWidget->getPassword();

    BBApi bb(user, pass);
    QString error = bb.login();
    if (!error.isEmpty()) {
        loginWidget->enableFields();
        loginWidget->setInformation(
                    "<html><font color=\"red\">"
                    "Unable to login: " +
                    error + "</color></html>");
        ui->nextButton->setEnabled(true);
        return;
    }

    enableNextButton(false);

    CountryList clist;
    if (!Util::readCountry(clist)) {
        QMessageBox::critical(this, "Error", "Could not read country.dat!");
        qApp->exit();
    }
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
    ui->backButton->setEnabled(true);
    enableNextButton(false);

    gridWidget->reset();
    ui->stackedWidget->setCurrentWidget(gridWidget);
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
        case Country:
            ui->stackedWidget->setCurrentWidget(summaryWidget);
            ui->nextButton->setEnabled(true);
            ui->backButton->setDisabled(true);
            break;
        case Login:
        case Summary:
            Q_UNREACHABLE();
            break;
    }
}

void MainWindow::updateTriggered()
{
    qDebug() << "App path : " << qApp->applicationDirPath();
    if (BBApi::getName().isEmpty()) {
        QMessageBox::information(this,
            "Error", "Please login first", QMessageBox::Ok);
        return;
    }

    QString path = qApp->applicationDirPath() + "/Updater";
    QStringList params;
    params << "-u" << BBApi::getName()
           << "-p" << BBApi::getPass()
           << "-n" << version;

    QProcess::startDetached(path, params);
    qApp->exit();
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

void MainWindow::enableNextButton(bool enabled)
{
    ui->nextButton->setEnabled(enabled);
    ui->nextButton->setToolTip(
                enabled ? "" : "You need to select at least "
                               "one country and one nationality");
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (ui->stackedWidget->currentIndex() == Progress) {
        progressWidget->stop();
    }

    Settings::save();
    QMainWindow::closeEvent(event);
}

