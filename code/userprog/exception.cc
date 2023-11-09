// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

#define MaxFileLength 32 // Do dai quy uoc cho file name

void IncreasePC()
{
	int counter = machine->ReadRegister(PCReg);
   	machine->WriteRegister(PrevPCReg, counter);
    	counter = machine->ReadRegister(NextPCReg);
    	machine->WriteRegister(PCReg, counter);
   	machine->WriteRegister(NextPCReg, counter + 4);
}



void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;
		case SC_Connect:
		{ //INPUT: int socketid(index), char *ip, int port
			int socketid = kernel->machine->ReadRegister(4); // Lay socketid tu thanh ghi so 5
			int virtAddr = kernel->machine->ReadRegister(5);  // Lay dia chi cua tham so ip tu thanh ghi so 4
			int port = kernel->machine->ReadRegister(6);        // Lay port tu thanh ghi so 6
			char* ip;
			if (socketid < 2 || socketid >=20 )
			{
				printf("\nKhong the connect vi id nam ngoai bang mo ta file/socket.");
				kernel->machine->WriteRegister(2, -1); // -1: Loi
				IncreasePC();
				return;
				break;
			}
			if (kernel->fileSystem->openfile[socketid] == NULL)
			{
				printf("\nKhong the connect vi socket nay khong ton tai.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				break;
			}
			ip = User2System(virtAddr, MAX_LENGTH_IP); 
			if(kernel->fileSystem->openfile[socketid]->Connect(ip, port)<0){
				cout << ip;
				kernel->machine->WriteRegister(2, -1);
			} else {
				kernel->machine->WriteRegister(2, 0);
			}
			delete [] ip ;
			IncreasePC();
			return;
			break;
		}
		case SC_Send:
		{
			int virtAddr = kernel->machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = kernel->machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = kernel->machine->ReadRegister(6);        // Lay id cua file tu thanh ghi so 6
			int tempt;
			char *buf;
			if (id < 0 || id >= 20)
			{
				printf("\nKhong the send vi id nam ngoai bang mo ta file.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			if (kernel->fileSystem->openfile[id] == NULL)
			{
				printf("\nKhong the send vi socket nay khong ton tai.");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			buf = User2System(virtAddr, charcount);// Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
			if (kernel->fileSystem->openfile[id]->type == 2 && kernel->fileSystem->openfile[id]->isConnect == true) {
				tempt = kernel->fileSystem->openfile[id]->Write(buf, charcount);
				if (tempt > 0) //Nếu số bytes viết ra lớn hơn 0
				{
				// So byte thuc su = NewPos - OldPos
					kernel->machine->WriteRegister(2, tempt); // Viết bytes vào thanh ghi 2
				} else {
					kernel->machine->WriteRegister(2, -1);
				}
			} else {
				kernel->machine->WriteRegister(2, -1);
			}
			delete buf;
			IncreasePC();
			return;

		}
		
			case SC_Seek:
		{
			// Input: Vi tri(int), id cua file(OpenFileID)
			// Output: -1: Loi, Vi tri thuc su: Thanh cong
			// Cong dung: Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
			int pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
			int id = machine->ReadRegister(5); // Lay id cua file
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 14)
			{
				printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nKhong the seek vi file nay khong ton tai.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra co goi Seek tren console khong
			if (id == 0 || id == 1)
			{
				printf("\nKhong the seek tren file console.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
			pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;
			if (pos > fileSystem->openf[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
			{
				printf("\nKhong the seek file den vi tri nay.");
				machine->WriteRegister(2, -1);
			}
			else
			{
				// Neu hop le thi tra ve vi tri di chuyen thuc su trong file
				fileSystem->openf[id]->Seek(pos);
				machine->WriteRegister(2, pos);
			}
			IncreasePC();
			return;
		}
		case SC_CreateFile:
		{
			int virtAddr;
			char* filename;
			
			virtAddr = machine->ReadRegister(4); 
			filename = User2System(virtAddr, MaxFileLength + 1);

			if (strlen(filename) == 0)
			{
				printf("\nFile name is not valid");
				machine->WriteRegister(2, -1); 
				delete[] filename;
				break;
			}

			if (filename == NULL)  
			{
				printf("\n Not enough memory in system");
				machine->WriteRegister(2, -1); 
				delete[] filename;
				break;
			}

			if (!fileSystem->Create(filename, 0)) 
			{
				printf("\nError create file '%s'", filename);
				machine->WriteRegister(2, -1);
				delete[] filename;
				break;
			}

			printf("\nCreate file '%s' success", filename);
			machine->WriteRegister(2, 0);
			delete[] filename;
			break;
		}
		

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			ASSERTNOTREACHED();

			break;

		
		case SC_Seek:
		{
			// Input: Vi tri(int), id cua file(OpenFileID)
			// Output: -1: Loi, Vi tri thuc su: Thanh cong
			// Cong dung: Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
			int pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
			int id = machine->ReadRegister(5); // Lay id cua file
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 14)
			{
				printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("\nKhong the seek vi file nay khong ton tai.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra co goi Seek tren console khong
			if (id == 0 || id == 1)
			{
				printf("\nKhong the seek tren file console.");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
			pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;
			if (pos > fileSystem->openf[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
			{
				printf("\nKhong the seek file den vi tri nay.");
				machine->WriteRegister(2, -1);
			}
			else
			{
				// Neu hop le thi tra ve vi tri di chuyen thuc su trong file
				fileSystem->openf[id]->Seek(pos);
				machine->WriteRegister(2, pos);
			}
			IncreasePC();
			return;
		}

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
		
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
