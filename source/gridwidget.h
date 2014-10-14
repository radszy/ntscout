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

#ifndef COUNTRYGRIDWIDGET_H
#define COUNTRYGRIDWIDGET_H

#include "country.h"

#include <QWidget>
#include <QResizeEvent>

struct SearchValues;
class CountryWidget;

namespace Ui {
class GridWidget;
}

class GridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GridWidget(QWidget *parent = 0);
    ~GridWidget();

    void reset();
    void rearrange();
    void setCountryList(CountryList clist);
    void checkIfCanProceed();
    QList<SearchValues*> getSearchValues();

    void resizeEvent(QResizeEvent* event);

public slots:
    void sortBy(int index);
    void searchCountry(QString text);
    void selectAll();
    void unselectAll();
    void showSelected();
    void countrySelected(CountryWidget* widget);
    void countryUnselected(CountryWidget* widget);

signals:
    void canProceed(bool);

private:
    CountryList countryList;
    QList<CountryWidget*> originalWidgets;
    QList<CountryWidget*> countryWidgets;
    QList<CountryWidget*> selectedWidgets;

    Ui::GridWidget *ui;
};

#endif // COUNTRYGRIDWIDGET_H
