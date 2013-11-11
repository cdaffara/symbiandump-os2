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
#include "keytool_utils.h"

/*static*/ CCertToolAddApps* CCertToolAddApps::NewLC(CCertToolController* aController)
	{
	CCertToolAddApps* self = new (ELeave) CCertToolAddApps(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CCertToolAddApps* CCertToolAddApps::NewL(CCertToolController* aController)
	{
	CCertToolAddApps* self = CCertToolAddApps::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}

CCertToolAddApps::CCertToolAddApps(CCertToolController* aController) : CCertToolList(aController), iCertIndex (-1)
	{
	}

	
CCertToolAddApps::~CCertToolAddApps()
	{
	Cancel();
	}

void CCertToolAddApps::RunL()
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
						iCertStore->Applications(*iCertInfos[iCertIndex], iApps, iStatus);
						iState = ESetApps;
						SetActive();
						}
					else 
						{
						User::Leave(KErrNotFound);
						}			
					}
				}
			}
			break;
		case ESetApps:
			{				
			if (iParams->iCertstoreIndex == -1)
				{
				iParams->iCertstoreIndex = 0;
				}
			if (iParams->iCertstoreIndex >= iCertStore->CertStoreCount())
				{
				iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_NOTEXIST);	
				User::Leave(KErrArgument);			
				}				
			// Check for duplicates
			for (TInt k = 0; k < iParams->iUIDs.Count(); k++)
				{
				if (iApps.Find(iParams->iUIDs[k]) != KErrNotFound)
					{
					TBuf<128> msg;
					const TDesC& tmp = KeyToolUtils::Uid2Des(iParams->iUIDs[k]);
					msg.Format(_L("The certificate is already trusted for application %S."), &tmp);
					iController->DisplayErrorL(msg, iParams->iPageWise);
					User::Leave(KErrArgument);
					}
				}

			// add the apps to the array
			for (TInt j = 0; j < iApps.Count(); j++)
				{
				iParams->iUIDs.Append(iApps[j]);
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
			}
			break;
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
			iCertInfos.Close(); 
			iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_ADDAPPS_SUCCESS);				
			CActiveScheduler::Stop();			
			}
			break;
		default:
			{
			User::Panic(_L("ADDAPPS action: Illegal state."), 1);
			}
		}
	}

TInt CCertToolAddApps::RunError(TInt aError)
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
