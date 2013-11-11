/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_ccttokentypeinfoattributes.h"
#include "t_certstoreout.h"
#include "t_certstoredefs.h"
#include "t_input.h"
#include <mctcertstore.h>

CTestAction* CCTTokenTypeInfoAttributes::NewL(RFs& aFs, 
															 CConsoleBase& aConsole,
															 Output& aOut,
															 const TTestActionSpec& aTestActionSpec)
	{
	CCTTokenTypeInfoAttributes* self = new(ELeave) CCTTokenTypeInfoAttributes(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}


CCTTokenTypeInfoAttributes::~CCTTokenTypeInfoAttributes()
	{
	iTokenTypes.Close();
	}

CCTTokenTypeInfoAttributes::CCTTokenTypeInfoAttributes(RFs& aFs, CConsoleBase& aConsole,
													   Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut), iState(EFinished)
	{
	}

void CCTTokenTypeInfoAttributes::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
		
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	
	}

void CCTTokenTypeInfoAttributes::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EFinished:
			{
			RArray<TUid> uidArray;
			CleanupClosePushL(uidArray);
			TUid uid1 = { KInterfaceCertStore };
						
			uidArray.AppendL(uid1);
			TCTFindTokenTypesByInterface filter(uidArray.Array());
			CCTTokenTypeInfo::ListL(iTokenTypes, filter);

			CleanupStack::PopAndDestroy();	// uidArray
			
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
			if (aStatus == KErrNoMemory)
				{	
			//	iState = EDelete;
				}
			else
				{
				iFinished = ETrue;
				}
			}
			break;
		}
	}

void CCTTokenTypeInfoAttributes::PerformCancel()
	{
	}

void CCTTokenTypeInfoAttributes::Reset()
	{
	iTokenTypes.Close();
	}

void CCTTokenTypeInfoAttributes::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iExpectedResult = KErrNone;
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CCTTokenTypeInfoAttributes::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CCTTokenTypeInfoAttributes::DoReportAction()
	{
	iOut.writeString(_L("Listing Unified Cert Store..."));
	iOut.writeNewLine();
	}

void CCTTokenTypeInfoAttributes::DoCheckResult(TInt /*aError*/)
	{
	TInt i = 0;
	TInt end = iTokenTypes.Count();
	while (i < end)
		{
		iOut.writeString(iTokenTypes[i]->Label());
		iOut.writeNewLine();
		const RArray<TCTTokenTypeAttribute>& attributes = iTokenTypes[i]->Attributes();
		int j = 0;
		int jEnd = attributes.Count();
		while (j < jEnd)
			{
			iOut.writeString(_L("UID:"));
			iOut.writeHex(attributes[j].iUID.iUid);
			iOut.writeString(_L(" "));
			iOut.writeString(_L("Attribute:"));
			iOut.writeNum(attributes[j].iVal);
			iOut.writeNewLine();
			j++;
			}
		i++;
		}
	}

