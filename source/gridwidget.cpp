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

#include "gridwidget.h"
#include "ui_gridwidget.h"
#include "countrywidget.h"
#include "searchdialog.h"

#include <qmath.h>

#include <QMenu>
#include <QAction>
#include <QDebug>

GridWidget::GridWidget(QWidget *parent) :
    QWidget(parent),
    markedWidget(nullptr),
    ui(new Ui::GridWidget)
{
    ui->setupUi(this);

    QMenu* menu = new QMenu(ui->selectAll);
    QAction* actCountry = menu->addAction("Country");
    QAction* actNationality = menu->addAction("Nationality");
    ui->selectAll->setMenu(menu);

    connect(actCountry, SIGNAL(triggered()), this, SLOT(selectAllCountry()));
    connect(actNationality, SIGNAL(triggered()), this, SLOT(selectAllNationality()));

    createShortcut(ui->searchField, Qt::CTRL + Qt::Key_F, "setFocus()");
    createShortcut(ui->scrollArea, Qt::CTRL + Qt::Key_G, "setFocus()");
    createShortcut(this, Qt::Key_Right, "markRight()");
    createShortcut(this, Qt::Key_Left, "markLeft()");
    createShortcut(this, Qt::Key_Up, "markUp()");
    createShortcut(this, Qt::Key_Down, "markDown()");
    createShortcut(this, Qt::Key_C, "selectMarkedCountry()");
    createShortcut(this, Qt::Key_N, "selectMarkedNationality()");
    createShortcut(this, Qt::CTRL + Qt::Key_Space, "showSearchDialog()");
}

GridWidget::~GridWidget()
{
    delete ui;
}

QAction* GridWidget::createShortcut(QWidget* widget, const QKeySequence& sequence,
                                    const QString& func)
{
    QAction* act = new QAction(widget);
    act->setShortcut(sequence);
    widget->addAction(act);
    connect(act, "2triggered()", widget, QString("1" + func).toLatin1());
    return act;
}

void GridWidget::reset()
{
    ui->showSelected->setChecked(false);
    ui->searchField->setText("");

    countryWidgets.clear();
    for (CountryWidget* widget : originalWidgets) {
        widget->setVisible(true);
        widget->unselect();
        countryWidgets.append(widget);
    }

    if (ui->sortBox->currentIndex() != ID) {
        ui->sortBox->setCurrentIndex(ID);
    } else {
        sortBy(ID);
    }
}

