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


#include <mctwritablecertstore.h>

#include "certtool_commands.h"
#include "certtool_controller.h"

#include "keytool_utils.h"

/*static*/ CCertToolRemoveApps* CCertToolRemoveApps::NewLC(CCertToolController* aController)
	{
	CCertToolRemoveApps* self = new (ELeave) CCertToolRemoveApps(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/*static*/ CCertToolRemoveApps* CCertToolRemoveApps::NewL(CCertToolController* aController)
	{
	CCertToolRemoveApps* self = CCertToolRemoveApps::NewLC(aController);
	CleanupStack::Pop(self);
	return self;
	}

CCertToolRemoveApps::CCertToolRemoveApps(CCertToolController* aController) : CCertToolList(aController)
	{
	}

	
CCertToolRemoveApps::~CCertToolRemoveApps()
	{
	Cancel();	
	}

void CCertToolRemoveApps::RunL()
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
					TInt certIndex = -1;
					for (TInt j = 0; j < iCertInfos.Count(); j++)
						{
						if (iCertInfos[j]->Label() == *iParams->iDefault) 
							{
							certIndex = j;
							break;
							}
						}	
						
					if (certIndex != -1)
						{
						iIndex = certIndex;
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
						iCertStore->Applications(*iCertInfos[iIndex], iApps, iStatus);
						iState = ERemoveApps;
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
		case ERemoveApps:
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
			TInt failCount = 0;				
			// Check for duplicates
			for (TInt k = 0; k < iParams->iUIDs.Count(); k++)
				{
				TInt idx = iApps.Find(iParams->iUIDs[k]);
				if ( idx == KErrNotFound)
					{
					failCount++;
					TBuf<128> msg;
					const TDesC& tmp = KeyToolUtils::Uid2Des(iParams->iUIDs[k]);
					msg.Format(_L("The certificate wan not trusted for application %S."), &tmp);
					iController->DisplayErrorL(msg, iParams->iPageWise);
					}
				else 
					{
					iApps.Remove(idx);
					}
				}

			if (failCount == iParams->iUIDs.Count())
				{
				CActiveScheduler::Stop();			
				}
			else
				{
				iState = EFinished;
				iCertStore->SetApplicability(*iCertInfos[iIndex], iApps, iStatus);
				SetActive();	
				}	
			}
			break;
		case EFinished:
			{
			iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_REMOVEAPPS_SUCCESS);			
			CActiveScheduler::Stop();			
			}
			break;
		default:
			{
			User::Panic(_L("REMOVEAPPS action: Illegal state."), 1);
			}
		}		

	}

TInt CCertToolRemoveApps::RunError(TInt aError)
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
			TRAP_IGNORE(iController->DisplayErrorL(_L("RemoveApps: Unknown error. "), aError));		
			TRAP_IGNORE(iController->DisplayErrorL(_L("\n"), iParams->iPageWise));			
			}
		}	

	return KErrNone;	
	}
