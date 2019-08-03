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

class BBApi : public QObject {
	Q_OBJECT

public:
	BBApi();
	BBApi(const QString& login, const QString& password);
	~BBApi();

	QString login(const QString& login, const QString& password);
	QString login();
	bool countries(CountryList& result);
	bool leagues(QList<int>& results, const LeagueDataList leagues);
	bool teams(QList<int>& results, QList<int> league);
	bool roster(PlayerList& results, QList<int> team);
	bool translatedNames(CountryList& list);
	bool releases(QString& tag, QString& download);
	QNetworkReply* downloadRelease(const QString& url);
	QByteArray downloadFlag(int id);

	Network* getNetwork() { return manager; }

	static QString getName() { return name; }
	static QString getPass() { return pass; }

private:
	Network* manager;

	static QString name;
	static QString pass;
};

#endif // BBAPI_H
