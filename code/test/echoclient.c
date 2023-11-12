#include "syscall.h"
#include "copyright.h"
#define maxlenmasage 100


int main()
{

    const int numSocket=4;
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

        //Read message from console to send to server
        Write("Input message of socket:",50,consoleOutputID);
        Read(messagesend[iter],maxlenmasage,consoleInputID);

        //Send message to server
        Send(socketid[iter],messagesend[iter],100);

        if (messagesend[iter][0]=='e' && messagesend[iter][1]=='x'
        && messagesend[iter][2]=='i' && messagesend[iter][3]=='t')
        {
            Write("Client exit\n",50,consoleOutputID);
            break;
        }
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
    Halt();
    return 0;
}