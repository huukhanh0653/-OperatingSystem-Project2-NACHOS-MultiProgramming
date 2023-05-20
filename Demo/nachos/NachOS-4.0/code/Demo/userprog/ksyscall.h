/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__
#define INT32_MIN -2147483648
#define MAX_NUM_LENGTH 11

#include "kernel.h"
#include "synchconsole.h"
#include "ksyscallhelper.h"
#include <stdlib.h>

void SysHalt() { kernel->interrupt->Halt(); }

int SysAdd(int op1, int op2) { return op1 + op2; }

int SysReadNum() {
    readUntilBlank();

    int len = strlen(_numberBuffer);

    // Return 0 if the buffer is empty
    if (len == 0) return 0;

    // Return INT32_MIN if the buffer contains "-2147483648"
    if (strcmp(_numberBuffer, "-2147483648") == 0) return INT32_MIN;

    // Determine if the number is negative
    bool isNegative = (_numberBuffer[0] == '-');

    // Count the number of leading zeros
    int leadingZeros = 0;
    bool isLeading = true;
    int num = 0;

    // Parse the number
    for (int i = isNegative; i < len; ++i) {
        char c = _numberBuffer[i];
        if (c == '0' && isLeading)
            ++leadingZeros;
        else
            isLeading = false;
        if (c < '0' || c > '9') {
                DEBUG(dbgSys, "Expected number but " << _numberBuffer << " found");
                return 0;
            }
            num = num * 10 + (c - '0');
    }

    // If the number has more than one leading zero or a leading zero and a non-zero digit, return 0
    if (leadingZeros > 1 || (leadingZeros && (num || isNegative))) {
        DEBUG(dbgSys, "Expected number but " << _numberBuffer << " found");
        return 0;
    }

    // Negate the number if it is negative
    if (isNegative)
        num = -num;

    // If the length of the buffer is less than or equal to MAX_NUM_LENGTH - 2, return the number
    if (len <= MAX_NUM_LENGTH - 2) return num;

    // Compare the number to the buffer
    if (compareNumAndString(num, _numberBuffer))
        return num;
    else
        DEBUG(dbgSys,
            "Expected int32 number but " << _numberBuffer << " found");

    // Return 0 if the number is not valid
    return 0;
}

void SysPrintNum(int num) {
    // Print "0" if the number is 0
    if (num == 0) {
        kernel->synchConsoleOut->PutChar('0');
        return;
    }

    // Print "-2147483648" if the number is INT32_MIN
    if (num == INT32_MIN) {
        kernel->synchConsoleOut->PutChar('-');
        for (int i = 0; i < 10; ++i)
            kernel->synchConsoleOut->PutChar("2147483648"[i]);
        return;
    }

    // Negate the number if it is negative and print a "-" character
    if (num < 0) {
        kernel->synchConsoleOut->PutChar('-');
        num = -num;
    }

    // Convert the number to a string and print it
    int n = 0;
    while (num) {
        _numberBuffer[n++] = num % 10;
        num /= 10;
    }
    for (int i = n - 1; i >= 0; --i)
        kernel->synchConsoleOut->PutChar(_numberBuffer[i] + '0');
}

char SysReadChar() { return kernel->synchConsoleIn->GetChar(); }

void SysPrintChar(char character) {
    kernel->synchConsoleOut->PutChar(character);
}

int SysRandomNum() { return random(); }

char* SysReadString(int length) {
    char* buffer = new char[length + 1];
    for (int i = 0; i < length; i++) {
        buffer[i] = SysReadChar();
    }
    buffer[length] = '\0';
    return buffer;
}

void SysPrintString(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        kernel->synchConsoleOut->PutChar(buffer[i]);
    }
    kernel->synchConsoleOut->PutChar('\0');
}

bool SysCreateFile(char* fileName) {
    bool success;
    int fileNameLength = strlen(fileName);

    if (fileNameLength == 0) {
        DEBUG(dbgSys, "\nFile name can't be empty");
        success = false;

    } else if (fileName == NULL) {
        DEBUG(dbgSys, "\nNot enough memory in system");
        success = false;

    } else {
        DEBUG(dbgSys, "\nFile's name read successfully");
        if (!kernel->fileSystem->Create(fileName)) {
            DEBUG(dbgSys, "\nError creating file");
            success = false;
        } else {
            success = true;
        }
    }

    return success;
}

int SysOpen(char* fileName, int type) {
    if (type != 0 && type != 1) return -1;

    int id = kernel->fileSystem->Open(fileName, type);
    if (id == -1) return -1;
    DEBUG(dbgSys, "\nOpened file");
    return id;
}

int SysClose(int id) { return kernel->fileSystem->Close(id); }

int SysRead(char* buffer, int charCount, int fileId) {
    if (fileId == 0) {
        return kernel->synchConsoleIn->GetString(buffer, charCount);
    }
    return kernel->fileSystem->Read(buffer, charCount, fileId);
}

int SysWrite(char* buffer, int charCount, int fileId) {
    if (fileId == 1) {
        return kernel->synchConsoleOut->PutString(buffer, charCount);
    }
    return kernel->fileSystem->Write(buffer, charCount, fileId);
}

int SysSeek(int seekPos, int fileId) {
    if (fileId <= 1) {
        DEBUG(dbgSys, "\nCan't seek in console");
        return -1;
    }
    return kernel->fileSystem->Seek(seekPos, fileId);
}

int SysSocketTcp() {
    return kernel->socket->Insert();
}

int SysConnectSocket(int socketId, char* ipAddress, int portID) {
    return kernel->socket->Connect(socketId, ipAddress, portID);
}

int SysSendSocket(int socketId, char *buffer, int len) {
    return kernel->socket->Send(socketId, buffer);
}

int SysReceiveSocket(int socketId, char *buffer) {
    return kernel->socket->Receive(socketId, buffer);
}

int SysCloseSocket(int socketId) {
    return kernel->socket->Close(socketId);
}

int SysExec(char* name) {
    // cerr << "call: `" << name  << "`"<< endl;
    OpenFile* oFile = kernel->fileSystem->Open(name);
    if (oFile == NULL) {
        DEBUG(dbgSys, "\nExec:: Can't open this file.");
        return -1;
    }

    delete oFile;

    // Return child process id
    return kernel->pTab->ExecUpdate(name);
}

int SysJoin(int id) { return kernel->pTab->JoinUpdate(id); }

int SysExit(int id) { return kernel->pTab->ExitUpdate(id); }

int SysCreateSemaphore(char* name, int initialValue) {
    int res = kernel->semTab->Create(name, initialValue);

    if (res == -1) {
        DEBUG('a', "\nError creating semaphore");
        delete[] name;
        return -1;
    }

    return 0;
}

int SysWait(char* name) {
    int res = kernel->semTab->Wait(name);

    if (res == -1) {
        DEBUG('a', "\nSemaphore not found");
        delete[] name;
        return -1;
    }

    return 0;
}

int SysSignal(char* name) {
    int res = kernel->semTab->Signal(name);

    if (res == -1) {
        DEBUG('a', "\nSemaphore not found");
        delete[] name;
        return -1;
    }

    return 0;
}

int SysGetPid() { return kernel->currentThread->processID; }

#endif /* ! __USERPROG_KSYSCALL_H__ */
