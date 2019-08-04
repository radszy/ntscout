#include "bbapi.h"
#include "network.h"
#include "settings.h"
#include "updatewidget.h"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

#include <QDomDocument>
#include <QDomNodeList>
#include <QDomElement>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QDebug>

QString BBApi::mName = QString();
QString BBApi::mPass = QString();

BBApi::BBApi()
        : mNetwork(new Network)
{
}

BBApi::BBApi(const QString& login, const QString& password)
        : mNetwork(new Network)
{
	mName = login;
	mPass = password;
}

BBApi::~BBApi()
{
	delete mNetwork;
}

QString BBApi::login(const QString& login, const QString& password)
{
	QUrl url("http://bbapi.buzzerbeater.com/login.aspx?login=" + login + "&code=" + password);
	QByteArray data = mNetwork->get(url);

	QDomDocument doc;
	doc.setContent(data);
	const QDomNodeList& error = doc.elementsByTagName("error");
	if (!error.isEmpty()) {
		return error.item(0).attributes().namedItem("message").toAttr().value();
	}

	const QDomNodeList& loggedIn = doc.elementsByTagName("loggedIn");
	if (!loggedIn.isEmpty()) {
		return "";
	}

	return "Unrecognized error";
}

QString BBApi::login()
{
	return login(mName, mPass);
}

bool BBApi::countries(Countries& result)
{
	QUrl url("http://bbapi.buzzerbeater.com/countries.aspx");
	QByteArray data = mNetwork->get(url);

	QDomDocument doc;
	doc.setContent(data);
	const QDomNodeList& nodes = doc.elementsByTagName("country");

	for (int i = 0; i < nodes.count(); ++i) {
		const QDomNode& node = nodes.at(i);
		const QDomNamedNodeMap& attr = node.attributes();

		Country country;
		country.id = attr.namedItem("id").toAttr().value().toInt();
		country.divisions = attr.namedItem("divisions").toAttr().value().toInt();
		country.users = attr.namedItem("users").toAttr().value().toInt();
		country.name = node.toElement().text();
		result.append(country);
	}

	return true;
}

bool BBApi::leagues(Ids& results, const Leagues& leagues)
{
	QList<QUrl> urls;

	for (const League& league : leagues) {
		for (const Id divisionId : league.divisionIds) {
			QUrl url("http://bbapi.buzzerbeater.com/leagues.aspx?countryid=" +
			         QString::number(league.countryId) +
			         "&level=" + QString::number(divisionId));
			urls.append(url);
		}
	}

	QList<QByteArray> data = mNetwork->get(urls);

	for (const QByteArray& content : data) {
		QDomDocument doc;
		doc.setContent(content);
		const QDomNodeList& nodes = doc.elementsByTagName("league");

		for (int j = 0; j < nodes.count(); ++j) {
			Id leagueId =
			        nodes.at(j).attributes().namedItem("id").toAttr().value().toInt();
			results.append(leagueId);
		}
	}

	return true;
}

bool BBApi::teams(Ids& results, const Ids& leaguesIds)
{
	QList<QUrl> urls;

	for (Id leagueId : leaguesIds) {
		QUrl url("http://bbapi.buzzerbeater.com/standings.aspx?leagueid=" +
		         QString::number(leagueId));
		urls.append(url);
	}

	QList<QByteArray> data = mNetwork->get(urls);

	for (const QByteArray& content : data) {
		QDomDocument doc;
		doc.setContent(content);
		const QDomNodeList& nodes = doc.elementsByTagName("team");

		for (int j = 0; j < nodes.count(); ++j) {
			Id teamId =
			        nodes.at(j).attributes().namedItem("id").toAttr().value().toInt();

			if (!Settings::searchBots) {
				bool isBot =
				        nodes.at(j).firstChildElement("isBot").text().toInt() == 1;

				if (!isBot) {
					results.append(teamId);
				}
			}
			else {
				results.append(teamId);
			}
		}
	}

	return true;
}

bool BBApi::roster(PlayerList& results, const Ids& teamIds)
{
	QList<QUrl> urls;

	for (Id teamId : teamIds) {
		QUrl url("http://bbapi.buzzerbeater.com/roster.aspx?teamid=" +
		         QString::number(teamId));
		urls.append(url);
	}

	QList<QByteArray> data = mNetwork->get(urls);

	for (const QByteArray& content : data) {
		QDomDocument doc;
		doc.setContent(content);
		Id teamId = doc.elementsByTagName("roster")
		                    .at(0)
		                    .attributes()
		                    .namedItem("teamid")
		                    .toAttr()
		                    .value()
		                    .toInt();

		const QDomNodeList& nodes = doc.elementsByTagName("player");

		for (int j = 0; j < nodes.count(); ++j) {
			const QDomNode& node = nodes.at(j);

			Player player;
			player.teamId = teamId;
			player.id = node.attributes().namedItem("id").toAttr().value().toInt();
			player.firstname = node.firstChildElement("firstName").text();
			player.lastname = node.firstChildElement("lastName").text();

			const auto& nationality = node.firstChildElement("nationality");
			player.nationalityName = nationality.text();
			player.nationalityId = nationality.attribute("id").toInt();

			player.age = node.firstChildElement("age").text().toInt();
			player.height = node.firstChildElement("height").text().toInt();
			player.dmi = node.firstChildElement("dmi").text().toInt();
			player.salary = node.firstChildElement("salary").text().toInt();
			player.bestPos = node.firstChildElement("bestPosition").text();
			player.potential = node.firstChildElement("skills")
			                           .firstChildElement("potential")
			                           .text()
			                           .toInt();
			results.append(player);
		}
	}

	return true;
}

// --- BBAPI UNRELATED ---
bool BBApi::translatedNames(Countries& countries)
{
	QUrl url("https://raw.githubusercontent.com/"
	         "rszymanski/ntscout/master/names-en.txt");
	QByteArray data = mNetwork->get(url);

	const QList<QByteArray> names = data.split('\n');
	const int len = qMin(countries.count(), names.count());

	for (int i = 0; i < len; ++i) {
		countries[i].name_en = names[i];
	}

	return true;
}

bool BBApi::releases(QString& tag, QString& download)
{
	QUrl url("https://api.github.com/repos/rszymanski/ntscout/releases");
	QByteArray data = mNetwork->get(url);

	QJsonDocument doc = QJsonDocument::fromBinaryData(data);
	QJsonArray array = doc.array();
	if (array.empty()) {
		return false;
	}

	QJsonObject release = array.first().toObject();
	tag = release.value("tag_name").toString("0.0");

	QJsonArray assets = release.value("assets").toArray();
	if (assets.empty()) {
		return false;
	}

	download = assets.first().toObject().value("browser_download_url").toString();
	return true;
}

QNetworkReply* BBApi::downloadRelease(const QString& url)
{
	return mNetwork->getRaw(url);
}

QByteArray BBApi::downloadFlag(int id)
{
	QUrl url = QString("http://www.buzzerbeater.com/images/flags/flag_%1.gif").arg(id);
	return mNetwork->get(url);
}
