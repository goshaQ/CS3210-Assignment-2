#include <algorithm>
#include <iterator>
#include <iostream>
#include <sstream>

#include "Player.h"

Player::Player(int playerId, int playerRank, char t, MPI_Comm *cTeam, MPI_Comm *cFieldTile) : id(playerId), rank(playerRank)
{
    defaultDefencePosition = new Position;
    defaultAttackPosition = new Position;
    position = new Position;
    statistics = new Statistics;
    commTeam = cTeam;
    commFieldTile = cFieldTile;
    team = t;

    ballPosition = nullptr;
    ballDelta = 0;

    isGoalScored = false;

    position->x = (&invalidPosition)->x;
    position->y = (&invalidPosition)->y;

    speedSkill = (rand() % MAX_PLAYER_SKILL) + 1;
    speedSkill = (speedSkill > 10) ? 10 : speedSkill;

    dribblingSkill = (rand() % (MAX_PLAYER_SKILL - speedSkill)) + 1;
    dribblingSkill = (dribblingSkill > 10) ? 10 : dribblingSkill;
    dribblingSkill = (MAX_PLAYER_SKILL - speedSkill - dribblingSkill > 10) ? MAX_PLAYER_SKILL - speedSkill - 10 : dribblingSkill;
    dribblingSkill = (MAX_PLAYER_SKILL - speedSkill - dribblingSkill > 0) ? dribblingSkill : dribblingSkill - 1;

    kickSkill = MAX_PLAYER_SKILL - speedSkill - dribblingSkill;

    switch (playerId)
    {
        case 0:
            defaultDefencePosition->x = (team == 'A') ? 16 : FIELD_LENGTH - 16;
            defaultDefencePosition->y = 21;

            defaultAttackPosition->x = (team == 'A') ? 40 : FIELD_LENGTH - 40;
            defaultAttackPosition->y = 24;
            break;
        case 1:
            defaultDefencePosition->x = (team == 'A') ? 16 : FIELD_LENGTH - 16;
            defaultDefencePosition->y = 39;

            defaultAttackPosition->x = (team == 'A') ? 40 : FIELD_LENGTH - 40;
            defaultAttackPosition->y = 48;
            break;
        case 2:
            defaultDefencePosition->x = (team == 'A') ? 16 : FIELD_LENGTH - 16;
            defaultDefencePosition->y = 57;

            defaultAttackPosition->x = (team == 'A') ? 40 : FIELD_LENGTH - 40;
            defaultAttackPosition->y = 72;
            break;
        case 3:
            defaultDefencePosition->x = (team == 'A') ? 16 : FIELD_LENGTH - 16;
            defaultDefencePosition->y = 75;

            defaultAttackPosition->x = (team == 'A') ? 48 : FIELD_LENGTH - 48;
            defaultAttackPosition->y = 24;
            break;
        case 4:
            defaultDefencePosition->x = (team == 'A') ? 32 : FIELD_LENGTH - 32;
            defaultDefencePosition->y = 21;

            defaultAttackPosition->x = (team == 'A') ? 48 : FIELD_LENGTH - 48;
            defaultAttackPosition->y = 48;
            break;
        case 5:
            defaultDefencePosition->x = (team == 'A') ? 32 : FIELD_LENGTH - 32;
            defaultDefencePosition->y = 39;

            defaultAttackPosition->x = (team == 'A') ? 48 : FIELD_LENGTH - 48;
            defaultAttackPosition->y = 72;
            break;
        case 6:
            defaultDefencePosition->x = (team == 'A') ? 32 : FIELD_LENGTH - 32;
            defaultDefencePosition->y = 57;

            defaultAttackPosition->x = (team == 'A') ? 56 : FIELD_LENGTH - 56;
            defaultAttackPosition->y = 16;
            break;
        case 7:
            defaultDefencePosition->x = (team == 'A') ? 32 : FIELD_LENGTH - 32;
            defaultDefencePosition->y = 75;

            defaultAttackPosition->x = (team == 'A') ? 56 : FIELD_LENGTH - 56;
            defaultAttackPosition->y = 32;
            break;
        case 8:
            defaultDefencePosition->x = (team == 'A') ? 48 : FIELD_LENGTH - 48;
            defaultDefencePosition->y = 32;

            defaultAttackPosition->x = (team == 'A') ? 56 : FIELD_LENGTH - 56;
            defaultAttackPosition->y = 48;
            break;
        case 9:
            defaultDefencePosition->x = (team == 'A') ? 48 : FIELD_LENGTH - 48;
            defaultDefencePosition->y = 64;

            defaultAttackPosition->x = (team == 'A') ? 56 : FIELD_LENGTH - 56;
            defaultAttackPosition->y = 64;
            break;
        case 10:
            defaultDefencePosition->x = (team == 'A') ? 56 : FIELD_LENGTH - 56;
            defaultDefencePosition->y = 48;

            defaultAttackPosition->x = (team == 'A') ? 56 : FIELD_LENGTH - 56;
            defaultAttackPosition->y = 80;
            break;
    }
}

