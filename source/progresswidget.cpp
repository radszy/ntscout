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

#include "progresswidget.h"
#include "ui_progresswidget.h"

#include "bbapi.h"
#include "network.h"
#include "worker.h"
#include "settings.h"
#include "searchvalues.h"

#include <qmath.h>

#include <QMovie>
#include <QThread>
#include <QDebug>

ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressWidget)
{
    ui->setupUi(this);

    state = 0;
    divisions = {0,0};
    leagues = {0,0};
    teams = {0,0};
    players = {0,0};

    movie = new QMovie(":/icons/progress");
    ui->divisionsProgress->setMovie(movie);
    movie->start();

    qRegisterMetaType<PlayerList>("PlayerList");
}

ProgressWidget::~ProgressWidget()
{
    delete ui;
}

void ProgressWidget::reset()
{
    state = 0;

    divisions = {0,0};
    leagues = {0,0};
    teams = {0,0};
    players = {0,0};

    ui->progressBar->setValue(0);
    ui->divisionTasks->setText("");
    ui->divisionsProgress->setPixmap(QPixmap());
    ui->divisionsProgress->setMovie(movie);
    ui->leagueTasks->setText("");
    ui->leaguesProgress->setPixmap(QPixmap());
    ui->teamTasks->setText("");
    ui->teamsProgress->setPixmap(QPixmap());
    ui->playerTasks->setText("");
    ui->playersProgress->setPixmap(QPixmap());

    searchValues.clear();
    playerLists.clear();
    filteredPlayers.clear();

    qDeleteAll(workers);
    workers.clear();
}

void ProgressWidget::stop()
{
    for (int i = 0; i < workers.count(); ++i) {
        workers.at(i)->quit();
        workers.at(i)->wait();
    }
    reset();
}

void ProgressWidget::start(QList<SearchValues*>& values)
{
    searchValues.append(values);

    LeagueDataList dataList;
    int divCount = 0;
    for (int i = 0; i < values.count(); ++i) {
        if (values.at(i)->countrySet) {
            LeagueData data;
            data.countryid = values.at(i)->countryid;
            for (int j = 0; j < values.at(i)->divCount; ++j) {
                if (values.at(i)->div[j]) {
                    data.divisions.append(j + 1);
                    divCount++;
                }
            }
            dataList.append(data);
        }
    }

    divisions = {0, divCount};
    ui->divisionTasks->setText(
                QString::number(divisions.first) + " / " +
                QString::number(divisions.second));

    BBApi bb;
    bb.login();

    connect(bb.getNetwork(), SIGNAL(finished(QNetworkReply*)),
            this, SLOT(requestDone()));

    QList<int> divisionList;
    bb.leagues(divisionList, dataList);

    leagues = {0, divisionList.count()};
    ui->leagueTasks->setText(
                QString::number(leagues.first) + " / " +
                QString::number(leagues.second));

    int tasks = qMin((int)Settings::tasks, divisionList.count());

    QList<QList<int>> div;
    for (int i = 0; i < tasks; i++) {
        div.append(QList<int>());
    }
    
    int count = (int)qFloor(divisionList.count() / tasks);
    for (int i = 0; i < tasks; i++) {
        QList <int> d = divisionList.mid(i * count, (i + 1 == tasks ? -1 : count));
        div[i].append(d);
    }

    for (int i = 0; i < tasks; ++i) {
        Worker *worker = new Worker(div[i], this);
        connect(worker,SIGNAL(finished(PlayerList)), this, SLOT(workerFinished(PlayerList)));
        connect(worker, SIGNAL(foundTeams(int)), this, SLOT(teamsFound(int)));
        worker->start();
        workers.append(worker);
    }

    nextState();
}

void ProgressWidget::filterPlayers()
{
    PlayerList playerList;
    for (int i = 0; i < playerLists.count(); ++i) {
        playerList.append(playerLists.at(i));
    }

    players = {0, playerList.count()};

    for (int i = 0; i < playerList.count(); ++i) {
        const Player* p = &playerList.at(i);

        for (int j = 0; j < searchValues.count(); ++j) {
            const SearchValues* sv = searchValues.at(j);
            if (!sv->nationalitySet) {
                continue;
            }
            if (p->nationalityid == sv->countryid) {
                if (p->age < sv->age.first || p->age > sv->age.second)
                    continue;
                if (p->potential < sv->potential.first ||
                    p->potential > sv->potential.second)
                    continue;
                if (p->salary < sv->salary.first ||
                    p->salary > sv->salary.second)
                    continue;
                if (p->dmi < sv->dmi.first || p->dmi > sv->dmi.second)
                    continue;
                filteredPlayers.append(*p);
            }
        }

        ui->playerTasks->setText(
                    QString::number(i + 1) + " / " +
                    QString::number(players.second));
    }

    if (players.second == 0) {
        setAsDone(ui->teamsProgress);
        ui->playerTasks->setText(
                    QString::number(players.first) + " / " +
                    QString::number(players.second));
    }

    ui->playersProgress->setMovie(nullptr);
    ui->playersProgress->setPixmap(QPixmap(":/icons/done"));
    ui->progressBar->setValue(ui->progressBar->maximum());
    emit finished(true);
}

void ProgressWidget::setAsDone(QLabel* progress)
{
    progress->setMovie(nullptr);
    progress->setPixmap(QPixmap(":/icons/done"));
}

void ProgressWidget::progressDivisions()
{
    divisions.first++;
    ui->divisionTasks->setText(QString::number(divisions.first) +" / " +
                               QString::number(divisions.second));
    if (divisions.first == divisions.second) {
        setAsDone(ui->divisionsProgress);
        ui->leaguesProgress->setMovie(movie);
    }
    updateProgress(divisions, 5, 0);
}

void ProgressWidget::progressLeagues()
{
    leagues.first++;
    ui->leagueTasks->setText(QString::number(leagues.first) +
                             " / " + QString::number(leagues.second));
    if (leagues.first == leagues.second) {
        nextState();
        setAsDone(ui->leaguesProgress);
        ui->teamsProgress->setMovie(movie);
    }
    updateProgress(leagues, 15, 5);
}

void ProgressWidget::progressTeams()
{
    teams.first++;
    ui->teamTasks->setText(QString::number(teams.first) +
                             " / " + QString::number(teams.second));
    if (teams.first == teams.second) {
        setAsDone(ui->teamsProgress);
        ui->playersProgress->setMovie(movie);
    }
    updateProgress(teams, 80, 20);
}

void ProgressWidget::updateProgress(const QPair<int,int>& pair,
                                    int curr, int prev)
{
    int n = pair.first * 100 / pair.second;
    int x = curr * n / 100 + prev;
    ui->progressBar->setValue(x);
}

void ProgressWidget::requestDone()
{
    switch (state) {
        case Divisions:
            progressDivisions();
            break;
        case Leagues:
            progressLeagues();
            break;
        case Teams:
            progressTeams();
            break;
        case Players:
            Q_UNREACHABLE();
            break;
    }
}

void ProgressWidget::teamsFound(int count)
{
    teams.second += count;
    ui->teamTasks->setText(
                QString::number(teams.first) + " / " +
                QString::number(teams.second));
}

void ProgressWidget::workerFinished(PlayerList playerList)
{
    playerLists.append(playerList);

    bool done = true;
    for (int i = 0; i < workers.count(); ++i) {
        done &= workers.at(i)->isDone();
    }

    if (done) {
        filterPlayers();
    }
}

