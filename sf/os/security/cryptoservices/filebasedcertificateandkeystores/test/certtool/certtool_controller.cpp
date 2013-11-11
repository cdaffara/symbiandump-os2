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


#include <barsc.h>
#include <barsread.h>
#include <utf.h>

#include "certtool_controller.h"


_LIT(KCertToolResources,"Z:\\Resource\\Apps\\keytool.RSC");

CCertToolController::CCertToolController(MKeyToolView& aView) : CController(aView)
	{
	}

CCertToolController::~CCertToolController()
	{	
	delete iEngine; 
	}

/*static*/ CCertToolController*  CCertToolController::NewLC(MKeyToolView& aView)
	{
	CCertToolController* self = new (ELeave) CCertToolController(aView);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self; 
	}

void CCertToolController::ConstructL()
	{
	User::LeaveIfError(iFsSession.Connect()); // Connect session
 
	TFileName fileName(KCertToolResources);
	 
	// open file, leave if error
	iResourceFile.OpenL(iFsSession, fileName);

	iResourceFile.ConfirmSignatureL();
	
	iEngine = CCertToolEngine::NewL(this);
	}

void CCertToolController::HandleCommandL(TInt aCommand, CKeyToolParameters* aParam)
	{
	TInt certstoreIndex = -1;
	switch (aCommand) 
		{
		case KUsageCommand:
			{
			iEngine->DisplayUsageL(aParam);
			}
			break;
		case KListCommand:
			{
			iEngine->ListL(aParam);			
			CActiveScheduler::Start();	
			}
			break;
		case KImportCommand:
			{
			if (aParam->iPrivate)
				{
				CKeyToolParameters* parm = CKeyToolParameters::NewLC();
				parm->iDefault = aParam->iPrivate->AllocL();
				if(aParam->iLabel)
					{
					parm->iLabel = aParam->iLabel->AllocL();											
					}
				else
					{
					parm->iLabel = aParam->iDefault->AllocL();					
					}
				iEngine->ImportPrivateL(parm);
				CActiveScheduler::Start();
				delete parm->iDefault;
 				parm->iDefault = NULL;
 				if(aParam->iLabel)
 					{
 					parm->iDefault = aParam->iLabel->AllocL();						
 					}
 				else
 					{
					parm->iDefault = aParam->iDefault->AllocL();					
 					}
				parm->iPolicy = CKeyToolParameters::ESetManagerPolicy;
				iEngine->SetManagerPolicyL(parm);
				CActiveScheduler::Start();
				CleanupStack::PopAndDestroy(parm);								
				}			
			iEngine->ImportL(aParam);
			CActiveScheduler::Start();
			if (aParam->iLabel)
				{
				delete aParam->iDefault;
				aParam->iDefault = NULL;
				aParam->iDefault = aParam->iLabel->AllocL();
				}
			iEngine->ListL(aParam);			
			CActiveScheduler::Start();				
			}
			break;	
		case KRemoveCommand:
			{
			if (aParam->iRemoveKey)
				{
				CKeyToolParameters* parm = CKeyToolParameters::NewLC();
				parm->iDefault = aParam->iDefault->AllocL();	
				iEngine->RemovePrivateL(parm);
				CActiveScheduler::Start();
				CleanupStack::PopAndDestroy(parm);
				}
			iEngine->RemoveL(aParam);
			CActiveScheduler::Start();				
			}	
			break;
		case KSetAppsCommand:
			{
			certstoreIndex = aParam->iCertstoreIndex;
			iEngine->SetAppsL(aParam);
			if (aParam->iLabel)
				{
				delete aParam->iDefault;
				aParam->iDefault = NULL;
				aParam->iDefault = aParam->iLabel->AllocL();
				}
			CActiveScheduler::Start();
			aParam->iCertstoreIndex = certstoreIndex;
			iEngine->ListL(aParam);			
			CActiveScheduler::Start();							
			}	
			break;
		case KListStoresCommand:
			{
			iEngine->ListStoresL(aParam);
			CActiveScheduler::Start();				
			}
			break;
		case KAddAppsCommand:
			{
			certstoreIndex = aParam->iCertstoreIndex;
			iEngine->AddAppsL(aParam);
			if (aParam->iLabel)
				{
				delete aParam->iDefault;
				aParam->iDefault = NULL;
				aParam->iDefault = aParam->iLabel->AllocL();
				}
			CActiveScheduler::Start();
			aParam->iCertstoreIndex = certstoreIndex;	
			iEngine->ListL(aParam);			
			CActiveScheduler::Start();						
			}	
			break;				
		case KRemoveAppsCommand:
			{
			certstoreIndex = aParam->iCertstoreIndex;
			iEngine->RemoveAppsL(aParam);
			CActiveScheduler::Start();	
			aParam->iCertstoreIndex = certstoreIndex;
			iEngine->ListL(aParam);			
			CActiveScheduler::Start();						
			}	
			break;
		default:
			{
			DisplayLocalisedMsgL(R_CERTTOOL_ERR_COMMAND);
			User::Leave(KErrNotSupported);			
			}
		}
	}
