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

private:
    Ui::MainWindow *ui;

    LoginWidget* loginWidget;
    CountryGridWidget* countries;
};

#endif // MAINWINDOW_H
