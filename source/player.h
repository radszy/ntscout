#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

struct Player {
    QString firstname;
    QString lastname;
    QString nationalityname;
    QString bestpos;
    int id;
    int teamid;
    int nationalityid;
    int age;
    int height;
    int dmi;
    int salary;
    int gameshape;
    int potential;
};

typedef QList<Player> PlayerList;

#endif // PLAYER_H
