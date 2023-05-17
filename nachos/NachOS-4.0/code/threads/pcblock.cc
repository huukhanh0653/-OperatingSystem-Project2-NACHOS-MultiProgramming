#include "addrspace.h"
#include "main.h"
#include "synch.h"
#include "thread.h"

#include <stdio.h>

#include "pcblock.h"

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
