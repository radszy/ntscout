#include "summarywidget.h"
#include "ui_summarywidget.h"

#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>
#include <QTextStream>

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

void SummaryWidget::setResults(PlayerList p)
{
    if (p.empty()) {
        ui->summaryLabel->setText("No players found.");
        ui->openButton->setDisabled(true);
        return;
    }

    QString date (QDateTime::currentDateTime().toString(Qt::ISODate));
    date.replace('T', ' ');
    date.replace(':', '-');

    QDir dir;
    if (!dir.exists("results"))
        dir.mkdir("results");

    QFile file("results/" + date + ".csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, "File Error", "Could not save players.", QMessageBox::Ok);
        return;
    }

    QTextStream f(&file);
    f << "id;Name;Position;Age;Height;Salary;Potential;DMI;Nationality;Link\n";

    for (int i = 0; i < p.count(); i++) {
        f << p.at(i).id << ";";
        f << p.at(i).firstname + " " + p.at(i).lastname << ";";
        f << p.at(i).bestpos << ";";
        f << p.at(i).age << ";";
        f << int(p.at(i).height * 2.54 + 0.5) << ";";
        f << p.at(i).salary << ";";
        f << p.at(i).potential << ";";
        f << p.at(i).dmi << ";";
        f << p.at(i).nationalityname << ";";
        f << "http://www.buzzerbeater.com/player/" + QString::number(p.at(i).id) + "/overview.aspx\n";
    }

    ui->summaryLabel->setText(QString("Found ") +
                              QString::number(p.count()) +
                              QString("players.") +
                              QString("Results have been saved in ") +
                              QString("results/" + date + ".csv"));
}
