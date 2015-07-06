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

#include "network.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

Network::Network(QObject *parent) :
    QNetworkAccessManager(parent)
{
    runningRequests = 0;
}

QNetworkReply *Network::getRaw(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    return QNetworkAccessManager::get(request);
}

QByteArray Network::get(const QUrl &url)
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

QList<QByteArray> Network::get(const QList<QUrl> &urls)
{
    QList<QByteArray> result;
    QList<QNetworkReply*> replies;

    runningRequests += urls.count();
    for (const QUrl& url : urls) {
        QNetworkRequest request(url);
        request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

        QNetworkReply* reply = QNetworkAccessManager::get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
        replies.append(reply);
    }

    QEventLoop loop;
    connect(this, SIGNAL(finishedAll()), &loop, SLOT(quit()));
    loop.exec();

    for (QNetworkReply* reply : replies) {
        result.append(reply->readAll());
    }

    qDeleteAll(replies);
    return result;
}

void Network::onFinished()
{
    runningRequests--;
    if (runningRequests == 0) {
        emit finishedAll();
    }
}
