#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = 0);
    ~LoginWidget();

    void setInformation(const QString information);

    QString getLogin();
    QString getPassword();

private:
    Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
