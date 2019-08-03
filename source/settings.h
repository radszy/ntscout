#ifndef SETTINGS_H
#define SETTINGS_H

#include <QPair>
#include <array>

enum Metrics {
	Centimeters,
	Meters,
	Inches,
	Feet,
};

class Settings {	
public:
	static constexpr int MAX_DIVISION_COUNT = 6;

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

	static std::array<bool, MAX_DIVISION_COUNT> checkedDivisions;
};

#endif // SETTINGS_H
