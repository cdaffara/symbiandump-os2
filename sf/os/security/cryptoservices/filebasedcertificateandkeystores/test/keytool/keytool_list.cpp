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
*
*/


#include <mctkeystore.h>

#include "keytool_utils.h"
#include "keytool_commands.h"
#include "keytool_controller.h"

#include <keytool.rsg>

/*static*/ CKeytoolList* CKeytoolList::NewLC(CKeyToolController* aController)
	{
	CKeytoolList* self = new (ELeave) CKeytoolList(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CKeytoolList* CKeytoolList::NewL(CKeyToolController* aController)
	{
	CKeytoolList* self = CKeytoolList::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}

CKeytoolList::CKeytoolList(CKeyToolController* aController) : CKeyToolCommand(aController)
	{
	}
	
CKeytoolList::~CKeytoolList()
	{
	Cancel();
	iKeys.Close();
	}

void CKeytoolList::ConstructL()
	{
	CActiveScheduler::Add(this);
	
	// Construct a catch-all filter!
	iFilter.iKeyAlgorithm = CCTKeyInfo::EInvalidAlgorithm;
	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;
	iFilter.iUsage = EPKCS15UsageAll;	
	}
	
void CKeytoolList::DoCommandL(CUnifiedKeyStore& aKeyStore, CKeyToolParameters* aParam)
	{
	Cancel();
	iParams = aParam;
	iKeyStore = &aKeyStore;		
	iState = EListKeys;
	if (aParam->iKeystoreIndex != -1)
		{
		if (aKeyStore.KeyStoreCount() <= aParam->iKeystoreIndex)
			{
			iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_NOTEXIST);			
			User::Leave(KErrArgument);
			}
		MCTKeyStore& chosen = aKeyStore.KeyStore(aParam->iKeystoreIndex);
		iKeyStoreImp = &chosen;
		chosen.List(iKeys, iFilter, iStatus);
		}
	else 
		{
		aParam->iKeystoreIndex = 0;
		iKeyStore->List(iKeys, iFilter, iStatus);		
		}		 	
	SetActive();
	}

void CKeytoolList::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		// A problem occured. Handle gracefully.
		User::Leave(iStatus.Int());
		}
		
	switch (iState)
		{	 
		case EListKeys:
			{
			if (iKeys.Count()==0)
				{
				iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_EMPTY);				
				}
			else
				{				
				if (iParams->iDefault)
					{
					RArray<CCTKeyInfo*> theKeys = KeyToolUtils::MatchKey(iKeys, *iParams->iDefault);
					CleanupClosePushL(theKeys);
					if (theKeys.Count() > 0)
						{
						for (TInt k = 0; k < theKeys.Count(); k++)
							{
							iController->DisplayKeyL(*theKeys[k], iParams->iIsDetailed, iParams->iPageWise);
							}
						}
					else 
						{
						iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_NOTFOUND);						
						}
					CleanupStack::PopAndDestroy(&theKeys);
					}
				else 
					{
					TInt keyCount = iKeys.Count();
					for (TInt i = 0; i < keyCount; i++)
 						{
 						iController->DisplayKeyL(*iKeys[i], iParams->iIsDetailed, iParams->iPageWise);
 						}
					}
				}
			CActiveScheduler::Stop();
			}
			break;
		default:
			{
			User::Panic(_L("LIST action: Illegal state."), 1);
			}
			break;
		}	
	}
	
TInt CKeytoolList::RunError(TInt aError)
	{
	CActiveScheduler::Stop();

	switch (aError)
		{
		case KErrNotFound :
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_NOTFOUND));
			}
			break;
		default:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_LISTFAIL, aError));			
			}
		}	

	return KErrNone;
	}

void CKeytoolList::DoCancel()
	{
	switch (iState)
		{
		case EListKeys:
			{
			iKeyStore->Cancel();
			}
			break;
		default :	
			{
			
			}
		}
	}

