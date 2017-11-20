#ifndef GLOBAL_H
#define GLOBAL_H

#include <mpi.h>
#include <cstdlib>

struct Position
{
    int x;
    int y;
};

struct Stat
{
    int distanceTraveledSinceTheBeginning;
    int ballWasReachedSinceTheBeginningCount;
    int ballWasWonSinceTheBeginningCount;
    int ballChallenge;

    bool ballWasReached;
    bool ballWasWon;

    int ballPositionX;
    int ballPositionY;
    int initialPositionX;
    int initialPositionY;
    int finalPositionX;
    int finalPositionY;
};

extern const int FIELD_TILE_NUMBER;
extern const int FIELD_TILE_SIDE;
extern const int MAX_PLAYER_SKILL;
extern const int MAX_RUNNING_PLAYERS;
extern const int MAX_ROUND_NUMBER;
extern const int TEAM_SIZE;
extern const int FIELD_LENGTH;
extern const int FIELD_WIDTH;
extern const int SEED;

extern MPI_Datatype POSITION;
extern MPI_Datatype STAT;
extern Position invalidPosition;
extern Position goalPosition;

#endif //GLOBAL_H
