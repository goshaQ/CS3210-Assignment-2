#ifndef PLAYER_H
#define PLAYER_H

#include "Global.h"
#include "Statistics.h"

class Player
{
public:
    explicit Player(int playerId, int playerRank, char t, MPI_Comm *cTeam, MPI_Comm *cFieldTile);
    explicit Player(int playerId, int playerRank, char t);
    ~Player();

    void PlayRound();
    void DetermineBallLocation();
    int *DetermineNearestPlayersToBall();
    int FindDistance(Position *pos1, Position *pos2);
    void MoveToPosition(Position *pos);
    void SendStatistics();
    Position * KickBall();
    void SendPosition(Position *pos);
    void SendPosition(Position *pos, int tileIdx);
    void JumpToDefaultPosition();
    void SetGameTactics(bool gameTactics);
    char ReceiveOutcome();

    int GetId() const;
    int GetRank() const;
    char GetTeam() const;
    Position *GetPosition() const;
    Statistics *GetStatistics() const;

    void SetPosition(Position *pos);

private:
    int id, rank;
    int speedSkill, dribblingSkill, kickSkill;
    int ballChallenge;
    bool isDefenceTactics;
    bool isGoalScored;
    char team;

    int ballDelta;
    Position *ballPosition;

    Position *position;
    Position *defaultDefencePosition;
    Position *defaultAttackPosition;
    Statistics *statistics;

    MPI_Comm *commTeam;
    MPI_Comm *commFieldTile;
};

#endif //PLAYER_H
