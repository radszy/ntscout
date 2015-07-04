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

#ifndef WORKER_H
#define WORKER_H

#include "player.h"

#include <QThread>

class Network;
class ProgressWidget;

class Worker : public QThread
{
    Q_OBJECT
public:
    explicit Worker(QList<int> leagueList, ProgressWidget *parent = 0);

    void run();
    bool isDone() {return done;}

public slots:
    PlayerList getPlayers();

signals:
    void foundTeams(int);
    void finished(PlayerList);

private:
    QList<int> leagues;
    QList<int> teams;
    PlayerList players;

    ProgressWidget* parent;
    bool done;
};

#endif // WORKER_H
