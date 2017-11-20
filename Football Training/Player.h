#ifndef PLAYER_H
#define PLAYER_H

#include "Global.h"
#include "Statistics.h"

class Player
{
public:
    explicit Player(int playerId, int playerRank);
    ~Player();

    void PlayRound();
    Position * DetermineBallLocation();
    void MoveToBall(Position *ballPos);
    void KickBall(Position * ballPos);
    void SendPosition(Position *pos);

    int GetId() const;
    int GetRank() const;
    Position *GetPosition() const;
    Statistics *GetStatistics() const;

    void SetPosition(Position *pos);

private:
    int id;
    int rank;

    Position *position;
    Statistics *statistics;
};

#endif //PLAYER_H
