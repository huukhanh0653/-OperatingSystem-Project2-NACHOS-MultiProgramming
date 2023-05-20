#ifndef SOCKET_H
#define SOCKET_H
#include "sysdep.h"

#define MAX_FILEDESCRIPTOR 20
#define MAX_ID 20

typedef int socketDescriptor;

class Socket {
   private:
    socketDescriptor* fileDescriptor;

   public:
    Socket() {
        fileDescriptor = new socketDescriptor[MAX_FILEDESCRIPTOR];
        for (int i = 0; i < MAX_FILEDESCRIPTOR; i++) {
            fileDescriptor[i] = -1;
        }
    }

    int Insert() {
        int freeIndex = -1;
        for (int i = 0; i < MAX_FILEDESCRIPTOR; i++) {
            if (fileDescriptor[i] == -1) {
                freeIndex = i;
                break;
            }
        }

        if (freeIndex == -1) {
            return -1;
        }

        int socketDescriptor = OpenSocketTCP();
        fileDescriptor[freeIndex] = socketDescriptor;
        return socketDescriptor;
    }

    int Connect(int socketDescriptor, char* ip, int port) {
        return ConnectSocket(socketDescriptor, ip, port);
    }

    int Send(int socketDescriptor, char *buffer) {  
        for (int i = 0; i < MAX_FILEDESCRIPTOR; i++) {
            if (fileDescriptor[i] == socketDescriptor) {
                SendMessageToSocket(socketDescriptor, buffer);
                return 0;
            }
        }

        return -1;
    }
    
    int Receive(int socketDescriptor, char *buffer) {
        for (int i = 0; i < MAX_FILEDESCRIPTOR; i++) {
            if (fileDescriptor[i] == socketDescriptor) {
                RecieveMessageFromSocket(socketDescriptor, buffer);
                return 0;
            }
        }
        return -1;
    }

    int Close(int socketDescriptor) {
         for (int i = 0; i < MAX_FILEDESCRIPTOR; i++) {
            if (fileDescriptor[i] == socketDescriptor) {
                CloseSocket(socketDescriptor);
                return 0;
            }
        }

        return -1;
    }
  
    ~Socket() {
        delete[] fileDescriptor;
    }
};

#endif
