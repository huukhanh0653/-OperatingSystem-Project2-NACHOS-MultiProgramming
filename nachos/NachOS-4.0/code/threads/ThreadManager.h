#include "main.h"
#include "ptable.h"
#include "stable.h"
#include "map.h"
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
