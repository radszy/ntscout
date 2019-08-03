#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::apply()
{
    Settings::tasks = ui->tasks->value();
    Settings::metrics = ui->metrics->currentIndex();
    Settings::searchBots = ui->bots->isChecked();
}

int SettingsDialog::exec()
{
    ui->tasks->setValue(Settings::tasks);
    ui->metrics->setCurrentIndex(Settings::metrics);
    ui->bots->setChecked(Settings::searchBots);

    return QDialog::exec();
}
