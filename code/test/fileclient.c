#include "syscall.h"
int main(int argc, char** argv)
{
    char filename1[255]; // filename source
    char filename2[255]; // filename destination
    int OnlyReadType=1;  //type open file source
    int ReadWriteType=0; //type open file destination
    int fileID1=-1;      //fileID source
    int fileID2=-1;      //fileID destination
    char buffersend[255];  //buffer to send
    char bufferrecv[255]; //buffer to receive
    int size = 0;      //Num of char read from file source
    int socketID=-1;
    int connectID=-1;
    int sendID=-1;
    int receiveID=-1;
    int c,d;
    int socketclose=-1;
    char bufferclose[100];
    //Read name file source from console
    int a = Write("Nhap vao ten file nguon: ", 100, consoleOutputID);
    int b = Read(filename1, 255, consoleInputID);
    //Open file source
    fileID1 = Open(filename1, OnlyReadType);
    if(fileID1 == -1)
    {
        Write("Khong the mo file nguon\n", 100, consoleOutputID);
        Halt();
    }
    //Read name file destination from console
    c = Write("Nhap vao ten file dich: ", 100, consoleOutputID);
    d = Read(filename2, 255, consoleInputID);

    //Open file destination
    fileID2 = Open(filename2, ReadWriteType);
    if(fileID2 == -1)
    {
        Create(filename2);
    }
    //Open file destination again
    fileID2 = Open(filename2, ReadWriteType);
    if(fileID2 == -1)
    {
        Write("Khong the mo file dich\n", 100, consoleOutputID);
        Halt();
    }

    //Read file from source
    Read(buffersend, 255, fileID1);


    //Create socket ,connect and send data to server
    socketID = SocketTCP();
    connectID = Connect(socketID, "127.0.0.1",1234);
    sendID = Send(socketID, buffersend, 100);
    if(sendID == -1)
    {
        Write("Khong the gui data den socket \n", 100, consoleOutputID);
        Halt();
    }
    // Receive data from server
    receiveID = Receive(socketID, bufferrecv, 100);
    if(receiveID == -1)
    {
        Write("Khong the nhan data tu socket \n", 100, consoleOutputID);
        Halt();
    }
    // Write data to destination
    Write(bufferrecv,6, fileID2);
    //Close socket
    Close_Socket(socketID);
    //Close file source and destination
    Close(fileID1);
    Close(fileID2);

    Write("Gui nhan du lieu thanh cong\n", 100, consoleOutputID);
    Halt();
    return 0;
}
