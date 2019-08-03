#ifndef COUNTRY_H
#define COUNTRY_H

#include <QString>

struct Country
{
    QString name;
    QString name_en;
    int id;
    int divisions;
    int users;
};

typedef QList<Country> CountryList;

#endif // COUNTRY_H
