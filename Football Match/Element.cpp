#include <algorithm>
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

bool Element::Contains(const Player *player)
{
    return std::find(players->begin(), players->end(), player) != players->end();
}

std::list<const Player *> *Element::GetPlayers() const {
    return players;
}

Element::~Element()
{
    delete players;
}
