#include "exception_utils.h"

//ThreadController Tab;

//!---------------------- PART II - Multiple Processing -------------------------------
// Usage: create a process from a program and schedule it for execution
// Input: address to the program name
// Output: the process ID, or -1 on failure
void HandleExec()
{
	DEBUG(dbgSys, "Syscall: Exec(filename)");

	int addr = kernel->machine->ReadRegister(4);
	DEBUG(dbgSys, "Register 4: " << addr);

	char *fileName;
	fileName = User2System(addr, 255);
	DEBUG(dbgSys, "Read file name: " << fileName);

	DEBUG(dbgSys, "Scheduling execution...");
	int result = kernel->pTable->ExecuteUpdate(fileName);

	DEBUG(dbgSys, "Writing result to register 2: " << result);
	kernel->machine->WriteRegister(2, result);
	delete fileName;
    return IncPC();
}

// Usage: block the current thread until the child thread has exited
// Input: ID of the thread being joined
// Output: exit code of the thread
void HandleJoin()
{
	DEBUG(dbgSys, "Syscall: Join");
	int id = kernel->machine->ReadRegister(4);
	int result = kernel->pTable->JoinUpdate(id);
	kernel->machine->WriteRegister(2, result);
    return IncPC();
}

// Usage: exit current thread
// Input: exit code to pass to parent
// Output: none
void HandleExit()
{
	DEBUG(dbgSys, "Syscall: Exit");
	int exitCode = kernel->machine->ReadRegister(4);
	int result = kernel->pTable->ExitUpdate(exitCode);
    return IncPC();
}

// Usage: Create a semaphore
// Input : name of semphore and int for semaphore value
// Output : success: 0, fail: -1
void HandleCreateSemaphore()
{
	// Load name and value of semaphore
	int virtAddr = kernel->machine->ReadRegister(4); // read name address from 4th register
	int semVal = kernel->machine->ReadRegister(5);	 // read type from 5th register
	char *name = User2System(virtAddr, MaxFileNameLength); // Copy semaphore name charArray form userSpace to systemSpace
	
	// Validate name
	if(name == NULL)
	{
		// DEBUG(dbgSynch, "\nNot enough memory in System");
		cerr << "Not enough memory in System\n";
		kernel->machine->WriteRegister(2, -1);
		delete[] name;
		return;
	}
	
	int res = kernel->sTable->Create(name, semVal);

	// Check error
	if(res == -1)
	{
		// DEBUG('a', "\nCan not create semaphore");
		cerr << "Can not create semaphore" << endl;
	}
	
	delete[] name;
	kernel->machine->WriteRegister(2, res);
	return IncPC();
}

// Usage: Sleep
// Input : name of semaphore
// Output : success: 0, fail: -1
void HandleWait()
{
	// Load name of semaphore
	int virtAddr = kernel->machine->ReadRegister(4);
	char *name = User2System(virtAddr, MaxFileNameLength + 1);

	// Validate name
	if(name == NULL)
	{
		// DEBUG(dbgSynch, "\nNot enough memory in System");
		printf("\nNot enough memory in System");
		kernel->machine->WriteRegister(2, -1);
		delete[] name;
		return;
	}

	int res = kernel->sTable->Wait(name);
	
	// Check error
	if(res == -1)
	{
		// DEBUG(dbgSynch, "\nNot exists semaphore");
		printf("\nNot exists semaphore");
	}

	delete[] name;
	kernel->machine->WriteRegister(2, res);
	return IncPC();
}

// Usage: Wake up
// Input : name of semaphore
// Output : success: 0, fail: -1
void HandleSignal()
{
	// Load name of semphore
	int virtAddr = kernel->machine->ReadRegister(4);
	char *name = User2System(virtAddr, MaxFileNameLength + 1);

	// Validate name
	if(name == NULL)
	{
		// DEBUG(dbgSynch, "\nNot enough memory in System");
		printf("\n Not enough memory in System");
		kernel->machine->WriteRegister(2, -1);
		delete[] name;
		return;
	}
	
	int res = kernel->sTable->Signal(name);

	// Check error
	if(res == -1)
	{
		// DEBUG(dbgSynch, "\nNot exists semaphore");
		printf("\nNot exists semaphore");
	}
	
	delete[] name;
	kernel->machine->WriteRegister(2, res);
	return IncPC();
}