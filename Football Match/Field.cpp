#include <iosfwd>
#include <sstream>
#include <iostream>

#include "Field.h"

Field::Field(int rank, MPI_Comm *commF, MPI_Comm *commTA, MPI_Comm *commTB)
{
    field = new Element **[FIELD_TILE_SIDE];
    for (int i = 0; i < FIELD_TILE_SIDE; ++i)
    {
        field[i] = new Element *[FIELD_TILE_SIDE];
        for (int j = 0; j < FIELD_TILE_SIDE; ++j)
        {
            field[i][j] = new Element();
        }
    }

    fieldTileId = rank;

    commField = commF;
    commTeamA = commTA;
    commTeamB = commTB;

    teamA = new Player *[TEAM_SIZE];
    teamB = new Player *[TEAM_SIZE];
    if (fieldTileId == FIELD_TILE_NUMBER / 2) ball = new Ball();
}

Position * Field::ReceiveTeamPositions(const MPI_Comm *comm)
{
    auto *recvBuffer = new Position[TEAM_SIZE + 1];

    MPI_Gather(MPI_IN_PLACE, NULL, NULL, recvBuffer, 1, POSITION, 0, *comm);
    return recvBuffer;
}

void Field::UpdateTeamPosition(Position *positions, Player **team)
{
    Position *position;
    Player *player;

    for (int id = 0, rank = 0; id < TEAM_SIZE; ++id) {
        rank = id + 1;

        position = &positions[rank];
        player = team[id];

        if (position->x == (&invalidPosition)->x && position->y == (&invalidPosition)->y)
        {
            if (player != nullptr)
            {
                field[player->GetPosition()->y % FIELD_TILE_SIDE][player->GetPosition()->x % FIELD_TILE_SIDE]->RemovePlayer(player);

                delete team[id];
                team[id] = nullptr;
            }

            continue;
        }

        if (player == nullptr)
        {
            team[id] = new Player(id, rank, (team == teamA) ? 'A' : 'B');
            player = team[id];
        }

        Move(player, position);
        player->SetPosition(position);
    }
}

void Field::Reset()
{
    if (ball != nullptr)
        ball->JumpToDefaultPosition();

    for (int id = 0; id < TEAM_SIZE; ++id) {
        if (teamA[id] != nullptr)
        {
            field[teamA[id]->GetPosition()->y % FIELD_TILE_SIDE][teamA[id]->GetPosition()->x % FIELD_TILE_SIDE]->RemovePlayer(teamA[id]);

            delete teamA[id];
            teamA[id] = nullptr;
        }

        if (teamB[id] != nullptr)
        {
            field[teamB[id]->GetPosition()->y % FIELD_TILE_SIDE][teamB[id]->GetPosition()->x % FIELD_TILE_SIDE]->RemovePlayer(teamB[id]);

            delete teamB[id];
            teamB[id] = nullptr;
        }
    }

    EnsureBallIntegrity();
}

Stat * Field::ReceiveStatistics(MPI_Comm *comm)
{
    auto *statTeam = new Stat[TEAM_SIZE + 1];

    MPI_Gather(MPI_IN_PLACE, NULL, NULL, statTeam, 1, STAT, 0, *comm);
    return statTeam;
}

void Field::PrintStatistics(int round, Stat *statTeamA, Stat *statTeamB)
{
    std::stringstream stream;

    stream << round << std::endl;
    stream << statTeamA[1].ballPositionX << " " << statTeamA[1].ballPositionY << std::endl;

    for (int id = 1; id < TEAM_SIZE + 1; ++id)
    {
        stream << (id - 1) << " " << statTeamA[id].initialPositionX << " " << statTeamA[id].initialPositionY << " ";
        stream << statTeamA[id].finalPositionX << " " << statTeamA[id].finalPositionY << " ";
        stream<< (int) statTeamA[id].ballWasReached << " " << (int) statTeamA[id].ballWasWon << " ";
        stream << statTeamA[id].distanceTraveledSinceTheBeginning << " ";
        stream << statTeamA[id].ballWasReachedSinceTheBeginningCount << " ";
        stream << statTeamA[id].ballWasWonSinceTheBeginningCount << " ";
        stream << statTeamA[id].ballChallenge << std::endl;
    }

    for (int id = 1; id < TEAM_SIZE + 1; ++id) {
        stream << (id - 1) << " " << statTeamB[id].initialPositionX << " " << statTeamB[id].initialPositionY << " ";
        stream << statTeamB[id].finalPositionX << " " << statTeamB[id].finalPositionY << " ";
        stream << (int) statTeamB[id].ballWasReached << " " << (int) statTeamB[id].ballWasWon << " ";
        stream << statTeamB[id].distanceTraveledSinceTheBeginning << " ";
        stream << statTeamB[id].ballWasReachedSinceTheBeginningCount << " ";
        stream << statTeamB[id].ballWasWonSinceTheBeginningCount << " ";
        stream << statTeamB[id].ballChallenge << std::endl;
    }

    std::cout << stream.str();
}

