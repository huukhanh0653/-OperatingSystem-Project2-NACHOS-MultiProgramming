#ifndef PCB_H
#define PCB_H

#include "synch.h"
#include "thread.h"

#include "addrspace.h"
#include "main.h"
#include "synch.h"
#include "thread.h"

#include <stdio.h>

class Semaphore;

// The process control block
//
//!stores and manages information necessary to execute and join
//* Process's:
//!    Process ID
//!    Parent ID
//!    The executable file containing the program
//!    The thread used to execute the program
//!    Information for managing joining 
//
// Instances of this class are entries in the system-wide process table,
// manage by PTable

class PCB {

private:
    int pid;
    int exitCode;
    // Name of the executable file to load and run
    char *file;
    // The thread object to be run
    Thread *thread;
    // Number of joined processes
    int numwait;
    // Semaphore to use for controlling join process
    Semaphore *joinsemaphore;
    // Semaphore to use for controlling exit process
    Semaphore *exitsemaphore;
    Semaphore *mutex;

public:
    int parentID;

    // Constructor
    PCB();
    // Destructor
    ~PCB();

    // Initialize the first PCB
    //
    // This constructor should be used exclusively for the first program 
    // executed by Nachos. Because we are now supporting process control
    // blocks for Nachos, so it's obvious that the first process should be the
    // first one to be allocated to the process table.

    // Creates thread and schedules for program stored in `fileName`
    PCB(const char *fileName, Thread *thread);


    //
    // Returns the process ID on success, -1 on failure
    int Exec(char *fileName, int pid);

    int GetID();

    // Returns the name of the file used to run this process.
    const char *GetExecutableFileName();

    // Returns the current Thread object of this process. Use with care.
    const Thread* GetThread();

    // Returns the number of processes this process is waiting for.
    int GetNumWait();

    // Blocks until JoinRelease() is called. This method is called by the 
    // parent process that wants to join into this process (i.e. waiting for
    // this process to finish).
    void JoinWait();

    // Releases the parent that are waiting for this process.
    void JoinRelease();

    void ExitWait();

    void ExitRelease();

    void IncNumWait();

    void DecNumWait();

    void SetExitCode(int exitCode);

    int GetExitCode();
};

// Entry point for executing processes
static void StartProcess(void *args)
{
    char *fileName = (char*) args;
    DEBUG(dbgThread, "PCB: Starting process " << fileName);
    AddrSpace *addrspace = new AddrSpace;
    if (addrspace->Load(fileName)) {
        addrspace->Execute();
    }
    ASSERTNOTREACHED();
}

PCB::PCB()
{
    this->pid = -1;
    this->parentID = -1;
    this->numwait = 0;
    this->exitCode = 0;
    this->file = NULL;
    this->thread = NULL;
    this->joinsemaphore = new Semaphore("joinsemaphore", 0);
    this->exitsemaphore = new Semaphore("exitsemaphore", 0);
    this->mutex = new Semaphore("mutex", 1);
}

PCB::PCB(const char *fileName, Thread *thread)
    : PCB()
{
    this->pid = 0;
    this->file = new char[strlen(fileName) + 1];
    strcpy(this->file, fileName);
    this->thread = thread;
}

PCB::~PCB()
{
    delete this->file;
    if (this->thread) {
        this->thread->Finish();
        delete this->thread;
    }
    delete this->joinsemaphore;
    delete this->exitsemaphore;
    delete this->mutex;
}

int PCB::GetID()
{
    return this->pid;
}

const char* PCB::GetExecutableFileName()
{
    return this->file;
}

const Thread* PCB::GetThread()
{
    return this->thread;
}

int PCB::GetNumWait()
{
    return this->numwait;
}

int PCB::GetExitCode()
{
    return this->exitCode;
}

void PCB::SetExitCode(int exitCode)
{
    exitCode = exitCode;
}

void PCB::IncNumWait()
{
    this->mutex->P();
    this->numwait++;
    this->mutex->V();
}

void PCB::DecNumWait()
{
    this->mutex->P();
    this->numwait--;
    this->mutex->V();
}

int PCB::Exec(char *fileName, int pid)
{
    this->mutex->P();
    DEBUG(dbgThread, "PCB: Setting things up for " << fileName << "...");

    this->pid = pid;

    // Copy the executable file name into local storage, since `fileName`
    // is going to be reused elsewhere
    this->file = new char[strlen(fileName)];
    strcpy(this->file, fileName);

    DEBUG(dbgThread, "PCB: Forking " << file << "...");
    this->thread = new Thread(file);
    this->thread->Fork(StartProcess, file);
    this->mutex->V();
    return this->pid;
}

void PCB::JoinWait()
{
    this->joinsemaphore->P();
}

void PCB::ExitWait()
{
    this->exitsemaphore->P();
}

void PCB::JoinRelease()
{
    this->joinsemaphore->V();
}

void PCB::ExitRelease()
{
    this->exitsemaphore->V();
}

#endif // PCB_H
