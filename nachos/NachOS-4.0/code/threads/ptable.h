#ifndef PTABLE_H
#define PTABLE_H

#include "pc_block.h"
#include "synch.h"
#include "thread.h"

#define MAX_PROCESSES 10

class Semaphore;

// Process Table
class ProcessTable {

private:
    // The array of system-wide PCBs to manage
    PCB *blocks[MAX_PROCESSES];
    // Number of alive processes in the system
    int totalProcesses;
    // Manages free slots in the PCBs table
    Map *reception;
    // Semaphore
    Semaphore *semaphore;
    
public:
    // Initializes process table with `size` entries
    ProcessTable(int size = MAX_PROCESSES);

    ~ProcessTable();

    // Allocates the start up process (first executed by Nachos)
    void InitializeFirstProcess(const char* fileName, Thread* thread);

    int GetCurrentThreadId();

    // Sets up PCB and schedules the program stored in `fileName`.
    // Returns the process id on success, -1 otherwise
    int UpdateExecuting(char *fileName);

    // Updates the process table when the current thread join into the thread
    // `id`
    int UpdateJoining(int id);

    // Updates the process table when the current thread exit with exit code
    // `ec`
    int UpdateExiting(int ec);

    // Return an index to a free slot in the table, returns -1
    // if the table is full
    int GetFreeSlot();

    // Returns TRUE if pid exists
    bool IsExist(int pid);

    // Removes a PCB
    void Remove(int pid);

    // Print the current process table
    void Print();
};


#endif // ProcessTable_H