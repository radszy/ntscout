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

#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>

class Network : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);

    QNetworkReply* getRaw(const QUrl &url);
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
