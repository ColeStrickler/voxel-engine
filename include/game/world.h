#ifndef WORLD_H
#define WORLD_H
#include <vector>
#include "player.h"



enum Biome
{
    Mixed,
};



typedef struct WorldStats
{

}WorldStats;


/*
    World 
*/
class World
{
public:
    World();
    ~World();
    // TextureMap* 
private:
    std::vector<Player> m_ActivePlayers;
};



#endif