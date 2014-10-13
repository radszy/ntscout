#include "progresswidget.h"
#include "ui_progresswidget.h"

#include "bbapi.h"
#include "network.h"
#include "worker.h"

#include <qmath.h>

#include <QMovie>
#include <QThread>
#include <QDebug>

ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressWidget)
{
    ui->setupUi(this);

    movie = new QMovie(":/icons/progress");
    ui->divisionsProgress->setMovie(movie);
    movie->start();

    qRegisterMetaType<PlayerList>("PlayerList");
}

ProgressWidget::~ProgressWidget()
{
    delete ui;
}

void ProgressWidget::start(QList<SearchValues*>& values)
{
    searchValues.append(values);

    LeagueDataList dataList;
    int divCount = 0;
    int leagueCount = 0;
    for (int i = 0; i < values.count(); ++i) {
        if (values.at(i)->countrySet) {
            LeagueData data;
            data.countryid = values.at(i)->countryid;
            for (int j = 0; j < 6; ++j) {
                if (values.at(i)->div[j]) {
                    data.divisions.append(j + 1);
                    divCount++;
                    switch(j + 1) {
                        case 1: leagueCount += 1; break;
                        case 2: leagueCount += 4; break;
                        case 3: leagueCount += 16; break;
                        case 4: leagueCount += 64; break;
                        case 5: leagueCount += 256; break;
                        case 6: leagueCount += 512; break;
                    }
                }
            }
            dataList.append(data);
        }
    }

    ui->progressBar->setMaximum(divCount + leagueCount + leagueCount * 16);

    divisions = {0, divCount};
    ui->divisionTasks->setText(QString::number(divisions.first) +
                               " / " + QString::number(divisions.second));

    BBApi bb;
    bb.login();

    state = 0;

    connect(bb.getNetwork(), SIGNAL(finished(QNetworkReply*)),
            this, SLOT(requestDone()));

    QList<int> divisionList;
    bb.leagues(divisionList, dataList);

    leagues = {0, divisionList.count()};
    ui->leagueTasks->setText(QString::number(leagues.first) +
                             " / " + QString::number(leagues.second));

    int tasks = 10;

    QList<int> div[tasks];
    int count = (int)qFloor(divisionList.count() / tasks);
    for (int i = 0; i < tasks; i++) {
        QList <int> d = divisionList.mid(i * count, (i + 1 == tasks ? -1 : count));
        div[i].append(d);
    }

    qDeleteAll(workers);
    workers.clear();
    for (int i = 0; i < tasks; ++i) {
        Worker *worker = new Worker(div[i], this);
        connect(worker,SIGNAL(finished(PlayerList)), this, SLOT(searchDone(PlayerList)));
        worker->start();
        workers.append(worker);
    }

    state = 1;
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
                filtered.append(*p);
            }
        }

        ui->playerTasks->setText(QString::number(i+1) +
                                 " / " + QString::number(players.second));
    }

    ui->playersProgress->setMovie(nullptr);
    ui->playersProgress->setPixmap(QPixmap(":/icons/done"));
    emit finished(true);
}

PlayerList ProgressWidget::getResults()
{
    return filtered;
}

void ProgressWidget::requestDone()
{
    switch (state) {
        case 0:
            divisions.first++;
            ui->divisionTasks->setText(QString::number(divisions.first) +
                                       " / " + QString::number(divisions.second));
            if (divisions.first == divisions.second) {
                ui->divisionsProgress->setMovie(nullptr);
                ui->divisionsProgress->setPixmap(QPixmap(":/icons/done"));
                ui->leaguesProgress->setMovie(movie);
            }
            break;
        case 1:
            leagues.first++;
            ui->leagueTasks->setText(QString::number(leagues.first) +
                                     " / " + QString::number(leagues.second));
            if (leagues.first == leagues.second) {
                state = 2;
                ui->leaguesProgress->setMovie(nullptr);
                ui->leaguesProgress->setPixmap(QPixmap(":/icons/done"));
                ui->teamsProgress->setMovie(movie);
                teams = {0, leagues.second * 16};
            }
            break;
        case 2:
            teams.first++;
            ui->teamTasks->setText(QString::number(teams.first) +
                                     " / " + QString::number(teams.second));
            if (teams.first == teams.second) {
                ui->teamsProgress->setMovie(nullptr);
                ui->teamsProgress->setPixmap(QPixmap(":/icons/done"));
                ui->playersProgress->setMovie(movie);
            }
            break;
        default:
            break;
    }

    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

void ProgressWidget::searchDone(PlayerList playerList)
{
    playerLists.append(playerList);

    if (playerLists.count() == workers.count()) {
        filterPlayers();
    }
}
