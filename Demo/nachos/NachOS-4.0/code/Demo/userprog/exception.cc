// exception.cc

// 	DH KHTN - DHQG TPHCM			/
// 	20125004 Hoang Hiep		/
// 	20125081 Vo Huynh		/

//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we supportID is
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

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"

#define MaxFileLength 32
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
// before returning (by calling IncreasePC()). (Or else you'll loop
// making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void IncreasePC() {
	int currentPC = kernel->machine->ReadRegister(PCReg);
	kernel->machine->WriteRegister(PrevPCReg, currentPC);
		int nextPC = kernel->machine->ReadRegister(NextPCReg);
		kernel->machine->WriteRegister(PCReg, nextPC);
	kernel->machine->WriteRegister(NextPCReg, nextPC + 4);
}

char* User2System(int addr, int convert_length = -1) {
    int length = 0;
    bool stop = false;
    char* str;

    do {
        int oneChar;
        kernel->machine->ReadMem(addr + length, 1, &oneChar);
        length++;
        // if convert_length == -1, we use '\0' to terminate the process
        // otherwise, we use convert_length to terminate the process
        stop = ((oneChar == '\0' && convert_length == -1) ||
                length == convert_length);
    } while (!stop);

    str = new char[length];
    for (int i = 0; i < length; i++) {
        int oneChar;
        kernel->machine->ReadMem(addr + i, 1,
                                 &oneChar);  // copy characters to kernel space
        str[i] = (unsigned char)oneChar;
    }
    return str;
}

/**
 * @brief Convert system string to user string
 *
 * @param str string to convert
 * @param addr addess of user string
 * @param convert_length set max length of string to convert, leave
 * blank to convert all characters of system string
 * @return void
 */
void System2User(char* str, int addr, int convert_length = -1) {
    int length = (convert_length == -1 ? strlen(str) : convert_length);
    for (int i = 0; i < length; i++) {
        kernel->machine->WriteMem(addr + i, 1,
                                  str[i]);  // copy characters to user space
    }
    kernel->machine->WriteMem(addr + length, 1, '\0');
}

/**
 * Handle not implemented syscall
 * This method will write the syscall to debug log and increase
 * the program counter.
 */
void handle_not_implemented_SC(int type) {
    DEBUG(dbgSys, "Not yet implemented syscall " << type << "\n");
    return IncreasePC();
}

void handle_SC_Halt() {
    DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
    SysHalt();
    ASSERTNOTREACHED();
}

void handle_SC_Add() {
    DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + "
                         << kernel->machine->ReadRegister(5) << "\n");

    /* Process SysAdd Systemcall*/
    int result;
    result = SysAdd(
        /* int op1 */ (int)kernel->machine->ReadRegister(4),
        /* int op2 */ (int)kernel->machine->ReadRegister(5));

    DEBUG(dbgSys, "Add returning with " << result << "\n");
    /* Prepare Result */
    kernel->machine->WriteRegister(2, (int)result);

    return IncreasePC();
}

void handle_SC_ReadNum() {
    int result = SysReadNum();
    kernel->machine->WriteRegister(2, result);
    return IncreasePC();
}

void handle_SC_PrintNum() {
    int character = kernel->machine->ReadRegister(4);
    SysPrintNum(character);
    return IncreasePC();
}

void handle_SC_ReadChar() {
    char result = SysReadChar();
    kernel->machine->WriteRegister(2, (int)result);
    return IncreasePC();
}

void handle_SC_PrintChar() {
    char character = (char)kernel->machine->ReadRegister(4);
    SysPrintChar(character);
    return IncreasePC();
}

void handle_SC_RandomNum() {
    int result;
    result = SysRandomNum();
    kernel->machine->WriteRegister(2, result);
    return IncreasePC();
}

