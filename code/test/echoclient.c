#include "syscall.h"
#include "copyright.h"


int main()
{
    int i;
    char buf[100];
    int socketid1=SocketTCP();
    int rsconnect=Connect(socketid1,"0.0.0.0",8888);
    int rssend=Send(socketid1,"hello baby",10);
    int rsrecv=Receive(socketid1,buf,100);
    int rssend2=Send(socketid1,"Close",5);
    int rsclose=Close_Socket(socketid1);
    Halt();
    return 0;
}