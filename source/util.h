#ifndef UTIL_H
#define UTIL_H

#include "country.h"

#include <QPoint>

class Util {
public:
	static bool readCountry(CountryList& countryList, QString& error);
	static bool writeCountry(CountryList& countryList);
	static bool newFile(const QString& path, const QByteArray& data);
	static void copyFolder(QString sourceFolder, QString destFolder);
	static QPoint screenCenter(int width, int height);
	static QString formatTime(int elapsed);
};

#endif // UTIL_H
