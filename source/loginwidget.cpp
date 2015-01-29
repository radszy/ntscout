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
