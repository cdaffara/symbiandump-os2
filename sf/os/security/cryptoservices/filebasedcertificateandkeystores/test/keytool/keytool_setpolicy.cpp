/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/*static*/ CKeytoolSetPolicy* CKeytoolSetPolicy::NewLC(CKeyToolController* aController)
	{
	CKeytoolSetPolicy* self = new (ELeave) CKeytoolSetPolicy(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CKeytoolSetPolicy* CKeytoolSetPolicy::NewL(CKeyToolController* aController)
	{
	CKeytoolSetPolicy* self = CKeytoolSetPolicy::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}

CKeytoolSetPolicy::CKeytoolSetPolicy(CKeyToolController* aController) : CKeytoolList(aController)
	{
	}

CKeytoolSetPolicy::~CKeytoolSetPolicy()
	{
	Cancel();
	}

void CKeytoolSetPolicy::RunL()
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
			iKey = KeyToolUtils::findKey(iKeys, *iParams->iDefault);
			if (iKey)
				{
				TSecureId secureId; // Application secure ID 
				TSecurityPolicy securePolicy;
				idx = 0;
				if (iParams->iPolicy == CKeyToolParameters::ESetManagerPolicy)
					{
					securePolicy = ECapabilityWriteDeviceData;
					iKeyStore->SetManagementPolicy(iKey->Handle(), securePolicy, iStatus);
					}
				else
					{
					if (iParams->iPolicy == CKeyToolParameters::ESetUserPolicy)
						{
						secureId = iParams->iUIDs[idx];
						securePolicy = secureId;
						}
					else if (iParams->iPolicy == CKeyToolParameters::ESetAllUsersPolicy)
						{
						securePolicy = TSecurityPolicy::EAlwaysPass;
						}
					else if (iParams->iPolicy == CKeyToolParameters::ERemoveUserPolicy)
						{
						iParams->iUIDs.Close();
						iParams->iUIDs.Append(TUid::Uid(KUidSecurityKeytool));
						secureId = iParams->iUIDs[idx];
						securePolicy = secureId;
						}
					else
						{
						User::Leave(KErrNotFound); //should not come here
						}
					iKeyStore->SetUsePolicy(iKey->Handle(), securePolicy, iStatus);
					}
				iState = EFinished;
				SetActive();
				}
			else
				{
				User::Leave(KErrNotFound);
				}
			}
			break;
		case EFinished:
			{
			if (iParams->iPolicy == CKeyToolParameters::ESetManagerPolicy)
				{
				iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_SMANAGERSUCCESS);
				}
			else if (iParams->iPolicy == CKeyToolParameters::ESetUserPolicy)
				{
				iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_SUSERSUCCESS);
				}
			else if (iParams->iPolicy == CKeyToolParameters::ESetAllUsersPolicy)
				{
				iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_SALLUSERSSUCCESS);
				}
			else 
				{
				iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_RUSERSUCCESS);
				}
			CActiveScheduler::Stop();			
			}
			break;
		default:
			{
			User::Panic(_L("SETUSERS action: Illegal state."), 1);			
			}
		}		
	}

TInt CKeytoolSetPolicy::RunError(TInt aError)
	{
	CActiveScheduler::Stop();

	switch (aError)
		{
		case KErrNotFound :
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_NOTFOUND));
			}
			break;
		case KErrAccessDenied :
			{			
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_ACCESS));
			}
			break;
		default:
			{
			if (iParams->iPolicy == CKeyToolParameters::ESetManagerPolicy)
				{
				TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_SMANAGERFAIL, aError));
				}
			else if (iParams->iPolicy == CKeyToolParameters::ESetUserPolicy)
				{
				TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_SUSERFAIL, aError));
				}
			else if (iParams->iPolicy == CKeyToolParameters::ESetAllUsersPolicy)
				{
				TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_SALLUSERSFAIL, aError));
				}
			else 
				{
				TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_KEYTOOL_ERR_RUSERFAIL, aError));
				}
			}
		}	

	return KErrNone;
	}

