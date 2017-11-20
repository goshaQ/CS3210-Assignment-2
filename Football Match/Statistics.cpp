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

Stat *Statistics::GetStruct() const {
    auto *statistics = new Stat;

    statistics->distanceTraveledSinceTheBeginning = distanceTraveledSinceTheBeginning;
    statistics->ballWasReachedSinceTheBeginningCount = ballWasReachedSinceTheBeginningCount;
    statistics->ballWasWonSinceTheBeginningCount = ballWasWonSinceTheBeginningCount;

    statistics->ballWasReached = ballWasReached;
    statistics->ballWasWon = ballWasWon;

    statistics->initialPositionX = initialPosition->x;
    statistics->initialPositionY = initialPosition->y;
    statistics->finalPositionX = finalPosition->x;
    statistics->finalPositionY = finalPosition->y;

    return statistics;
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

void Statistics::SetFinalPosition(Position *finalPos) {
    finalPosition->x = finalPos->x;
    finalPosition->y = finalPos->y;
}

void Statistics::SetInitPosition(Position *initialPos) {
    initialPosition->x = initialPos->x;
    initialPosition->y = initialPos->y;
}

void Statistics::AddDistance() {
    distanceTraveledSinceTheBeginning += std::abs(initialPosition->x - finalPosition->x) + std::abs(initialPosition->y - finalPosition->y);
}
