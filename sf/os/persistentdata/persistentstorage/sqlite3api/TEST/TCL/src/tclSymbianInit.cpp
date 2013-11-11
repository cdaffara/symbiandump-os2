// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Provides a Symbian version of the main program and Tcl_AppInit
// procedure for Tcl applications (without Tk).
// 
//

#include "tcl.h"
#include "tclPort.h"
#include "tclInt.h"
#include "tclIntPlatDecls.h"
#include <e32test.h>

#ifdef __WINSCW__ 
#include <e32std.h>     //RPointerArray

#include <pls.h> // For emulator WSD API 
const TUid KTCLDLLUid3 = {0}; // Must change
const TInt KMaxDataKey = 10;
#endif // __WINSCW__

/*
 * The following macros convert between TclFile's and fd's.  The conversion
 * simple involves shifting fd's up by one to ensure that no valid fd is ever
 * the same as NULL.  Note that this code is duplicated from tclUnixPipe.c
 */

#define MakeFile(fd) ((TclFile)((fd)+1))
#define GetFd(file) (((int)file)-1)

#ifdef __WINSCW__ 
//The following code will run only on the emulator

//Put the global count into a structure
struct DLLData
{
	// TCL globals
	char* tclExecutableName;
	char* tclNativeExecutableName;

	void* dataKey[KMaxDataKey];
	int inFinalize;
	int subsystemsInitialized;
	void* allocHead;	
	void* defaultEncoding;
	void* systemEncoding;
	Tcl_HashTable encodingTable;	
	SyncObjRecord keyRecord;
	Tcl_HashTable typeTable;
	int typeTableInitialized;
	int encodingsInitialized;	
	char* tclDefaultEncodingDir;
	char* tclLibraryPathStr;	
	int opTableInitialized;
	Tcl_HashTable opHashTable;
	Tcl_HashTable auxDataTypeTable;
	int auxDataTypeTableInitialized;
	void* cwdPathPtr;
	int cwdPathEpoch;
	void* refArray;
	int spaceAvl;
	int inUse;
	TclPlatformType tclPlatform;	
	void* firstNotifierPtr;
	
	// Symbian globals
	char fileNames[8][L_tmpnam + 9];
};

//Initialization function
TInt InitializeGlobals(DLLData* aData)
{
   memset(aData, 0, sizeof(DLLData));
   aData->tclPlatform = TCL_PLATFORM_UNIX;
   return KErrNone;
}

//Define a way to access the structure
//On the first call to this function, memory will be allocated with the specified
//Uid as an identifier and the Initialization function will be called
//Subsequent calls to this function return the allocated memory
struct DLLData* GetGlobals()
{
   return Pls<DLLData>(KTCLDLLUid3, InitializeGlobals);
}

//Clean up memory allocated for PLS used for storing globals
int CleanupGlobals(void)
{
	return FreePls(GetGlobals());
}

void* get_gFileName(int index)
{
   return &(GetGlobals()->fileNames[index]);
}

char** get_tclExecutableName()
{
   return &(GetGlobals()->tclExecutableName);
}

char** get_tclNativeExecutableName()
{
   return &(GetGlobals()->tclNativeExecutableName);
}

void** get_dataKey(int index)
{
   return &(GetGlobals()->dataKey[index]);
}

void* get_inFinalize()
{
   return &(GetGlobals()->inFinalize);
}

void* get_subsystemsInitialized()
{
   return &(GetGlobals()->subsystemsInitialized);
}

void** get_allocHead()
{
   return &(GetGlobals()->allocHead);
}

void** get_defaultEncoding()
{
   return &(GetGlobals()->defaultEncoding);
}

void** get_systemEncoding()
{
   return &(GetGlobals()->systemEncoding);
}

void* get_encodingTable()
{
   return &(GetGlobals()->encodingTable);
}

void* get_keyRecord()
{
   return &(GetGlobals()->keyRecord);
}

void* get_typeTable()
{
   return &(GetGlobals()->typeTable);
}

void* get_typeTableInitialized()
{
   return &(GetGlobals()->typeTableInitialized);
}

void* get_encodingsInitialized()
{
   return &(GetGlobals()->encodingsInitialized);
}

char** get_tclDefaultEncodingDir()
{
   return &(GetGlobals()->tclDefaultEncodingDir);
}

