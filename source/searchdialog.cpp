#include "searchdialog.h"
#include "ui_searchdialog.h"

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

SearchValues SearchDialog::getValues()
{
    SearchValues values;
    values.countrySet = ui->countryGroup->isChecked();
    values.div[0] = ui->div1Check->isChecked();
    values.div[1] = ui->div2Check->isChecked();
    values.div[2] = ui->div3Check->isChecked();
    values.div[3] = ui->div4Check->isChecked();
    values.div[4] = ui->div5Check->isChecked();
    values.div[5] = ui->div6Check->isChecked();

    values.nationalitySet = ui->nationalityGroup->isChecked();
    values.age = {ui->ageMin->value(), ui->ageMax->value()};
    values.salary = {ui->salMin->value(), ui->salMax->value()};
    values.potential = {ui->potMin->value(), ui->potMax->value()};
    values.dmi = {ui->dmiMin->value(), ui->dmiMax->value()};
    return values;
}

void SearchDialog::setValues(SearchValues values)
{
    ui->countryGroup->setChecked(values.countrySet);
    ui->div1Check->setChecked(values.div[0]);
    ui->div2Check->setChecked(values.div[1]);
    ui->div3Check->setChecked(values.div[2]);
    ui->div4Check->setChecked(values.div[3]);
    ui->div5Check->setChecked(values.div[4]);
    ui->div6Check->setChecked(values.div[5]);

    ui->nationalityGroup->setChecked(values.nationalitySet);
    ui->ageMin->setValue(values.age.first);
    ui->ageMax->setValue(values.age.second);
    ui->salMax->setValue(values.salary.first);
    ui->salMax->setValue(values.salary.second);
    ui->potMin->setValue(values.potential.first);
    ui->potMax->setValue(values.potential.second);
    ui->dmiMin->setValue(values.dmi.first);
    ui->dmiMax->setValue(values.dmi.second);
}
