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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class LoginWidget;
class GridWidget;
class ProgressWidget;
class SummaryWidget;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum Page {
        Login,
        Country,
        Progress,
        Summary
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void proceedToCountryWidget();
    void proceedToProgressWidget();
    void proceedToSummaryWidget();
    void goBackToCountryWidget();

    void closeEvent(QCloseEvent* event);

public slots:
    void nextClicked();
    void backClicked();
    void updateTriggered();
    void settingsTriggered();
    void reportTriggered();
    void aboutTriggered();
    void enableNextButton(bool enabled);

private:
    Ui::MainWindow *ui;

    LoginWidget* loginWidget;
    GridWidget* gridWidget;
    ProgressWidget* progressWidget;
    SummaryWidget* summaryWidget;
    SettingsDialog* settingsDialog;

    const QString version = "7.4";
};

#endif // MAINWINDOW_H
