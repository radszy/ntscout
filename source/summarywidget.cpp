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

SummaryWidget::SummaryWidget(QWidget* parent)
        : QWidget(parent)
        , ui(new Ui::SummaryWidget)
{
	ui->setupUi(this);
}

SummaryWidget::~SummaryWidget()
{
	delete ui;
}

void SummaryWidget::reset()
{
	ui->resultLabel->setText("");
	ui->fileLabel->setText("");
	ui->openFile->setEnabled(true);
	ui->openDirectory->setEnabled(true);
}

void SummaryWidget::setResults(const PlayerList& playerList)
{
	if (playerList.empty()) {
		ui->resultLabel->setText("No players found.");
		ui->openFile->setDisabled(true);
		ui->openDirectory->setDisabled(true);
		return;
	}

	QDir dir;
	if (!dir.exists("results")) {
		dir.mkdir("results");
	}

	QString date(QDateTime::currentDateTime().toString(Qt::ISODate));
	date.replace('T', '_');
	date.replace(':', '-');

	QFile file("results/" + date + ".csv");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(
		        this, "File Error", "Could not save results.", QMessageBox::Ok);
		return;
	}

	QString (SummaryWidget::*convert)(int) = nullptr;
	switch (Settings::metrics) {
		case Centimeters:
			convert = &SummaryWidget::toCentimeters;
			break;
		case Meters:
			convert = &SummaryWidget::toMeters;
			break;
		case Inches:
			convert = &SummaryWidget::toInches;
			break;
		case Feet:
			convert = &SummaryWidget::toFeet;
			break;
	}

	QTextStream stream(&file);
	stream << "teamID;ID;Name;Position;Age;Height;Salary;Potential;DMI;Nationality;Link\n";
	for (const Player& player : playerList) {
		stream << player.teamid << ";";
		stream << player.id << ";";
		stream << QString("%1 %2;").arg(player.firstname, player.lastname);
		stream << player.bestpos << ";";
		stream << player.age << ";";
		stream << ((this->*convert)(player.height)) << ";";
		stream << player.salary << ";";
		stream << player.potential << ";";
		stream << player.dmi << ";";
		stream << player.nationalityname << ";";
		stream << QString("http://www.buzzerbeater.com/player/%1/overview.aspx\n")
		                  .arg(player.id);
	}

	ui->resultLabel->setText(QString("Found %1 player%2.")
	                                 .arg(playerList.count())
	                                 .arg(playerList.count() != 1 ? "s" : ""));
	ui->fileLabel->setText(QString("%1.csv").arg(date));
}

void SummaryWidget::openFile()
{
	QFileInfo info("results/" + ui->fileLabel->text());
	QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteFilePath()));
}

void SummaryWidget::openDirectory()
{
	QFileInfo info("results/" + ui->fileLabel->text());
	QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
}

QString SummaryWidget::toCentimeters(int value)
{
	return QString::number(int(value * 2.54 + 0.5));
}

QString SummaryWidget::toMeters(int value)
{
	QString result = QString::number(int(value * 2.54 + 0.5) / 100.0);
	if (result.count() == 3) {
		result.append("0");
	}
	return result;
}

QString SummaryWidget::toInches(int value)
{
	return QString::number(value);
}

QString SummaryWidget::toFeet(int value)
{
	return QString("%1'%2\"").arg(value / 12).arg(value % 12);
}
