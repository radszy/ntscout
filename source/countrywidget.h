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

#ifndef COUNTRYWIDGET_H
#define COUNTRYWIDGET_H

#include "country.h"
#include "searchdialog.h"

#include <QWidget>
#include <QMouseEvent>

class SearchDialog;

namespace Ui {
class CountryWidget;
}

class CountryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CountryWidget(QWidget *parent = 0);
    ~CountryWidget();

    void setFlag(const QPixmap& pixmap);
    void setName(const QString& name);
    void setDivisions(int divisions);
    void setUsers(int users);
    void setID(int id);
    void setInitialToolTip(const QString& tooltip);

    QString getName() const {return name;}
    QString getNameEn() const {return nameEn;}
    int getID() const {return id;}
    int getDivisions() const {return divisions;}
    int getUsers() const {return users;}

    void selectAsCountry();
    void unselect();
    bool isSelected();

    void mousePressEvent(QMouseEvent* event);

private:
    void updateFrame();

    SearchValues searchValues;
    QString name;
    QString nameEn;
    int id;
    int divisions;
    int users;

    static SearchDialog* searchDialog;
    Ui::CountryWidget *ui;
};

#endif // COUNTRYWIDGET_H
