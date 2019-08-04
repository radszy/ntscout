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

using LeagueDataList = QList<LeagueData>;

class BBApi : public QObject {
	Q_OBJECT

public:
	BBApi();
	BBApi(const QString& login, const QString& password);
	~BBApi();

	QString login(const QString& login, const QString& password);
	QString login();
	bool countries(Countries& result);
	bool leagues(QList<int>& results, const LeagueDataList& leagues);
	bool teams(QList<int>& results, const QList<int>& league);
	bool roster(PlayerList& results, const QList<int>& team);
	bool translatedNames(Countries& countries);
	bool releases(QString& tag, QString& download);
	QNetworkReply* downloadRelease(const QString& url);
	QByteArray downloadFlag(int id);

	Network* network() const { return mManager; }
	static QString name() { return mName; }
	static QString pass() { return mPass; }

private:
	Network* mManager;

	static QString mName;
	static QString mPass;
};

#endif // BBAPI_H
