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

public:
	explicit ProgressWidget(QWidget* parent = nullptr);
	~ProgressWidget();

	void start(QList<SearchValues*>& values);
	void reset();
	void stop();

	PlayerList getResults() { return mFilteredPlayers; }
	QString getElapsedTime() { return mElapsedTime; }
	int getRequests() { return mRequests; }

public slots:
	void requestDone();
	void teamsFound(int count);
	void workerFinished(const PlayerList& playerList);

signals:
	void finished(bool);

private:
	enum class State {
		Divisions,
		Leagues,
		Teams,
		Players,
	};

	class Progress {
	public:
		Progress() = default;
		Progress(int value, int max)
		        : mValue(value)
		        , mMax(max)
		{
		}

		void increment() { mValue++; }
		int value() const { return mValue; }
		int max() const { return mMax; }
		bool isDone() { return mValue >= mMax; }
		int percent() const { return mValue * 100 / mMax; }
		QString toString() { return QString("%1 / %2").arg(mValue).arg(mMax); }

	private:
		int mValue = 0;
		int mMax = 0;
	};

	Leagues leagues(const QList<SearchValues*>& values, int& count);
	void filterPlayers();
	void setAsDone(QLabel* progress);
	void nextState();
	void progressDivisions();
	void progressLeagues();
	void progressTeams();
	void updateTotalProgress(const Progress& progress, int previous, int current);

	QList<SearchValues*> mSearchValues;
	QList<Worker*> mWorkers;
	QList<PlayerList> mPlayerList;
	PlayerList mFilteredPlayers;

	Progress mDivisions;
	Progress mLeagues;
	Progress mTeams;
	Progress mPlayers;

	QTime mTime;
	QString mElapsedTime;

	Ui::ProgressWidget* mUi;
	QMovie* mMovie;

	State mState;
	int mRequests;
};

#endif // PROGRESSWIDGET_H
