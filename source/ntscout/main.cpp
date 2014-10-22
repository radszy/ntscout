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

#include "mainwindow.h"
#include "settings.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList args;
    for (int i = 0; i < argc; ++i) {
        args.append(argv[i]);
    }

    QCommandLineParser parser;
    QCommandLineOption postUpdateOption("post-update");
    parser.addOption(postUpdateOption);
    parser.parse(args);
    bool isPostUpdate = parser.isSet("post-update");
    if (isPostUpdate) {
        QThread::msleep(100);
        QFile oldu("Updater"), newu("Updater-new");
        if (oldu.exists() && newu.exists()) {
            oldu.remove();
            newu.rename("Updater");
        }
    }

    Settings::read();

    MainWindow w;
    w.show();

    return a.exec();
}

