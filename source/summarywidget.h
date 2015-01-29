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

#ifndef SUMMARYWIDGET_H
#define SUMMARYWIDGET_H

#include "player.h"

#include <QWidget>

namespace Ui {
class SummaryWidget;
}

class SummaryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SummaryWidget(QWidget *parent = 0);
    ~SummaryWidget();

    void reset();
    void setResults(const PlayerList& playerList);

public slots:
    void openFile();
    void openDirectory();

private:
    QString toCentimeters(int value);
    QString toMeters(int value);
    QString toInches(int value);
    QString toFeet(int value);

    Ui::SummaryWidget *ui;
};

#endif // SUMMARYWIDGET_H
