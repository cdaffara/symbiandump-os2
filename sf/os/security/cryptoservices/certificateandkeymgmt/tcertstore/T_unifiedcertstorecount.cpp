/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_unifiedcertstorecount.h"
#include "tcertutils.h"
#include "t_certstoredefs.h"
#include "t_certstoreout.h"

CUnifiedCertStoreCount::~CUnifiedCertStoreCount()
	{
	}

CUnifiedCertStoreCount::CUnifiedCertStoreCount(RFs& aFs, CConsoleBase& aConsole,	
											   Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut)
	{
	}

void CUnifiedCertStoreCount::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	}

void CUnifiedCertStoreCount::PerformAction(TRequestStatus& aStatus)
	{
	iFinished = ETrue;
	DoPerformAction();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CUnifiedCertStoreCount::PerformCancel()
	{
	}

void CUnifiedCertStoreCount::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CUnifiedCertStoreCount::Reset()"), 1));
	}

void CUnifiedCertStoreCount::DoReportAction()
	{
//	iOut.writeString(_L("Getting applications..."));
	iOut.writeNewLine();
/*	iOut.writeString(_L("\tExpected applications :"));
	TInt iEnd = iExpectedApplications.Count();
	for (TInt i = 0; i < iEnd; i++)
		{
		iOut.writeString(_L(" "));
		iOut.writeNum(iExpectedApplications[i].iUid);
		}
	iOut.writeNewLine();
	iOut.writeNewLine();*/
	}	

void CUnifiedCertStoreCount::DoCheckResult(TInt /*aError*/)
	{
	DoWriteResult();
	iOut.writeNewLine();
	iOut.writeNum(iCount);
	iConsole.Printf(_L(" %D\n"),iCount);
	iOut.writeNewLine();
	}

