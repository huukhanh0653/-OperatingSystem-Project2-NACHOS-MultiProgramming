#include "syscall.h"

#define MODE_READWRITE 0
#define MODE_READ 1

#define stdin 0
#define stdout 1

#define MAX_FILENAME_LENGTH 32
#define MAX_FILENAME_Character 100

int main() {
    char buffer[MAX_FILENAME_Character];
    char filename[256];
    int i;
    int write;
    int fileid;
    int read;
    int len;

    PrintString("Enter the filename to read and copy: ");
	ReadString(filename, MAX_FILENAME_LENGTH);
    fileid = Open(filename, MODE_READ);

    if (fileid != -1) // Check error
	{
        fileid = Open(filename, 1);
        read = Read(buffer, MAX_FILENAME_Character, fileid);
        while (buffer[len] != '\0') ++len;
        // PrintNum(len);
        Close(fileid);
    }
    else
	{
		PrintString(" -> failed to open file!!\n\n");
	}

    // PrintString(buffer);
    PrintString("Enter the filename to write: ");
	ReadString(filename, MAX_FILENAME_LENGTH);
    fileid = Open(filename, MODE_READWRITE);

    if (fileid != -1) // Check error
	{
        write = Write(buffer, len, fileid);
        PrintString("Write ");
        PrintNum(write);
        // PrintNum(fileid);
        PrintString(" characters: ");
        PrintString(buffer);
        PrintString(" to: ");
        PrintString(filename);
        PrintString(". \n");
        Close(fileid);
    }
    else
	{
		PrintString(" -> failed to open file!!\n\n");
	}

    return 0;
}
