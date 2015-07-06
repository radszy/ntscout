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

#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include "bbapi.h"
#include "player.h"

#include <QWidget>
#include <QPair>
#include <QTime>

namespace Ui {
class ProgressWidget;
}

class QTimer;
class QMovie;
class QLabel;
class Worker;
struct SearchValues;

class ProgressWidget : public QWidget
{
    Q_OBJECT

    enum State {
        Divisions,
        Leagues,
        Teams,
        Players
    };

public:
    explicit ProgressWidget(QWidget *parent = 0);
    ~ProgressWidget();

    void start(QList<SearchValues*>& values);
    void reset();
    void stop();

    PlayerList getResults() {return filteredPlayers;}

public slots:
    void requestDone();
    void teamsFound(int count);
    void workerFinished(PlayerList playerList);
    void updateTimer();

signals:
    void finished(bool);

private:
    LeagueDataList getLeagueData(const QList<SearchValues*>& values, int& count);
    void filterPlayers();
    void setAsDone(QLabel* progress);
    void nextState() {state++;}
    void progressDivisions();
    void progressLeagues();
    void progressTeams();
    void updateProgress(const QPair<int,int>& pair, int curr, int prev);

    QList<SearchValues*> searchValues;
    QList<Worker*> workers;
    QList<PlayerList> playerLists;
    PlayerList filteredPlayers;

    QPair <int, int> divisions;
    QPair <int, int> leagues;
    QPair <int, int> teams;
    QPair <int, int> players;

    QTime time;
    QString elapsedTime;

    Ui::ProgressWidget *ui;
    QMovie* movie;
    QTimer* timer;

    int state;
};

#endif // PROGRESSWIDGET_H
