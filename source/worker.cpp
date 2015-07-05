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

#include "worker.h"
#include "network.h"
#include "bbapi.h"

#include "progresswidget.h"

#include <QDebug>

Worker::Worker(QList<int> leagueList, ProgressWidget *parent) :
    QThread(parent)
{
    done = false;
    this->parent = parent;
    leagues.append(leagueList);

    qRegisterMetaType<PlayerList>("PlayerList");
}

void Worker::run()
{
    BBApi bb;
    bb.login();
    connect(bb.getNetwork(),SIGNAL(finished(QNetworkReply*)),
            parent, SLOT(requestDone()));

    bb.teams(teams, leagues);
    if (teams.count() == 0) {
        done = true;
        emit foundTeams(0);
        emit finished(players);
        return;
    }

    emit foundTeams(teams.count());
    bb.roster(players, teams);

    done = true;
    emit finished(players);
}

PlayerList Worker::getPlayers()
{
    return players;
}
