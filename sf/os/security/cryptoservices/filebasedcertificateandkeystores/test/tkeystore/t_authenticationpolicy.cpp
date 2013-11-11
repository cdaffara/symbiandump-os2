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


#include <unifiedkeystore.h>
#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"
#include "t_output.h"

const TInt KKeyStoreEmpty = -1199;
_LIT(KEllipsis, "...");

/////////////////////////////////////////////////////////////////////////////////
// CAuthenticationPolicy
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CAuthenticationPolicy::NewL(RFs& aFs, 
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CAuthenticationPolicy::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CAuthenticationPolicy::NewLC(RFs& aFs,
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
	{
	CAuthenticationPolicy* self = new (ELeave) CAuthenticationPolicy(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CAuthenticationPolicy::~CAuthenticationPolicy()
	{
	}

CAuthenticationPolicy::CAuthenticationPolicy(RFs& aFs, CConsoleBase& aConsole, Output& aOut) :
	CKeyStoreTestAction(aFs, aConsole, aOut), iState(EInit)
	{
	}

void CAuthenticationPolicy::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
          
	iMode.Copy(Input::ParseElement(aTestActionSpec.iActionBody, KAuthModeStart, KAuthModeEnd));
	
	if(iMode.Compare(_L8("get")) == 0)
		{
		iExpectedExpression.Copy(Input::ParseElement(aTestActionSpec.iActionBody, KExpectedExpressionStart, KExpectedExpressionEnd));
		iExpectedFreshness = Input::ParseIntElement(aTestActionSpec.iActionBody, KExpectedFreshnessStart, KExpectedFreshnessEnd);
		}
	
	}

void CAuthenticationPolicy::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EInit:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
		
			iFilter.iUsage = EPKCS15UsageAll;
			keyStore->List(iKeys, iFilter, aStatus);
							
			iState = EListing;
					
			}
			break;

		case EListing:
				{
				switch(aStatus.Int())
					{				
					case KErrNone:
						{
						TCTTokenObjectHandle keyHandle;	
						if (iKeys.Count() == 0)
							{
							iConsole.Printf(_L("keyStore empty!!"));
							iState = EFinished;
							TRequestStatus *status = &aStatus;
							User::RequestComplete(status, KKeyStoreEmpty);			
							break;
							}
					
						// Select the key with the label we want!
						for (TInt j = 0; j < iKeys.Count(); j++)
							{
							if (iKeys[j]->Label() == *iLabel) 
								{
								keyHandle = iKeys[j]->Handle();
								break;
								}
							}
						if(keyHandle.iObjectId == 0)
							{
							iConsole.Printf(_L("No keys with matching label!!"));
							iState = EFinished;
							TRequestStatus *status = &aStatus;
							User::RequestComplete(status, KErrNotFound);			
							break;
							}
						
						CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
						if(iMode.Compare(_L8("set")) == 0)
							{
							keyStore->SetAuthenticationPolicy(keyHandle, *iAuthExpression, iFreshness, aStatus);
							iState = EFinished;
							}
						else if(iMode.Compare(_L8("get")) == 0)
							{
							if(iAuthExpression)
								{
								delete iAuthExpression;
								iAuthExpression = NULL;
								}
							iAuthExpression = HBufC::NewL(1024);
							keyStore->GetAuthenticationPolicy(keyHandle, iAuthExpression, iFreshness, aStatus);
							iState = EGetAuthenticationPolicy;
							}
						else
							{
							iConsole.Printf(_L("Invalid Mode"));
							iState = EFinished;
							TRequestStatus *status = &aStatus;
							User::RequestComplete(status, aStatus.Int());
							}
						}
						break;
					default:
						{
						iConsole.Printf(_L("keyStore->ExportKey returned: %d\n"), aStatus.Int());
						iState = EFinished;
						TRequestStatus *status = &aStatus;
						User::RequestComplete(status, aStatus.Int());
						}
						break;
					} // switch
				break;
				}
		case EGetAuthenticationPolicy:
			switch(aStatus.Int())
				{
				case KErrNone:
					{
					TInt err = KErrNone;
					HBufC* expectedExpr = HBufC::NewL(iExpectedExpression.Length());
					expectedExpr->Des().Copy(iExpectedExpression);
					if(iAuthExpression->Des().Compare(*expectedExpr) != 0 ||
							iFreshness != iExpectedFreshness)
						{
						err = KErrArgument;
						}
					iConsole.Printf(_L("Authentication policy returned: %S, %d\n"), iAuthExpression, iFreshness);
					delete expectedExpr;
					iState = EFinished;
					delete iAuthExpression;
					iAuthExpression = NULL;
					TRequestStatus *status = &aStatus;
					User::RequestComplete(status, err);
					break;
					}
				default:
					{
					iConsole.Printf(_L("Setting Authentication policy returned: %d\n"), aStatus.Int());
					iState = EFinished;
					TRequestStatus *status = &aStatus;
					User::RequestComplete(status, aStatus.Int());
					}
					break;
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
		
			iActionState = EPostrequisite;
			}
			break;
			
		default:
			break;	//	Nothing to do, for the compiler
		}
	}

void CAuthenticationPolicy::PerformCancel()
    {
    if (iState == EFinished)
        {
        CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
        }
    }

void CAuthenticationPolicy::Reset()
	{
	iState = EInit;
	}

void CAuthenticationPolicy::DoReportAction()
{
	if(iMode.Compare(_L8("set")) == 0)
		{
		_LIT(KSet, "Setting authentication policy...");
		iOut.writeString(KSet);
		}
    else if(iMode.Compare(_L8("get")) == 0)
		{
		_LIT(KGet, "Getting authentication policy...");
		iOut.writeString(KGet);
		}
	
	iOut.writeString(iLabel->Left(32));
	if (iLabel->Length() > 32)
		{
		iOut.writeString(KEllipsis);
		}
	
	iOut.writeNewLine();
}


void CAuthenticationPolicy::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		if (aError == KErrNone)
		{
			_LIT(KSuccessful, "Operation Successfull\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Operation Failed!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}
			else
			{
				_LIT(KFailed, "Operation failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
			iOut.writeNewLine();
		}
	}
}
