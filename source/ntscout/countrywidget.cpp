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
#include "searchdialog.h"
#include "settings.h"

#include <QMouseEvent>
#include <QDebug>

SearchDialog* CountryWidget::searchDialog = nullptr;

CountryWidget::CountryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CountryWidget)
{
    ui->setupUi(this);

    searchValues.countrySet = false;
    searchValues.divCount = 6;
    loadCountryValues();
    searchValues.countryid = id;

    searchValues.nationalitySet = false;
    loadNationalityValues();

    if (searchDialog == nullptr) {
        searchDialog = new SearchDialog;
    }
}

CountryWidget::~CountryWidget()
{
    delete ui;
}

void CountryWidget::loadNationalityValues()
{
    searchValues.age = Settings::age;
    searchValues.potential = Settings::pot;
    searchValues.salary = Settings::sal;
    searchValues.dmi = Settings::dmi;
}

void CountryWidget::loadCountryValues()
{
    for (int i = 0; i < searchValues.divCount; ++i) {
        searchValues.div[i] = Settings::checkedDiv[i];
    }
}

void CountryWidget::setFlag(const QPixmap& pixmap)
{
    ui->flagLabel->setPixmap(pixmap);
}

void CountryWidget::setName(const QString& name)
{
    this->name = name;
    QString shortened = name;
    int max = 15;
    if (name.count() > max) {
        shortened = name.mid(0, max - 3) + "...";
    }
    ui->nameLabel->setText(shortened);
}

void CountryWidget::setDivisions(int divisions)
{
    this->divisions = divisions;
    ui->divisionsLabel->setText("Divisions: " + QString::number(divisions));

    searchValues.divCount = divisions;
    for (int i = 0; i < searchValues.divCount; ++i) {
        searchValues.div[i] = true;
    }
    for (int i = searchValues.divCount; i < 6; ++i) {
        searchValues.div[i] = false;
    }
}

void CountryWidget::setUsers(int users)
{
    this->users = users;
    ui->usersLabel->setText("Users: " + QString::number(users));
}

void CountryWidget::setID(int id)
{
    this->id = id;
    searchValues.countryid = id;
}

void CountryWidget::setInitialToolTip(const QString& tooltip)
{
    nameEn = tooltip;
    setToolTip(nameEn);
}

void CountryWidget::selectAsCountry(bool select)
{
    searchValues.countrySet = select;
    updateFrame();
}

void CountryWidget::selectAsNationality(bool select)
{
    searchValues.nationalitySet = select;
    updateFrame();
}

void CountryWidget::unselect()
{
    searchValues.countrySet = false;
    searchValues.nationalitySet = false;
    updateFrame();
}

bool CountryWidget::isSelected()
{
    return searchValues.countrySet || searchValues.nationalitySet;
}

bool CountryWidget::isCountrySelected()
{
    return searchValues.countrySet;
}

bool CountryWidget::isNationalitySelected()
{
    return searchValues.nationalitySet;
}


void CountryWidget::markFrame()
{
    setStyleSheet("#frame {border: 3px dashed yellow;}");
}

void CountryWidget::unmarkFrame()
{
    setStyleSheet("#frame {border: 1px solid grey;}");
}

void CountryWidget::updateFrame()
{
    int set = searchValues.countrySet +
              searchValues.nationalitySet;
    switch (set) {
        case 0:
            ui->frame->setStyleSheet("");
            setToolTip(nameEn);
            emit unselected(this);
            break;
        case 1:
            if (searchValues.countrySet) {
                ui->frame->setStyleSheet(
                        "#frame { border-color: rgb(4, 171, 147);"
                        "background-color: rgb(0, 255, 127);}");
                setToolTip("Program will search for players in this country");
            }
            else {
                ui->frame->setStyleSheet(
                        "#frame { border-color: rgb(1, 83, 164);"
                        "background-color: rgb(0, 239, 239);}");
                setToolTip("Program will search for players of this nationality");
            }
            emit selected(this);
            break;
        case 2:
            ui->frame->setStyleSheet(
                    "#frame { border-color: rgb(236, 27, 36);"
                    "background-color: rgb(255, 85, 127);}");
            setToolTip("Program will search for players in this country\n"
                       "and of this nationality");
            emit selected(this);
            break;
        default:
            break;
    }
}

void CountryWidget::mousePressEvent(QMouseEvent* event)
{
    searchDialog->setWindowTitle(nameEn);
    searchDialog->setValues(&searchValues);

    int ret = searchDialog->exec();
    if (ret == QDialog::Accepted) {
        searchDialog->updateValues();
        updateFrame();
    }

    QWidget::mousePressEvent(event);
}
