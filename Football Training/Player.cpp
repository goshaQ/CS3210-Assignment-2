#include "Player.h"

Player::Player(int playerId, int playerRank) : id(playerId), rank(playerRank)
{
    position = new Position;
    position->x = rand() % FIELD_LENGTH;
    position->y = rand() % FIELD_WIDTH;

    statistics = new Statistics;
}

void Player::PlayRound()
{
    char buffer;

    Position *ballPos = DetermineBallLocation();
    MoveToBall(ballPos);
    SendPosition(position);

    MPI_Recv(&buffer, 1, MPI_CHAR, FIELD_PROCESS_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (buffer == 'W')
    {
        KickBall(ballPos);
        SendPosition(ballPos);
    }
}

Position * Player::DetermineBallLocation()
{
    auto *ballPos = new Position;
    MPI_Recv(ballPos, 1, POSITION, FIELD_PROCESS_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return ballPos;
}

void Player::MoveToBall(Position *ballPos)
{
    int stepCount = 0;
    bool isBallReached = false;

    while (stepCount++ < MAX_STEP_NUMBER && !isBallReached)
    {
        if (position->x < ballPos->x)
        {
            position->x++;
            goto finishStep;
        }

        if (position->y > ballPos->y)
        {
            position->y--;
            goto finishStep;
        }

        if (position->x > ballPos->x)
        {
            position->x--;
            goto finishStep;
        }

        if (position->y < ballPos->y)
        {
            position->y++;
            goto finishStep;
        }

        finishStep:
        if (position->x == ballPos->x && position->y == ballPos->y)
        {
            isBallReached = true;
        }
    }
}

void Player::SendPosition(Position *pos)
{
    MPI_Send(pos, 1, POSITION, FIELD_PROCESS_RANK, 0, MPI_COMM_WORLD);
}

void Player::KickBall(Position *ballPos)
{
    ballPos->x = rand() % FIELD_LENGTH;
    ballPos->y = rand() % FIELD_WIDTH;
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

void Player::SetPosition(Position *pos)
{
    delete position;
    position = pos;
}

Player::~Player()
{
    delete position;
    delete statistics;
}
