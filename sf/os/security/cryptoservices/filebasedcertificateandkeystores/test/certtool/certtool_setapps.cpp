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


#include "certtool_commands.h"
#include "certtool_controller.h"


/*static*/ CCertToolSetApps* CCertToolSetApps::NewLC(CCertToolController* aController)
	{
	CCertToolSetApps* self = new (ELeave) CCertToolSetApps(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CCertToolSetApps* CCertToolSetApps::NewL(CCertToolController* aController)
	{
	CCertToolSetApps* self = CCertToolSetApps::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}

CCertToolSetApps::CCertToolSetApps(CCertToolController* aController) : CCertToolList(aController), iCertIndex (-1)
	{
	}

	
CCertToolSetApps::~CCertToolSetApps()
	{
	Cancel();	
	}

void CCertToolSetApps::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		// A problem occured. Handle gracefully.
		User::Leave(iStatus.Int());
		}	
		
	switch (iState)
		{	 
		case EListCerts:
			{
			if (iCertInfos.Count()==0)
				{
				iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_EMPTY);
				CActiveScheduler::Stop();				
				}
			else
				{ 
				if (iParams->iDefault)    
					{
					// Look for a specific certificate					
					for (TInt j = 0; j < iCertInfos.Count(); j++)
						{
						if (iCertInfos[j]->Label() == *iParams->iDefault) 
							{
							iCertIndex = j;
							break;
							}
						}	
						
					if (iCertIndex != -1)
						{
						// Certificate found set app uids!
						if (iParams->iCertstoreIndex == -1)
							{
							iParams->iCertstoreIndex = 0;
							}
						if (iParams->iCertstoreIndex >= iCertStore->CertStoreCount())
							{
							iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_NOTEXIST);	
							User::Leave(KErrArgument);			
							}
						if(iCertInfos[iCertIndex]->CertificateOwnerType() == ECACertificate)							
							{
							iState = ESetTrust;
							}
						else
							{
							iState = EFinished;								
							}	
						iCertStore->SetApplicability(*iCertInfos[iCertIndex], iParams->iUIDs, iStatus);
						SetActive();
						break;
						}
					else 
						{
						User::Leave(KErrNotFound);
						}			
					}
				}
			}
		case ESetTrust:
			{
			iState = EFinished;
			const TBool caCert = ETrue;
			iCertStore->SetTrust(*iCertInfos[iCertIndex],caCert,iStatus);
			SetActive();				
			}
			break;
		case EFinished:
			{
			iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_SETAPPS_SUCCESS);				
			CActiveScheduler::Stop();			
			}
			break;
		default:
			{
			User::Panic(_L("SETAPPS action: Illegal state."), 1);
			}
		}
	}


TInt CCertToolSetApps::RunError(TInt aError)
	{
	CActiveScheduler::Stop();

	switch (aError)
		{
		case KErrNotFound :
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_NOTFOUND));
			TRAP_IGNORE(iController->DisplayErrorL(_L("\n"), iParams->iPageWise));			
			}
			break;
		default:
			{
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_ADDAPPS_FAIL));			
			TRAP_IGNORE(iController->DisplayErrorL(_L("\n"), iParams->iPageWise));			
			}
		}	

	return KErrNone;	
	}
