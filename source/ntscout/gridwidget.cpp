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

#include "gridwidget.h"
#include "ui_gridwidget.h"

#include "countrywidget.h"
#include "searchdialog.h"

#include <qmath.h>

#include <QDebug>

#include <QMenu>
#include <QAction>

GridWidget::GridWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GridWidget)
{
    ui->setupUi(this);

    QMenu* menu = new QMenu(ui->selectAll);
    QAction* actCountry = menu->addAction("Country");
    QAction* actNationality = menu->addAction("Nationality");
    ui->selectAll->setMenu(menu);
    connect(actCountry, SIGNAL(triggered()), this, SLOT(selectAllCountry()));
    connect(actNationality, SIGNAL(triggered()), this, SLOT(selectAllNationality()));
}

GridWidget::~GridWidget()
{
    delete ui;
}

void GridWidget::reset()
{
    ui->showSelected->setChecked(false);
    ui->searchField->setText("");

    countryWidgets.clear();
    for (int i = 0; i < originalWidgets.count(); ++i) {
        originalWidgets.at(i)->setVisible(true);
        originalWidgets.at(i)->unselect();
        countryWidgets.append(originalWidgets.at(i));
    }

    if (ui->sortBox->currentIndex() != 0) {
        ui->sortBox->setCurrentIndex(0);
    }
    else {
        sortBy(0);
    }
}

void GridWidget::setCountryList(CountryList clist)
{
    countryList = clist;
    for (int i = 0; i < countryList.count(); ++i) {
        CountryWidget* widget = new CountryWidget;
        widget->setFlag(QPixmap("flags/flag_" +
            QString::number(countryList.at(i).id) + ".gif"));
        widget->setName(countryList.at(i).name_en);
        widget->setInitialToolTip(countryList.at(i).name);
        widget->setDivisions(countryList.at(i).divisions);
        widget->setUsers(countryList.at(i).users);
        widget->setID(clist.at(i).id);
        originalWidgets.append(widget);
        countryWidgets.append(widget);
        connect(widget, SIGNAL(selected(CountryWidget*)),
                this, SLOT(countrySelected(CountryWidget*)));
        connect(widget, SIGNAL(unselected(CountryWidget*)),
                this, SLOT(countryUnselected(CountryWidget*)));
    }

    if (!originalWidgets.isEmpty()) {
        SearchDialog* dialog = originalWidgets
            .front()->getSearchDialog();
        connect(dialog, SIGNAL(updateDefaultValues()),
                this, SLOT(updateCountryWidgets()));
    }
}

void GridWidget::checkIfCanProceed()
{
    bool country = false;
    bool nationality = false;
    for (int i = 0; i < selectedWidgets.count(); ++i) {
        country |= selectedWidgets.at(i)->isCountrySelected();
        nationality |= selectedWidgets.at(i)->isNationalitySelected();
    }
    emit canProceed(country && nationality);
}

QList<SearchValues*> GridWidget::getSearchValues()
{
    QList<SearchValues*> searchValues;
    for (int i = 0; i < selectedWidgets.count(); ++i) {
        searchValues.append(selectedWidgets.at(i)->getSearchValues());
    }
    return searchValues;
}

void GridWidget::updateGrid()
{
    if (countryWidgets.isEmpty()) {
        return;
    }

    int cols = qMax(1, size().width() / countryWidgets.front()->width());
    int rows = qCeil(countryWidgets.count() / (double)cols);

    for (int i = 0; i < originalWidgets.count(); ++i) {
        ui->gridLayout->removeWidget(originalWidgets.at(i));
    }

    int k = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols && k < countryWidgets.count(); ++j, ++k) {
            ui->gridLayout->addWidget(countryWidgets.at(k), i, j);
        }
    }
}

void GridWidget::resizeEvent(QResizeEvent* event)
{
    updateGrid();

    QWidget::resizeEvent(event);
}