void Field::SendBallLocation()
{
    Position *ballPos;

    if (isGoalScored)
    {
        ballPos = &goalPosition;
        isGoalScored = false;
    } else if (ball == nullptr)
    {
        ballPos = &invalidPosition;
    } else
    {
        ballPos = ball->GetPosition();
    }

    MPI_Bcast(ballPos, 1, POSITION, 0, *commTeamA);
    MPI_Bcast(ballPos, 1, POSITION, 0, *commTeamB);
}

void Field::Move(Player *player, const Position *destPos)
{
    if (player->GetPosition()->x != (&invalidPosition)->x && player->GetPosition()->y != (&invalidPosition)->y)
    {
        field[player->GetPosition()->y % FIELD_TILE_SIDE][player->GetPosition()->x % FIELD_TILE_SIDE]->RemovePlayer(player);
    }

    field[destPos->y % FIELD_TILE_SIDE][destPos->x % FIELD_TILE_SIDE]->AddPlayer(player);
}

void Field::SendOutcome(int ballWinnerId)
{
    auto *sendTeamA = new char[TEAM_SIZE + 1], *sendTeamB = new char[TEAM_SIZE + 1];

    if (ballWinnerId == -1)
    {
        std::fill(sendTeamA, sendTeamA + TEAM_SIZE + 1, 'L');
        std::fill(sendTeamB, sendTeamB + TEAM_SIZE + 1, 'L');
    } else if (ballWinnerId == INT32_MIN)
    {
        std::fill(sendTeamA, sendTeamA + TEAM_SIZE + 1, 'C');
        std::fill(sendTeamB, sendTeamB + TEAM_SIZE + 1, 'C');
    } else
    {
        if (ballWinnerId < TEAM_SIZE)
        {
            std::fill(sendTeamA, sendTeamA + TEAM_SIZE + 1, 'T');
            std::fill(sendTeamB, sendTeamB + TEAM_SIZE + 1, 'L');
            sendTeamA[ballWinnerId + 1] = 'W';
        } else
        {
            std::fill(sendTeamA, sendTeamA + TEAM_SIZE + 1, 'L');
            std::fill(sendTeamB, sendTeamB + TEAM_SIZE + 1, 'T');
            sendTeamB[ballWinnerId - TEAM_SIZE + 1] = 'W';
        }
    }

    MPI_Scatter(sendTeamA, 1, MPI_CHAR, MPI_IN_PLACE, NULL, NULL, 0, *commTeamA);
    MPI_Scatter(sendTeamB, 1, MPI_CHAR, MPI_IN_PLACE, NULL, NULL, 0, *commTeamB);

    delete[] sendTeamA;
    delete[] sendTeamB;
}

int Field::DetermineBallWinner()
{
    std::list<const Player *> *players = field[ball->GetPosition()->y % FIELD_TILE_SIDE][ball->GetPosition()->x % FIELD_TILE_SIDE]->GetPlayers();
    if (players->empty())
    {
        return -1;
    } else if (players->size() == 1)
    {
        return (players->front()->GetTeam() == 'A')
               ? players->front()->GetId()
               : players->front()->GetId() + TEAM_SIZE;
    }

    SendOutcome(INT32_MIN);

    auto *teamABallChallenges = new int[TEAM_SIZE + 1], *teamBBallChallenges = new int[TEAM_SIZE + 1];
    int ballWinner, teamABallWinner = 1, teamBBallWinner = 1;

    MPI_Gather(MPI_IN_PLACE, NULL, NULL, teamABallChallenges, 1, MPI_INT, 0, *commTeamA);
    MPI_Gather(MPI_IN_PLACE, NULL, NULL, teamBBallChallenges, 1, MPI_INT, 0, *commTeamB);

    for (int i = 1; i < TEAM_SIZE + 1; ++i) {
        if (teamABallChallenges[teamABallWinner] < teamABallChallenges[i])
        {
            teamABallWinner = i;
        }

        if (teamBBallChallenges[teamBBallWinner] < teamBBallChallenges[i])
        {
            teamBBallWinner = i;
        }
    }

    ballWinner = (teamABallChallenges[teamABallWinner] > teamBBallChallenges[teamBBallWinner])
                 ? teamABallWinner - 1
                 : teamBBallWinner + TEAM_SIZE - 1;

    delete[] teamABallChallenges;
    delete[] teamBBallChallenges;

    return ballWinner;
}

