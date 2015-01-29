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

#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include "country.h"

#include <QWidget>

class QResizeEvent;
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

    QAction* createShortcut(QWidget* widget,
        QKeySequence sequence, QString func);
    void reset();
    void updateGrid();
    void checkIfCanProceed();
    void clearMarkedWidget();

    void setCountryList(CountryList clist);
    QList<SearchValues*> getSearchValues();

    void resizeEvent(QResizeEvent* event);

public slots:
    void sortBy(int index);
    void searchCountry(QString text);
    void selectAllCountry();
    void selectAllNationality();
    void unselectAll();
    void showSelected();
    void countrySelected(CountryWidget* widget);
    void countryUnselected(CountryWidget* widget);
    void updateCountryWidgets();

    void setMarkedWidget(CountryWidget* widget);
    void markRight();
    void markLeft();
    void markUp();
    void markDown();
    void selectMarkedCountry();
    void selectMarkedNationality();
    void showSearchDialog();

signals:
    void canProceed(bool);

private:
    CountryList countryList;
    QList<CountryWidget*> originalWidgets;
    QList<CountryWidget*> countryWidgets;
    QList<CountryWidget*> selectedWidgets;
    CountryWidget* markedWidget;

    Ui::GridWidget *ui;
};

#endif // GRIDWIDGET_H

