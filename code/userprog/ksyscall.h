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

#include "kernel.h"
#include "filesys.h"

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
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

void ModifyReturnPC()
{
  // set previous programm counter (debugging only)
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  // set programm counter to next instruction (all Instructions are 4 byte wide)
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(NextPCReg));

  // set next programm counter for brach execution
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

int ReadRegister(int num)
{
  return kernel->machine->ReadRegister(num);
}

void WriteRegister(int num, int value)
{
  kernel->machine->WriteRegister(num, value);
}

bool fileSystemCreate(char *filename)
{
  return kernel->fileSystem->Create(filename);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
