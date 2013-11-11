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

/*static*/ CKeytoolRemove* CKeytoolRemove::NewLC(CKeyToolController* aController)
	{
	CKeytoolRemove* self = new (ELeave) CKeytoolRemove(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CKeytoolRemove* CKeytoolRemove::NewL(CKeyToolController* aController)
	{
	CKeytoolRemove* self = CKeytoolRemove::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}

CKeytoolRemove::CKeytoolRemove(CKeyToolController* aController) : CKeytoolList(aController)
	{
	}
	
CKeytoolRemove::~CKeytoolRemove()
	{
	Cancel();
	iKeyList.Close();
	}

void CKeytoolRemove::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());
		}
		
	switch (iState)
		{	 
		case EListKeys:
			{
			iKeyList = 	KeyToolUtils::MatchKey(iKeys, *iParams->iDefault);
			if (iKeyList.Count() > 0)
				{
				iKeyStore->DeleteKey(iKeyList[idx]->Handle(), iStatus);
				iState = EFinished;								
				SetActive();
				}
			else
				{
				User::Leave(KErrNotFound);				
				}
			/*CCTKeyInfo* theKey = KeyToolUtils::findKey(iKeys, *iParams->iDefault);
			if (theKey)
				{
				iKeyStore->DeleteKey(theKey->Handle(), iStatus);
				iState = EFinished;
				SetActive();

				}
			else
				{
				User::Leave(KErrNotFound);
				}*/
			}
			break;
			
		case EFinished:
			{
			idx++;
			if (idx < iKeyList.Count())
				{
				iKeyStore->DeleteKey(iKeyList[idx]->Handle(), iStatus);
				SetActive();								
				}
			else 
				{
				iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_DELETESUCCESS);				
				CActiveScheduler::Stop();							
				}
			}
			break;
		default:
			{
			User::Panic(_L("REMOVE action: Illegal state."), 1);			
			}
		}
	
	}

