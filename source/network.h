#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>

class Network : public QNetworkAccessManager {
	Q_OBJECT
public:
	explicit Network(QObject* parent = nullptr);

	QNetworkReply* getRaw(const QUrl& url);
	QByteArray get(const QUrl& url);
	QList<QByteArray> get(const QList<QUrl>& urls);

public slots:
	void onFinished();

signals:
	void finishedAll();

private:
	int runningRequests;
};

#endif // NETWORK_H
