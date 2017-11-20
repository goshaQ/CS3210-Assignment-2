#ifndef BALL_H
#define BALL_H

#include "Global.h"

class Ball
{
public:
    explicit Ball();
    ~Ball();

    void JumpToDefaultPosition();
    void JumpToRandomPosition();

    Position *GetPosition() const;
    void SetPosition(Position *pos);

private:
    Position *defaultPosition;
    Position *position;

};

#endif //BALL_H
