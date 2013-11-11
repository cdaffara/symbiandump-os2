/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* tokenserverdebug.h
*
*/

 
#ifdef _DEBUG

#include "tokenserverdebug.h"
#include <f32file.h>

TInt TokenServerDebug::iCounter = 0;
TInt TokenServerDebug::iStartCount = 0;
TInt TokenServerDebug::iPauseCount = 0;
TInt TokenServerDebug::iInitialAllocCount = 0;

_LIT(KHeapErrorFile, "\\fsserver_heap_error");

void TokenServerDebug::StartOOMTest()
	{
	iStartCount = User::CountAllocCells();
	iCounter = 0;
	}

void TokenServerDebug::IncHeapFailPoint()
	{
	__UHEAP_FAILNEXT(iCounter);	
	++iCounter;
	}

void TokenServerDebug::ResetHeapFail()
	{
	__UHEAP_RESET;
	}

void TokenServerDebug::PauseOOMTest()
	{
	ASSERT(iPauseCount == 0); // Don't nest
	if (iCounter)
		{
		__UHEAP_RESET;
		iPauseCount = User::CountAllocCells();
		}
	}

void TokenServerDebug::ResumeOOMTest()
	{
	if (iCounter)
		{
		ASSERT(iPauseCount > 0);
		__UHEAP_FAILNEXT(iCounter - (iPauseCount - iStartCount));
		iPauseCount = 0;
		}
	}

void TokenServerDebug::HeapCheckStart()
	{
	iInitialAllocCount = User::CountAllocCells();
	}

void TokenServerDebug::HeapCheckEnd()
	{
	TInt finalAllocCount = User::CountAllocCells();
	TRAP_IGNORE(HeapErrorL(finalAllocCount != iInitialAllocCount));
	// ignore errors
	}

/**
 * Write or delete a file to signify to the test code that there's a memory leak
 * in the server.  This is unfortunately the best way of doing it since we can't
 * trap a panic when the server's shutting down.
 */
void TokenServerDebug::HeapErrorL(TBool aError)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	
	TDriveUnit sysDrive (fs.GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TFileName heapErrFile (driveName);
	heapErrFile.Append(KHeapErrorFile);
	
	if (aError)
		{
		RFile file;
		TInt err = file.Create(fs, heapErrFile, EFileWrite | EFileShareExclusive);
		if (err != KErrNone || err != KErrAlreadyExists)
			{
			User::Leave(err);
			}
		file.Close();
		}
	else
		{
		TInt err = fs.Delete(heapErrFile);
		if (err != KErrNone && err != KErrNotFound)
			{
			User::Leave(err);
			}
		}
	}

#endif
