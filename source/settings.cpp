#include "settings.h"

#include <QThread>
#include <QFile>
#include <QDataStream>
#include <QDebug>

quint8 Settings::tasks = QThread::idealThreadCount();
quint8 Settings::metrics = 0;
bool Settings::searchBots = false;
bool Settings::createLogs = false;
QPair<quint8, quint8> Settings::age = qMakePair(18, 99);
QPair<quint8, quint8> Settings::pot = qMakePair(0, 11);
QPair<quint32, quint32> Settings::sal = qMakePair(0, 999999);
QPair<quint32, quint32> Settings::dmi = qMakePair(0, 99999999);
std::array<bool, Settings::MAX_DIVISION_COUNT> Settings::checkedDivisions = {true};

bool Settings::read()
{
	QFile file("data/settings.dat");
	if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
		return false;
	}

	QDataStream stream(&file);
	stream >> age >> pot >> sal >> dmi >> tasks >> metrics >> searchBots >> createLogs;

	for (auto division : checkedDivisions) {
		stream >> division;
	}

	return true;
}

bool Settings::save()
{
	QFile file("data/settings.dat");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return false;
	}

	QDataStream stream(&file);
	stream << age << pot << sal << dmi << tasks << metrics << searchBots << createLogs;

	for (auto division : checkedDivisions) {
		stream << division;
	}

	return true;
}
