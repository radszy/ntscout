#include "worker.h"
#include "network.h"
#include "bbapi.h"

#include "progresswidget.h"

#include <QDebug>

Worker::Worker(QList<int> leagueList, ProgressWidget *parent) :
    QThread(parent)
{
    this->parent = parent;
    leagues.append(leagueList);

    qRegisterMetaType<PlayerList>("PlayerList");
}

void Worker::run()
{
    BBApi bb;
    bb.login();
    connect(bb.getNetwork(),SIGNAL(finished(QNetworkReply*)),
            parent,SLOT(requestDone()));
    bb.teams(teams, leagues);

    bb.roster(players, teams);

    emit finished(players);
}

PlayerList Worker::getPlayers()
{
    return players;
}
