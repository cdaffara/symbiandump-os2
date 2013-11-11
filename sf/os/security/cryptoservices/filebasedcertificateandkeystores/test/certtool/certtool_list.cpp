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
#include <ccertattributefilter.h>
#include <mctcertstore.h>


_LIT(KCaType, "ca");
_LIT(KUserType, "user");
_LIT(KPeerType, "peer");

// The number of certificates being listed at a time.
#define LIST_COUNT 30

/*static*/ CCertToolList* CCertToolList::NewLC(CCertToolController* aController)
	{
	CCertToolList* self = new (ELeave) CCertToolList(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
/*static*/ CCertToolList* CCertToolList::NewL(CCertToolController* aController)
	{
	CCertToolList* self = CCertToolList::NewLC(aController);
	CleanupStack::Pop(self);
	return self;	
	}
	
CCertToolList::CCertToolList(CCertToolController* aController) : CCertToolCommand(aController)
	{
	
	}
	
CCertToolList::~CCertToolList()
	{
	Cancel();
	delete iFilter;

	if (iCertApps.Find(iApps) == KErrNotFound)
		{
		iApps.Close();		
		}
	
	for (TInt k = 0; k < iCertApps.Count(); k++)
		{
		iCertApps[k].Close();											
		}
	iCertTrust.Close();
	iCertApps.Close();
	iCertInfos.Close();
	iParsedCerts.ResetAndDestroy();
	iParsedCerts.Close();
	}
	
void CCertToolList::ConstructL()
	{
	CActiveScheduler::Add(this);
	
	// This filter will accept any certificates
	iFilter = CCertAttributeFilter::NewL();
	}

/**
 * Lists the content of the unifiedcertstore. 
 * If a specific certstore implementation is specified using
 * the -store command line parameter, it will only list
 * the content of that particular store!
 */
void CCertToolList::DoCommandL(CUnifiedCertStore& aCertStore, CKeyToolParameters* aParam)
	{
	Cancel();
	iParams = aParam;	
	iState = EListCerts;
	iCertStore = &aCertStore;
	if (aParam->iCertstoreIndex != -1)
		{
		if (aCertStore.CertStoreCount() <= aParam->iCertstoreIndex)
			{
			iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_NOTEXIST);			
			User::Leave(KErrArgument);
			}
		MCTCertStore& chosen = aCertStore.CertStore(aParam->iCertstoreIndex);
		iCertStoreImp = &chosen;
		chosen.List(iCertInfos, *iFilter, iStatus);
		}
	else 
		{
		aParam->iCertstoreIndex = 0;
		iCertStore->List(iCertInfos, *iFilter, iStatus);		
		}
	SetActive();	
	}
	
	


void CCertToolList::InitializeIterations()
    { 	
	// Initialize the current list count and iCurrentListOffset with default LIST_COUNT.
    iCurrentListCount = iCurrentListOffset = LIST_COUNT;				
			
    if (iCertInfos.Count() < LIST_COUNT)
        {
        iCurrentListCount = iCurrentListOffset = iCertInfos.Count();
        }
				    
    // Compute the number of iterations for listing.    
    // The latter operation is to add another iteration count if iCertInfos 
    // count is not an exact multiple of LIST_COUNT.
	iNumberOfIterationsLeft =  (iCertInfos.Count() / LIST_COUNT) + !!(iCertInfos.Count() % LIST_COUNT);   
								

   // Initialize the last iteration offset with default LIST_COUNT.				
   iLastIterationOffset = LIST_COUNT;		
                		
   if (iCertInfos.Count() % LIST_COUNT)
       {
       // If the number of certificates is not an exact multiple of default LIST_COUNT.
   	   iLastIterationOffset = (iCertInfos.Count() % LIST_COUNT);        			 
       }
   }       
	
void CCertToolList::RunL()
	{
	if (iStatus.Int() != KErrNone)
		{
		if (iState != EGetApps && iStatus.Int() != KErrNotSupported)
			{
			// A problem occured. Handle gracefully.
			User::Leave(iStatus.Int());	
			}
		}		
		
	switch (iState)
		{
		case EListCerts :
			{
			// Now retrieve the actual certs!

			if (iCertInfos.Count()==0)
				{
				iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_EMPTY);
				CActiveScheduler::Stop();				
				}
			else
				{ 
				if (iParams->iDefault)    
					{
					KeyToolUtils::FilterCertsL(iCertInfos, *iParams->iDefault);					
					}
				else if (iParams->iOwnerType)
					{
					TDesC& ownerTypeDesc = *iParams->iOwnerType;
					TCertificateOwnerType ownerType;
					if (ownerTypeDesc.CompareF(KCaType) == KErrNone)
						{
						ownerType = ECACertificate;
						}
					else if (ownerTypeDesc.CompareF(KUserType) == KErrNone)
						{
						ownerType = EUserCertificate;
						}
					else if (ownerTypeDesc.CompareF(KPeerType) == KErrNone)
						{
						ownerType = EPeerCertificate;
						}
					else
						{
						iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_EMPTY);
						CActiveScheduler::Stop();
						break;
						}						
					KeyToolUtils::FilterCertsL(iCertInfos, ownerType);
					}

               
                InitializeIterations();   

  				iState = ERetrieve;
  				iIndex = 0;
				if (iIndex <= (iCertInfos.Count()-1 ))
					{
					iCertStore->Retrieve(*iCertInfos[iIndex], iCertificate, iStatus);
  					SetActive();
					}
				else
					{
					iState = EFinished;	
  					CActiveScheduler::Stop();					
					}
				}
			}
			break;
		case ERetrieve :
			{
            // This marks the beginning of the current iteration.           
			// Retrieve all the certificates up to the current list count.            
                       
			iParsedCerts.Append(iCertificate);
			iIndex++;

			if (iIndex < iCurrentListCount)
			    {
			    iCertStore->Retrieve(*iCertInfos[iIndex], iCertificate, iStatus);            
			    SetActive();
			    }
			else
			    {                
			    iState = EGetApps;
                                                 
			    // Start getting the applications starting from the iIndex(current list's beginning). 
			    iIndex = iCurrentListCount - iCurrentListOffset;
			    RUidArray t;
			    iApps = t;
			    iCertStore->Applications(*iCertInfos[iIndex], iApps, iStatus);
			    SetActive();
			    }
			}
			break;
		case EGetApps :
			{	
			iCertApps.Append(iApps);

			iIndex++;

			// Get all the applications up to the current list count.            
			if (iIndex < iCurrentListCount)
			    {      
			    RUidArray t;
			    iApps = t;				
			    iCertStore->Applications(*iCertInfos[iIndex], iApps, iStatus);
			    SetActive();
			    }
			else
			    {
			    iState = EGetTrust;
                                
		    	// Start getting the trust starting from the iIndex(current list's beginning). 
			    iIndex = iCurrentListCount - iCurrentListOffset;                
			    iCertStore->Trusted(*iCertInfos[iIndex], iTrust, iStatus);
			    SetActive();
			    }			
			}
			break;
		case EGetTrust :
			{	
			iCertTrust.Append(iTrust);

			iIndex++;
	
  			// Get all the trust up to the current list count. 
  			if (iIndex < iCurrentListCount) 
		            {
		            iCertStore->Trusted(*iCertInfos[iIndex], iTrust, iStatus);
		            SetActive();
		            }
  			else 
		            {
                    // Update the number of iterations.                  
                    iNumberOfIterationsLeft--;                                        

                    // Check if this happens to be the last iteration. If so, adjust the list offset.                                                                    
			        if (iNumberOfIterationsLeft == 0)
			            {
			            iCurrentListOffset = iLastIterationOffset;
			            }                                
                                
		            // We are done with the current list processing, it's time to display the collected data.                
		            TInt DisplayOffset = iCurrentListOffset;
                    
             	    // Display(list) the certificates just processed.  This marks the end of the current iteration.    
		            for (TInt i = iIndex - DisplayOffset; i < iCurrentListCount; i++)
		                 {
		                 iController->DisplayCertL(*iCertInfos[i], *iParsedCerts[i], iCertApps[i], iCertTrust[i], iParams->iIsDetailed, iParams->iPageWise);                      
	                         if (iParsedCerts[i] )
		                     {
		                     delete iParsedCerts[i];  // Relinquish the resources for facilitating the subsequent listing.
		                     iParsedCerts[i] = 0;     
		                     }
		                 }                 
                
    	                    // How many more are remaining??
		            TInt RemainingListCount = iCertInfos.Count() - iCurrentListCount;                
                

                            if (RemainingListCount == 0)
		                {
            	                iState = EFinished;
		                CActiveScheduler::Stop();
		                break;
		                }                                                    
                
		            if (RemainingListCount < LIST_COUNT)
		                {       
                        // Total number of certificates is not an exact multiple of default LIST_COUNT.
                        // Last iteration will have the list count lesser than the default LIST_COUNT.
                        // So, only one iteration is left.
		                iCurrentListCount += RemainingListCount;
		                }
		            else
		                {
                                //  Still a few more are left.
	                        iCurrentListCount += LIST_COUNT;
		                }
                  
		            // Start the next iteration.   
		            iState = ERetrieve;               
		            iCertStore->Retrieve(*iCertInfos[iIndex], iCertificate, iStatus);
		            SetActive();                                         
		            }			
			}
			break;
		case EFinished :
			{
			}
			break;
		default:
			{
			User::Panic(_L("LIST action: Illegal state."), 1);			
			}
			break;
		}
	}
	
void CCertToolList::DoCancel()
	{
	
	}
	
TInt CCertToolList::RunError(TInt aError)
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
			TRAP_IGNORE(iController->DisplayLocalisedMsgL(R_CERTTOOL_ERR_LISTFAIL, aError));		
			TRAP_IGNORE(iController->DisplayErrorL(_L("\n"), iParams->iPageWise));			
			}
		}	

	return KErrNone;	
	}
	
