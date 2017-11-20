#ifndef BALL_H
#define BALL_H

#include "Global.h"

class Ball
{
public:
    explicit Ball();
    ~Ball();

    Position *GetPosition() const;
    void SetPosition(Position *pos);

private:
    Position *position;

};

#endif //BALL_H
