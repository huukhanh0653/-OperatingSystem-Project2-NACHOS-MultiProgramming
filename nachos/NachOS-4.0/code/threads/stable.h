#ifndef STABLE_H
#define STABLE_H

#include "synch.h"
#include "map.h"

#define MAX_SEMAPHORE 10

//* SEMAPHORE TABLE
class STable
{
private:
    // Manager empty slot
    Map *map;
    // Lock manger (10 semaphore)
    Semaphore *table[MAX_SEMAPHORE];

public:
    // Constructor
    // Create 10 null semephore for semTab
    // Init bm
    STable();

    // Destructor
    ~STable();
    // Check semaphor "name" exists then create new semaphor or send error.
    int Create(char *name, int value);

    // Check semaphor "name" exists then call this->Acquire() to excute or send error.
    int Wait(char *name);

    // Check semaphor "name" exists then call this->Release() to excute or send error.
    int Signal(char *name);

    // Find free slot
    int FindFreeSlot();
};

STable::STable()
{
    this->map = new Map(MAX_SEMAPHORE);

    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        this->table[i] = NULL;
    }
}

STable::~STable()
{
    if (this->map)
    {
        delete this->map;
        this->map = NULL;
    }

    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        if (this->table[i])
        {
            delete this->table[i];
            this->table[i] = NULL;
        }
    }
}

int STable::Create(char *name, int value)
{
    DEBUG(dbgSynch, "STable::Create semaphore " << name << " - " << value);
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        if (!this->map->isFree(i))
        {
            if (strcmp(name, table[i]->getName()) == 0)
            {
                DEBUG(dbgSynch, "STable: Find existed semaphore");
                return -1;
            }
        }
    }

    int id = this->FindFreeSlot();

    if (id < 0)
    {
        DEBUG(dbgSynch, "STable: Error table is full");
        return -1;
    }

    this->table[id] = new Semaphore(name, value);
    DEBUG(dbgSynch, "STable: Create in table[" << id << "]");
    return 0;
}

int STable::Wait(char *name)
{
    DEBUG(dbgSynch, "STable::Wait(\"" << name << "\")");
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        // Check does slot[i] load semaphore
        if (!this->map->isFree(i))
        {
            // if yes then compare nam with name of semaphore in table
            if (strcmp(name, table[i]->getName()) == 0)
            {
                // If exist then make semaphore down()
                // DEBUG(dbgSynch, "STable: Find semaphore in table[" << i << "]");
                table[i]->P();
                return 0;
            }
        }
    }
    DEBUG(dbgSynch, "Not exists semaphore in wait");
    return -1;
}

int STable::Signal(char *name)
{
    DEBUG(dbgSynch, "STable::Signal(\"" << name << "\")");
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        // Check does slot[i] load semaphore
        if (!this->map->isFree(i))
        {
            // if yes then compare nam with name of semaphore in table
            if (strcmp(name, table[i]->getName()) == 0)
            {
                // If exist then make semaphore up()
                // DEBUG(dbgSynch, "STable: Find semaphore in table[" << i << "]");
                table[i]->V();
                return 0;
            }
        }
    }
    DEBUG(dbgSynch, "Not exists semaphore in signal");
    return -1;
}

int STable::FindFreeSlot()
{
    return (int)this->map->findFree();
}

#endif