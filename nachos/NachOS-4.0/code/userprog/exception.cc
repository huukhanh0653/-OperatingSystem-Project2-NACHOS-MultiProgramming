// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

// #include "copyright.h"
// #include "main.h"
// #include "syscall.h"
// #include "ksyscall.h"

#include "exception_part1.h"
#include "exception_part2.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();
			ASSERTNOTREACHED();
			break;
		}
		case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");

			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			IncPC();
			ASSERTNOTREACHED();

			break;
		}

		case SC_Create:
		{
			HandleCreate();
			break;
		}
		case SC_Remove:
		{
			HandleDelete();
			break;
		}

		case SC_Open:
		{
			HandleOpen();
			break;
		}

		case SC_Read:
		{
			HandleRead();
			break;
		}
		case SC_Write:
		{
			HandleWrite();
			break;
		}

		case SC_Seek:
		{
			HandleSeek();
			break;
		}

		case SC_Close:
		{
			HandleClose();
			break;
		}

		case SC_Connect:
		{
			HandleConnect();
			break;
		}

		case SC_SocketTCP:
		{
			HandleSocketTCP();
			break;
		}

		case SC_Send:
		{
			//! For advanced part, we don't use this case
			int idbuffer = kernel->machine->ReadRegister(4);
			int len = kernel->machine->ReadRegister(5);
			int addrSocketid = kernel->machine->ReadRegister(6);

			int socketid = SysGetIdSocket(addrSocketid);
			char *buffer = new char[len];
			buffer = User2System(idbuffer, len);
			SocketSend(buffer, len, socketid);
			IncPC();
			break;
		}

		case SC_Receive:
		{
			//! For advanced part, we don't use this case
			int idbuffer = kernel->machine->ReadRegister(4);
			int len = kernel->machine->ReadRegister(5);
			int addrSocketid = kernel->machine->ReadRegister(6);
			cout << "Address Socket Id: " << addrSocketid << endl;
			int socketid = SysGetIdSocket(addrSocketid);
			cout << "Socket Id: " << socketid << endl;
			char *buffer = new char[len];
			// buffer = User2System(idbuffer, 100);
			SocketReceive(buffer, len, socketid);
			System2User(idbuffer, len, buffer);
			IncPC();
			break;
		}

		case SC_Exec:
		{
			HandleExec();
			break;
		}

		case SC_Join:
		{
			HandleJoin();
			break;
		}

		case SC_Exit:
		{
			HandleExit();
			break;
		}

		case SC_CreateSemaphore:
		{
			HandleCreateSemaphore();
			break;
		}

		case SC_Wait:
		{
			HandleWait();
			break;
		}

		case SC_Signal:
		{
			HandleSignal();
			break;
		}
		
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
}
