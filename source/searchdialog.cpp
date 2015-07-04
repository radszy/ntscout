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

#include "searchdialog.h"
#include "ui_searchdialog.h"

#include "searchvalues.h"
#include "settings.h"

#include <QDebug>

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog)
{
    ui->setupUi(this);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::updateValues()
{
    searchValues->countrySet = ui->countryGroup->isChecked();
    searchValues->div[0] = ui->div1Check->isChecked();
    searchValues->div[1] = ui->div2Check->isChecked();
    searchValues->div[2] = ui->div3Check->isChecked();
    searchValues->div[3] = ui->div4Check->isChecked();
    searchValues->div[4] = ui->div5Check->isChecked();
    searchValues->div[5] = ui->div6Check->isChecked();

    searchValues->nationalitySet = ui->nationalityGroup->isChecked();
    searchValues->age = qMakePair(ui->ageMin->value(), ui->ageMax->value());
    searchValues->salary = qMakePair(ui->salMin->value(), ui->salMax->value());
    searchValues->potential = qMakePair(ui->potMin->value(), ui->potMax->value());
    searchValues->dmi = qMakePair(ui->dmiMin->value(), ui->dmiMax->value());

    if (ui->saveDefaultCountry->isChecked()) {
        Settings::checkedDivisions[0] = ui->div1Check->isChecked() &&
                                        !ui->div1Check->isHidden();
        Settings::checkedDivisions[1] = ui->div2Check->isChecked() &&
                                        !ui->div2Check->isHidden();
        Settings::checkedDivisions[2] = ui->div3Check->isChecked() &&
                                        !ui->div3Check->isHidden();
        Settings::checkedDivisions[3] = ui->div4Check->isChecked() &&
                                        !ui->div4Check->isHidden();
        Settings::checkedDivisions[4] = ui->div5Check->isChecked() &&
                                        !ui->div5Check->isHidden();
        Settings::checkedDivisions[5] = ui->div6Check->isChecked() &&
                                        !ui->div6Check->isHidden();
        emit updateDefaultValues();

    }
    if (ui->saveDefaultNationality->isChecked()) {
        Settings::age = qMakePair((quint8)ui->ageMin->value(), (quint8)ui->ageMax->value());
        Settings::pot = qMakePair((quint8)ui->potMin->value(), (quint8)ui->potMax->value());
        Settings::sal = qMakePair((quint32)ui->salMin->value(), (quint32)ui->salMax->value());
        Settings::dmi = qMakePair((quint32)ui->dmiMin->value(), (quint32)ui->dmiMax->value());
        emit updateDefaultValues();
    }
}

void SearchDialog::setValues(SearchValues* values)
{
    searchValues = values;

    ui->div1Check->setChecked(values->div[0]);
    ui->div2Check->setChecked(values->div[1]);

    ui->div3Check->setChecked(values->div[2]);
    ui->div3Check->setVisible(values->divCount > 2);

    ui->div4Check->setChecked(values->div[3]);
    ui->div4Check->setVisible(values->divCount > 3);

    ui->div5Check->setChecked(values->div[4]);
    ui->div5Check->setVisible(values->divCount > 4);

    ui->div6Check->setChecked(values->div[5]);
    ui->div6Check->setVisible(values->divCount > 5);

    ui->countryGroup->setChecked(values->countrySet);
    ui->nationalityGroup->setChecked(values->nationalitySet);

    ui->ageMin->setValue(values->age.first);
    ui->ageMax->setValue(values->age.second);
    ui->potMin->setValue(values->potential.first);
    ui->potMax->setValue(values->potential.second);
    ui->salMin->setValue(values->salary.first);
    ui->salMax->setValue(values->salary.second);
    ui->dmiMin->setValue(values->dmi.first);
    ui->dmiMax->setValue(values->dmi.second);

    ui->saveDefaultCountry->setChecked(false);
    ui->saveDefaultNationality->setChecked(false);
}

