#include "syscall.h"
#define MAX_FILENAME_LENGTH 32
#define MAX_FILENAME_Character 100

int main()
{
    char buffer[MAX_FILENAME_Character];
    int i;
    int write;
	char filename[MAX_FILENAME_LENGTH];
	int fileid;
	int read;
	int len = 0;
	int length;

	PrintString("Enter the filename to read: ");
	ReadString(filename, MAX_FILENAME_LENGTH);
    fileid = Open(filename, 1);

    if (fileid != -1) // Check error
	{
        read = Read(buffer, MAX_FILENAME_Character, fileid);
        while (buffer[len] != '\0') ++len;
        PrintString("Read ");
        PrintNum(len);
        PrintString(" characters:");
        PrintString(buffer);
        PrintString(". \n");
        Close(fileid);
    }
    else
	{
		PrintString(" -> failed to open file!!\n\n");
	}

    return 0;
}
