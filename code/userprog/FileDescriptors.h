#include"filesys.h"
#include "kernel.h"
#include "string.h"

#define ReadWriteFile 0
#define ReadFile 1

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
    int Open_File(char* name,int type);
    int Close_File();
    int Read(char* buffer,int size);
    int Write(char* buffer,int size);
    int Seek(int pos);
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
        _mode= type==0 ? ReadWriteFile : ReadFile;
        _id=_file->FileID();
        return _id;
    }
}
int FileDescriptor::Close_File(){
    if(_id==-1)
        return -1;
    else if(_file==NULL)
        return -1;
    else
    {
        Close(_id);
        _mode=-1;
        _id=-1;
        _file->~OpenFile();
        _file=NULL;
        _name=NULL;
        return 1;
    }
}

int FileDescriptor::Read(char* buffer,int size){
    return _file->Read(buffer,size);
}
int FileDescriptor::Write(char* buffer,int size){
    if(_mode==ReadWriteFile)
        return _file->Write(buffer,size);
    else{
        DEBUG(dbgSys, "\n Error: File is read-only");
        return -1;
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
    FileDescriptor* GetFileDescriptor(OpenFileID id);
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
            DEBUG(dbgSys, "\n Error: Can not open file '" << name << "'");
            return -1;
        }
        else
        {
            DEBUG(dbgSys, "\n Open file '" << name << "' successfully");
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
        if (_table[index].Close_File()==-1)
            return -1;
        else
            return 0;
    }
    else
    {
        DEBUG(dbgSys, "\n Error: File is not opening");
        return -1;
    }
}

FileDescriptor* Table::GetFileDescriptor(OpenFileID id){
    int index=IsOpening(id);
    if(index!=-1){
        return &_table[index];
    }
    else
    {
        DEBUG(dbgSys, "\n Error: File is not opening");
        return NULL;
    }
}
