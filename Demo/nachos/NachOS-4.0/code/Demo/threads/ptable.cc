#include "synch.h"
#include "ptable.h"

PTable::PTable(int size) {
    int i;
    psize = size;
    for (i = 0; i < size; i++) {
        pcb[i] = NULL;
    }
    bmsem = new Semaphore("bmsem", 1);
    pcb[0] = new PCB(0);
    pcb[0]->parentID = -1;
}

PTable::~PTable() {
    int i;
    for (i = 0; i < psize; i++) {
        if (!pcb[i]) delete pcb[i];
    }
    delete bmsem;
}

int PTable::ExecUpdate(char* name) {
    // Call mutex->P() to prevent the case of loading 2 processes at the same time.
    bmsem->P();

    // Check the validity of the program "name".
    // Check the existence of the program "name" by calling the Open method of the FileSystem class.
    if (name == NULL) {
        DEBUG(dbgSys, "\nPTable::Exec : Can't not execute name is NULL.\n");
        bmsem->V();
        return -1;
    }
    // Compare the program name and the name of currentThread to make sure that
    // this program is not calling itself.
    if (strcmp(name, kernel->currentThread->getName()) == 0) {
        DEBUG(dbgSys, "\nPTable::Exec : Can't not execute itself.\n");
        bmsem->V();
        return -1;
    }

    
    // Find an empty slot in the Ptable.
    int index = this->GetFreeSlot();

    // Check if have free slot
    if (index < 0) {
        DEBUG(dbgSys, "\nPTable::Exec :There is no free slot.\n");
        bmsem->V();
        return -1;
    }

    // If there is an empty slot, initialize a new PCB with processID equal to the index of this slot
    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(name);
    kernel->fileSystem->Renew(index);

    // parentID is the processID of currentThread.
    pcb[index]->parentID = kernel->currentThread->processID;

    // Call the Exec method of the PCB class.
    int pid = pcb[index]->Exec(name, index);

    // Call bmsem->V()
    bmsem->V();
    // Return the result of PCB->Exec.
    return pid;
}

int PTable::ExitUpdate(int exitcode) {
    // If the calling process is the main process, call Halt().
    int id = kernel->currentThread->processID;
    if (id == 0) {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
        return 0;
    }

    if (!IsExist(id)) {
        DEBUG(dbgSys, "Process " << id << " is not exist.");
        return -1;
    }

    // Otherwise, call SetExitCode to set the exitcode for the calling process
    pcb[id]->SetExitCode(exitcode);
    pcb[pcb[id]->parentID]->DecNumWait();

    // Call JoinRelease to release the parent process waiting for it (if any)
    // and ExitWait() to ask the parent process for permission to exit.
    pcb[id]->JoinRelease();
    pcb[id]->ExitWait();

    Remove(id);
    return exitcode;
}

int PTable::JoinUpdate(int id) {
    // We check the validity of the processID id and verify that the calling Join process
    // is the parent of the process with processID id. If not,
    // we report a logical error and return -1.
    if (id < 0 || id >= psize || pcb[id] == NULL ||
        pcb[id]->parentID != kernel->currentThread->processID) {
        DEBUG(dbgSys, "\nPTable::Join : Can't not join.\n");
        return -1;
    }

    // Increment numwait and call JoinWait() to wait for the child process to complete.
    pcb[pcb[id]->parentID]->IncNumWait();
    pcb[id]->JoinWait();

    // After the child process is complete, the process is released.
    // Handle exitcode.
    int exit_code = pcb[id]->GetExitCode();
    // ExitRelease() allows the child process to exit.
    pcb[id]->ExitRelease();
    return exit_code;
}

int PTable::GetFreeSlot() {
    int i;
    for (i = 0; i < psize; i++) {
        if (!pcb[i]) return i;
    }
    return -1;
}

bool PTable::IsExist(int pid) { return pcb[pid] != NULL; }

void PTable::Remove(int pid) {
    if (pcb[pid] != NULL) {
        delete pcb[pid];
        pcb[pid] = NULL;
    }
}

char* PTable::GetFileName(int id) { return pcb[id]->GetFileName(); }
