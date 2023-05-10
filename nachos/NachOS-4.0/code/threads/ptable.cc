#include "ptable.h"

//#include "bitmap.h"
#include "synch.h"
#include "map.h"

#include <stdio.h>

ProcessTable::ProcessTable(int size)
{
    totalProcesses = size;
    reception = new Map(totalProcesses);
    semaphore = new Semaphore("ProcessTable_bmsem", 1);

    for (int i = 0; i < MAX_PROCESSES; ++i) {
        blocks[i] = NULL;
    }
}

ProcessTable::~ProcessTable()
{
    delete reception;
    delete semaphore;
    for (int i = 0; i < totalProcesses; ++i) {
        delete blocks[i];
    }
}

void ProcessTable::InitializeFirstProcess(const char *fileName, Thread* thread)
{
    blocks[0] = new PCB(fileName, thread);
    totalProcesses++;
    reception->mark(0);
}

int ProcessTable::GetCurrentThreadId()
{
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (blocks[i] != 0) {
            if (blocks[i]->GetThread() == kernel->currentThread) {
                return blocks[i]->GetID();
            }
        }
    }
    return -1;
}

int ProcessTable::GetFreeSlot()
{
    return reception->findFree();
}

bool ProcessTable::IsExist(int pid)
{
    if (pid < 0 || pid >= MAX_PROCESSES) {
        return FALSE;
    }

    return blocks[pid] != NULL;
}

void ProcessTable::Remove(int pid)
{
    if (IsExist(pid)) {
        --totalProcesses;
        reception->clear(pid);
        delete blocks[pid];
        blocks[pid] = NULL;
    }
}

int ProcessTable::UpdateExecuting(char *fileName)
{
    semaphore->P();
    DEBUG(dbgThread, "ProcessTable::ExecUpdate(\"" << fileName << "\")");

    // Prevent self-execution
    DEBUG(dbgThread, "ProcessTable: Checking " << fileName << " for self-execution...");
    int currentThreadId = GetCurrentThreadId();
    if (strcmp(blocks[currentThreadId]->GetExecutableFileName(), fileName) == 0) {
        fprintf(stderr, "ProcessTable: %s cannot execute itself.\n", fileName);
        semaphore->V();
        return -1;
    }

    // Allocate a new PCB
    DEBUG(dbgThread, "ProcessTable: Looking for free slot in process table...");
    int slot = GetFreeSlot();
    if (slot == -1) {
        fprintf(stderr, "ProcessTable: Maximum number of processes reached.\n");
        semaphore->V();
        return -1;
    }
    DEBUG(dbgThread, "ProcessTable: Obtained slot " << slot);
    // PID = slot number
    blocks[slot] = new PCB();
    blocks[slot]->parentID = currentThreadId;
    
    // Schedule the program for execution
    DEBUG(dbgThread, "ProcessTable: Scheduling program for execution...");
    int result = blocks[slot]->Exec(fileName, slot);
    totalProcesses++;
    semaphore->V();
    return result;
}

int ProcessTable::UpdateJoining(int id)
{
    int currentThreadId = GetCurrentThreadId();
    if (!IsExist(id)) {
        fprintf(
            stderr,
            "ProcessTable: Join into an invalid process "
            "(there is no process with id: %d)\n",
            id
        );
        return -1;
    }

    if (id == currentThreadId) {
        fprintf(
            stderr,
            "ProcessTable: Process with id %d cannot join to itself\n",
            currentThreadId
        );
        return -2;
    } else if (blocks[id]->parentID != currentThreadId) {
        fprintf(
            stderr,
            "ProcessTable: Can only join parent to child process "
            "(process with id %d is not parent of process with id %d)\n",
            currentThreadId,
            id
        );
        return -3;
    }

    blocks[currentThreadId]->IncNumWait();
    blocks[id]->JoinWait();

    blocks[currentThreadId]->DecNumWait();
    blocks[id]->ExitRelease();
    return blocks[id]->GetExitCode();
}

int ProcessTable::UpdateExiting(int ec)
{
    int currentThreadId = GetCurrentThreadId();
    if (currentThreadId == 0) {
        kernel->interrupt->Halt();
    } else {
        blocks[currentThreadId]->SetExitCode(ec);
        blocks[currentThreadId]->JoinRelease();
        blocks[currentThreadId]->ExitWait();
        Remove(currentThreadId);
    }
}

void ProcessTable::Print()
{
    printf("\n\nTime: %d\n", kernel->stats->totalTicks);
    printf("Current process table:\n");
    printf("ID\tParent\tExecutable File\n");
    int currentThreadId = GetCurrentThreadId();
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        if (blocks[i]) {
            printf(
                "%d\t%d\t%s%s\n",
                blocks[i]->GetID(),
                blocks[i]->parentID,
                blocks[i]->GetExecutableFileName(),
                i == currentThreadId ? " (current thread) " : ""
            );  
        }
    }
}
