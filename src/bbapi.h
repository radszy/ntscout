#ifndef BBAPI_H
#define BBAPI_H

#include "player.h"

#include <QString>
#include <QList>

class Network;

struct LeagueData {
    int countryid;
    QList<int> divisions;
};

typedef QList<LeagueData> LeagueDataList;

class BBApi
{
public:
    BBApi();
    ~BBApi();

    bool login(const QString &login, const QString &password);
    bool countries();
    bool leagues(QList<int>& results, const LeagueDataList leagues);
    bool teams(QList<int>& results, QList<int> league);
    bool roster(PlayerList& results, QList<int> team);

private:
    Network* manager;
};

#endif // BBAPI_H
