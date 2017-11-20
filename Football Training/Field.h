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
    explicit Field();
    ~Field();

    void Move(Player *player, const Position *destPos);
    void UpdatePlayerPositions(Position **positions);
    void UpdateBallPosition(Position *position);
    void SendBallLocation();
    void SendOutcome(int ballWinnerId);
    void PlayRound();
    void PrintStatistics(int round);
    int DetermineBallWinner();
    Position ** ReceivePlayerPositions();
    Position * ReceiveBallPosition(int ballWinnerId);

private:
    const int TEAM_SIZE = 11;

    Element ***field;
    Player **team;
    Ball *ball;
};

#endif //FIELD_H