char** get_tclLibraryPathStr()
{
   return &(GetGlobals()->tclLibraryPathStr);
}

void* get_opTableInitialized()
{
   return &(GetGlobals()->opTableInitialized);
}

void* get_opHashTable()
{
   return &(GetGlobals()->opHashTable);
}

void* get_auxDataTypeTableInitialized()
{
   return &(GetGlobals()->auxDataTypeTableInitialized);
}

void* get_auxDataTypeTable()
{
   return &(GetGlobals()->auxDataTypeTable);
}

void** get_cwdPathPtr()
{
   return &(GetGlobals()->cwdPathPtr);
}

void* get_cwdPathEpoch()
{
   return &(GetGlobals()->cwdPathEpoch);
}

void** get_refArray()
{
   return &(GetGlobals()->refArray);
}

void* get_spaceAvl()
{
   return &(GetGlobals()->spaceAvl);
}

void* get_inUse()
{
   return &(GetGlobals()->inUse);
}

/*
 *----------------------------------------------------------------------
 *
 * TclPlatformExit --
 *
 *	This procedure implements the Symbian specific exit routine.
 *  Modelled after Macintosh version. 
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	We exit the process.
 *
 *----------------------------------------------------------------------
 */

void
TclpExit(
    int status)		/* Ignored. */
{
    // Free the PLS
    CleanupGlobals();

    exit(status);
}

void* get_tclPlatform()
{
   return &(GetGlobals()->tclPlatform);
}

void** get_firstNotifierPtr()
{
   return &(GetGlobals()->firstNotifierPtr);
}

#else
//Target device code
char tmpFileName[L_tmpnam + 9];
char fifoFileName[L_tmpnam + 9];
char inFileName[L_tmpnam + 9];
char outFileName[L_tmpnam + 9];
char errFileName[L_tmpnam + 9];
char inFileName1[L_tmpnam + 9];
char outFileName1[L_tmpnam + 9];
char errFileName1[L_tmpnam + 9];	

#endif

#include "tclSymbianGlobals.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADDPARAMTOCHILD 4

EXPORT_C void ChildProcessCleanup(int isChildProcess, int argc, char **argv)
{
	RDebug::Print(_L("###TclSqlite3: Child process cleanup - begin. argc = %d.\r\n"), argc);
	TBuf<256> buf;
	for(TInt i=0;i<argc;++i)
		{
		TPtrC8 p((const unsigned char*)(argv[i]));
		buf.Copy(p);
	    RDebug::Print(_L("   ### arg %d, value \"%S\"\r\n"), i, &buf);
		}
		
    // add fifo close & unlink
    if (isChildProcess == 1)
    	{
        RDebug::Print(_L("  ### Unlink 0.\r\n"));
    	
    	TPtrC8 p1((const unsigned char*)tmpFileName);
    	buf.Copy(p1);
        RDebug::Print(_L("   ### tmp file name \"%S\"\r\n"), &buf);
    	
    	TPtrC8 p2((const unsigned char*)fifoFileName);
    	buf.Copy(p2);
        RDebug::Print(_L("   ### fifo file name \"%S\"\r\n"), &buf);
        
    	TPtrC8 p3((const unsigned char*)inFileName);
    	buf.Copy(p3);
        RDebug::Print(_L("   ### input file name \"%S\"\r\n"), &buf);
    	
    	TPtrC8 p4((const unsigned char*)outFileName);
    	buf.Copy(p4);
        RDebug::Print(_L("   ### output file name \"%S\"\r\n"), &buf);
    	
    	TPtrC8 p5((const unsigned char*)errFileName);
    	buf.Copy(p5);
        RDebug::Print(_L("   ### err file name \"%S\"\r\n"), &buf);
    	
    	RDebug::Print(_L("   ### Close stdin, stdout and stderr.\r\n"));
    	close (TCL_STDIN);
    	close (TCL_STDOUT);
    	close (TCL_STDERR);
		for(TInt i=0, idx=argc-i-1; i<ADDPARAMTOCHILD && idx >= 0; ++i, --idx)
			{
    		if(argv[idx])
    			{
        		TPtrC8 p((const unsigned char*)(argv[idx]));
        		buf.Copy(p);
    	    	RDebug::Print(_L("   ### Unlink. Arg %d. Value \"%S\".\r\n"), idx, &buf);
    	    	unlink(argv[idx]);
    			}
			}
    	}
    else
    	{
        RDebug::Print(_L("  ### Unlink 1.\r\n"));
    	
    	TPtrC8 p1((const unsigned char*)inFileName1);
    	buf.Copy(p1);
        RDebug::Print(_L("   ### 1 input file name \"%S\"\r\n"), &buf);
    	
    	TPtrC8 p2((const unsigned char*)outFileName1);
    	buf.Copy(p2);
        RDebug::Print(_L("   ### 1 output file name \"%S\"\r\n"), &buf);

    	TPtrC8 p3((const unsigned char*)errFileName1);
    	buf.Copy(p3);
        RDebug::Print(_L("   ### 1 err file name \"%S\"\r\n"), &buf);

    	unlink(inFileName1);
    	unlink(outFileName1);
    	unlink(errFileName1);
    	}
	RDebug::Print(_L("###TclSqlite3: Child process cleanup - end.\r\n"));
	}