void GridWidget::setCountryList(const CountryList& list)
{
    countryList = list;
    for (auto&& country : countryList) {
        CountryWidget* widget = new CountryWidget;
        widget->setFlag(QPixmap("flags/flag_" + QString::number(country.id) + ".gif"));
        widget->setName(country.name_en);
        widget->setInitialToolTip(country.name);
        widget->setDivisions(country.divisions);
        widget->setUsers(country.users);
        widget->setID(country.id);

        originalWidgets.append(widget);
        countryWidgets.append(widget);

        connect(widget, SIGNAL(selected(CountryWidget*)),
                this, SLOT(countrySelected(CountryWidget*)));
        connect(widget, SIGNAL(unselected(CountryWidget*)),
                this, SLOT(countryUnselected(CountryWidget*)));
        connect(widget, SIGNAL(hovered(CountryWidget*)),
                this, SLOT(setMarkedWidget(CountryWidget*)));
    }

    if (!originalWidgets.isEmpty()) {
        SearchDialog* dialog = originalWidgets.front()->getSearchDialog();
        connect(dialog, SIGNAL(updateDefaultValues()), this, SLOT(updateCountryWidgets()));
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

void GridWidget::clearMarkedWidget()
{
    if (markedWidget != nullptr) {
        if (!countryWidgets.contains(markedWidget)) {
            markedWidget->unmarkFrame();
            markedWidget = nullptr;
        }
    }
}

QList<SearchValues*> GridWidget::getSearchValues()
{
    QList<SearchValues*> searchValues;
    for (CountryWidget* widget : selectedWidgets) {
        searchValues.append(widget->getSearchValues());
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

    for (CountryWidget* widget : originalWidgets) {
        ui->gridLayout->removeWidget(widget);
    }

    int k = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols && k < countryWidgets.count(); ++j, ++k) {
            ui->gridLayout->addWidget(countryWidgets.at(k), i, j);
        }
    }

    clearMarkedWidget();
}

void GridWidget::resizeEvent(QResizeEvent* event)
{
    updateGrid();

    QWidget::resizeEvent(event);
}

void GridWidget::sortBy(int index)
{
    QList<CountryWidget*>& widgets = countryWidgets.count() < originalWidgets.count() ?
                                         countryWidgets : originalWidgets;

    qSort(widgets.begin(), widgets.end(),
          [index](const CountryWidget* a, const CountryWidget* b) -> bool
    {
        switch (index) {
            case ID: return a->getID() < b->getID();
            case Name: return a->getName() < b->getName();
            case Divisions: return a->getDivisions() < b->getDivisions();
            case Users: default: return a->getUsers() < b->getUsers();
        }
    });

    // updateGrid is based on countryWidgets
    if (countryWidgets.count() >= originalWidgets.count()) {
        countryWidgets.clear();
        countryWidgets.append(originalWidgets);
    }

    updateGrid();
}

void GridWidget::searchCountry(QString text)
{
    for (CountryWidget* widget : originalWidgets) {
        widget->setVisible(false);
    }
    countryWidgets.clear();

    if (text.isEmpty()) {
        for (CountryWidget* widget : originalWidgets) {
            widget->setVisible(true);
            countryWidgets.append(widget);
        }
    }
    else {
        for (CountryWidget* widget : originalWidgets) {
            if (widget->getName().contains(text, Qt::CaseInsensitive) ||
                widget->getNameEn().contains(text, Qt::CaseInsensitive))
            {
                widget->setVisible(true);
                countryWidgets.append(widget);
            }
        }
    }

    updateGrid();
}

void GridWidget::selectAllCountry()
{
    for (CountryWidget* widget : countryWidgets) {
        widget->selectAsCountry();
    }
}

void GridWidget::selectAllNationality()
{
    for (CountryWidget* widget : countryWidgets) {
        widget->selectAsNationality();
    }
}

void GridWidget::unselectAll()
{
    for (CountryWidget* widget : countryWidgets) {
        widget->unselect();
        widget->loadNationalityValues();
        widget->loadCountryValues();
    }
}

void GridWidget::showSelected()
{
    for (CountryWidget* widget : originalWidgets) {
        widget->setVisible(false);
    }
    countryWidgets.clear();

    if (ui->showSelected->isChecked()) {
        for (CountryWidget* widget : originalWidgets) {
            if (widget->isSelected()) {
                widget->setVisible(true);
                countryWidgets.append(widget);
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
    if (selectedWidgets.empty() && ui->showSelected->isChecked()) {
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
            country->loadCountryValues();
        }
    }
}

void GridWidget::setMarkedWidget(CountryWidget* widget)
{
    if (markedWidget != nullptr) {
        markedWidget->unmarkFrame();
    }
    markedWidget = widget;
    markedWidget->markFrame();
}

void GridWidget::markRight()
{
    if (!ui->scrollArea->hasFocus() ||
        countryWidgets.isEmpty()) {
        return;
    }

    if (markedWidget != nullptr) {
        markedWidget->unmarkFrame();

        int cols = ui->gridLayout->columnCount();
        int index = countryWidgets.indexOf(markedWidget);

        if ((index + 1) % cols != 0 && index < countryWidgets.count() - 1) {
            markedWidget = countryWidgets.at(index + 1);
        }
    } else {
        markedWidget = countryWidgets.first();
    }

    markedWidget->markFrame();
    ui->scrollArea->ensureWidgetVisible(markedWidget);
}

void GridWidget::markLeft()
{
    if (!ui->scrollArea->hasFocus() || countryWidgets.isEmpty()) {
        return;
    }

    if (markedWidget != nullptr) {
        markedWidget->unmarkFrame();

        int cols = ui->gridLayout->columnCount();
        int index = countryWidgets.indexOf(markedWidget);

        if ((index + 1) % cols != 1) {
            markedWidget = countryWidgets.at(index - 1);
        }
    } else {
        markedWidget = countryWidgets.first();
    }

    markedWidget->markFrame();
    ui->scrollArea->ensureWidgetVisible(markedWidget);
}

void GridWidget::markDown()
{
    if (!ui->scrollArea->hasFocus() || countryWidgets.isEmpty()) {
        return;
    }

    if (markedWidget != nullptr) {
        markedWidget->unmarkFrame();

        int cols = ui->gridLayout->columnCount();
        int index = countryWidgets.indexOf(markedWidget);

        if (index + cols < countryWidgets.count()) {
            markedWidget = countryWidgets.at(index + cols);
        }
    } else {
        markedWidget = countryWidgets.first();
    }

    markedWidget->markFrame();
    ui->scrollArea->ensureWidgetVisible(markedWidget);
}

void GridWidget::selectMarkedCountry()
{
    if (markedWidget == nullptr) {
        return;
    }

    bool select = !markedWidget->isCountrySelected();
    markedWidget->selectAsCountry(select);
}

void GridWidget::selectMarkedNationality()
{
    if (markedWidget == nullptr) {
        return;
    }

    bool select = !markedWidget->isNationalitySelected();
    markedWidget->selectAsNationality(select);
}

void GridWidget::showSearchDialog()
{
    if (!ui->scrollArea->hasFocus() || markedWidget == nullptr) {
        return;
    }

    markedWidget->showSearchDialog();
}

void GridWidget::markUp()
{
    if (!ui->scrollArea->hasFocus() || countryWidgets.isEmpty()) {
        return;
    }

    if (markedWidget != nullptr) {
        markedWidget->unmarkFrame();

        int cols = ui->gridLayout->columnCount();
        int index = countryWidgets.indexOf(markedWidget);

        if (index >= cols) {
            markedWidget = countryWidgets.at(index - cols);
        }
    } else {
        markedWidget = countryWidgets.first();
    }

    markedWidget->markFrame();
    ui->scrollArea->ensureWidgetVisible(markedWidget);
}

