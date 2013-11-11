/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



#include <e32std.h>
#include <e32base.h>
#include <e32cons.h>
#include <utf.h>
#include <errno.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <pdrstore.h>
#include "glibbackend.h"

#define MAX_COMMAND_LINE_LENGTH 256

extern "C"
{
	
EXPORT_C int spawnv(int mode,const char * path,const char **argv)
{
	TText16 iPath16[KMaxFileName];
	char replacePath[KMaxFileName];
	
	/* replace / with // */
	strcpy(replacePath,path);
	char *temp = strchr(replacePath,'/');
	while (temp)
	{
		*temp = '\\';
		temp = strchr(replacePath,'/');
	} // end while
	
	// convert narrow char to wide char
	if(mbstowcs((wchar_t *)iPath16, replacePath, strlen(replacePath)+1) == (size_t)(-1))
	{
			return -1;
	}
	
	TBuf16<KMaxFileName> iCmdName16;
	
	iCmdName16.FillZ(KMaxFileName);
	
	iCmdName16.Copy(iPath16);
	
	TInt i;
	
	TInt iVal = 0;
	
	while(argv[iVal])
	{
		iVal++;
	}	//end while
		
	
	TBuf16<MAX_COMMAND_LINE_LENGTH> iArgv16;
	
	iArgv16.FillZ(MAX_COMMAND_LINE_LENGTH);
	
	iArgv16.SetLength(0);
	
	for(i = 0; i < iVal ; i++)
	{
		TText16 temp[MAX_COMMAND_LINE_LENGTH];
		
		if(mbstowcs((wchar_t *)temp, argv[i], strlen(argv[i])+1) == (size_t)(-1))
		{
				return -1;
		}
		
		TPtrC16 iTemp(temp,strlen(argv[i]));
		
		iArgv16.Append(iTemp);
		
		if(i != iVal-1)
			iArgv16.Append(L' ');
		
	} // end for
	
	RProcess iProcess;
	TRequestStatus iStatus;
	TInt iRetVal = iProcess.Create(iCmdName16,iArgv16);
		
	if(iRetVal != KErrNone)
	{
		switch(iRetVal)
		{
			case KErrNotFound : 	errno = ENOENT;
									break;
			case KErrNoMemory : 	errno = ENOMEM ;
									break;
			case KErrNotSupported : errno = ENOEXEC;
									break;
			case KErrBadName : 		errno = ENAMETOOLONG;
									break;
			default:   	 	 		break;
		}
		
		return -1;
	}
	
	if(mode == P_WAIT)
	{
		iProcess.Logon(iStatus);
		iProcess.Resume();
 		User::WaitForRequest(iStatus);
 		iProcess.Close();
 		return iStatus.Int();
	}
	else
	{
		iProcess.Resume();	
		return iProcess.Id();	
	}
}

EXPORT_C int spawnvp(int mode,const char * path,const char **argv)
{
	return spawnv(mode,path,argv);
}


static int ValidateHandle(HANDLE handle)
{
	RProcess iProcess;
	if( iProcess.Open(handle) < 0)
		return 0;
	
	THandleInfo handleinfo;
	iProcess.HandleInfo(&handleinfo);
	
	if(handleinfo.iNumOpenInProcess ==0 && handleinfo.iNumOpenInThread == 0 && 
	 handleinfo.iNumProcesses == 0 && handleinfo.iNumThreads == 0)
		return 0;
	else
		return 1;
}

EXPORT_C BOOL CloseHandle(HANDLE handle)
{
	RProcess iProcess;
	if( iProcess.Open(handle) < 0)
		return 0;
	
	if(ValidateHandle(handle))
		iProcess.Close();
	
	return 1;
}

EXPORT_C HANDLE GetCurrentProcess()
{
	return RProcess().Id();
}

BOOL DuplicateHandle(HANDLE hSourceProcessHandle,HANDLE hSourceHandle,HANDLE hTargetProcessHandle,LPHANDLE lpTargetHandle,DWORD dwDesiredAccess,BOOL bInheritHandle,DWORD dwOptions)
{
	return 0;
}

EXPORT_C int spawnve(int mode,const char * path,const char **argv,const char **envp)
{
	return spawnv(mode,path,argv);
}

EXPORT_C int spawnvpe(int mode,const char * path,const char **argv,const char **envp)
{
	return spawnv(mode,path,argv);
}


} /* close extern "C"*/
