#include "main.h"
#include "ptable.h"
#include "stable.h"
#include "map_semaphore.h"
#include "machine.h"

class ThreadController
{
public:
    ProcessTable *processTab;

    ThreadController()
    {

        processTab = new ProcessTable;
    }
    ~ThreadController()
    {

    
    }
};