Player::Player(int playerId, int playerRank, char t) : id(playerId), rank(playerRank)
{
    position = new Position;
    statistics = nullptr;
    team = t;

    position->x = (&invalidPosition)->x;
    position->y = (&invalidPosition)->y;
}

void Player::SendPosition(Position *pos) {
    int tileIdx;

    tileIdx = (pos->y / FIELD_TILE_SIDE) * 4 + (pos->x / FIELD_TILE_SIDE);
    for (int i = 0; i < FIELD_TILE_NUMBER; ++i) {
        MPI_Gather((i == tileIdx) ? pos : &invalidPosition, 1, POSITION, NULL, NULL, NULL, 0, commFieldTile[i]);
    }
}

void Player::SendPosition(Position *pos, int tileIdx)
{
    MPI_Gather(pos, 1, POSITION, NULL, NULL, NULL, 0, commFieldTile[tileIdx]);
}

void Player::JumpToDefaultPosition() {
    position->x = (isDefenceTactics) ? defaultDefencePosition->x : defaultAttackPosition->x;
    position->y = (isDefenceTactics) ? defaultDefencePosition->y : defaultAttackPosition->y;
}

void Player::SetGameTactics(bool gameTactics)
{
    isDefenceTactics = gameTactics;
}

void Player::PlayRound()
{
    int distance, tileIdx, *nearestPlayers;
    char outcome;
    Position *newBallPosition;

    statistics->SetInitPosition(position);

    DetermineBallLocation();

    if (isGoalScored)
    {
        JumpToDefaultPosition();
        isGoalScored = false;

        if (id == 0)
            printf("GOAL\n");
    }

    if (isDefenceTactics)
    {
        // If player is defender of a zone, depending on whether the ball is currently within that zone,
        // either he moves to the ball, either he moves to the center of a zone
        if (0 <= id && id < TEAM_SIZE - MAX_RUNNING_PLAYERS)
        {
            distance = FindDistance(defaultDefencePosition, ballPosition);
            if (distance <= 18)
            {
                MoveToPosition(ballPosition);
            } else
            {
                distance = FindDistance(defaultDefencePosition, position);
                if (distance != 0)
                {
                    MoveToPosition(defaultDefencePosition);
                }
            }
        // If player is not assigned to a specific zone, just move to the ball
        } else if (TEAM_SIZE - MAX_RUNNING_PLAYERS <= id && id < TEAM_SIZE)
        {
            MoveToPosition(ballPosition);
        }
    } else
    {
        // If player is one of the nearest players to the ball he moves towards the ball
        nearestPlayers = DetermineNearestPlayersToBall();
        if (std::find(nearestPlayers, nearestPlayers + MAX_RUNNING_PLAYERS, id) != nearestPlayers + MAX_RUNNING_PLAYERS)
        {
            MoveToPosition(ballPosition);
        // If player is not of the nearest players he moves among axis X
        } else
        {
            int tmp;
            tmp = (ballDelta + position->x >= 0) ? ballDelta + position->x : position->x;
            tmp = (tmp < FIELD_LENGTH) ? tmp : position->x;

            Position tmpPosition = {tmp, position->y};
            MoveToPosition(&tmpPosition);
        }
    }

    SendPosition(position);

    statistics->SetFinalPosition(position);
    statistics->AddDistance();
    statistics->BallNotWon();
    if (position->x == ballPosition->x && position->y == ballPosition->y)
    {
        statistics->BallReached();
    } else
    {
        statistics->BallNotReached();
    }

    outcome = ReceiveOutcome();
    if (outcome == 'T')
    {
        // Send invalid position just to let one of the teammates send new ball position
        tileIdx = (ballPosition->y / FIELD_TILE_SIDE) * 4 + (ballPosition->x / FIELD_TILE_SIDE);

        SendPosition(&invalidPosition, tileIdx);
    } else if (outcome == 'W')
    {
        statistics->BallWon();

        // Kick and send new ball position
        tileIdx = (ballPosition->y / FIELD_TILE_SIDE) * 4 + (ballPosition->x / FIELD_TILE_SIDE);

        newBallPosition = KickBall();
        SendPosition(newBallPosition, tileIdx);
        delete newBallPosition;
    }
}

void Player::DetermineBallLocation()
{
    auto *recvBuff = new Position;

    for (int i = 0; i < FIELD_TILE_NUMBER; ++i) {
        MPI_Bcast(recvBuff, 1, POSITION, 0, commFieldTile[i]);

        if (recvBuff->x != (&invalidPosition)->x && recvBuff->y != (&invalidPosition)->y)
        {
            if (recvBuff->x == (&goalPosition)->x && recvBuff->y == (&goalPosition)->y)
            {
                isGoalScored = true;
            } else
            {
                if (ballPosition == nullptr)
                {
                    ballPosition = new Position;
                } else
                {
                    ballDelta = ballPosition->x - recvBuff->x;
                }

                ballPosition->x = recvBuff->x;
                ballPosition->y = recvBuff->y;
            }
        }
    }
}

