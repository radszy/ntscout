#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    layout()->setAlignment(ui->usernameField, Qt::AlignHCenter);
    layout()->setAlignment(ui->accessKeyField, Qt::AlignHCenter);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::setInformation(const QString information)
{
    ui->informationLabel->setText(information);
}
