#include "util.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QGuiApplication>
#include <QScreen>
#include <QDir>
#include <QtMath>
#include <QDebug>

bool Util::readCountry(CountryList& countryList, QString& error)
{
	QFile file("data/country.dat");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		error = "couldn't open country.dat";
		return false;
	}
	QTextStream stream(&file);
	QString input;
	while (!(input = stream.readLine()).isEmpty()) {
		QStringList list = input.split(",");
		Country country;
		country.id = list.at(0).toInt();
		country.name = list.at(1);
		country.name_en = list.at(2);
		country.divisions = list.at(3).toInt();
		country.users = list.at(4).toInt();
		countryList.append(country);
	}

	if (countryList.isEmpty() || countryList.first().id != 1) {
		error = "data seems corrupted";
		return false;
	}

	return true;
}

bool Util::writeCountry(CountryList& countryList)
{
	QFile file("data/country.dat");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}
	QTextStream stream(&file);
	for (const Country& country : countryList) {
		stream << country.id << "," << country.name << "," << country.name_en << ","
		       << country.divisions << "," << country.users << "\n";
	}
	return true;
}

bool Util::newFile(const QString& path, const QByteArray& data)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}

	file.write(data);
	return true;
}

void Util::copyFolder(const QString& sourceFolder, const QString& destFolder)
{
	QDir sourceDir(sourceFolder);
	if (!sourceDir.exists()) {
		return;
	}

	QDir destDir;
	if (!destDir.exists(destFolder)) {
		destDir.mkdir(destFolder);
	}

	QStringList files = sourceDir.entryList(QDir::Files);
	for (const QString& file : files) {
		QString srcName = sourceFolder + QDir::separator() + file;
		QString destName = destFolder + QDir::separator() + file;
		QFile::copy(srcName, destName);
	}

	files.clear();
	files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	for (const QString& file : files) {
		QString srcName = sourceFolder + QDir::separator() + file;
		QString destName = destFolder + QDir::separator() + file;
		copyFolder(srcName, destName);
	}
}

QPoint Util::screenCenter(int width, int height)
{
	QRect screen = QGuiApplication::screens().first()->geometry();
	return QPoint(screen.width() / 2 - width / 2, screen.height() / 2 - height / 2);
}

QString Util::formatTime(int elapsed)
{
	int total = elapsed / 1000;
	int hours = qFloor(total / 3600);
	int minutes = floor((total / 60) % 60);
	int seconds = total % 60;

	return QString("%1:%2:%3")
	        .arg(hours, 2, 10, QChar('0'))
	        .arg(minutes, 2, 10, QChar('0'))
	        .arg(seconds, 2, 10, QChar('0'));
}
