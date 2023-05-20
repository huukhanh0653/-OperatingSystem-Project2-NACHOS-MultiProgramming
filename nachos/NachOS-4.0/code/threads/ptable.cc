#include "ptable.h"
#include "synch.h"

PTable::PTable()
{
    totalProcesses = size;
    reception = new Bitmap(totalProcesses);
    semaphore = new Semaphore("PTable_bmsem", 1);

    for (int i = 0; i < MAX_PROCESS; ++i)
        blocks[i] = NULL;

    // -> Initialize first process
    // -> (start up process, run automatically by Nachos)
    blocks[0] = new PCB(0);
    blocks[0]->parentID = -1;
}

PTable::PTable(int size)
{
    totalProcesses = size;
    reception = new Bitmap(totalProcesses);
    semaphore = new Semaphore("PTable_bmsem", 1);

    for (int i = 0; i < totalProcesses; ++i)
        blocks[i] = NULL;

    // -> Initialize first process
    // -> (start up process, run automatically by Nachos)
    blocks[0] = new PCB(0);
    blocks[0]->parentID = -1;
}

PTable::~PTable()
{
    delete reception;
    delete semaphore;
    for (int i = 0; i < totalProcesses; ++i)
    {
        if (!blocks[i])
            delete blocks[i];
    }
}

int PTable::GetCurrentThreadId()
{
    Thread *current = kernel->currentThread;
    if (current != NULL)
        return current->pid;
    return -1;
}

int PTable::GetFreeSlot()
{
    return reception->FindAndSet();
}

bool PTable::IsExist(int pid)
{
    if (pid < 0 || pid >= MAX_PROCESS)
    {
        return FALSE;
    }

    return blocks[pid] != NULL;
}

void PTable::Remove(int pid)
{
    if (IsExist(pid))
    {
        --totalProcesses;
        reception->Clear(pid);
        delete blocks[pid];
        blocks[pid] = NULL;
    }
}

int PTable::ExecuteUpdate(char *fileName)
{
    semaphore->P();
    DEBUG(dbgThread, "PTable::ExecUpdate(\"" << fileName << "\")");

    // Avoid self-execution
    DEBUG(dbgThread, "PTable: Checking " << fileName << " for self-execution...");
    int currentThreadId = GetCurrentThreadId();
    if (strcmp(blocks[currentThreadId]->GetExecutableFileName(), fileName) == 0)
    {
        cerr << "PTable: %s cannot execute itself.\n", fileName;
        semaphore->V();
        return -1;
    }

    // Allocate a new PCB
    DEBUG(dbgThread, "PTable: Look for free slot in process table...");
    int slot = GetFreeSlot();
    if (slot == -1)
    {
        cerr << "PTable: Maximum number of processes reached.\n";
        semaphore->V();
        return -1;
    }

    // PID = slot number
    this->blocks[slot] = new PCB();
    this->blocks[slot]->parentID = currentThreadId;

    // Schedule the program for execution
    DEBUG(dbgThread, "PTable: Schedul program for execution...");

    this->totalProcesses++;
    this->semaphore->V();

    // Return the PID of PCB->Exec if success, else return -1
    return this->blocks[slot]->Exec(fileName, slot);
}

int PTable::JoinUpdate(int id)
{
    int currentThreadId = GetCurrentThreadId();
    if (!IsExist(id))
    {
        cerr << "PTable: Join into an invalid process\n";
        return -1;
    }

    if (id == currentThreadId)
    {
        cerr << "PTable: Process with id " << currentThreadId << " cannot join to itself\n";
        return -2;
    }

    if (blocks[id]->parentID != currentThreadId)
    {
        cerr << "PTable: Can only join parent to child process \n";
        return -3;
    }

    // Increment numwait and call JoinWait() to wait for the child process to complete.
    blocks[currentThreadId]->IncNumWait();
    blocks[id]->JoinWait();

    // After the child process is complete, the process is released.
    blocks[id]->ExitRelease();

    return blocks[id]->GetExitCode();
}

int PTable::ExitUpdate(int exitcode)
{
    int currentThreadId = GetCurrentThreadId();
    if (currentThreadId == 0)
    {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
        return 0;
    }
    if (!IsExist(currentThreadId))
    {
        DEBUG(dbgSys, "Process " << currentThreadId << " is not exist.");
        return -1;
    }

    blocks[currentThreadId]->SetExitCode(exitcode);
    blocks[currentThreadId]->JoinRelease();
    blocks[currentThreadId]->ExitWait();
    Remove(currentThreadId);
}

const char *PTable::GetFileName(int id)
{
    return this->blocks[id]->GetExecutableFileName();
}

// void PTable::Print()
// {
//     printf("\n\nTime: %d\n", kernel->stats->totalTicks);
//     printf("Current process table:\n");
//     printf("ID\tParent\tExecutable File\n");
//     int currentThreadId = GetCurrentThreadId(kernel->currentThread);
//     for (int i = 0; i < MAX_PROCESSES; ++i)
//     {
//         if (blocks[i])
//         {
//             printf(
//                 "%d\t%d\t%s%s\n",
//                 blocks[i]->GetID(),
//                 blocks[i]->parentID,
//                 blocks[i]->GetExecutableFileName(),
//                 i == currentThreadId ? " (current thread) " : "");
//         }
//     }
// }
