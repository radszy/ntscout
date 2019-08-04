#ifndef BBAPI_H
#define BBAPI_H

#include "country.h"
#include "player.h"

#include <QObject>
#include <QString>
#include <QList>

class Network;
class QNetworkReply;

using Id = int;
using Ids = QList<Id>;

struct League {
	Id countryId;
	Ids divisionIds;
};

using Leagues = QList<League>;

class BBApi : public QObject {
	Q_OBJECT

public:
	BBApi();
	BBApi(const QString& login, const QString& password);
	~BBApi();

	QString login(const QString& login, const QString& password);
	QString login();
	bool countries(Countries& result);
	bool leagues(Ids& results, const Leagues& leagues);
	bool teams(Ids& results, const Ids& leaguesIds);
	bool roster(PlayerList& results, const Ids& teamIds);
	bool translatedNames(Countries& countries);
	bool releases(QString& tag, QString& download);
	QNetworkReply* downloadRelease(const QString& url);
	QByteArray downloadFlag(int id);

	Network* network() const { return mNetwork; }
	static QString name() { return mName; }
	static QString pass() { return mPass; }

private:
	Network* mNetwork;

	static QString mName;
	static QString mPass;
};

#endif // BBAPI_H
