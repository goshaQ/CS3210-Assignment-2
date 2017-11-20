#ifndef STATISTICS_H
#define STATISTICS_H

#include "Global.h"

class Statistics
{
public:
    explicit Statistics();
    ~Statistics();

    void BallReached();
    void BallWon();
    void BallNotReached();
    void BallNotWon();
    void SetInitPosition(Position *initialPos);
    void SetFinalPosition(Position *finalPos);
    void AddDistance();

    int GetDistanceTraveledSinceTheBeginning() const;
    int GetBallWasReachedSinceTheBeginningCount() const;
    int GetBallWasWonSinceTheBeginningCount() const;

    bool IsBallReached() const;
    bool IsBallWon() const;

    Position *GetInitialPosition() const;
    Position *GetFinalPosition() const;

    Stat * GetStruct() const;

private:
    int distanceTraveledSinceTheBeginning;
    int ballWasReachedSinceTheBeginningCount;
    int ballWasWonSinceTheBeginningCount;

    bool ballWasReached;
    bool ballWasWon;

    Position *initialPosition;
    Position *finalPosition;
};

#endif //STATISTICS_H
