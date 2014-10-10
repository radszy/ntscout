#ifndef BBAPI_H
#define BBAPI_H

#include "country.h"
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

    QString login(const QString &login, const QString &password);
    bool countries(CountryList& result);
    bool leagues(QList<int>& results, const LeagueDataList leagues);
    bool teams(QList<int>& results, QList<int> league);
    bool roster(PlayerList& results, QList<int> team);

    void namesEn(CountryList& countries);

private:
    Network* manager;
};

#endif // BBAPI_H
