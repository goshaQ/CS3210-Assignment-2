#include "Ball.h"

Ball::Ball()
{
    position = new Position;
    position->x = rand() % FIELD_LENGTH;
    position->y = rand() % FIELD_WIDTH;
}

Position * Ball::GetPosition() const
{
    return position;
}

void Ball::SetPosition(Position *pos)
{
    if ((pos->x >= FIELD_LENGTH || pos->x < 0) && (pos->y >= FIELD_WIDTH || pos->y < 0))
    {
        pos->x = rand() % FIELD_LENGTH;
        pos->y = rand() % FIELD_WIDTH;
    }

    delete position;
    position = pos;
}

Ball::~Ball()
{
    delete position;
}