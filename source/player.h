#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

struct Player {
	QString firstname;
	QString lastname;
	QString nationalityName;
	QString bestPos;
	int id;
	int teamId;
	int nationalityId;
	int age;
	int height;
	int dmi;
	int salary;
	int gameShape;
	int potential;
};

using PlayerList = QList<Player>;

#endif // PLAYER_H