#define MAX_READ_STRING_LENGTH 255
void handle_SC_ReadString() {
    int memPtr = kernel->machine->ReadRegister(4);  // read address of C-string
    char* buffer = new char[MAX_READ_STRING_LENGTH + 1];
    int bytesRead = 0;

    // Read string from console until newline or buffer is full
    while (bytesRead < MAX_READ_STRING_LENGTH) {
        char c = SysReadChar();
        if (c == '\n' || c == '\r') {
            break;
        }
        buffer[bytesRead++] = c;
    }

    // Null-terminate the string
    buffer[bytesRead] = '\0';

    // Copy the string to user space
    System2User(buffer, memPtr);

    // Free allocated memory
    delete[] buffer;

    // Increment Program Counter and return
    IncreasePC();
    return;
}

void handle_SC_PrintString() {
    int memPtr = kernel->machine->ReadRegister(4);  // read address of C-string
    char* buffer = User2System(memPtr);

    SysPrintString(buffer, strlen(buffer));
    delete[] buffer;
    return IncreasePC();
}

void handle_SC_CreateFile() {
    int virtAddr = kernel->machine->ReadRegister(4);
    char* fileName = User2System(virtAddr);

    if (SysCreateFile(fileName))
        kernel->machine->WriteRegister(2, 0);
    else
        kernel->machine->WriteRegister(2, -1);

    delete[] fileName;
    return IncreasePC();
}

void handle_SC_RemoveFile() {
    int virtAddr = kernel->machine->ReadRegister(4); // read the virtual address of the filename
    char* fileName = User2System(virtAddr); // convert the virtual address to kernel address and copy the filename
    if (fileName == NULL)
    {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        kernel->machine->WriteRegister(2, -1);
        IncreasePC();
        return;
    }

    bool success = kernel->fileSystem->Remove(fileName); // remove the file from the file system
    if (!success)
    {
        printf("\nFailed to remove file: %s", fileName);
        kernel->machine->WriteRegister(2, -1);
    }
    else
    {
        printf("\nFile successfully removed: %s", fileName);
        kernel->machine->WriteRegister(2, 0);
    }

    delete[] fileName;
    IncreasePC();
}

void handle_SC_Open() {
    int virtAddr = kernel->machine->ReadRegister(4);
    char* fileName = User2System(virtAddr);
    int type = kernel->machine->ReadRegister(5);

    kernel->machine->WriteRegister(2, SysOpen(fileName, type));

    delete fileName;
    return IncreasePC();
}

void handle_SC_Close() {
    int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysClose(id));

    return IncreasePC();
}

void handle_SC_Read() {
    int virtAddr = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    int fileId = kernel->machine->ReadRegister(6);

    char* buffer = new char[charCount + 1];
    int readCount = SysRead(buffer, charCount, fileId);

    buffer[readCount] = '\0';
    System2User(buffer, virtAddr);

    delete[] buffer;
    kernel->machine->WriteRegister(2, readCount);
    IncreasePC();
}

void handle_SC_Write() {
    int virtAddr = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    char* buffer = User2System(virtAddr, charCount);
    int fileId = kernel->machine->ReadRegister(6);

    kernel->machine->WriteRegister(2, SysWrite(buffer, charCount, fileId));
    System2User(buffer, virtAddr, charCount);

    delete[] buffer;
    return IncreasePC();
}

/**
 * Handle SC_Seek
 * This method will seek the file to the given position.
 * @param seekPos: seek position (use -1 to seek to end of file) (get from R4)
 * @param fileId: file descripAddresstor (get from R5)
 * @return -1 if failed to seek, otherwise return the new position
 */
void handle_SC_Seek() {
    int seekPos = kernel->machine->ReadRegister(4);
    int fileId = kernel->machine->ReadRegister(5);

    kernel->machine->WriteRegister(2, SysSeek(seekPos, fileId));

    return IncreasePC();
}

/**
 * @brief handle System Call Exec
 * @param virtAddr: virtual address of user string name (get from R4)
 * @return -1 if failed to Exec, otherwise return id of new process
 * (write result to R2)
 */
