#include <iosfwd>
#include <sstream>
#include <iostream>
#include "Field.h"

Field::Field()
{
    int rank;

    field = new Element **[FIELD_WIDTH];
    for (int i = 0; i < FIELD_WIDTH; ++i)
    {
        field[i] = new Element *[FIELD_LENGTH];
        for (int j = 0; j < FIELD_LENGTH; ++j)
        {
            field[i][j] = new Element();
        }
    }

    team = new Player *[TEAM_SIZE];
    for (int id = 0; id < TEAM_SIZE; ++id)
    {
        rank = id + 1;

        team[id] = new Player(id, rank);
    }

    ball = new Ball;
}

void Field::Move(Player *player, const Position *destPos)
{
    field[player->GetPosition()->y][player->GetPosition()->x]->RemovePlayer(player);
    field[destPos->y][destPos->x]->AddPlayer(player);
}

void Field::UpdatePlayerPositions(Position **positions)
{
    Position *position;
    Player *player;

    for (int id = 0; id < TEAM_SIZE; ++id)
    {
        position = positions[id];
        player = team[id];

        if (position->x == ball->GetPosition()->x && position->y == ball->GetPosition()->y)
        {
            player->GetStatistics()->BallReached();
        } else
        {
            player->GetStatistics()->BallNotReached();
        }

        player->GetStatistics()->BallNotWon();
        player->GetStatistics()->SetPosition(player->GetPosition(), position);

        Move(player, position);
        player->SetPosition(position);
    }
}

void Field::SendBallLocation()
{
    for (int id = 0; id < TEAM_SIZE; ++id)
    {
        MPI_Send(ball->GetPosition(), 1, POSITION, team[id]->GetRank(), 0, MPI_COMM_WORLD);
    }
}

void Field::SendOutcome(int ballWinnerId)
{
    char buffer;

    for (int id = 0; id < TEAM_SIZE; ++id)
    {
        buffer = (id == ballWinnerId) ? 'W' : 'L';
        MPI_Send(&buffer, 1, MPI_CHAR, team[id]->GetRank(), 1, MPI_COMM_WORLD);
    }
}

int Field::DetermineBallWinner()
{
    int ballWinnerId;
    Position *ballPos;
    std::list<const Player *> *players;
    std::list<const Player *>::iterator iterator;

    ballPos = ball->GetPosition();
    players = field[ballPos->y][ballPos->x]->GetPlayers();

    switch (players->size())
    {
        case 0:
            ballWinnerId = -1;
            break;
        case 1:
            iterator = players->begin();

            (*iterator)->GetStatistics()->BallWon();
            ballWinnerId = (*iterator)->GetId();
            break;
        default:
            iterator = players->begin();
            std::advance(iterator, rand() % (players->size() - 1));

            (*iterator)->GetStatistics()->BallWon();
            ballWinnerId = (*iterator)->GetId();
    }

    return ballWinnerId;
}

Position ** Field::ReceivePlayerPositions()
{
    auto **positions = new Position *[TEAM_SIZE];
    Position *pos;

    for (int id = 0; id < TEAM_SIZE; ++id)
    {
        pos = new Position;

        MPI_Recv(pos, 1, POSITION, team[id]->GetRank(), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        positions[id] = pos;
    }

    return positions;
}

void Field::UpdateBallPosition(Position *position) {
    ball->SetPosition(position);
}

Position *Field::ReceiveBallPosition(int ballWinnerId) {
    auto *pos = new Position;

    MPI_Recv(pos, 1, POSITION, team[ballWinnerId]->GetRank(), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return pos;
}


void Field::PrintStatistics(int round) {
    std::stringstream stream;

    stream << round << std::endl;
    stream << ball->GetPosition()->x << " " << ball->GetPosition()->y << std::endl;

    for (int id = 0; id < TEAM_SIZE; ++id)
    {
        Statistics *statistics = team[id]->GetStatistics();
        stream << id << " " << statistics->GetInitialPosition()->x << " " << statistics->GetInitialPosition()->y << " ";
        stream << statistics->GetFinalPosition()->x << " " << statistics->GetFinalPosition()->y << " ";
        stream<< (int) statistics->IsBallReached() << " " << (int) statistics->IsBallWon() << " ";
        stream << statistics->GetDistanceTraveledSinceTheBeginning() << " ";
        stream << statistics->GetBallWasReachedSinceTheBeginningCount() << " ";
        stream << statistics->GetBallWasWonSinceTheBeginningCount() << std::endl;
    }

    std::cout << stream.str();
}

void Field::PlayRound() {
    int ballWinnerId;
    Position **positions, *position;

    SendBallLocation();

    positions = ReceivePlayerPositions();
    UpdatePlayerPositions(positions);

    ballWinnerId = DetermineBallWinner();
    SendOutcome(ballWinnerId);

    if (ballWinnerId != -1)
    {
        position = ReceiveBallPosition(ballWinnerId);
        UpdateBallPosition(position);
    }
}

Field::~Field()
{
    for (int i = 0; i < FIELD_WIDTH; ++i)
    {
        delete[] field[i];
    }

    delete[] field;

    delete[] team;
    delete ball;
}

