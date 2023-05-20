#include "syscall.h"
#define MAX_FILENAME_LENGTH 32
#define MAX_FILENAME_Character 100

int main()
{
    char buffer[MAX_FILENAME_Character];
    char buffer2[MAX_FILENAME_Character];
    int i;
    int write;
	char filename[MAX_FILENAME_LENGTH];
	int fileid;
	int read;
	int len = 0, len2 = 0;
	int length;

	PrintString("Enter the filename 1 to read: ");
	ReadString(filename, MAX_FILENAME_LENGTH);
    fileid = Open(filename, 1);

    if (fileid != -1) // Check error
	{
        read = Read(buffer, MAX_FILENAME_Character, fileid);
        while (buffer[len] != '\0') ++len;
        Close(fileid);
    }
    else
	{
		PrintString(" -> failed to open file!!\n\n");
	}

    PrintString("Enter the filename 2 to read: ");
	ReadString(filename, MAX_FILENAME_LENGTH);
    fileid = Open(filename, 1);

    if (fileid != -1) // Check error
	{
        read = Read(buffer2, MAX_FILENAME_Character, fileid);
        while (buffer2[len2] != '\0') ++len2;
        PrintString("Read ");
        PrintNum(Add(len, len2));
        PrintString(" characters:");
        PrintString(buffer);
        PrintString(". \n");
        PrintString(buffer2);
        PrintString(". \n");
        Close(fileid);
    }
    else
	{
		PrintString(" -> failed to open file!!\n\n");
	}

    PrintString("Enter the filename to write to: ");
	ReadString(filename, MAX_FILENAME_LENGTH);
    fileid = Open(filename, 0);

    if (fileid != -1) // Check error
	{
        write = Write(buffer, len, fileid);
        write = Write("\n", 1, fileid);
        write = Write(buffer2, len2, fileid);
        PrintString("Write to: ");
        PrintString(filename);
        Close(fileid);
    }
    else
	{
		PrintString(" -> failed to open file!!\n\n");
	}

    return 0;
}