void handle_SC_Exec() {
    int virtAddr;
    virtAddr = kernel->machine->ReadRegister(4);   // read the program name's address from register r4
    char* name;
    name = User2System(virtAddr);  // Get the program name and copy it to the kernel
    if (name == NULL) {
        DEBUG(dbgSys, "\n Not enough memory in System");
        ASSERT(false);
        kernel->machine->WriteRegister(2, -1);
        return IncreasePC();
    }

    kernel->machine->WriteRegister(2, SysExec(name));

    return IncreasePC();
}

/**
 * @brief handle System Call Join
 * @param id: thread id (get from R4)
 * @return -1 if failed to join, otherwise return exit code of
 * the thread. (write result to R2)
 */
void handle_SC_Join() {
    int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysJoin(id));
    return IncreasePC();
}

/**
 * @brief handle System Call Exit
 * @param id: thread id (get from R4)
 * @return -1 if failed to exit, otherwise return exit code of
 * the thread. (write result to R2)
 */
void handle_SC_Exit() {
    int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysExit(id));
    return IncreasePC();
}

// SOCKET

void handle_SC_Socket_Tcp() {
    kernel->machine->WriteRegister(2, SysSocketTcp());
    return IncreasePC();
}

void handle_SC_Connect_Socket() {
    int socketId = kernel->machine->ReadRegister(4);
    int virtAddr = kernel->machine->ReadRegister(5);
    char *ipAddress = User2System(virtAddr);
    int portID = kernel->machine->ReadRegister(6);
    kernel->machine->WriteRegister(2, SysConnectSocket(socketId, ipAddress, portID));
    return IncreasePC();
}

void handle_SC_Send() {
    int socketId = kernel->machine->ReadRegister(4);
    int virtAddr = kernel->machine->ReadRegister(5);
    char* buffer = User2System(virtAddr);
    int len = kernel->machine->ReadRegister(6);
    
    kernel->machine->WriteRegister(2, SysSendSocket(socketId, buffer, len));
    return IncreasePC();
}

void handle_SC_Recieve() {
    int socketId = kernel->machine->ReadRegister(4);
    int virtAddr = kernel->machine->ReadRegister(5);
    char* buffer = User2System(virtAddr);
    kernel->machine->WriteRegister(2, SysReceiveSocket(socketId, buffer));
    System2User(buffer, virtAddr);
    return IncreasePC();
}

void handle_SC_Close_Socket() {
    int socketId = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysCloseSocket(socketId));
    return IncreasePC();
}

// SEMAPHORE

void handle_SC_CreateSemaphore() {
    int virtAddr = kernel->machine->ReadRegister(4);
    int semval = kernel->machine->ReadRegister(5);

    char* name = User2System(virtAddr);
    if (name == NULL) {
        DEBUG(dbgSys, "\n Not enough memory in System");
        ASSERT(false);
        kernel->machine->WriteRegister(2, -1);
        delete[] name;
        return IncreasePC();
    }

    kernel->machine->WriteRegister(2, SysCreateSemaphore(name, semval));
    delete[] name;
    return IncreasePC();
}

void handle_SC_Wait() {
    int virtAddr = kernel->machine->ReadRegister(4);

    char* name = User2System(virtAddr);
    if (name == NULL) {
        DEBUG(dbgSys, "\n Not enough memory in System");
        ASSERT(false);
        kernel->machine->WriteRegister(2, -1);
        delete[] name;
        return IncreasePC();
    }

    kernel->machine->WriteRegister(2, SysWait(name));
    delete[] name;
    return IncreasePC();
}

void handle_SC_Signal() {
    int virtAddr = kernel->machine->ReadRegister(4);

    char* name = User2System(virtAddr);
    if (name == NULL) {
        DEBUG(dbgSys, "\n Not enough memory in System");
        ASSERT(false);
        kernel->machine->WriteRegister(2, -1);
        delete[] name;
        return IncreasePC();
    }

    kernel->machine->WriteRegister(2, SysSignal(name));
    delete[] name;
    return IncreasePC();
}

