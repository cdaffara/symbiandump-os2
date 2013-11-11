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


/**
 @file
*/
  
#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"

const int KErrDifferent = -99999;

/////////////////////////////////////////////////////////////////////////////////
// CCompare
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CCompare::NewL(RFs& aFs, 
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CCompare::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

CTestAction* CCompare::NewLC(RFs& aFs,
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CCompare* self = new (ELeave) CCompare(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CCompare::~CCompare()
{
}

CCompare::CCompare(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CKeyStoreTestAction(aFs, aConsole, aOut)
{
 	iState = EComparing;
}


void CCompare::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;

	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> ramDrive (sysDrive.Name());
	ramDrive.Append(_L("\\tkeystore\\data\\"));
	
	iOriginalFile = ramDrive;
	iNewFile = ramDrive;
	
	TFileName buf;
	buf.FillZ();

	buf.Copy(Input::ParseElement(aTestActionSpec.iActionBody, 
							KOriginalFile, 
							KOriginalFileEnd, pos, err));	
//	Now the filename itself
	iOriginalFile.Append(buf);

	buf.Copy(Input::ParseElement(aTestActionSpec.iActionBody, 
							KNewFile, 
							KNewFileEnd, pos, err));
//	Now the filename itself
	iNewFile.Append(buf);

//	iExpectedResult = SetExpectedResultL(Input::ParseElement(aTestActionSpec.iActionResult, 
//															KReturnStart, 
//															KReturnEnd, pos, err));
}


void CCompare::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
	case EComparing:
			{
			TInt res = KErrNone;
			TRAPD(err, res  = CompareFilesL());
			if (KErrNone != err)
				{
				res = err;
				}
			iState = EFinished;
			TRequestStatus *status = &aStatus;
			User::RequestComplete(status, res);			
			}
			break;
	case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			iFinished = ETrue;
			}
			break;
		}
	}


void CCompare::PerformCancel()
    {
    // nothing to cancel
    }
 

void CCompare::Reset()
{}


void CCompare::DoReportAction()
{
	_LIT(KComparing, "Comparison in progress...");
	iOut.writeString(KComparing);
	TPtr theLabel(iLabel->Des());
	iOut.writeString(theLabel);
	iOut.writeNewLine();
}

// Bitwise comparison of iOriginalFile and iNewFile
TInt CCompare::CompareFilesL()
	{
		TInt res = KErrNone;

		RFs theFs;
		CleanupClosePushL(theFs);
		User::LeaveIfError(theFs.Connect());
			
		RFile original;
		TInt r = original.Open(theFs, iOriginalFile, EFileRead);
		User::LeaveIfError(r);
		CleanupClosePushL(original);

		RFile generated;
		r = generated.Open(theFs, iNewFile, EFileRead);
		User::LeaveIfError(r);
		CleanupClosePushL(generated);
		
		TInt pos1 = 0;
		TInt pos2 = 0;
		
		TBuf8<1> ptr1;
		TBuf8<1> ptr2;

		original.Read(pos1, ptr1);
		generated.Read(pos2, ptr2);
		
		while (ptr1.Length()!=0)
			{
			if (ptr1.Compare(ptr2) != 0)
				{
				res = KErrDifferent;				
				pos = pos1;
				break;
				}
			pos1++;
			pos2++;
			original.Read(pos1, ptr1);
			generated.Read(pos2, ptr2);				
			}
		if ((res != KErrDifferent)&& (ptr2.Length()!=0))
			{
			 res = KErrDifferent;			
			pos = pos1;
			}
			
		CleanupStack::PopAndDestroy(3, &theFs);
		
		return res;
	}

void CCompare::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		if (aError == KErrNone)
		{
			_LIT(KSuccessful, "Key comparison successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
		else
		{
			TBuf<256> msg;
			_LIT(KFailedDetails, "First difference at offset %d  \n");			
			msg.Format(KFailedDetails, pos);
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Key comparison failure!!!  Import or export problem...\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				iOut.writeNewLine();				
				iConsole.Write(msg);
				iOut.writeString(msg);
			}
			else
			{
				_LIT(KFailed, "Key comparison failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
				iOut.writeNewLine();				
				iConsole.Write(msg);
				iOut.writeString(msg);
			}

			iOut.writeNewLine();
			iOut.writeNewLine();
		}
	}
}
