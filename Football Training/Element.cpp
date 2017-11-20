#include "Element.h"

Element::Element()
{
    players = new std::list<const Player *>;
}

void Element::RemovePlayer(const Player *player)
{
    players->remove(player);
}

void Element::AddPlayer(const Player *player)
{
    players->push_back(player);
}

std::list<const Player *> *Element::GetPlayers() const {
    return players;
}

Element::~Element()
{
    delete players;
}