void handle_SC_GetPid() {
    kernel->machine->WriteRegister(2, SysGetPid());
    return IncreasePC();
}


void ExceptionHandler(ExceptionType which) {
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
        case NoException:  // return control to kernel
            kernel->interrupt->setStatus(SystemMode);
            DEBUG(dbgSys, "Switch to system mode\n");
            break;
        case PageFaultException:
		DEBUG('a', "\n No valid translation found");
		printf("\n\n No valid translation found");
		SysHalt();
		break;

	case ReadOnlyException:
		DEBUG('a', "\n Write attempted to page marked read-only");
		printf("\n\n Write attempted to page marked read-only");
		SysHalt();
		break;

	case BusErrorException:
		DEBUG('a', "\n Translation resulted invalid physical address");
		printf("\n\n Translation resulted invalid physical address");
		SysHalt();
		break;

	case AddressErrorException:
		DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		SysHalt();
		break;

	case OverflowException:
		DEBUG('a', "\nInteger overflow in add or sub.");
		printf("\n\n Integer overflow in add or sub.");
		SysHalt();
		break;

	case IllegalInstrException:
		DEBUG('a', "\n Unimplemented or reserved instr.");
		printf("\n\n Unimplemented or reserved instr.");
		SysHalt();
		break;

	case NumExceptionTypes:
		DEBUG('a', "\n Number exception types");
		printf("\n\n Number exception types");
		SysHalt();
		break;

        case SyscallException:
            switch (type) {
                case SC_Halt:
                    return handle_SC_Halt();
                case SC_Add:
                    return handle_SC_Add();
                case SC_ReadNum:
                    return handle_SC_ReadNum();
                case SC_PrintNum:
                    return handle_SC_PrintNum();
                case SC_ReadChar:
                    return handle_SC_ReadChar();
                case SC_PrintChar:
                    return handle_SC_PrintChar();
                case SC_RandomNum:
                    return handle_SC_RandomNum();
                case SC_ReadString:
                    return handle_SC_ReadString();
                case SC_PrintString:
                    return handle_SC_PrintString();
                case SC_CreateFile:
                    return handle_SC_CreateFile();
                case SC_Open:
                    return handle_SC_Open();
                case SC_Close:
                    return handle_SC_Close();
                case SC_Read:
                    return handle_SC_Read();
                case SC_Write:
                    return handle_SC_Write();
                case SC_Seek:
                    return handle_SC_Seek();
                case SC_Remove:
                    return handle_SC_RemoveFile();
                case SC_SocketTcp:
                    return handle_SC_Socket_Tcp();
                case SC_ConnectSocket:
                    return handle_SC_Connect_Socket();
                case SC_Send:
                    return handle_SC_Send();
                case SC_Receive:
                    return handle_SC_Recieve();
                case SC_CloseSocket:
                    return handle_SC_Close_Socket();
                case SC_Exec:
                    return handle_SC_Exec();
                case SC_Join:
                    return handle_SC_Join();
                case SC_Exit:
                    return handle_SC_Exit();
                case SC_CreateSemaphore:
                    return handle_SC_CreateSemaphore();
                case SC_Wait:
                    return handle_SC_Wait();
                case SC_Signal:
                    return handle_SC_Signal();
                
                /**
                 * Handle all not implemented syscalls
                 * If you want to write a new handler for syscall:
                 * - Remove it from this list below
                 * - Write handle_SC_name()
                 * - Add new case for SC_name
                 */
                case SC_Create:
                case SC_ThreadFork:
                case SC_ThreadYield:
                case SC_ExecV:
                case SC_ThreadExit:
                case SC_ThreadJoin:
                    return handle_not_implemented_SC(type);

                default:
                    cerr << "Unexpected system call " << type << "\n";
                    break;
            }
            break;
        default:
            cerr << "Unexpected user mode exception" << (int)which << "\n";
            break;
    }
    ASSERTNOTREACHED();
}
