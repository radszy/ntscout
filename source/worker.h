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
