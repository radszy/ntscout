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

#include "bbapi.h"
#include "network.h"
#include "settings.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

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

    QDomDocument doc;
    doc.setContent(data);
    const QDomNodeList& error = doc.elementsByTagName("error");
    if (!error.isEmpty()) {
        return error.item(0).attributes()
                .namedItem("message")
                .toAttr().value();
    }

    const QDomNodeList& loggedIn = doc.elementsByTagName("loggedIn");
    if (!loggedIn.isEmpty()) {
        return "";
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

    QDomDocument doc;
    doc.setContent(data);
    const auto& nodes = doc.elementsByTagName("country");

    for (int i = 0; i < nodes.count(); ++i) {
        const auto& node = nodes.at(i);
        const auto& attr = node.attributes();

        Country country;
        country.id = attr.namedItem("id")
                     .toAttr().value().toInt();
        country.divisions = attr.namedItem("divisions")
                            .toAttr().value().toInt();
        country.users = attr.namedItem("users")
                        .toAttr().value().toInt();
        country.name = node.toElement().text();
        result.append(country);
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
        int teamid = doc.elementsByTagName("roster")
                     .at(0).attributes().namedItem("teamid")
                     .toAttr().value().toInt();

        const auto& nodes = doc.elementsByTagName("player");

        for (int j = 0; j < nodes.count(); ++j) {
            const auto& node = nodes.at(j);

            Player player;
            player.teamid = teamid;
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
