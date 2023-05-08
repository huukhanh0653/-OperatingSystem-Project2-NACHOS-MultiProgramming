#include "stable.h"

// Constructor
STable::STable()
{	
	this->map = new Map(MAX_SEMAPHORE);
	
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		this->table[i] = NULL;
	}
}

// Destructor
STable::~STable()
{
	if(this->map)
	{
		delete this->map;
		this->map = NULL;
	}

	for(int i=0; i < MAX_SEMAPHORE; i++)
	{
		if(this->table[i])
		{
			delete this->table[i];
			this->table[i] = NULL;
		}
	}
	
}

int STable::Create(char *name, int value)
{
	DEBUG(dbgSynch, "STable::Create semaphore "<< name << " - " << value);
	// Check exists semphore
	for(int i=0; i<MAX_SEMAPHORE; i++)
	{
		if(!this->map->isFree(i))
		{
			// DEBUG(dbgSynch, "Name "<<i<<": "<<table[i]->getName());
			if(strcmp(name, table[i]->getName()) == 0)
			{
				DEBUG(dbgSynch, "STable: Find existed semaphore");
				return -1;
			}
		}
		
	}
	// Find free slot in table
	int id = this->FindFreeSlot();
	
	// If table is full then return semaphorebm-1
	if (id < 0)
	{
		DEBUG(dbgSynch, "STable: Error table is full");
		return -1;
	}

    // If find empty slot then load semaphore to table[id]
	this->table[id] = new Semaphore(name, value);
	DEBUG(dbgSynch, "STable: Create in table[" << id << "]");
	return 0;
}

int STable::Wait(char *name)
{
    DEBUG(dbgSynch, "STable::Wait(\"" << name << "\")");
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
        // Check does slot[i] load semaphore
		if(!this->map->isFree(i))
		{
            // if yes then compare nam with name of semaphore in table
			if(strcmp(name, table[i]->getName()) == 0)
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
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
        // Check does slot[i] load semaphore
		if(!this->map->isFree(i))
		{
            // if yes then compare nam with name of semaphore in table
			if(strcmp(name, table[i]->getName()) == 0)
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
	return (int) this->map->findFree();
}