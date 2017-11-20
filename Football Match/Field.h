#ifndef FIELD_H
#define FIELD_H

#include "Element.h"
#include "Ball.h"
#include "Player.h"

#include <mpi.h>
#include <cstdlib>

class Field
{
public:
    explicit Field(int rank, MPI_Comm *commF, MPI_Comm *commTA, MPI_Comm *commTB);
    ~Field();

    void UpdateTeamPosition(Position *positions, Player **team);

    void Move(Player *player, const Position *destPos);
    void UpdateBallPosition(Position *position);
    void SendBallLocation();
    void SendOutcome(int ballWinnerId);
    void PlayRound();
    void EnsureBallIntegrity();
    void Reset();
    int DetermineBallWinner();
    Position * ReceiveTeamPositions(const MPI_Comm *comm);
    Position * ReceiveBallPosition(int ballWinnerId);
    Stat * ReceiveStatistics(MPI_Comm *comm);
    void PrintStatistics(int round, Stat *statTeamA, Stat *statTeamB);

    Player ** GetTeamA() const;
    Player ** GetTeamB() const;

private:
    int fieldTileId;
    bool isGoalScored;

    MPI_Comm *commField;
    MPI_Comm *commTeamA;
    MPI_Comm *commTeamB;

    Element ***field;
    Player **teamA;
    Player **teamB;
    Ball *ball;
};

#endif //FIELD_H
