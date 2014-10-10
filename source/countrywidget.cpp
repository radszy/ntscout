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

#include "countrywidget.h"
#include "ui_countrywidget.h"

#include <QDebug>

CountryWidget::CountryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CountryWidget)
{
    ui->setupUi(this);
}

CountryWidget::~CountryWidget()
{
    delete ui;
}

void CountryWidget::setFlag(const QPixmap pixmap)
{
    ui->flagLabel->setPixmap(pixmap);
}

void CountryWidget::setName(const QString name)
{
    QString shortened = name;
    int max = 15;
    if (name.count() > max) {
        shortened = name.mid(0, max-3) + "...";
    }
    ui->nameLabel->setText(shortened);
}

void CountryWidget::setDivisions(const QString divisions)
{
    ui->divisionsLabel->setText("Divisions: " + divisions);
}

void CountryWidget::setUsers(const QString users)
{
    ui->usersLabel->setText("Users: " + users);
}

void CountryWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug() << "left";
    }
    else if (event->button() == Qt::RightButton) {
        qDebug() << "right";
    }
}

