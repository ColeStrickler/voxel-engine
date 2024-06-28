#ifndef MOB_H
#define MOB_H

enum MobType
{
    CodFish,
    CatFish,
    Squirrel,
    Zombie
};

/*
    We will have an event system, may want to set timers such that when a timer goes off, we will wake up a thread, do the computation and then
    display the results in our render loop. For example: we have a Mob that will move about every 2s, we set a timer for 2s and when that goes off, 
    a pool of worker threads will grab the timer, have the mob go through the animation
*/

class Mob
{
public:
    Mob();
    ~Mob();
private:
    int m_Health;
};



#endif