#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>

class Network : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);

    QByteArray get(const QUrl url);
    QList<QByteArray> get(const QList<QUrl> urls);

signals:
    void finishedAll();

public slots:
    void onFinished();

private:
    int running_requests;

};

#endif // NETWORK_H
