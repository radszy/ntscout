#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget* parent)
        : QWidget(parent)
        , ui(new Ui::LoginWidget)
{
	ui->setupUi(this);
}

LoginWidget::~LoginWidget()
{
	delete ui;
}

void LoginWidget::disableFields()
{
	ui->usernameField->setDisabled(true);
	ui->accessKeyField->setDisabled(true);
}

void LoginWidget::enableFields()
{
	ui->usernameField->setEnabled(true);
	ui->accessKeyField->setEnabled(true);
}

void LoginWidget::setInformation(const QString& information)
{
	ui->informationLabel->setText(information);
}

void LoginWidget::setError(const QString& error)
{
	setInformation(QString("<html><font color=\"red\">%1</color></html>").arg(error));
}

QString LoginWidget::getLogin()
{
	return ui->usernameField->text();
}

QString LoginWidget::getPassword()
{
	return ui->accessKeyField->text();
}
