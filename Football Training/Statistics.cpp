#include "Statistics.h"

Statistics::Statistics()
{
    distanceTraveledSinceTheBeginning = 0;
    ballWasReachedSinceTheBeginningCount = 0;
    ballWasWonSinceTheBeginningCount = 0;

    ballWasReached = false;
    ballWasWon = false;

    initialPosition = new Position;
    finalPosition = new Position;
}

void Statistics::BallReached() {
    ballWasReached = true;
    ballWasReachedSinceTheBeginningCount++;
}

void Statistics::BallWon() {
    ballWasWon = true;
    ballWasWonSinceTheBeginningCount++;
}

void Statistics::BallNotReached()
{
    ballWasReached = false;
}

void Statistics::BallNotWon()
{
    ballWasWon = false;
}

int Statistics::GetDistanceTraveledSinceTheBeginning() const {
    return distanceTraveledSinceTheBeginning;
}

int Statistics::GetBallWasReachedSinceTheBeginningCount() const {
    return ballWasReachedSinceTheBeginningCount;
}

int Statistics::GetBallWasWonSinceTheBeginningCount() const {
    return ballWasWonSinceTheBeginningCount;
}

void Statistics::SetPosition(Position *initialPos, Position *finalPos) {
    initialPosition->x = initialPos->x;
    initialPosition->y = initialPos->y;

    finalPosition->x = finalPos->x;
    finalPosition->y = finalPos->y;

    distanceTraveledSinceTheBeginning += std::abs(initialPosition->x - finalPos->x) + std::abs(initialPosition->y - finalPos->y);
}

bool Statistics::IsBallReached() const {
    return ballWasReached;
}

bool Statistics::IsBallWon() const {
    return ballWasWon;
}

Position *Statistics::GetInitialPosition() const {
    return initialPosition;
}

Position *Statistics::GetFinalPosition() const {
    return finalPosition;
}

Statistics::~Statistics()
{
    delete initialPosition;
    delete finalPosition;
}
