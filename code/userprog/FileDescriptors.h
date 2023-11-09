#include"filesys.h"
#include "kernel.h"
#include "string.h"
#include "post.h"
#include "synchconsole.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define File_Mode 0
#define Socket_Mode 1
#define Stdin_Mode 2
#define Stdout_Mode 3

typedef int OpenFileID ;

//===============================================
class FileDescriptor{
private:
    int _mode;
    OpenFileID _id;
    OpenFile* _file=NULL;
    char *_name;
public:
    FileDescriptor();
    ~FileDescriptor();
    int GetIdDescriptor()
    {
        return _id;
    }
    int GetModeDescriptor()
    {
        return _mode;
    }
    OpenFile* GetFileDescriptor()
    {
        return _file;
    }
    char* GetNameDescriptor()
    {
        return _name;
    }
    //Function for File
    int Open_File(char* name,int type);
    int Close_File();
    int Read(char* buffer,int size);
    int Write(char* buffer,int size);
    int Seek(int pos);
    //Function for Socket
    int SocketTCP();
    int Connect(int socketid,char* host,int port);
//    3.	Cài đặt system call int Send(int socketid, char *buffer, int len), int Receive(int socketid, char *buffer, int len). Gửi và nhận dữ liệu từ socket.
//    - Nếu thành công thì trả về số lượng bytes gửi đi hoặc nhận được
//    - Nếu kết nối bị đóng trả về 0
//    - Nếu thất bại trả về -1
    int Send(int socketid,char* buffer,int len);
    int Receive(int socketid,char* buffer,int len);
    int Close_Socket();
};
FileDescriptor::FileDescriptor(){
    _mode=-1;
    _id=-1;
    _file=NULL;
    _name=NULL;
}
FileDescriptor::~FileDescriptor(){
    _mode=-1;
    _id=-1;
}
int FileDescriptor::Open_File(char* name,int type){
    _file=kernel->fileSystem->Open(name,type);
    if(_file==NULL){
        return -1;
    }
    else{
        _name=name;
        _mode=File_Mode;
        _id=_file->FileID();
        return _id;
    }
}
int FileDescriptor::Close_File(){
    if(_id==-1)
        return -1;
    else
    {
        Close(_id);
        _mode=-1;
        _id=-1;
        //_file->~OpenFile();
        _file=NULL;
        _name=NULL;
        return 1;
    }
}
int FileDescriptor::SocketTCP(){
    int socketid;
    socketid=socket(AF_INET,SOCK_STREAM,0);
    if(socketid<0){
        DEBUG(dbgSys, "\n Error: Can not create socket");
        return -1;
    }
    else
    {
        this->_mode=Socket_Mode;
        this->_id=socketid;
        this->_file=NULL;
        this->_name=NULL;
        return socketid;
    }
}
int FileDescriptor::Connect(int socketid,char* host,int port){
    struct sockaddr_in serverAddr;
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(port);
    serverAddr.sin_addr.s_addr=inet_addr(host);
    if(connect(socketid,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0){
        DEBUG(dbgSys, "\n Error: Can not connect to server");
        return -1;
    }
    else
    {
        return 0;
    }
}

//void Send(PacketHeader pktHdr, MailHeader mailHdr, char *data);
int FileDescriptor::Send(int socketid,char* buffer,int len){
    //Thieu ====>>>>>Neu dong ket noi thi tra ve 0
    //Set time out
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    setsockopt(socketid, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
    //Send data
    int sent=send(socketid,buffer,len,0);
    if(sent<0){
        DEBUG(dbgSys, "\n Error: Can not send data");
        return -1;
    }
    else
    {
        return sent;
    }
}
int FileDescriptor::Receive(int socketid,char* buffer,int len){
    //Thieu ====>>>>>Neu dong ket noi thi tra ve 0
    //Set time out
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    setsockopt(socketid, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
    //Receive data
    int received=recv(socketid,buffer,len,0);
    if(received<0){
        DEBUG(dbgSys, "\n Error: Can not receive data");
        return -1;
    }
    else
    {
        DEBUG(dbgSys, "\n Received data: "<<buffer);
        return received;
    }
}
int FileDescriptor::Close_Socket(){
    if(_id==-1)
        return -1;
    else
    {
         close(_id);
        _mode=-1;
        _id=-1;
        return 1;
    }
}
//=======================================================
class Table{
private:
    FileDescriptor *_table;
    const int _capacity=20;
public:
    Table();
    ~Table();
    int FindFreeSlot();
    int IsOpening(OpenFileID id);
    int IsOpeningWithName(char* name);
    int Close(OpenFileID id);
    OpenFileID Open(char* name,int type);
    void PrintOpeningFile();
    int SocketTCP();
    int Connect(int socketid,char* host,int port);
    int Send(int socketid,char* buffer,int len);
    int Receive(int socketid,char* buffer,int len);
    int Close_Socket(int socketid);

};

Table::Table(){
    _table=new FileDescriptor[_capacity];
}
Table::~Table(){
    //delete _table;   //De delete _table thi chay xong bi Segmentation fault (core dumped)
}
void Table::PrintOpeningFile(){
    for(int i=2;i<_capacity;i++){
        if(_table[i].GetModeDescriptor()!=-1)
            cout << _table[i].GetNameDescriptor() << endl;
    }
}
int Table::FindFreeSlot(){
    for(int i=2;i<_capacity;i++){
        if(_table[i].GetModeDescriptor()==-1)
            return i;
    }
    return -1;
}
OpenFileID Table::Open(char* name,int type){
    int slot=FindFreeSlot();
    if(slot==-1){
        DEBUG(dbgSys, "\n Error: File table is full");
        return -1;
    }
    else{
        if(_table[slot].Open_File(name,type)==-1)
        {
            return -1;
        }
        else
        {
            return _table[slot].GetIdDescriptor();
        }
    }
}
int Table::IsOpening(OpenFileID id){
    for (int i=2;i<_capacity;i++){
        if(_table[i].GetIdDescriptor()==id)
            return i;
    }
    return -1;
}
int Table::IsOpeningWithName(char* name){
    for (int i=2;i<_capacity;i++){
        if(_table[i].GetModeDescriptor()!=-1 && strcasecmp(_table[i].GetNameDescriptor(),name)==0)
            return i;
    }
    return -1;
}
int Table::Close(OpenFileID id){
    int index=IsOpening(id);
    if(index!=-1){
        return _table[index].Close_File();
    }
    else
    {
        DEBUG(dbgSys, "\n Error: File is not opening");
        return -1;
    }
}
int Table::SocketTCP(){
    int slot=FindFreeSlot();
    if(slot==-1){
        DEBUG(dbgSys, "\n Error: File table is full");
        return -1;
    }
    else{
        return _table[slot].SocketTCP();
    }
}
int Table::Connect(int socketid,char* host,int port){
    int index=IsOpening(socketid);
    if(index!=-1){
        return _table[index].Connect(socketid,host,port);
    }
    else
    {
        DEBUG(dbgSys, "\n Error: Socket is not opening");
        return -1;
    }
}
int Table::Send(int socketid,char* buffer,int len){
    int index=IsOpening(socketid);
    if(index!=-1){
        return _table[index].Send(socketid,buffer,len);
    }
    else
    {
        DEBUG(dbgSys, "\n Error: Socket is not opening");
        return -1;
    }
}
int Table::Receive(int socketid,char* buffer,int len){
    int index=IsOpening(socketid);
    if(index!=-1){
        return _table[index].Receive(socketid,buffer,len);
    }
    else
    {
        DEBUG(dbgSys, "\n Error: Socket is not opening");
        return -1;
    }
}
int Table::Close_Socket(int socketid){
    int index=IsOpening(socketid);
    if(index!=-1){
        return _table[index].Close_Socket();
    }
    else
    {
        DEBUG(dbgSys, "\n Error: Socket is not opening");
        return -1;
    }
}