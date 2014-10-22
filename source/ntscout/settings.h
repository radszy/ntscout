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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QPair>

enum Metrics {
    Centimeters,
    Meters,
    Inches,
    Feet
};

class Settings
{
public:
    static bool read();
    static bool save();

    static quint8 tasks;
    static quint8 metrics;
    static bool searchBots;

    static QPair <quint8, quint8> age;
    static QPair <quint8, quint8> pot;
    static QPair <quint32, quint32> sal;
    static QPair <quint32, quint32> dmi;

    static bool checkedDiv[6];

};

#endif // SETTINGS_H

