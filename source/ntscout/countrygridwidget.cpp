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

#include "countrygridwidget.h"
#include "ui_countrygridwidget.h"

#include "countrywidget.h"

#include <qmath.h>

#include <QDebug>

CountryGridWidget::CountryGridWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CountryGridWidget)
{
    ui->setupUi(this);
}

CountryGridWidget::~CountryGridWidget()
{
    delete ui;
}

void CountryGridWidget::setCountryList(CountryList clist)
{
    countryList = clist;
    for (int i = 0; i < countryList.count(); ++i) {
        CountryWidget* widget = new CountryWidget;
        widget->setFlag(QPixmap("flags/flag_" +
            QString::number(countryList.at(i).id) + ".gif"));
        widget->setName(countryList.at(i).name_en);
        widget->setToolTip(countryList.at(i).name);
        widget->setDivisions(QString::number(countryList.at(i).divisions));
        widget->setUsers(QString::number(countryList.at(i).users));
        countryWidgets.append(widget);
    }
}

void CountryGridWidget::resizeEvent(QResizeEvent* event)
{
//    qDebug() << event->size();
//    qDebug() << cnt[0]->size();

    int cols = qMax(1, event->size().width() / countryWidgets.front()->width());
    int rows = qCeil(countryWidgets.count() / (double)cols);

    if (countryWidgets.isEmpty()) {
        qDebug() << "aww";
        return;
    }

//    qDebug() << cols;
//    qDebug() << rows;

    int k = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols && k < countryWidgets.count(); ++j, ++k) {
            ui->gridLayout->removeWidget(countryWidgets.at(k));
            ui->gridLayout->addWidget(countryWidgets.at(k), i, j);
        }
    }

    QWidget::resizeEvent(event);
}
