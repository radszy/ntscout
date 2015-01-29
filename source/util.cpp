//Copyright (C) <2014>  <RSX>

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

#include "util.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDesktopWidget>

bool Util::readCountry(CountryList& countryList)
{
    QFile file("data/country.dat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
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
    return true;
}

bool Util::writeCountry(CountryList& countryList)
{
    QFile file("data/country.dat");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream stream(&file);
    for (const auto& country : countryList) {
        stream << country.id << ","
            << country.name << ","
            << country.name_en << ","
            << country.divisions << ","
            << country.users << "\n";
    }
    return true;
}

QPoint Util::screenCenter(int width, int height)
{
    QDesktopWidget desktop;
    QRect screen = desktop.screenGeometry();
    return QPoint(screen.width() / 2 - width / 2,
                  screen.height() / 2 - height / 2);
}
