#include "network.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

Network::Network(QObject *parent) :
    QNetworkAccessManager(parent)
{
    running_requests = 0;
}

QByteArray Network::get(const QUrl url)
{
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    QNetworkReply* reply = QNetworkAccessManager::get(request);

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
    loop.exec();

    return reply->readAll();
}

QList<QByteArray> Network::get(const QList<QUrl> urls)
{
    QList<QByteArray> result;
    QList<QNetworkReply*> replies;

    running_requests += urls.count();
    for (int i = 0; i < urls.count(); ++i) {
        QNetworkRequest request(urls.at(i));
        request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

        QNetworkReply* reply = QNetworkAccessManager::get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
        replies.append(reply);
    }

    QEventLoop loop;
    connect(this, SIGNAL(finishedAll()), &loop, SLOT(quit()));
    loop.exec();

    for (int i = 0; i < replies.count(); ++i) {
        result.append(replies.at(i)->readAll());
    }
    return result;
}

void Network::onFinished()
{
    running_requests--;
    if (running_requests == 0) {
        emit finishedAll();
    }
}
