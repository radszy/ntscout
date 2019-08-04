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
        : mManager(new Network)
{
}

BBApi::BBApi(const QString& login, const QString& password)
        : mManager(new Network)
{
	mName = login;
	mPass = password;
}

BBApi::~BBApi()
{
	delete mManager;
}

QString BBApi::login(const QString& login, const QString& password)
{
	QUrl url("http://bbapi.buzzerbeater.com/login.aspx"
	         "?login=" +
	         login + "&code=" + password);
	QByteArray data = mManager->get(url);

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

	return "???";
}

QString BBApi::login()
{
	return login(mName, mPass);
}

bool BBApi::countries(Countries& result)
{
	QUrl url("http://bbapi.buzzerbeater.com/countries.aspx");
	QByteArray data = mManager->get(url);

	QDomDocument doc;
	doc.setContent(data);
	const auto& nodes = doc.elementsByTagName("country");

	for (int i = 0; i < nodes.count(); ++i) {
		const auto& node = nodes.at(i);
		const auto& attr = node.attributes();

		Country country;
		country.id = attr.namedItem("id").toAttr().value().toInt();
		country.divisions = attr.namedItem("divisions").toAttr().value().toInt();
		country.users = attr.namedItem("users").toAttr().value().toInt();
		country.name = node.toElement().text();
		result.append(country);
	}

	return true;
}

bool BBApi::leagues(QList<int>& results, const LeagueDataList& leagues)
{
	QList<QUrl> urls;
	for (int i = 0; i < leagues.count(); ++i) {
		LeagueData data = leagues.at(i);
		for (int j = 0; j < data.divisions.count(); ++j) {
			QUrl url("http://bbapi.buzzerbeater.com/leagues.aspx"
			         "?countryid=" +
			         QString::number(data.countryid) +
			         "&level=" + QString::number(data.divisions.at(j)));
			urls.append(url);
		}
	}
	QList<QByteArray> data = mManager->get(urls);

	for (int i = 0; i < data.count(); ++i) {
		QDomDocument doc;
		doc.setContent(data.at(i));
		const auto& nodes = doc.elementsByTagName("league");

		for (int j = 0; j < nodes.count(); ++j) {
			int id = nodes.at(j).attributes().namedItem("id").toAttr().value().toInt();
			results.append(id);
		}
	}

	return true;
}

bool BBApi::teams(QList<int>& results, const QList<int>& league)
{
	QList<QUrl> urls;
	for (int i = 0; i < league.count(); ++i) {
		QUrl url("http://bbapi.buzzerbeater.com/standings.aspx?"
		         "leagueid=" +
		         QString::number(league.at(i)));
		urls.append(url);
	}
	QList<QByteArray> data = mManager->get(urls);

	for (int i = 0; i < data.count(); ++i) {
		QDomDocument doc;
		doc.setContent(data.at(i));
		const auto& nodes = doc.elementsByTagName("team");

		for (int j = 0; j < nodes.count(); ++j) {
			int id = nodes.at(j).attributes().namedItem("id").toAttr().value().toInt();
			if (!Settings::searchBots) {
				bool isBot =
				        nodes.at(j).firstChildElement("isBot").text().toInt() == 1;
				if (!isBot) {
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

bool BBApi::roster(PlayerList& results, const QList<int>& team)
{
	QList<QUrl> urls;
	for (int i = 0; i < team.count(); ++i) {
		QUrl url("http://bbapi.buzzerbeater.com/roster.aspx?"
		         "teamid=" +
		         QString::number(team.at(i)));
		urls.append(url);
	}
	QList<QByteArray> data = mManager->get(urls);

	for (int i = 0; i < data.count(); ++i) {
		QDomDocument doc;
		doc.setContent(data.at(i));
		int teamid = doc.elementsByTagName("roster")
		                     .at(0)
		                     .attributes()
		                     .namedItem("teamid")
		                     .toAttr()
		                     .value()
		                     .toInt();

		const auto& nodes = doc.elementsByTagName("player");

		for (int j = 0; j < nodes.count(); ++j) {
			const auto& node = nodes.at(j);

			Player player;
			player.teamid = teamid;
			player.id = node.attributes().namedItem("id").toAttr().value().toInt();
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
	QByteArray data = mManager->get(url);

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
	QByteArray data = mManager->get(url);

	QJsonDocument doc = QJsonDocument::fromBinaryData(data);
	auto array = doc.array();
	if (array.empty()) {
		return false;
	}

	auto release = array.first().toObject();
	tag = release.value("tag_name").toString("0.0");

	auto assets = release.value("assets").toArray();
	if (assets.empty()) {
		return false;
	}

	download = assets.first().toObject().value("browser_download_url").toString();
	return true;
}

QNetworkReply* BBApi::downloadRelease(const QString& url)
{
	return mManager->getRaw(url);
}

QByteArray BBApi::downloadFlag(int id)
{
	QUrl url = QString("http://www.buzzerbeater.com/images/flags/flag_%1.gif").arg(id);
	return mManager->get(url);
}
