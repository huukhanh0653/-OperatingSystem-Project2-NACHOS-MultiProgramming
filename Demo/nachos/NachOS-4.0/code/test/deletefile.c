#include "syscall.h"
#define MAX_LENGTH 255

int main() {
    char fileName[256];
    int length;

    PrintString("Enter file's name: ");
    ReadString(fileName, MAX_LENGTH);

    if (Remove(fileName) == 0) {
        PrintString("File ");
        PrintString(fileName);
        PrintString(" delete successfully!\n");
    } else
        PrintString("delete file failed\n");

    return 0;
}
