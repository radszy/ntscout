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

#include "bbapi.h"
#include "network.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QXmlStreamReader>

#include <QDebug>

BBApi::BBApi() :
    manager(new Network)
{
}

BBApi::~BBApi()
{
    delete manager;
}

QString BBApi::login(const QString& login, const QString& password)
{
    QUrl url = "http://bbapi.buzzerbeater.com/login.aspx"
               "?login=" + login + "&code=" + password;
    QByteArray data = manager->get(url);

    QXmlStreamReader reader(data);
    reader.readNextStartElement();
    reader.readNextStartElement();
    if (reader.name() == "loggedIn") {
//        qDebug() << "loggedin";
        return "";
    }
    else if (reader.name() == "error") {
        return reader.attributes().value("message").toString();
    }

    return "???";
}

bool BBApi::countries(CountryList& result)
{
    QUrl url ("http://bbapi.buzzerbeater.com/countries.aspx");
    QByteArray data = manager->get(url);

    QXmlStreamReader reader(data);
    reader.readNextStartElement();
    reader.readNextStartElement();
    reader.readNextStartElement();

    while (reader.name() == "country") {
        Country country;
        country.id = reader.attributes().value("id").toInt();
        country.divisions = reader.attributes().value("divisions").toInt();
        country.users = reader.attributes().value("users").toInt();
        country.name = reader.readElementText();
        result.append(country);
        reader.readNextStartElement();
    }

    qDebug() << result.count();

    return true;
}

bool BBApi::leagues(QList<int>& results, const LeagueDataList leagues)
{
    QList <QUrl> urls;
    for (int i = 0; i < leagues.count(); ++i) {
        LeagueData data = leagues.at(i);
        for (int j = 0; j < data.divisions.count(); ++j) {
            QUrl url = "http://bbapi.buzzerbeater.com/leagues.aspx"
                       "?countryid=" + QString::number(data.countryid) +
                       "&level=" + QString::number(data.divisions.at(j));
            urls.append(url);
        }
    }

    QList<QByteArray> data = manager->get(urls);

    for (int i = 0; i < data.count(); ++i) {
        QXmlStreamReader reader(data.at(i));
        reader.readNextStartElement();
        reader.readNextStartElement();
        if (reader.name() == "error") {
            return false;
        }

        while (reader.name() == "league") {
            int id = reader.attributes().value("id").toInt();
            results.append(id);

            reader.readNextStartElement();
        }
    }

    return true;
}

bool BBApi::teams(QList<int>& results, QList<int> league)
{
    QList <QUrl> urls;
    for (int i = 0; i < league.count(); ++i) {
        QUrl url = "http://bbapi.buzzerbeater.com/standings.aspx?"
                   "leagueid=" + QString::number(league.at(i));
        urls.append(url);
    }

    QList<QByteArray> data = manager->get(urls);

    for (int i = 0; i < data.count(); ++i) {
        QXmlStreamReader reader(data.at(i));
        reader.readNextStartElement();
        reader.readNextStartElement();
        if (reader.name() == "error") {
            return false;
        }

        reader.readNextStartElement();
        reader.readNextStartElement();
        reader.readNextStartElement();
        reader.readNextStartElement();
        reader.readNextStartElement();
        reader.readNextStartElement();
        reader.readNextStartElement();

        while (reader.name() == "team") {
            int id = reader.attributes().value("id").toInt();
            results.append(id);

            reader.skipCurrentElement();
            reader.readNextStartElement();
        }

        reader.readNextStartElement();
        reader.readNextStartElement();
        while (reader.name() == "team") {
            int id = reader.attributes().value("id").toInt();
            results.append(id);

            reader.skipCurrentElement();
            reader.readNextStartElement();
        }
    }

//    Q_ASSERT(results.count() == 16);

    return true;
}

bool BBApi::roster(PlayerList& results, QList<int> team)
{
    QList <QUrl> urls;
    for (int i = 0; i < team.count(); ++i) {
        QUrl url = "http://bbapi.buzzerbeater.com/roster.aspx?"
                   "teamid=" + QString::number(team.at(i));
        urls.append(url);
    }

    QList<QByteArray> data = manager->get(urls);

    for (int i = 0; i < data.count(); ++i) {
        QXmlStreamReader reader(data.at(i));
        reader.readNextStartElement();
        reader.readNextStartElement();
        if (reader.name() == "error") {
            return false;
        }

        reader.readNextStartElement();

        while (reader.name() == "player") {

            Player player;

            player.id = reader.attributes().value("id").toInt();
//            qDebug() << reader.attributes().value("id");

            reader.readNextStartElement();
            player.firstname = reader.readElementText();
//            qDebug() << "firstname:" << reader.readElementText();

            reader.readNextStartElement();
            player.lastname = reader.readElementText();
//            qDebug() << "lastname:" << reader.readElementText();

            reader.readNextStartElement();
            player.nationalityid = reader.attributes().value("id").toInt();
            player.nationalityname = reader.readElementText();
//            qDebug() << "nationalityid:" << reader.attributes().value("id");

            reader.readNextStartElement();
            player.age = reader.readElementText().toInt();
//            qDebug() << "age:" << reader.readElementText();

            reader.readNextStartElement();
            player.height = reader.readElementText().toInt();
//            qDebug() << "height:" << reader.readElementText();

            reader.readNextStartElement();
            player.dmi = reader.readElementText().toInt();
//            qDebug() << "dmi:" << reader.readElementText();

            reader.readNextStartElement();
            player.salary = reader.readElementText().toInt();
//            qDebug() << "salary:" << reader.readElementText();

            reader.readNextStartElement();
            player.bestpos = reader.readElementText();
//            qDebug() << "bestpos:" << reader.readElementText();

            reader.readNextStartElement();
            reader.readNextStartElement();
            player.gameshape = reader.readElementText().toInt();
//            qDebug() << "gameshape:" << reader.readElementText();

            reader.readNextStartElement();
            player.potential = reader.readElementText().toInt();
//            qDebug() << "potential:" << reader.readElementText();

            reader.skipCurrentElement();
            reader.skipCurrentElement();
            reader.readNextStartElement();

            results.append(player);
        }
    }

    qDebug() << results.count();

    return true;
}

void BBApi::namesEn(CountryList& countries)
{
    QUrl url("https://raw.githubusercontent.com/"
             "rsxee/NTScout/master/names-en.txt");
    QByteArray data = manager->get(url);

    QList<QByteArray> datalist = data.split('\n');
    for (int i = 0; i < countries.count(); ++i) {
        countries[i].name_en = datalist.at(i);
    }

    // Q_ASSERT(names.count() < datalist.count());
}
