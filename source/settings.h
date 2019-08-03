#ifndef SETTINGS_H
#define SETTINGS_H

#include <QPair>

#define DivisionCount 6

enum Metrics {
	Centimeters,
	Meters,
	Inches,
	Feet,
};

class Settings {
public:
	static bool read();
	static bool save();

	static quint8 tasks;
	static quint8 metrics;
	static bool searchBots;
	static bool createLogs;

	static QPair<quint8, quint8> age;
	static QPair<quint8, quint8> pot;
	static QPair<quint32, quint32> sal;
	static QPair<quint32, quint32> dmi;

	static bool checkedDivisions[DivisionCount];
};

#endif // SETTINGS_H
