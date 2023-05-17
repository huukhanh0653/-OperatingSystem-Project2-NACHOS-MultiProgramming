#ifndef MAP_H
#define MAP_H

#include "thread.h"
#include "list.h"
#include "threads/main.h"
/*
    This class is similarly defined to bitmap but simpler
    because I've not understood the bit operation just like: AND, OR, etc.
    Therefore I used this instead.
    Map is used to manage kinds of linear array object.
*/
class Map
{
private:
    bool *_map;
    size_t _size;
    size_t _free;

public:
    Map(size_t size)
    {
        this->_size = size;
        this->_free = size;
        this->_map = new bool[this->_size];
        for (size_t i = 0; i < this->_size; i++)
            this->_map[i] = true; //* Set as FREE status
    }

    ~Map()
    {
        delete[] this->_map;
    }

    void mark(size_t which)
    {
        if (!this->_map[which])
            return;
        this->_map[which] = false;
        this->_free--;
    }

    bool isFree(size_t which)
    {
        return (this->_map[which] == true ? true : false);
    }

    size_t findFree()
    {
        size_t i;
        for (i = 0; i < this->_size; i++)
            if (this->isFree(i))
            {
                this->mark(i);
                return i;
            }
        return -1;
    }

    void clear(size_t which)
    {
        this->_map[which] = true;
    }

    size_t numFree()
    {
        return this->_free;
    }
};

class MySemaphore
{
public:
    MySemaphore(char *debugName, int initialValue); // set initial value
    ~MySemaphore();                                 // de-allocate MySemaphore
    char *getName() { return name; }              // debugging assist

    //* Down/Wait function -> P
    void P(); // these are the only operations on a MySemaphore
    //* Up/Signal function -> V
    void V();        // they are both *atomic*
    void SelfTest(); // test routine for MySemaphore implementation
private:
    char *name; // useful for debugging
    int value;  // MySemaphore value, always >= 0
    List<Thread *> *queue;
};

MySemaphore::MySemaphore(char *debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List<Thread *>;
}

//----------------------------------------------------------------------
// MySemaphore::MySemaphore
// 	De-allocate MySemaphore, when no longer needed.  Assume no one
//	is still waiting on the MySemaphore!
//----------------------------------------------------------------------

MySemaphore::~MySemaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// MySemaphore::P
// 	Wait until MySemaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void MySemaphore::P()
{
    Interrupt *interrupt = kernel->interrupt;
    Thread *currentThread = kernel->currentThread;

    // disable interrupts
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    while (value == 0)
    {                                 // MySemaphore not available
        queue->Append(currentThread); // so go to sleep
        currentThread->Sleep(FALSE);
    }
    value--; // MySemaphore available, consume its value

    // re-enable interrupts
    (void)interrupt->SetLevel(oldLevel);
}

//----------------------------------------------------------------------
// MySemaphore::V
// 	Increment MySemaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that interrupts
//	are disabled when it is called.
//----------------------------------------------------------------------

void MySemaphore::V()
{
    Interrupt *interrupt = kernel->interrupt;

    // disable interrupts
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    if (!queue->IsEmpty())
    { // make thread ready.
        kernel->scheduler->ReadyToRun(queue->RemoveFront());
    }
    value++;

    // re-enable interrupts
    (void)interrupt->SetLevel(oldLevel);
}

#endif