#include "synch.h"
#include "pcb.h"

PCB::PCB(int id) {
    this->processID = kernel->currentThread->processID;
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    multex = new Semaphore("multex", 1);
}

PCB::~PCB() {
    delete joinsem;
    delete exitsem;
    delete multex;

    if (thread) {
        thread->FreeSpace();
        thread->Finish();
        // delete thread;
    }

    delete[] filename;
}

void StartProcess_2(void* pid) {
    int id;
    id = *((int*)pid);
    // Get the fileName of this process id
    char* fileName = kernel->pTab->GetFileName(id);

    AddrSpace* space;
    space = new AddrSpace(fileName);

    if (space == NULL) {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }

    space->Execute();
    // kernel->currentThread->space = space;

    // space->InitRegisters();	// set the initial register values
    // space->RestoreState();	// load page table register

    // kernel->machine->Run();	// jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

int PCB::Exec(char* filename, int id) {
    // cerr << filename << ' ' << pid << endl;
    multex->P();

    this->thread = new Thread(filename, true);
    if (this->thread == NULL) {
        printf("\nPCB::Exec: Not enough memory!\n");
        multex->V();  // Release the CPU to give way to another process
        return -1;    // Return -1 if failed
    }

    // Set the processID of this thread to id.
    this->thread->processID = id;
    // Set the parrentID of this thread to the processID of the thread that calls Exec
    this->parentID = kernel->currentThread->processID;
    // Call Fork(StartProcess_2,id) to execute the process => We cast the thread to int,
    // and then we cast it back to its original type in StartProcess function.

    // Do not use the id variable here, as it is a local variable,
    // and its value is lost when this function ends.
    this->thread->Fork(StartProcess_2, &this->thread->processID);

    multex->V();
    // Return id.
    return id;
}

int PCB::GetID() { return thread->processID; }

int PCB::GetNumWait() { return numwait; }

void PCB::JoinWait() {
    // Call joinsem->P() to make the thread enter a blocked state and wait,
    // for JoinRelease to continue execution.
    joinsem->P();
}

void PCB::ExitWait() {
    // Call exitsem-->V() to make the thread enter a blocked state and wait,
    // for ExitRelease to continue execution.
    exitsem->P();
}

void PCB::JoinRelease() {
    // Call joinsem->V() to release the thread waiting in JoinWait().
    joinsem->V();
}

void PCB::ExitRelease() {
    // Call exitsem->V() to release the waiting thread.
    exitsem->V();
}

void PCB::IncNumWait() {
    multex->P();
    ++numwait;
    multex->V();
}

void PCB::DecNumWait() {
    multex->P();
    if (numwait > 0) --numwait;
    multex->V();
}

void PCB::SetExitCode(int ec) { exitcode = ec; }

int PCB::GetExitCode() { return exitcode; }

void PCB::SetFileName(char* fn) { strcpy(filename, fn); }

char* PCB::GetFileName() {
    // cerr << "get file name" << ' ' << filename << endl;
    return filename;
}
