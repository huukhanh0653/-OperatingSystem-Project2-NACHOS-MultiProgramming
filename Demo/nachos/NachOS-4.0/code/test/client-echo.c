#include "syscall.h"
#define PORT 8080

int main() {
    int socketId[3];
    char messageRecieve[100];
    int i = 0;
    for (i = 0; i < 4; i++) {
        socketId[i] = SocketTcp();
        if(socketId[i] < 0) {
            PrintString("Create Socket Fail !!!");
        }

        if(ConnectSocket(socketId[i], "127.0.0.1", PORT) < 0) {
            PrintString("Connect Fail !!! \n");
            Halt();
        }

        Send(socketId[i], "Test Socket", 11);
        Receive(socketId[i], messageRecieve);
        PrintString(messageRecieve);
        PrintChar('\n');
        Close(socketId[i]);
    }
  
    Halt();
}