void GridWidget::sortBy(int index)
{
    QList<CountryWidget*>& widgets =
            countryWidgets.count() < originalWidgets.count() ?
                countryWidgets : originalWidgets;

    switch (index) {
        case 0: // ID
            qSort(widgets.begin(), widgets.end(),
                  [](const CountryWidget* a,
                  const CountryWidget* b) -> bool
            {
                return a->getID() < b->getID();
            });
            break;
        case 1: // Name
            qSort(widgets.begin(), widgets.end(),
                  [](const CountryWidget* a,
                  const CountryWidget* b) -> bool
            {
                return a->getName() < b->getName();
            });
            break;
        case 2: // Divisions
            qSort(widgets.begin(), widgets.end(),
                  [](const CountryWidget* a,
                  const CountryWidget* b) -> bool
            {
                return a->getDivisions() > b->getDivisions();
            });
            break;
        case 3: // Users
            qSort(widgets.begin(), widgets.end(),
                  [](const CountryWidget* a,
                  const CountryWidget* b) -> bool
            {
                return a->getUsers() > b->getUsers();
            });
            break;
    }

    // updateGrid is based on countryWidgets
    if (countryWidgets.count() >= originalWidgets.count()) {
        countryWidgets.clear();
        countryWidgets.append(originalWidgets);
    }

    updateGrid();
}

void GridWidget::searchCountry(QString text)
{
    for (int i = 0; i < originalWidgets.count(); ++i) {
        originalWidgets.at(i)->setVisible(false);
    }
    countryWidgets.clear();

    if (text.isEmpty()) {
        for (int i = 0; i < originalWidgets.count(); ++i) {
            originalWidgets.at(i)->setVisible(true);
            countryWidgets.append(originalWidgets.at(i));
        }
    }
    else {
        for (int i = 0; i < originalWidgets.count(); ++i) {
            if (originalWidgets.at(i)->getName().contains(text, Qt::CaseInsensitive) ||
                originalWidgets.at(i)->getNameEn().contains(text, Qt::CaseInsensitive))
            {
                originalWidgets.at(i)->setVisible(true);
                countryWidgets.append(originalWidgets.at(i));
            }
        }
    }

    updateGrid();
}

void GridWidget::selectAllCountry()
{
    for (int i = 0; i < countryWidgets.count(); ++i) {
        countryWidgets.at(i)->selectAsCountry();
    }
}

void GridWidget::selectAllNationality()
{
    for (int i = 0; i < countryWidgets.count(); ++i) {
        countryWidgets.at(i)->selectAsNationality();
    }
}

void GridWidget::unselectAll()
{
    for (int i = 0; i < countryWidgets.count(); ++i) {
        countryWidgets.at(i)->unselect();
        countryWidgets.at(i)->loadNationalityValues();
    }
}

void GridWidget::showSelected()
{
    for (int i = 0; i < originalWidgets.count(); ++i) {
        originalWidgets.at(i)->setVisible(false);
    }
    countryWidgets.clear();

    if (ui->showSelected->isChecked()) {
        for (int i = 0; i < originalWidgets.count(); ++i) {
            if (originalWidgets.at(i)->isSelected()) {
                originalWidgets.at(i)->setVisible(true);
                countryWidgets.append(originalWidgets.at(i));
            }
        }
    }
    else {
        searchCountry(ui->searchField->text());
    }

    updateGrid();
}

void GridWidget::countrySelected(CountryWidget* widget)
{
    if (!selectedWidgets.contains(widget)) {
        selectedWidgets.append(widget);
    }
    checkIfCanProceed();
}

void GridWidget::countryUnselected(CountryWidget* widget)
{
    selectedWidgets.removeOne(widget);
    if (selectedWidgets.count() == 0 &&
        ui->showSelected->isChecked())
    {
        ui->showSelected->setChecked(false);
        showSelected();
    }
    checkIfCanProceed();
}

void GridWidget::updateCountryWidgets()
{
    for (CountryWidget* country : originalWidgets) {
        if (!country->isSelected()) {
            country->loadNationalityValues();
        }
    }
}

