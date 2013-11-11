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

/*static*/ CCertToolRemove* CCertToolRemove::NewLC(CCertToolController* aController)
	{
	CCertToolRemove* self = new (ELeave) CCertToolRemove(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
/*static*/ CCertToolRemove* CCertToolRemove::NewL(CCertToolController* aController)
	{
	CCertToolRemove* self = CCertToolRemove::NewLC(aController);
	CleanupStack::Pop(self);
	return self;	  
	}
	
CCertToolRemove::CCertToolRemove(CCertToolController* aController) : CCertToolList(aController)
	{
	
	}
	
CCertToolRemove::~CCertToolRemove()
	{
	Cancel();
	}
	
void CCertToolRemove::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		User::Leave(iStatus.Int());
		}	
	switch (iState)
		{	 
		case EListCerts:
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
				iState = EFinished;
				SetActive();				
				iCertStore->Remove(*iCertInfos[certIndex], iStatus);
				}
			else 
				{
				User::Leave(KErrNotFound);
				}			
			}
			break;
		case EFinished:
			{

			User::LeaveIfError(iStatus.Int());

		     // We are done!
		    iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_REMOVE_SUCCESS);   
			 		
			CActiveScheduler::Stop();
			}
			break;
		default:
			{
			User::Panic(_L("REMOVE action: Illegal state."), 1);					
			}			
		}
	}

TInt CCertToolRemove::RunError(TInt aError)
    {
    CActiveScheduler::Stop();

    switch (aError)
        {
        case KErrNotFound :
            {
            TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_NOTFOUND));         
            }
            break;
        default:
            {
            TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_REMOVE_FAILURE, aError));                
            }
        }   

	TRAP_IGNORE(iController->DisplayErrorL(_L("\n"), iParams->iPageWise));  
    return KErrNone;        
    }

