#include "progresswidget.h"
#include "ui_progresswidget.h"

#include "bbapi.h"
#include "network.h"
#include "worker.h"
#include "settings.h"

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
    int leagueCount = 0;
    for (int i = 0; i < values.count(); ++i) {
        if (values.at(i)->countrySet) {
            LeagueData data;
            data.countryid = values.at(i)->countryid;
            for (int j = 0; j < values.at(i)->divCount; ++j) {
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

    connect(bb.getNetwork(), SIGNAL(finished(QNetworkReply*)),
            this, SLOT(requestDone()));

    QList<int> divisionList;
    bb.leagues(divisionList, dataList);

    leagues = {0, divisionList.count()};
    ui->leagueTasks->setText(QString::number(leagues.first) +
                             " / " + QString::number(leagues.second));

    int tasks = qMin(Settings::tasks, divisionList.count());

    QList<int> div[tasks];
    int count = (int)qFloor(divisionList.count() / tasks);
    for (int i = 0; i < tasks; i++) {
        QList <int> d = divisionList.mid(i * count, (i + 1 == tasks ? -1 : count));
        div[i].append(d);
    }

    for (int i = 0; i < tasks; ++i) {
        Worker *worker = new Worker(div[i], this);
        connect(worker,SIGNAL(finished(PlayerList)), this, SLOT(workerFinished(PlayerList)));
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

        ui->playerTasks->setText(QString::number(i+1) +
                                 " / " + QString::number(players.second));
    }

    if (players.second == 0) {
        ui->playerTasks->setText(QString::number(players.first) +
                                 " / " + QString::number(players.second));
    }

    ui->playersProgress->setMovie(nullptr);
    ui->playersProgress->setPixmap(QPixmap(":/icons/done"));
    emit finished(true);
}

void ProgressWidget::nextState()
{
    state++;
}

void ProgressWidget::progressDivisions()
{
    divisions.first++;
    ui->divisionTasks->setText(QString::number(divisions.first) +
                               " / " + QString::number(divisions.second));
    if (divisions.first == divisions.second) {
        ui->divisionsProgress->setMovie(nullptr);
        ui->divisionsProgress->setPixmap(QPixmap(":/icons/done"));
        ui->leaguesProgress->setMovie(movie);
    }
}

void ProgressWidget::progressLeagues()
{
    leagues.first++;
    ui->leagueTasks->setText(QString::number(leagues.first) +
                             " / " + QString::number(leagues.second));
    if (leagues.first == leagues.second) {
        nextState();
        ui->leaguesProgress->setMovie(nullptr);
        ui->leaguesProgress->setPixmap(QPixmap(":/icons/done"));
        ui->teamsProgress->setMovie(movie);
        teams = {0, leagues.second * 16};
    }
}

void ProgressWidget::progressTeams()
{
    teams.first++;
    ui->teamTasks->setText(QString::number(teams.first) +
                             " / " + QString::number(teams.second));
    if (teams.first == teams.second) {
        ui->teamsProgress->setMovie(nullptr);
        ui->teamsProgress->setPixmap(QPixmap(":/icons/done"));
        ui->playersProgress->setMovie(movie);
    }
}

PlayerList ProgressWidget::getResults()
{
    return filteredPlayers;
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
        default:
            break;
    }

    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

void ProgressWidget::workerFinished(PlayerList playerList)
{
    playerLists.append(playerList);

    if (playerLists.count() == workers.count()) {
        filterPlayers();
    }
}
