#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include "player.h"
#include "searchvalues.h"

#include <QWidget>
#include <QPair>

namespace Ui {
class ProgressWidget;
}

class QMovie;
class Worker;

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

    void reset();
    void stop();
    void start(QList<SearchValues*>& values);
    void filterPlayers();
    void nextState();

    void progressDivisions();
    void progressLeagues();
    void progressTeams();

    PlayerList getResults();

public slots:
    void requestDone();
    void workerFinished(PlayerList playerList);

signals:
    void finished(bool);

private:
    Ui::ProgressWidget *ui;

    QMovie* movie;

    QPair <int, int> divisions;
    QPair <int, int> leagues;
    QPair <int, int> teams;
    QPair <int, int> players;

    QList<SearchValues*> searchValues;
    QList<Worker*> workers;
    QList<PlayerList> playerLists;
    PlayerList filteredPlayers;

    int state;
};

#endif // PROGRESSWIDGET_H
