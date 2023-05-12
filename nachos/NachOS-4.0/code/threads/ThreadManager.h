#include "main.h"
#include "ptable.h"
#include "stable.h"
#include "map.h"
#include "machine.h"

class ThreadController
{
public:
    Map *PhysPageMap;
    Semaphore *addrLock;
    ProcessTable *processTab;
    STable *semaphoreTab;

    ThreadController()
    {
        PhysPageMap = new Map(NumPhysPages);
        addrLock = new Semaphore("addrLock", 1);
        processTab = new ProcessTable;
        semaphoreTab = new STable;
    }
    ~ThreadController()
    {
        delete PhysPageMap;
        delete addrLock;
        delete processTab;
        delete semaphoreTab;
    }
};