Position *Field::ReceiveBallPosition(int ballWinnerId)
{
    auto *recvTeam = new Position[TEAM_SIZE + 1];
    auto *ballPos = new Position;

    MPI_Gather(MPI_IN_PLACE, NULL, NULL, recvTeam, 1, POSITION, 0, (ballWinnerId < TEAM_SIZE) ? *commTeamA : *commTeamB);

    ballPos->x = recvTeam[(ballWinnerId < TEAM_SIZE) ? ballWinnerId + 1 : ballWinnerId - TEAM_SIZE + 1].x;
    ballPos->y = recvTeam[(ballWinnerId < TEAM_SIZE) ? ballWinnerId + 1 : ballWinnerId - TEAM_SIZE + 1].y;

    delete[] recvTeam;
    return ballPos;
}

void Field::UpdateBallPosition(Position *position) {
    if ((position->x >= FIELD_LENGTH || position->x < 0) && (42 < position->y && position->y < 52))
    {
        ball->JumpToDefaultPosition();

        isGoalScored = true;
        return;
    }

    if ((position->x >= FIELD_LENGTH || position->x < 0) || (position->y >= FIELD_WIDTH || position->y < 0))
    {
        ball->JumpToRandomPosition();
    } else
    {
        ball->SetPosition(position);
    }
}

void Field::EnsureBallIntegrity()
{
    int yLB, yUB, xLB, xUB;
    auto *ballPositions = new Position[FIELD_TILE_NUMBER];

    MPI_Allgather((ball != nullptr) ? ball->GetPosition() : &invalidPosition, 1, POSITION, ballPositions, 1, POSITION, *commField);

    delete ball;
    ball = nullptr;

    for (int i = 0; i < FIELD_TILE_NUMBER; ++i)
    {
        yLB = (fieldTileId / 4) * FIELD_TILE_SIDE;
        yUB = yLB + FIELD_TILE_SIDE;
        xLB = (fieldTileId % 4) * FIELD_TILE_SIDE;
        xUB = xLB + FIELD_TILE_SIDE;

        if (((yLB <= ballPositions[i].y) && (ballPositions[i].y < yUB)) &&
                ((xLB <= ballPositions[i].x) && (ballPositions[i].x < xUB)))
        {
            auto *ballPosition = new Position;
            ballPosition->x = ballPositions[i].x;
            ballPosition->y = ballPositions[i].y;

            ball = new Ball;
            ball->SetPosition(ballPosition);

            break;
        }
    }

    delete[] ballPositions;
}

void Field::PlayRound()
{
    int ballWinnerId;
    Position *posTeamA, *posTeamB, *position;

    SendBallLocation();

    posTeamA = ReceiveTeamPositions(commTeamA);
    posTeamB = ReceiveTeamPositions(commTeamB);

    UpdateTeamPosition(posTeamA, teamA);
    UpdateTeamPosition(posTeamB, teamB);

    if (ball != nullptr)
    {
        ballWinnerId = DetermineBallWinner();
        SendOutcome(ballWinnerId);

        if (ballWinnerId != -1)
        {
            position = ReceiveBallPosition(ballWinnerId);
            UpdateBallPosition(position);
        }
    }

    EnsureBallIntegrity();

    delete[] posTeamA;
    delete[] posTeamB;
}

Player ** Field::GetTeamA() const {
    return teamA;
}

Player ** Field::GetTeamB() const {
    return teamB;
}


Field::~Field()
{
    for (int i = 0; i < FIELD_TILE_SIDE; ++i)
    {
        delete[] field[i];
    }

    delete[] field;

    delete[] teamA;
    delete[] teamB;
    delete ball;
}
