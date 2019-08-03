#include "worker.h"
#include "network.h"
#include "bbapi.h"

#include "progresswidget.h"

#include <QDebug>

Worker::Worker(QList<int> leagueList, ProgressWidget* parent)
        : QThread(parent)
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
	connect(bb.getNetwork(), SIGNAL(finished(QNetworkReply*)), parent, SLOT(requestDone()));

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
