#include "synch.h"
#include "stable.h"

STable::STable() {
    this->bm = new Bitmap(MAX_SEMAPHORE);

    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        this->semTab[i] = NULL;
    }
}

STable::~STable() {
    if (this->bm) {
        delete this->bm;
        this->bm = NULL;
    }
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (this->semTab[i]) {
            delete this->semTab[i];
            this->semTab[i] = NULL;
        }
    }
}

int STable::Create(char* name, int init){  
    // Check if this semaphore already exists
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (bm->Test(i)) {
            if (strcmp(name, semTab[i]->GetName()) == 0) {
                return -1;
            }
        }
    }
    // Find a free slot in semTab
    int id = this->FindFreeSlot();

    // If no free slot is found, return -1
    if (id < 0) {
        return -1;
    }

    // If a free slot is found, create a new semaphore in semTab[id]
    this->semTab[id] = new Sem(name, init);
    return 0;
}

int STable::Wait(char* name) {
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        // Check if a semaphore has been created in slot i
        if (bm->Test(i)) {
            // If a semaphore exists, compare its name with the input name
            if (strcmp(name, semTab[i]->GetName()) == 0) {
                // If the semaphore exists, call its wait() method
                semTab[i]->wait();
                return 0;
            }
        }
    }
    printf("Semaphore not found.");
    return -1;
}

int STable::Signal(char* name) {
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        // Check if a semaphore has been created in slot i
        if (bm->Test(i)) {
            // If a semaphore exists, compare its name with the input name
            if (strcmp(name, semTab[i]->GetName()) == 0) {
                // If the semaphore exists, call its signal() method
                semTab[i]->signal();
                return 0;
            }
        }
    }
    printf("Semaphore not found.");
    return -1;
}

int STable::FindFreeSlot() { return this->bm->FindAndSet(); }
