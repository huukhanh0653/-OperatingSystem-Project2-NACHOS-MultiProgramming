#include "exception_utils.h"

void HandleHalt()
{
	DEBUG(dbgSys, "\n Shutdown, initiated by user program.\n");
	SysHalt();
	ASSERTNOTREACHED();
}

// Handle syscall add
void HandleAdd()
{
	DEBUG(dbgSys, "\n Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

	/* Process SysAdd System call*/
	int result;
	result = SysAdd(/*op1*/ (int)kernel->machine->ReadRegister(4), /*op2*/ (int)kernel->machine->ReadRegister(5));
	DEBUG(dbgSys, "\n Add returning with " << result << "\n");

	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	cout << "Add: " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << " = " << result << "\n";

	return IncPC();
}

// Handle syscall create file
void HandleCreate()
{
	int virtAddr;
	char *filename;
	DEBUG('a', "\n SC_CreateFile call ...");
	DEBUG('a', "\n Reading virtual address of filename");

	virtAddr = kernel->machine->ReadRegister(4);
	DEBUG('a', "\n Reading filename.");

	// Sao chep khong gian bo nho User sang System, voi do dang toi da la (32 + 1) bytes
	filename = User2System(virtAddr, MaxFileNameLength + 1);

	if (SysCreate(filename))
		kernel->machine->WriteRegister(2, 0);
	else
		kernel->machine->WriteRegister(2, -1);

	delete filename;
	return IncPC();
}

// Handle syscall delete file
void HandleDelete()
{
	int virtAddr = kernel->machine->ReadRegister(4);		   // Lay dia chi cua tham so name tu thanh ghi so 4
	char *filename = User2System(virtAddr, MaxFileNameLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength

	kernel->machine->WriteRegister(2, SysRemove(filename));

	return IncPC();
}

// Handle syscall open file
void HandleOpen()
{
	int virtAddr = kernel->machine->ReadRegister(4);		   // Lay dia chi cua tham so name tu thanh ghi so 4
	int type = kernel->machine->ReadRegister(5);			   // Lay tham so type tu thanh ghi so 5
	char *filename = User2System(virtAddr, MaxFileNameLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength

	kernel->machine->WriteRegister(2, SysOpen(filename, type));

	delete[] filename;
	return IncPC();
}

// Handle syscall close file
void HandleClose()
{
	// Input id cua file(OpenFileID)
	//  Output: 0: thanh cong, -1 that bai
	int id = kernel->machine->ReadRegister(4); // Lay tham so id tu thanh ghi so 4

	kernel->machine->WriteRegister(2, SysClose(id));

	return IncPC();
}
void HandleRead()
{
	int virtAddr = kernel->machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
	int charCount = kernel->machine->ReadRegister(5); // Lay tham so charCount tu thanh ghi so 5
	char *buffer = User2System(virtAddr, charCount);  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charCount
	int fileId = kernel->machine->ReadRegister(6);	  // Lay tham so id tu thanh ghi so 6

	DEBUG(dbgFile, "Read " << charCount << " chars from file " << fileId << "\n");

	kernel->machine->WriteRegister(2, SysRead(buffer, charCount, fileId));

	System2User(virtAddr, charCount, buffer); // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer dai charCount

	delete[] buffer;
	return IncPC();
}

void HandleWrite()
{
	int virtAddr = kernel->machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
	int charCount = kernel->machine->ReadRegister(5); // Lay tham so charCount tu thanh ghi so 5
	char *buffer = User2System(virtAddr, charCount);  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charCount
	int fileId = kernel->machine->ReadRegister(6);	  // Lay tham so id tu thanh ghi so 6

	kernel->machine->WriteRegister(2, SysWrite(buffer, charCount, fileId));

	System2User(virtAddr, charCount, buffer); // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer dai charCount

	delete[] buffer;
	return IncPC();
}

void HandleSeek()
{
	int seekPos = kernel->machine->ReadRegister(4); // Lay tham so pos tu thanh ghi so 4
	int fileId = kernel->machine->ReadRegister(5);	// Lay tham so id tu thanh ghi so 5

	kernel->machine->WriteRegister(2, SysSeek(seekPos, fileId));
	return IncPC();
}

void HandleConnect()
{
	int arg1 = kernel->machine->ReadRegister(4); // socketid
	int arg2 = kernel->machine->ReadRegister(5); // ip
	int arg3 = kernel->machine->ReadRegister(6); // port

	// convert the IP address from user space to kernel space
	char *kernelIP = new char[MAX_STRING_SIZE];
	copyStringFromMachine(arg2, kernelIP, MAX_STRING_SIZE);

	// call the connect function from network utility library
	int result = SysConnect(arg1, kernelIP, arg3);
	// set the return value
	kernel->machine->WriteRegister(2, result);

	delete[] kernelIP;
	return IncPC();
}

void HandleSocketTCP()
{
	kernel->machine->WriteRegister(2, SysSocketTCP());
	return IncPC();
}

// Send the data to the server and set the timeout of 20 seconds
//! For advanced part, we don't use this function
int SocketSend(char *buffer, int charCount, int fileId)
{
	int shortRetval = -1;
	struct timeval tv;
	tv.tv_sec = 20; /* 20 Secs Timeout */
	tv.tv_usec = 0;
	if (setsockopt(fileId, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv)) < 0)
	{
		printf("Time Out\n");
		return -1;
	}
	shortRetval = send(fileId, buffer, charCount, 0);

	return shortRetval;
}

// receive the data from the server
//! For advanced part, we don't use this function
int SocketReceive(char *buffer, int charCount, int fileId)
{
	int shortRetval = -1;
	struct timeval tv;
	tv.tv_sec = 20; /* 20 Secs Timeout */
	tv.tv_usec = 0;
	if (setsockopt(fileId, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv)) < 0)
	{
		printf("Time Out\n");
		return -1;
	}
	shortRetval = recv(fileId, buffer, charCount, 0);
	cout << "Response: " << buffer << endl;
	IncPC();
	return shortRetval;
}
