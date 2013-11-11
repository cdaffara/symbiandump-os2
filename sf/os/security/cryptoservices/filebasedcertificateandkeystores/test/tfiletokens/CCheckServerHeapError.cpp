/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "t_filetokens.h"
#include "t_input.h"
#include "t_output.h"
#include <e32base.h>

/////////////////////////////////////////////////////////////////////////////////
// CCheckServerHeapError
/////////////////////////////////////////////////////////////////////////////////

_LIT(KHeapErrorFile, "\\fsserver_heap_error");

EXPORT_C CTestAction* CCheckServerHeapError::NewL(RFs& aFs, 
												  CConsoleBase& aConsole, 
												  Output& aOut,
												  const TTestActionSpec& aTestActionSpec)
	{
	CCheckServerHeapError* self = new (ELeave) CCheckServerHeapError(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CCheckServerHeapError::CCheckServerHeapError(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CCheckServerHeapError::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());	
	TBuf<128> heapErrFile (driveName);
	heapErrFile.Append(KHeapErrorFile);
	TInt err = iFs.Delete(heapErrFile);
	if (err != KErrNone && err != KErrNotFound)
		{
		User::Leave(err);
		}
	}

CCheckServerHeapError::~CCheckServerHeapError()
	{
	}

void CCheckServerHeapError::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CCheckServerHeapError::PerformAction(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	RFile file;
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<128> heapErrFile (driveName);
	heapErrFile.Append(KHeapErrorFile);
	
	TInt err = file.Open(iFs, heapErrFile, EFileRead | EFileShareExclusive);

	if (err != KErrNone && err != KErrNotFound)
		{
		iResult = EFalse;
		iFinished = ETrue;			
		User::RequestComplete(status, err);
		return;
		}

	file.Close();
	
	iResult = (err == KErrNotFound);
	iFinished = ETrue;			
	User::RequestComplete(status, KErrNone);
	}

void CCheckServerHeapError::DoPerformPostrequisite(TRequestStatus& aStatus)
	{	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CCheckServerHeapError::PerformCancel()
	{
    // nothing to cancel
	}

void CCheckServerHeapError::Reset()
	{
	}

void CCheckServerHeapError::DoReportAction()
	{
	iOut.writeString(_L("Checking for server heap error..."));
	iOut.writeNewLine();
	}

void CCheckServerHeapError::DoCheckResult(TInt /*aError*/)
	{
	if (iFinished)
		{
		if (iResult)
			{
			_LIT(KSuccessful, "No heap error\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			_LIT(KFailed, "!!!Server heap error detected!!!\n");
			iConsole.Write(KFailed);
			iOut.writeString(KFailed);
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}
