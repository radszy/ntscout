#ifndef COUNTRY_H
#define COUNTRY_H

#include <QString>

struct Country {
	QString name;
	QString name_en;
	int id;
	int divisions;
	int users;
};

using Countries = QList<Country>;

#endif // COUNTRY_H
