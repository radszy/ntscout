#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::setInformation(const QString information)
{
    ui->informationLabel->setText(information);
}

QString LoginWidget::getLogin()
{
    return ui->usernameField->text();
}

QString LoginWidget::getPassword()
{
    return ui->accessKeyField->text();
}