// Symbian main hook for tclappinit
EXPORT_C int ChildProcessInit (int *argc, char ***argv)
{
    //set the stdin,stdout,stderr to the child process. the fds pass to the posix_spawn() in argv
    TclFile inputFile = NULL;
    TclFile outputFile= NULL;
    TclFile errorFile = NULL;
    int joinThisError;
    int fd[4] = {0, 0, 0, 0};
    char errSpace[200 + TCL_INTEGER_SPACE];
    int anerr = 0;
	TBuf<256> buf;

    RDebug::Print(_L("###TclSqlite3: Child process init - begin. argc = %d.\r\n"), argc != NULL ? *argc : 0);
    if(argc)
    	{
    	for(TInt i=0;i<*argc;++i)
    		{
    		TPtrC8 p((const unsigned char*)((*argv)[i]));
    		buf.Copy(p);
    	    RDebug::Print(_L("   ### arg %d, value \"%S\"\r\n"), i, &buf);
    		}
    	}
   //set the stdin,stdout,stderr and pipeid to the child process. the fds pass to the posix_spawn() in argv
	if (*argc >= 5)
		{
		// fifoFile
		RDebug::Print(_L("  ### Fifo file. Arg %d.\r\n"), *argc-4);
		if((*argv)[*argc-4])
			{
			fd[0] = open((*argv)[*argc-4],O_WRONLY);
			if (fd[0] == -1)
				{
				RDebug::Print(_L("   ### fd[0](fifoFile) errno is %d\r\n"), errno);
				}
			else
				{
	    		TPtrC8 p((const unsigned char*)((*argv)[*argc-4]));
	    		buf.Copy(p);
				RDebug::Print(_L("   ### fifoFile is \"%S\", fd[0] is %d\r\n"), &buf, fd[0]);				
				}
		    //fd = atoi((*argv)[*argc-1]);
			}
		else
			{
			RDebug::Print(_L("   ### Fifo file - (*argv)[*argc-4] is 0.\r\n"));
			//should add later
			}
		// inputFile
		RDebug::Print(_L("  ### Input file. Arg %d.\r\n"), *argc-3);
		if(((*argv)[*argc-3])&&(strcmp((*argv)[*argc-3],"STD")))
			{
			fd[3] = open((*argv)[*argc-3],O_RDONLY); 
			inputFile = MakeFile(fd[3]);
			if (fd[3] == -1)
				{
				RDebug::Print(_L("   ### fd[3](inputFile) errno is %d\r\n"), errno);
				}
			else
				{
	    		TPtrC8 p((const unsigned char*)((*argv)[*argc-3]));
	    		buf.Copy(p);
				RDebug::Print(_L("   ### inputFile is \"%S\", fd[3] is %d\r\n"), &buf, fd[3]);					
				}
			    //inputFile = (TclFile) (atoi((*argv)[*argc-4]));
			}
		else
			{
			RDebug::Print(_L("   ### Input file - ((*argv)[*argc-3])&&(strcmp((*argv)[*argc-3],\"STD\")) is 0.\r\n"));
			//should add later
			}
		// outputFile
		RDebug::Print(_L("  ### Output file. Arg %d\r\n"), *argc-2);
		if(((*argv)[*argc-2])&&(strcmp((*argv)[*argc-2],"STD")))
			{
			fd[2] = open((*argv)[*argc-2],O_WRONLY);
			outputFile = MakeFile(fd[2]);
		    if (fd[2] == -1)
		    	{
		    	RDebug::Print(_L("   ### fd[2](outputFile) errno is %d\r\n"), errno);
		    	}
		    else
				{
	    		TPtrC8 p((const unsigned char*)((*argv)[*argc-2]));
	    		buf.Copy(p);
				RDebug::Print(_L("   ### outputFile is \"%S\", fd[2] is %d\r\n"), &buf, fd[2]);					
				}
		    
			//outputFile = (TclFile) (atoi((*argv)[*argc-3]));
			}
		else
			{
			RDebug::Print(_L("   ### Output file - ((*argv)[*argc-2])&&(strcmp((*argv)[*argc-2],\"STD\")) is 0.\r\n"));
			//should add later
			//outputFile = MakeFile(1);
			}
		// errorFile
		RDebug::Print(_L("  ### Error file. Arg %d\r\n"), *argc-1);
		if(((*argv)[*argc-1])&&(strcmp((*argv)[*argc-1],"STD")))
			{
			fd[1] = open((*argv)[*argc-1],O_WRONLY);
			errorFile = MakeFile(fd[1]);
			if (fd[1] == -1)
				{
				RDebug::Print(_L("   ### fd[1] errorFile errno is %d\r\n"), errno);
				}
			else
				{
	    		TPtrC8 p((const unsigned char*)((*argv)[*argc-1]));
	    		buf.Copy(p);
				RDebug::Print(_L("   ### errorFile is \"%S\", fd[1] is %d\r\n"), &buf, fd[1]);
				}
		    //errorFile = (TclFile) (atoi((*argv)[*argc-2]));
			}
		else
			{
			RDebug::Print(_L("   ### Output file - ((*argv)[*argc-1])&&(strcmp((*argv)[*argc-1],\"STD\")) is 0.\r\n"));
			//should add later
			}
		//*argc = *argc-4;
		
		joinThisError = errorFile && (errorFile == outputFile);

		//fd = GetFd(errPipeOut);
    
		//
		// Set up stdio file handles for the child process.
		//

		if (!SetupStdFile(inputFile, TCL_STDIN)
			|| !SetupStdFile(outputFile, TCL_STDOUT)
			|| (!joinThisError && !SetupStdFile(errorFile, TCL_STDERR))
			|| (joinThisError &&
				((dup2(1,2) == -1) ||
				 (fcntl(2, F_SETFD, 0) != 0)))) 
			//if (!SetupStdFile(errorFile, TCL_STDERR))
			{
			RDebug::Print(_L("   ### child process couldn't set up input/output, error: %d\r\n"), errno);
			sprintf(errSpace,"child process couldn't set up input/output, error: %d\r\n", errno);
			write(fd[0], errSpace, (size_t) strlen(errSpace));
			close(fd[0]);
			unlink((*argv)[*argc-4]);
		    RDebug::Print(_L("###TclSqlite3: Child process init - end 1.\r\n"));
			_exit(1);
			}

		sprintf(errSpace,"OK\r\n");
		write(fd[0], errSpace, (size_t) strlen(errSpace));
		anerr = close(fd[0]);
		anerr = unlink((*argv)[*argc-4]);
	    RDebug::Print(_L("###TclSqlite3: Child process init - end 2. anerr=%d.\r\n"), anerr);
	   	return 1;
		}
    
    RDebug::Print(_L("###TclSqlite3: Child process init - end 3.\r\n"));
    return 0;			
}

void TclPrint1(const char* aFmt, const char* aStr)
	{
	TBuf<128> fmt;
	fmt.Copy(TPtrC8((const TUint8*)aFmt));
	TBuf<128> str;
	str.Copy(TPtrC8((const TUint8*)aStr));
	RDebug::Print(fmt, &str);	
	}

void TclPrint2(const char* aFmt, const char* aStr, int aNum)
	{
	TBuf<128> fmt;
	fmt.Copy(TPtrC8((const TUint8*)aFmt));
	TBuf<128> str;
	str.Copy(TPtrC8((const TUint8*)aStr));
	RDebug::Print(fmt, &str, aNum);	
	}

void TclPrint3(const char* aFmt)
	{
	TBuf<128> fmt;
	fmt.Copy(TPtrC8((const TUint8*)aFmt));
	RDebug::Print(fmt);	
	}

#ifdef __cplusplus
}
#endif
