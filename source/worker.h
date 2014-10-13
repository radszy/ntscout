#ifndef WORKER_H
#define WORKER_H

#include "bbapi.h"
#include "player.h"
#include "progresswidget.h"

#include <QThread>

class Network;

class Worker : public QThread
{
    Q_OBJECT
public:
    explicit Worker(QList<int> leagueList, ProgressWidget *parent = 0);

    void run();

public slots:
    PlayerList getPlayers();

signals:
    void finished(PlayerList);

private:
    QList<int> leagues;
    QList<int> teams;
    PlayerList players;

    ProgressWidget* parent;

};

#endif // WORKER_H
