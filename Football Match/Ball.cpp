#include "Ball.h"

Ball::Ball()
{
    defaultPosition = new Position;
    defaultPosition->x = FIELD_LENGTH / 2;
    defaultPosition->y = FIELD_WIDTH / 2;

    position = new Position;
    position->x = defaultPosition->x;
    position->y = defaultPosition->y;
}

void Ball::JumpToDefaultPosition()
{
    position->x = defaultPosition->x;
    position->y = defaultPosition->y;
}

void Ball::JumpToRandomPosition()
{
    auto *ballPos = new Position;
    ballPos->x = rand() % FIELD_LENGTH;
    ballPos->y = rand() % FIELD_WIDTH;

    SetPosition(ballPos);
}

Position * Ball::GetPosition() const
{
    return position;
}

void Ball::SetPosition(Position *pos)
{
    delete position;
    position = pos;
}

Ball::~Ball()
{
    delete position;
}