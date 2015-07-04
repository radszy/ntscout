//Copyright (C) <2015>  <RSX>

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
#include "searchvalues.h"

#include <QWidget>

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

    void loadNationalityValues();
    void loadCountryValues();
    void setFlag(const QPixmap& pixmap);
    void setName(const QString& value);
    void setDivisions(int value);
    void setUsers(int value);
    void setID(int value);
    void setInitialToolTip(const QString& tooltip);

    QString getName() const {return name;}
    QString getNameEn() const {return nameEn;}
    int getID() const {return id;}
    int getDivisions() const {return divisions;}
    int getUsers() const {return users;}
    SearchValues* getSearchValues() {return &searchValues;}

    void selectAsCountry(bool value = true);
    void selectAsNationality(bool value = true);
    void unselect();
    bool isSelected() const;
    bool isCountrySelected() const;
    bool isNationalitySelected() const;

    void markFrame();
    void unmarkFrame();

    void showSearchDialog();
    static SearchDialog* getSearchDialog() {
        return searchDialog;
    }

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

signals:
    void selected(CountryWidget*);
    void unselected(CountryWidget*);
    void hovered(CountryWidget*);

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

