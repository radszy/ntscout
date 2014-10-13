#include "summarywidget.h"
#include "ui_summarywidget.h"

#include "settings.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QMessageBox>
#include <QDateTime>
#include <QTextStream>
#include <QDesktopServices>

SummaryWidget::SummaryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SummaryWidget)
{
    ui->setupUi(this);
}

SummaryWidget::~SummaryWidget()
{
    delete ui;
}

void SummaryWidget::reset()
{
    ui->resultsLabel->setText("");
    ui->fileLabel->setText("");
    ui->openFile->setEnabled(true);
    ui->openDirectory->setEnabled(true);
}

void SummaryWidget::setResults(PlayerList p)
{
    if (p.empty()) {
        ui->resultsLabel->setText("No players found.");
        ui->openFile->setDisabled(true);
        ui->openDirectory->setDisabled(true);
        return;
    }

    QString date (QDateTime::currentDateTime().toString(Qt::ISODate));
    date.replace('T', ' ');
    date.replace(':', '-');

    QDir dir;
    if (!dir.exists("results")) {
        dir.mkdir("results");
    }

    QFile file("results/" + date + ".csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, "File Error", "Could not save results.", QMessageBox::Ok);
        return;
    }

    QTextStream f(&file);
    f << "id;Name;Position;Age;Height;Salary;Potential;DMI;Nationality;Link\n";

    double metrics = 1, rounding = 0;
    if (Settings::metrics == 0) {
        metrics = 2.54;
        rounding = 0.5;
    }

    for (int i = 0; i < p.count(); i++) {
        f << p.at(i).id << ";";
        f << p.at(i).firstname + " " + p.at(i).lastname << ";";
        f << p.at(i).bestpos << ";";
        f << p.at(i).age << ";";
        f << int(p.at(i).height * metrics + rounding) << ";";
        f << p.at(i).salary << ";";
        f << p.at(i).potential << ";";
        f << p.at(i).dmi << ";";
        f << p.at(i).nationalityname << ";";
        f << "http://www.buzzerbeater.com/player/" + QString::number(p.at(i).id) + "/overview.aspx\n";
    }

    ui->resultsLabel->setText(QString("Found ") +
                              QString::number(p.count()) +
                              QString(" players."));
    ui->fileLabel->setText(QString(date + ".csv"));
}

void SummaryWidget::openFile()
{
    QFileInfo info("results/" + ui->fileLabel->text());
    QDesktopServices::openUrl(QUrl("file://" + info.absoluteFilePath()));
}

void SummaryWidget::openDirectory()
{
    QFileInfo info("results/" + ui->fileLabel->text());
    QDesktopServices::openUrl(QUrl("file://" + info.absolutePath()));
}
