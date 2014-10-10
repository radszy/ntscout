#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class LoginWidget;
class CountryGridWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void nextClicked();
    void backClicked();
    void updateTriggered();
    void settingsTriggered();
    void reportTriggered();
    void aboutTriggered();

private:
    Ui::MainWindow *ui;

    LoginWidget* loginWidget;
    CountryGridWidget* countryGridWidget;
};

#endif // MAINWINDOW_H
