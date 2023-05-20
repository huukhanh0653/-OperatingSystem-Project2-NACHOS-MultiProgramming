#include "syscall.h"
#define MAX_LENGTH 255

int main() {
    char fileName[256];
    int length;

    // PrintString("Enter file's name's length: ");
    // length = ReadNum();
    PrintString("Enter file's name: ");
    ReadString(fileName, MAX_LENGTH);

    if (CreateFile(fileName) == 0) {
        PrintString("File ");
        PrintString(fileName);
        PrintString(" created successfully!\n");
    } else
        PrintString("Create file failed\n");
}
