#ifndef UTILS_H
#define UTILS_H

#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "ptable.h"
#include "stable.h"
#include "pcblock.h"

#define MaxFileNameLength 32

void IncPC()
{
	// Similar to: register[PrevPCReg] = register[PCReg].
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	// Similar to: register[PCReg] = register[NextPCReg].
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(NextPCReg));

	// Similar to: register[NextPCReg] = PCReg right after.
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(NextPCReg) + 4);
}

char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;

	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);

	// printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		MACHINE->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

// Copy memory zone from System space to User space

int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		MACHINE->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

#endif