#include "syscall.h"
#define MAX_LENGTH 32
#define MAX_LENGTH_FILE 100
#define PORT 8080
#define MAX_FILENAME_LENGTH 32

int main() {
    int openFileId;
	int fileSize;
	int len;
	char c;
	char fileName[MAX_LENGTH];
	char content[MAX_LENGTH];
    char messageRecieve[MAX_LENGTH];
    int socketId;
    int i;

	PrintString("Enter the filename to read: ");
	ReadString(fileName, MAX_FILENAME_LENGTH);

	openFileId = Open(fileName, 1);
	
	if (openFileId != -1)
	{
		fileSize = Seek(-1, openFileId);
		PrintString("\n");
		i = 0;
		Seek(0, openFileId);
		for (; i < fileSize; i++)
		{
			Read(&c, 1, openFileId);
			content[i] = c;
		}
        content[fileSize] = '\0';
		Close(openFileId);

        socketId = SocketTcp();
        if(socketId < 0) {
            PrintString("Create Socket Fail !!!");
        }

        if(ConnectSocket(socketId, "127.0.0.1", PORT) < 0) {
            PrintString("Connect Fail !!! \n");
            Halt();
        }

		// PrintString("Send file to socket \n");
        Send(socketId, content, fileSize);
        Receive(socketId, messageRecieve);


        Close(socketId);
	}
	else
	{
		PrintString("Open File Fail!!\n\n");
	}
    
    // Write into file
	PrintString("Receive: \n");
	PrintString(messageRecieve);
	PrintString("\n");
	PrintString("Enter the filename to write: ");
	ReadString(fileName, MAX_FILENAME_LENGTH);
    openFileId = Open(fileName, 0);
	
	if (openFileId != -1)
	{
		fileSize = Seek(-1, openFileId);
		// i = 0;
		Seek(0, openFileId);
        Write(messageRecieve, fileSize, openFileId);
		Close(openFileId);
	}
	else
	{
		PrintString("Open File Fail!!\n\n");
	}
    Halt();
}