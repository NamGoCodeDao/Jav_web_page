/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__
//
#include "kernel.h"
#include "filesys.h"
#include "FileDescriptors.h"
Table table;
void SysHalt()
{
  kernel->interrupt->Halt();
}

char *User2System(int virtAddr, int limit)
{
  int i; // index
  int oneChar;
  char *kernelBuf = NULL;

  kernelBuf = new char[limit + 1]; // need for terminal string
  if (kernelBuf == NULL)
    return kernelBuf;

  memset(kernelBuf, 0, limit + 1);

  // printf("\n Filename u2s:");
  for (i = 0; i < limit; i++)
  {
    kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
    kernelBuf[i] = (char)oneChar;
    // printf("%c",kernelBuf[i]);
    if (oneChar == 0)
      break;
  }
  return kernelBuf;
}

int System2User(int virtAddr, int len, char *buffer)
{
  if (len < 0)
    return -1;
  if (len == 0)
    return len;

  int i = 0;
  int oneChar = 0;

  do
  {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);

  return i;
}

void IncreasePC()
{
  // set previous programm counter (debugging only)
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  // set programm counter to next instruction (all Instructions are 4 byte wide)
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(NextPCReg));

  // set next programm counter for brach execution
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

int SysCreate(int virtAddr)
{
  DEBUG(dbgSys, "\n Reading virtual address of filename");
  char *filename;
  // Read virtual address of filename from register 4
  virtAddr = kernel->machine->ReadRegister(4);

  DEBUG(dbgSys, "\n Reading filename.");
  // MaxFileLength is 32
  int result = 0;
  filename = User2System(virtAddr, MaxFileLength + 1);

  if (filename == NULL)
  {
    DEBUG(dbgSys, "\n Filename is not valid");
    result = -1;
  }
  else
  {
    DEBUG(dbgSys, "\n Finish reading filename.");
    DEBUG(dbgSys, "\n File name : '" << filename << "'");

    // Create file with size = 0
    if (!kernel->fileSystem->Create(filename))
    {
      DEBUG(dbgSys, "\n Error create file '" << filename << "'");
      result = -1;
    }
    else
    {
      DEBUG(dbgSys, "\n Create file '" << filename << "' successfully");
    }
  }

  delete[] filename;
  return result;
}

 int SysOpen(int virtAddr, int type)
 {
   OpenFileID FileID;
   if (type != 0 && type != 1)
   {
     DEBUG(dbgSys, "\n Invalid type");
     return -1;
   }

   DEBUG(dbgSys, "\n Reading virtual address of filename");
   char *filename = User2System(virtAddr, MaxFileLength);

   if (filename == NULL || strlen(filename) == 0)
   {
     DEBUG(dbgSys, "\n Filename is not valid");
     return -1;
   }

   FileID=table.Open(filename, type);
   if (FileID == -1)
   {
     DEBUG(dbgSys, "\n Error open file '" << filename << "'");
     return -1;
   }
   else
   {
     DEBUG(dbgSys, "\n Open file '" << filename << "' successfully");
     return FileID;
   }
   delete[] filename;
 }
 int SysClose(OpenFileId  id)
 {
     if (table.Close(id) == -1)
      {
        DEBUG(dbgSys, "\n Error close file '" << id << "'");
        return -1;
      }
      else
      {
          DEBUG(dbgSys, "\n Close file '" << id << "' successfully");
          return 0;
      }
    return 0;
 }
int SysRemove(int virtAddr)
{
  int result=-1;
  char *filename ;
  filename= User2System(virtAddr, MaxFileLength);
  DEBUG(dbgSys, "\n Reading virtual address of filename");
  if (filename == NULL || strlen(filename) == 0)
  {
    DEBUG(dbgSys, "\n Filename is not valid");
    result= -1;
  }
  if (table.IsOpeningWithName(filename)!=-1)
  {
    DEBUG(dbgSys, "\n File is opening");
    result= -1;
  }
  else if(TRUE)
  {
    if (!kernel->fileSystem->Remove(filename))
    {
      DEBUG(dbgSys, "\n Error remove file '" << filename << "'");
      result= -1;
    }
    else
    {
      DEBUG(dbgSys, "\n Remove file '" << filename << "' successfully");
      result= 0;
    }
  }
  delete[] filename;
  return result;
}
#endif /* ! __USERPROG_KSYSCALL_H__ */