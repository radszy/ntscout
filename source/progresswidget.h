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

class ProgressWidget : public QWidget {
	Q_OBJECT

	enum State {
		Divisions,
		Leagues,
		Teams,
		Players,
	};

public:
	explicit ProgressWidget(QWidget* parent = nullptr);
	~ProgressWidget();

	void start(QList<SearchValues*>& values);
	void reset();
	void stop();

	PlayerList getResults() { return filteredPlayers; }
	QString getElapsedTime() { return elapsedTime; }
	int getRequests() { return requests; }

public slots:
	void requestDone();
	void teamsFound(int count);
	void workerFinished(const PlayerList& playerList);

signals:
	void finished(bool);

private:
	LeagueDataList getLeagueData(const QList<SearchValues*>& values, int& count);
	void filterPlayers();
	void setAsDone(QLabel* progress);
	void nextState() { state++; }
	void progressDivisions();
	void progressLeagues();
	void progressTeams();
	void updateProgress(const QPair<int, int>& pair, int curr, int prev);

	QList<SearchValues*> searchValues;
	QList<Worker*> workers;
	QList<PlayerList> playerLists;
	PlayerList filteredPlayers;

	QPair<int, int> divisions;
	QPair<int, int> leagues;
	QPair<int, int> teams;
	QPair<int, int> players;

	QTime time;
	QString elapsedTime;

	Ui::ProgressWidget* ui;
	QMovie* movie;

	int state;
	int requests;
};

#endif // PROGRESSWIDGET_H
