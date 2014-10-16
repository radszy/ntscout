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
#include "settings.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QXmlStreamReader>

#include <QDomDocument>
#include <QDomNodeList>
#include <QDomElement>

#include <QDebug>

QString BBApi::name = QString();
QString BBApi::pass = QString();

BBApi::BBApi() :
    manager(new Network)
{
}

BBApi::BBApi(const QString& login, const QString& password) :
    manager(new Network)
{
    name = login;
    pass = password;
}

BBApi::~BBApi()
{
    delete manager;
}

QString BBApi::login(const QString& login, const QString& password)
{
    QUrl url("http://bbapi.buzzerbeater.com/login.aspx"
             "?login=" + login + "&code=" + password);
    QByteArray data = manager->get(url);

    QXmlStreamReader reader(data);
    reader.readNextStartElement();
    reader.readNextStartElement();
    if (reader.name() == "loggedIn") {
        return "";
    }
    else if (reader.name() == "error") {
        return reader.attributes().value("message").toString();
    }

    return "???";
}

QString BBApi::login()
{
    return login(name, pass);
}

bool BBApi::countries(CountryList& result)
{
    QUrl url("http://bbapi.buzzerbeater.com/countries.aspx");
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

    return true;
}

bool BBApi::leagues(QList<int>& results, const LeagueDataList leagues)
{
    QList <QUrl> urls;
    for (int i = 0; i < leagues.count(); ++i) {
        LeagueData data = leagues.at(i);
        for (int j = 0; j < data.divisions.count(); ++j) {
            QUrl url("http://bbapi.buzzerbeater.com/leagues.aspx"
                     "?countryid=" + QString::number(data.countryid) +
                     "&level=" + QString::number(data.divisions.at(j)));
            urls.append(url);
        }
    }

    QList<QByteArray> data = manager->get(urls);

    for (int i = 0; i < data.count(); ++i) {
        QDomDocument doc;
        doc.setContent(data.at(i));
        const auto& nodes = doc.elementsByTagName("league");

        for (int j = 0; j < nodes.count(); ++j) {
            int id = nodes.at(j).attributes()
                     .namedItem("id").toAttr().value().toInt();
            results.append(id);
        }
    }

    return true;
}

bool BBApi::teams(QList<int>& results, QList<int> league)
{
    QList <QUrl> urls;
    for (int i = 0; i < league.count(); ++i) {
        QUrl url("http://bbapi.buzzerbeater.com/standings.aspx?"
                 "leagueid=" + QString::number(league.at(i)));
        urls.append(url);
    }

    QList<QByteArray> data = manager->get(urls);

    for (int i = 0; i < data.count(); ++i) {
        QDomDocument doc;
        doc.setContent(data.at(i));
        const auto& nodes = doc.elementsByTagName("team");

        for (int j = 0; j < nodes.count(); ++j) {
            int id = nodes.at(j).attributes().namedItem("id")
                     .toAttr().value().toInt();
            if (Settings::searchBots == false) {
                bool isBot = nodes.at(j)
                             .firstChildElement("isBot")
                             .text().toInt() == 1;
                if (isBot == false) {
                    results.append(id);
                }
            }
            else {
                results.append(id);
            }
        }
    }

    return true;
}

bool BBApi::roster(PlayerList& results, QList<int> team)
{
    QList <QUrl> urls;
    for (int i = 0; i < team.count(); ++i) {
        QUrl url("http://bbapi.buzzerbeater.com/roster.aspx?"
                 "teamid=" + QString::number(team.at(i)));
        urls.append(url);
    }

    QList<QByteArray> data = manager->get(urls);


    for (int i = 0; i < data.count(); ++i) {
        QDomDocument doc;
        doc.setContent(data.at(i));
        const auto& nodes = doc.elementsByTagName("player");

        for (int j = 0; j < nodes.count(); ++j) {
            const auto& node = nodes.at(j);

            Player player;
            player.id = node.attributes().namedItem("id")
                     .toAttr().value().toInt();
            player.firstname = node.firstChildElement("firstName").text();
            player.lastname = node.firstChildElement("lastName").text();

            const auto& nationality = node.firstChildElement("nationality");
            player.nationalityname = nationality.text();
            player.nationalityid = nationality.attribute("id").toInt();

            player.age = node.firstChildElement("age").text().toInt();
            player.height = node.firstChildElement("height").text().toInt();
            player.dmi = node.firstChildElement("dmi").text().toInt();
            player.salary = node.firstChildElement("salary").text().toInt();
            player.bestpos = node.firstChildElement("bestPosition").text();
            player.potential = node.firstChildElement("skills")
                               .firstChildElement("potential").text().toInt();
            results.append(player);
        }
    }

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