int * Player::DetermineNearestPlayersToBall()
{
    int sendBuffer = FindDistance(position, ballPosition), *recvBuffer = new int[TEAM_SIZE];
    auto *res = new int[MAX_RUNNING_PLAYERS];

    MPI_Allgather(&sendBuffer, 1, MPI_INT, recvBuffer, 1, MPI_INT, *commTeam);

    std::fill(res, res + MAX_RUNNING_PLAYERS, 0);
    for (int i = 0; i < TEAM_SIZE; ++i)
    {
        for (int j = 0; j < MAX_RUNNING_PLAYERS; ++j) {
            if (recvBuffer[res[j]] > recvBuffer[i])
            {
                res[j] = i;
                break;
            }
        }
    }

    return res;
}


int Player::FindDistance(Position *pos1, Position *pos2)
{
    return std::abs(pos1->x - pos2->x) + std::abs(pos1->y - pos2->y);
}

void Player::MoveToPosition(Position *pos)
{
    int stepCount = 0;
    bool isPositionReached = false;

    while (stepCount++ < speedSkill && !isPositionReached)
    {
        if (position->x < pos->x)
        {
            position->x++;
            goto finishStep;
        }

        if (position->y > pos->y)
        {
            position->y--;
            goto finishStep;
        }

        if (position->x > pos->x)
        {
            position->x--;
            goto finishStep;
        }

        if (position->y < pos->y)
        {
            position->y++;
            goto finishStep;
        }

        finishStep:
        if (position->x == pos->x && position->y == pos->y)
        {
            isPositionReached = true;
        }
    }
}

Position * Player::KickBall()
{
    int x, y;
    auto *ballPos = new Position;
    ballPos->x = ballPosition->x;
    ballPos->y = ballPosition->y;

    if (isDefenceTactics)
    {
        if (id == 1 || id == 2 || id == 5 || id == 6 || id == 8 || id == 9 || id == 10)
        {
            x = (2 * kickSkill) / 2 + (2 * kickSkill) % 2;
            y = (2 * kickSkill) / 2;

            ballPos->x = (team == 'A') ? ballPosition->x + x : ballPosition->x - x;
            ballPos->y = (id == 1 || id == 5 || id == 8) ? ballPosition->y - y : ballPosition->y + y;
        } else
        {
            x = (2  * kickSkill);

            ballPos->x = (team == 'A') ? ballPosition->x + x : ballPosition->x -x;
            ballPos->y = ballPosition->y;
        }
    } else {
        x = (42 < ballPos->y && ballPos->y < 52)
            ? (2 * kickSkill)
            : rand() % (2 * kickSkill) + 1;
        y = (x == (2 * kickSkill))
            ? 0
            : rand() % (2 * kickSkill - x) + 1;

        ballPos->x = (team == 'A') ? ballPosition->x + x : ballPosition->x - x;
        ballPos->y = (42 > ballPos->y) ? ballPosition->y + y : ballPosition->y - y;
    }

    return ballPos;
}

char Player::ReceiveOutcome()
{
    int tileIdx;
    char outcome;

    tileIdx = (ballPosition->y / FIELD_TILE_SIDE) * 4 + (ballPosition->x / FIELD_TILE_SIDE);
    MPI_Scatter(NULL, NULL, NULL, &outcome, 1, MPI_CHAR, 0, commFieldTile[tileIdx]);

    if (outcome == 'C')
    {
        ballChallenge = (position->x == ballPosition->x && position->y == ballPosition->y)
                ? dribblingSkill * ((rand() % 10) + 1)
                : -1;

        MPI_Gather(&ballChallenge, 1, MPI_INT, NULL, NULL, NULL, 0, commFieldTile[tileIdx]);
        outcome = ReceiveOutcome();
    }

    return outcome;
}

void Player::SendStatistics()
{
    Stat *stat = statistics->GetStruct();

    stat->ballChallenge = ballChallenge;
    stat->ballPositionX = ballPosition->x;
    stat->ballPositionY = ballPosition->y;

    MPI_Gather(stat, 1, STAT, NULL, NULL, NULL, 0, commFieldTile[0]);
    delete stat;
}

Position *Player::GetPosition() const
{
    return position;
}


Statistics *Player::GetStatistics() const {
    return statistics;
}

int Player::GetId() const
{
    return id;
}

int Player::GetRank() const
{
    return rank;
}

char Player::GetTeam() const
{
    return team;
}

void Player::SetPosition(Position *pos)
{
    position->x = pos->x;
    position->y = pos->y;
}

Player::~Player()
{
    delete position;
    delete statistics;
}
