#ifndef ELEMENT_H
#define ELEMENT_H

#include "Player.h"
#include <list>

class Element
{
public:
    explicit Element();
    ~Element();

    void RemovePlayer(const Player *participant);
    void AddPlayer(const Player *participant);

    std::list<const Player *> * GetPlayers() const;
private:
    std::list<const Player *> *players = nullptr;
};

#endif //ELEMENT_H
