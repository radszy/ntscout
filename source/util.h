//Copyright (C) <2015>  <RSX>

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef UTIL_H
#define UTIL_H

#include "country.h"

#include <QPoint>

class Util
{
public:
    static bool readCountry(CountryList& countryList, QString& error);
    static bool writeCountry(CountryList& countryList);
    static bool newFile(const QString& path, const QByteArray& data);
    static void copyFolder(QString sourceFolder, QString destFolder);
    static QPoint screenCenter(int width, int height);
    static QString formatTime(int elapsed);
};

#endif // UTIL_H
