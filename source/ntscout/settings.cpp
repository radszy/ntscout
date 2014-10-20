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

#include "settings.h"

#include <QThread>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QDebug>

quint8 Settings::tasks = QThread::idealThreadCount();
quint8 Settings::metrics = 0;
bool Settings::searchBots = false;
bool Settings::checkUpdates = false;

bool Settings::read()
{
    QFile file("data/settings.dat");
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream stream(&file);
    stream >> tasks;
    stream >> metrics;
    stream >> searchBots;
    stream >> checkUpdates;
    return true;
}

bool Settings::save()
{
    QFile file("data/settings.dat");
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream stream(&file);
    stream << tasks;
    stream << metrics;
    stream << searchBots;
    stream << checkUpdates;
    return true;
}
