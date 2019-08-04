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

ProgressWidget::ProgressWidget(QWidget* parent)
        : QWidget(parent)
        , mUi(new Ui::ProgressWidget)
{
	mUi->setupUi(this);

	mState = State::Divisions;
	mRequests = 0;
	mDivisions = Progress();
	mLeagues = Progress();
	mTeams = Progress();
	mPlayers = Progress();

	mMovie = new QMovie(":/icons/progress");
	mUi->divisionsProgress->setMovie(mMovie);
	mMovie->start();

	qRegisterMetaType<PlayerList>("PlayerList");
}

ProgressWidget::~ProgressWidget()
{
	delete mUi;
}

void ProgressWidget::reset()
{
	mState = State::Divisions;
	mRequests = 0;

	mDivisions = Progress();
	mLeagues = Progress();
	mTeams = Progress();
	mPlayers = Progress();

	mUi->progressBar->setValue(0);
	mUi->divisionTasks->setText("");
	mUi->divisionsProgress->setPixmap(QPixmap());
	mUi->divisionsProgress->setMovie(mMovie);
	mUi->leagueTasks->setText("");
	mUi->leaguesProgress->setPixmap(QPixmap());
	mUi->teamTasks->setText("");
	mUi->teamsProgress->setPixmap(QPixmap());
	mUi->playerTasks->setText("");
	mUi->playersProgress->setPixmap(QPixmap());

	mSearchValues.clear();
	mPlayerList.clear();
	mFilteredPlayers.clear();

	qDeleteAll(mWorkers);
	mWorkers.clear();
}

void ProgressWidget::stop()
{
	for (Worker* worker : mWorkers) {
		worker->quit();
		worker->wait();
	}
	reset();
}

Leagues ProgressWidget::leagues(const QList<SearchValues*>& values, int& count)
{
	Leagues leagues;

	for (SearchValues* value : values) {
		if (value->countrySet) {
			League league;
			league.countryId = value->countryid;

			for (int i = 0; i < value->divCount; i++) {
				if (value->div[i]) {
					league.divisionIds.append(i + 1);
				}
			}

			count += league.divisionIds.count();
			leagues.append(league);
		}
	}

	return leagues;
}

void ProgressWidget::start(QList<SearchValues*>& values)
{
	mTime.start();

	mSearchValues.append(values);

	int divCount = 0;
	Leagues dataList = leagues(values, divCount);
	mDivisions = Progress(0, divCount);
	mUi->divisionTasks->setText(mDivisions.toString());

	BBApi bb;
	bb.login();
	connect(bb.network(), SIGNAL(finished(QNetworkReply*)), this, SLOT(requestDone()));

	Ids divisionIds;
	bb.leagues(divisionIds, dataList);

	mLeagues = Progress(0, divisionIds.count());
	mUi->leagueTasks->setText(mLeagues.toString());

	int tasks = qMin((int)Settings::tasks, divisionIds.count());

	QList<QList<int>> div;
	for (int i = 0; i < tasks; i++) {
		div.append(QList<int>());
	}

	int count = (int)qFloor(divisionIds.count() / tasks);
	for (int i = 0; i < tasks; i++) {
		QList<int> d = divisionIds.mid(i * count, (i + 1 == tasks ? -1 : count));
		div[i].append(d);
	}

	for (int i = 0; i < tasks; ++i) {
		auto worker = new Worker(div[i], this);
		connect(worker,
		        SIGNAL(finished(PlayerList)),
		        this,
		        SLOT(workerFinished(PlayerList)));
		connect(worker, SIGNAL(foundTeams(int)), this, SLOT(teamsFound(int)));
		worker->start();
		mWorkers.append(worker);
	}

	nextState();
}

void ProgressWidget::filterPlayers()
{
	PlayerList playerList;

	for (int i = 0; i < mPlayerList.count(); ++i) {
		playerList.append(mPlayerList.at(i));
	}

	for (const Player& player : playerList) {
		for (const SearchValues* value : mSearchValues) {
			if (!value->nationalitySet) {
				continue;
			}

			if (player.nationalityId == value->countryid) {
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

				mFilteredPlayers.append(player);
			}
		}
	}

	mPlayers = Progress(playerList.count(), playerList.count());
	mUi->playerTasks->setText(mPlayers.toString());
	setAsDone(mUi->playersProgress);

	mUi->progressBar->setValue(mUi->progressBar->maximum());

	mElapsedTime = Util::formatTime(mTime.elapsed());
	mRequests = mLeagues.value() + mDivisions.value() + mTeams.value() + 1;
	emit finished(true);
}

void ProgressWidget::setAsDone(QLabel* progress)
{
	progress->setMovie(nullptr);
	progress->setPixmap(QPixmap(":/icons/done"));
}

void ProgressWidget::nextState()
{
	switch (mState) {
		case State::Divisions:
			mState = State::Leagues;
			break;

		case State::Leagues:
			mState = State::Teams;
			break;

		case State::Teams:
			mState = State::Players;
			break;

		case State::Players:
			break;
	}
}

void ProgressWidget::progressDivisions()
{
	mDivisions.increment();
	mUi->divisionTasks->setText(mDivisions.toString());

	if (mDivisions.isDone()) {
		setAsDone(mUi->divisionsProgress);
		mUi->leaguesProgress->setMovie(mMovie);
	}

	updateTotalProgress(mDivisions, 0, 5);
}

void ProgressWidget::progressLeagues()
{
	mLeagues.increment();
	mUi->leagueTasks->setText(mLeagues.toString());

	if (mLeagues.isDone()) {
		nextState();
		setAsDone(mUi->leaguesProgress);
		mUi->teamsProgress->setMovie(mMovie);
	}

	updateTotalProgress(mLeagues, 5, 15);
}

void ProgressWidget::progressTeams()
{
	mTeams.increment();
	mUi->teamTasks->setText(mTeams.toString());

	if (mTeams.isDone()) {
		setAsDone(mUi->teamsProgress);
		mUi->playersProgress->setMovie(mMovie);
	}

	updateTotalProgress(mTeams, 20, 80);
}

void ProgressWidget::updateTotalProgress(const Progress& progress, int previous, int current)
{
	const int percent = progress.percent();
	const int total = current * percent / 100 + previous;
	mUi->progressBar->setValue(total);
}

void ProgressWidget::requestDone()
{
	switch (mState) {
		case State::Divisions:
			progressDivisions();
			break;
		case State::Leagues:
			progressLeagues();
			break;
		case State::Teams:
			progressTeams();
			break;
		case State::Players:
			Q_UNREACHABLE();
			break;
	}
}

void ProgressWidget::teamsFound(int count)
{
	mTeams = Progress(mTeams.value(), mTeams.max() + count);
	mUi->teamTasks->setText(mTeams.toString());
}

void ProgressWidget::workerFinished(const PlayerList& playerList)
{
	mPlayerList.append(playerList);

	for (Worker* worker : mWorkers) {
		if (!worker->isDone()) {
			return;
		}
	}

	filterPlayers();
}
