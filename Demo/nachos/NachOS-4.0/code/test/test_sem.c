
#include "syscall.h"
int main() {
    SpaceId newProc;
    OpenFileId input = _ConsoleInput;
    OpenFileId output = _ConsoleOutput;
    char ch, buffer[60];
    int i;

    int f_success = CreateSemaphore("Continue", 1);
    if (f_success == -1) {
        PrintString("CreateSemaphore failed\n");
        return 1;
    }

    while (1) {
        Wait("Continue");
        PrintString("Enter program you want to run: ");

        i = 0;

        do {
            Read(&buffer[i], 1, input);

        } while (buffer[i++] != '\n');

        buffer[--i] = '\0';

        if (i > 0) {
            
            if (buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't') 
            {
                PrintString("Exit shell");
                PrintString(". \n");
                Halt();
            }
            newProc = Exec(buffer);
            if (newProc == -1)
            {
                PrintString("Cannot find that program ");
                PrintString(". \n");
            }
            else
            {
                Join(newProc);
            }
        }

        PrintString("Do You want to continue (y/n): ");

        i = 0;

        do {
            Read(&buffer[i], 1, input);

        } while (buffer[i++] != '\n');

        buffer[--i] = '\0';

        if (i > 0) {
            if (buffer[0] == 'y')
            {
                Signal("Continue");
            }
            else
            {
                PrintString("Exit shell");
                PrintString(". \n");
                Halt();
            }
        }
    }
}
