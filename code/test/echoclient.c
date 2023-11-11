#include "syscall.h"
#include "copyright.h"
#define maxlenmasage 100


int main()
{

    const int numSocket=5;  //Last socket is used to close server
    int socketid[numSocket];
    char *messagesend[5];
    char *messagerecv[5];
    int iter=0;
    //Create 4 socket
    while(iter<numSocket)
    {
        socketid[iter]=SocketTCP();
        iter++;
    }

    //Connect each socket to Server with port 8888 and ip "0.0.0.0"
    iter=0;
    while(iter<numSocket)
    {
        //Connect to server
        Connect(socketid[iter], "127.0.0.1", 1234);

        if(iter==numSocket-1) {
            Send(socketid[iter],"exit",100);
            Write("Closed server\n",50,consoleOutputID);
            Close(socketid[iter]);
            break;
        }
        //Read message from console to send to server
        Write("Input message of socket:",50,consoleOutputID);
        Read(messagesend[iter],maxlenmasage,consoleInputID);

        //Send message to server
        Send(socketid[iter],messagesend[iter],100);

        //Receive message from server
        Receive(socketid[iter],messagerecv[iter],maxlenmasage);

        //Print message received from server
        Write("Messages received:",50,consoleOutputID);
        Write(messagerecv[iter],100,consoleOutputID);
        Write("\n",1,consoleOutputID);

        //Close socket
        Close(socketid[iter]);
        iter++;
    }
    //Send message to close server


    Halt();
    return 0;
}