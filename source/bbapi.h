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

#ifndef BBAPI_H
#define BBAPI_H

#include "country.h"
#include "player.h"

#include <QObject>
#include <QString>
#include <QList>

class Network;
class QNetworkReply;

struct LeagueData {
    int countryid;
    QList<int> divisions;
};

typedef QList<LeagueData> LeagueDataList;

class BBApi : public QObject
{
    Q_OBJECT

public:
    BBApi();
    BBApi(const QString &login, const QString &password);
    ~BBApi();

    QString login(const QString &login, const QString &password);
    QString login();
    bool countries(CountryList& result);
    bool leagues(QList<int>& results, const LeagueDataList leagues);
    bool teams(QList<int>& results, QList<int> league);
    bool roster(PlayerList& results, QList<int> team);
    bool translatedNames(CountryList& list);
    bool releases(QString &tag, QString &download);
    QNetworkReply* downloadRelease(const QString& url);
    QByteArray downloadFlag(int id);

    Network* getNetwork() {return manager;}

    static QString getName() {return name;}
    static QString getPass() {return pass;}

private:
    Network* manager;

    static QString name;
    static QString pass;
};

#endif // BBAPI_H

