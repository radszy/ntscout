#include "progresswidget.h"
#include "ui_progresswidget.h"

#include "network.h"
#include "worker.h"
#include "settings.h"
#include "searchvalues.h"
#include "util.h"

#include <QtMath>
#include <QMovie>
#include <QThread>
#include <QDebug>

ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressWidget)
{
    ui->setupUi(this);

    state = 0;
    requests = 0;
    divisions = qMakePair(0, 0);
    leagues = qMakePair(0, 0);
    teams = qMakePair(0, 0);
    players = qMakePair(0, 0);

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
    requests = 0;

    divisions = qMakePair(0, 0);
    leagues = qMakePair(0, 0);
    teams = qMakePair(0, 0);
    players = qMakePair(0, 0);

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
    for (Worker* worker : workers) {
        worker->quit();
        worker->wait();
    }
    reset();
}

LeagueDataList ProgressWidget::getLeagueData(const QList<SearchValues*>& values, int& count)
{
    LeagueDataList list;

    for (SearchValues* value : values) {
        if (value->countrySet) {
            LeagueData data;
            data.countryid = value->countryid;
            for (int i = 0; i < value->divCount; i++) {
                if (value->div[i]) {
                    data.divisions.append(i + 1);
                }
            }
            count += data.divisions.count();
            list.append(data);
        }
    }

    return list;
}

void ProgressWidget::start(QList<SearchValues*>& values)
{
    time.start();

    searchValues.append(values);

    int divCount = 0;
    LeagueDataList dataList = getLeagueData(values, divCount);
    divisions = qMakePair(0, divCount);
    ui->divisionTasks->setText(QString("%1 / %2").arg(divisions.first).arg(divisions.second));

    BBApi bb;
    bb.login();
    connect(bb.getNetwork(), SIGNAL(finished(QNetworkReply*)), this, SLOT(requestDone()));

    QList<int> divisionList;
    bb.leagues(divisionList, dataList);

    leagues = qMakePair(0, divisionList.count());
    ui->leagueTasks->setText(QString("%1 / %2").arg(leagues.first).arg(leagues.second));

    int tasks = qMin((int)Settings::tasks, divisionList.count());

    QList<QList<int>> div;
    for (int i = 0; i < tasks; i++) {
        div.append(QList<int>());
    }

    int count = (int)qFloor(divisionList.count() / tasks);
    for (int i = 0; i < tasks; i++) {
        QList<int> d = divisionList.mid(i * count, (i + 1 == tasks ? -1 : count));
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

    players = qMakePair(0, playerList.count());

    for (const Player& player : playerList) {
        for (const SearchValues* value : searchValues) {
            if (!value->nationalitySet) {
                continue;
            }

            if (player.nationalityid == value->countryid) {
                if (player.age < value->age.first ||
                    player.age > value->age.second ||
                    player.potential < value->potential.first ||
                    player.potential > value->potential.second ||
                    player.salary < value->salary.first ||
                    player.salary > value->salary.second ||
                    player.dmi < value->dmi.first ||
                    player.dmi > value->dmi.second) {
                    continue;
                }

                filteredPlayers.append(player);
            }
        }
    }
    ui->playerTasks->setText(QString("%1 / %1").arg(players.second));

    if (players.second == 0) {
        setAsDone(ui->teamsProgress);
        ui->playerTasks->setText(QString("%1 / %2").arg(players.first).arg(players.second));
    }

    ui->playersProgress->setMovie(nullptr);
    ui->playersProgress->setPixmap(QPixmap(":/icons/done"));
    ui->progressBar->setValue(ui->progressBar->maximum());

    elapsedTime = Util::formatTime(time.elapsed());
    requests = leagues.first + divisions.first + teams.first + 1;
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
    ui->divisionTasks->setText(QString("%1 / %2").arg(divisions.first).arg(divisions.second));

    if (divisions.first == divisions.second) {
        setAsDone(ui->divisionsProgress);
        ui->leaguesProgress->setMovie(movie);
    }

    updateProgress(divisions, 5, 0);
}

void ProgressWidget::progressLeagues()
{
    leagues.first++;
    ui->leagueTasks->setText(QString("%1 / %2").arg(leagues.first).arg(leagues.second));

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
    ui->teamTasks->setText(QString("%1 / %2").arg(teams.first).arg(teams.second));

    if (teams.first == teams.second) {
        setAsDone(ui->teamsProgress);
        ui->playersProgress->setMovie(movie);
    }

    updateProgress(teams, 80, 20);
}

void ProgressWidget::updateProgress(const QPair<int,int>& pair, int curr, int prev)
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
    ui->teamTasks->setText(QString("%1 / %2").arg(teams.first).arg(teams.second));
}

void ProgressWidget::workerFinished(PlayerList playerList)
{
    playerLists.append(playerList);

    for (Worker* worker : workers) {
        if (!worker->isDone()) {
            return;
        }
    }

    filterPlayers();
}
