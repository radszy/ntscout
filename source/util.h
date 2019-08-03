#ifndef UTIL_H
#define UTIL_H

#include "country.h"

#include <QPoint>

class Util {
public:
	static bool readCountry(Countries& countries, QString& error);
	static bool writeCountry(const Countries& countries);
	static bool newFile(const QString& path, const QByteArray& data);
	static void copyFolder(const QString& sourceFolder, const QString& destFolder);
	static QPoint screenCenter(int width, int height);
	static QString formatTime(int elapsed);
};

#endif // UTIL_H
