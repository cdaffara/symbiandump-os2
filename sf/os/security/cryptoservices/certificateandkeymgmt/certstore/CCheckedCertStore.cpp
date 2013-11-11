/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CCheckedCertStore.h"

#include <signed.h>
#include <x509cert.h>
#include <x509certext.h>
#include <wtlscert.h>
#include <x509keys.h>
#include <securityerr.h>

#include "certificateapps.h"

_LIT(KPanicCategory, "CCheckedCertStore");
#define assert(x) __ASSERT_ALWAYS((x), User::Panic(KPanicCategory, 1));

/////////////////////////////////////////////////////////////////////
//	CCheckedCertStore
/////////////////////////////////////////////////////////////////////

CCheckedCertStore::~CCheckedCertStore()
    {
    Cancel();
    Cleanup();
    
	// Release the cert store - no need to release the token since this would
	// have been done as part of MCTTokenInterface::Release()
    iCertStore.Release();

	iFs.Close();
    }

CCheckedCertStore::CCheckedCertStore(MCTCertStore& aTokenIF, RProperty& aProperty)
:	CActive(EPriorityStandard), 
	iCertStore(aTokenIF),
	iPSCertstoreChangePropertyRef(aProperty)	
    {
	// need to add reference since we now have the token
	iCertStore.Token().AddRef();
    }

CCheckedCertStore::CCheckedCertStore(MCTWritableCertStore& aTokenIF, RProperty& aProperty)
:	CActive(EPriorityStandard),
	iCertStore(aTokenIF),
	iWritableCertStore(&aTokenIF),
	iPSCertstoreChangePropertyRef(aProperty)
    {
	// need to add reference since we now have the token
	iCertStore.Token().AddRef();
    }

/*static*/ CCheckedCertStore* CCheckedCertStore::NewCheckedCertStoreL(MCTTokenInterface* aTokenIF, RProperty& aProperty)
    {
    assert(aTokenIF);
	MCTCertStore& tokenInterface = static_cast<MCTCertStore&>(*aTokenIF);
	CCheckedCertStore* me = new (ELeave) CCheckedCertStore(tokenInterface, aProperty);
    CleanupReleasePushL(*me);
    me->ConstructL();
    CleanupStack::Pop();
	return (me);
    }
	
/*static*/ CCheckedCertStore* CCheckedCertStore::NewCheckedWritableCertStoreL(MCTTokenInterface* aTokenIF, RProperty& aProperty)
    {
    assert(aTokenIF);
	MCTWritableCertStore& tokenInterface = static_cast<MCTWritableCertStore&>(*aTokenIF);
	CCheckedCertStore* me = new (ELeave) CCheckedCertStore(tokenInterface, aProperty);
    CleanupReleasePushL(*me);
    me->ConstructL();
    CleanupStack::Pop();
	return (me);
    }

void CCheckedCertStore::ConstructL()
    {
	User::LeaveIfError(iFs.Connect());	
	CActiveScheduler::Add(this);
    }

MCTToken& CCheckedCertStore::Token()
    {
	return iCertStore.Token();
    }

//	May require checking against the keystore *after* calling the server to complete
//	the List request
void CCheckedCertStore::List(RMPointerArray<CCTCertInfo>& aCerts, const CCertAttributeFilter& aFilter,
                             TRequestStatus& aStatus)
    {
    assert(iState == EIdleState);

    // Only allow filtering on key usage for user certs
    if (aFilter.iKeyUsage != EX509UsageAll &&
        (!aFilter.iOwnerTypeIsSet || EUserCertificate != aFilter.iOwnerType))
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNotSupported);
        return;
        }

    // Store caller parameters for later reference
    iCallerCerts = &aCerts;
    iCallerFilter = &aFilter;
    aStatus = KRequestPending;
    iCallerStatus = &aStatus;

    iState = EList;
    iCertStore.List(aCerts, aFilter, iStatus);
    SetActive();
    }

void CCheckedCertStore::CancelList()
    {
    if (iState == EList ||
        iState == EInitKeyStoreForList ||
        iState == EGetKeyInfosForList)
        {
        Cancel();
        }
    }

void CCheckedCertStore::GetCert(CCTCertInfo*& aCertInfo, const TCTTokenObjectHandle& aHandle, 
                                TRequestStatus& aStatus)
    {
    assert(iState == EIdleState);
    iCertStore.GetCert(aCertInfo, aHandle, aStatus);
    }

void CCheckedCertStore::CancelGetCert()
    {
    iCertStore.CancelGetCert();
    }

void CCheckedCertStore::Applications(const CCTCertInfo& aCertInfo, RArray<TUid>& aApplications,
									 TRequestStatus& aStatus)
    {
    assert(iState == EIdleState);
    iCertStore.Applications(aCertInfo, aApplications, aStatus);
    }

void CCheckedCertStore::CancelApplications()
    {
    iCertStore.CancelApplications();
    }

void CCheckedCertStore::IsApplicable(const CCTCertInfo& aCertInfo, TUid aApplication, 
                                     TBool& aIsApplicable, TRequestStatus& aStatus)
    {
    assert(iState == EIdleState);
    iCertStore.IsApplicable(aCertInfo, aApplication, aIsApplicable, aStatus);
    }

void CCheckedCertStore::CancelIsApplicable()
    {
    iCertStore.CancelIsApplicable();
    }

void CCheckedCertStore::Trusted(const CCTCertInfo& aCertInfo, TBool& aTrusted, 
                                TRequestStatus& aStatus)
    {
    assert(iState == EIdleState);
    iCertStore.Trusted(aCertInfo, aTrusted, aStatus);
    }

void CCheckedCertStore::CancelTrusted()
    {
    iCertStore.CancelTrusted();
    }

void CCheckedCertStore::Retrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert, 
                                 TRequestStatus& aStatus)
    {
    assert(iState == EIdleState);
    iCertStore.Retrieve(aCertInfo, aEncodedCert, aStatus);
    }

void CCheckedCertStore::CancelRetrieve()
    {
    iCertStore.CancelRetrieve();
    }


void CCheckedCertStore::Add(const TDesC& aLabel, 
							TCertificateFormat aFormat,
                            TCertificateOwnerType aCertificateOwnerType, 
                            const TKeyIdentifier* aSubjectKeyId,
                            const TKeyIdentifier* aIssuerKeyId,
                            const TDesC8& aCert, 
                            TRequestStatus& aStatus)
    {
 	// default value for aDeletable = ETrue		
	Add( aLabel, aFormat, aCertificateOwnerType, aSubjectKeyId, 
			aIssuerKeyId, aCert, ETrue, aStatus );
    }
  
// new Add(.., TBool aDeletable, ..) method from MCTWritableCertStore
void CCheckedCertStore::Add( const TDesC& aLabel, 
							 TCertificateFormat aFormat,
                             TCertificateOwnerType aCertificateOwnerType, 
                             const TKeyIdentifier* aSubjectKeyId,
                             const TKeyIdentifier* aIssuerKeyId,
                             const TDesC8& aCert, 
                             const TBool aDeletable,
                             TRequestStatus& aStatus
                             )
    {
    assert(iWritableCertStore);
    assert(iState == EIdleState);

    TRAPD(err, DoAddL(	aLabel, 
    					aFormat,
    					aCertificateOwnerType, 
    					aSubjectKeyId, 
    					aIssuerKeyId, 
    					aCert, 
    					aDeletable,
    					aStatus		) );
    
	if (err != KErrNone)
		{
    	    Complete(err);
		}
	}

 
void CCheckedCertStore::DoAddL(	const TDesC& aLabel,
								TCertificateFormat aFormat,
                               	TCertificateOwnerType aCertificateOwnerType, 
                               	const TKeyIdentifier* aSubjectKeyId,
                               	const TKeyIdentifier* aIssuerKeyId,
                               	const TDesC8& aCert, 
                               	const TBool aDeletable,
                               	TRequestStatus& aStatus)
    {
    
    //	Store caller parameters for later use
	    aStatus			= KRequestPending;
	    iCallerStatus	= &aStatus;
	    iFormat 	 	= aFormat;
	    iCertificateOwnerType = aCertificateOwnerType;
	    iSubjectKeyId	= aSubjectKeyId;
	    iIssuerKeyId	= aIssuerKeyId;
		iDeletable  	= aDeletable;

	// Store (copy)  aCert (cert data) into iCertificate[:HBufC8]
	    assert(!iCertificate);
	    iCertificate = HBufC8::NewMaxL(aCert.Size());
	    TPtr8 theCert(iCertificate->Des());
	    theCert.FillZ();
	    theCert.Copy(aCert);

	// Store (copy) aLabel (cert label) into iCertLabel[:HBufC]
	    assert(!iCertLabel);
	    iCertLabel = HBufC::NewMaxL(aLabel.Length());
	    TPtr theLabel(iCertLabel->Des());
	    theLabel.FillZ();
	    theLabel.Copy(aLabel);

    //	Checks subject key ID with certificate data, and sets up key filter
    //	which is used later to determine whether there is a key with the
    //	appropriate subject and thus, if it is OK to add the certificate	
	    ComputeAndCheckSubjectKeyIdL();

    //	Is keystore checking required? Only if a user certificate
    if (EUserCertificate==aCertificateOwnerType)
		{
        	InitialiseKeyStoreL(EInitKeyStoreForAdd);
		}
    else
		{
        iState = EAdd;
        
        // try new method first
        iWritableCertStore->Add( *iCertLabel,			// call new method
        						 iFormat,
        						 iCertificateOwnerType,
                                 iSubjectKeyId,
                                 iIssuerKeyId,
                                 *iCertificate,
                                 iDeletable,			// with deletable param
                                 iStatus );                    
        SetActive();
		}
	}



void CCheckedCertStore::CancelAdd()
    {
    if (iState == EInitKeyStoreForAdd ||
        iState == EGetKeyInfosForAdd ||
        iState == EAdd || iState == EOldAdd )
        {
        Cancel();
        }
    }

void CCheckedCertStore::ComputeAndCheckSubjectKeyIdL()
    {
	switch (iFormat)
        {
		case EX509Certificate:
            {
			TPtr8 thePtr(iCertificate->Des());
			CX509Certificate* cert = CX509Certificate::NewLC(thePtr);

			TKeyUsageX509 x509Usage = EX509UsageNone;
			const CX509CertExtension* ext = cert->Extension(KKeyUsage);

			if (!ext)
                {
				x509Usage = EX509UsageAll;
                }
			else
                {
				CX509KeyUsageExt* keyUsageExt = CX509KeyUsageExt::NewLC(ext->Data());
				
				if (keyUsageExt->IsSet(EX509DigitalSignature))
					{
					x509Usage |= EX509UsageDigitalSignature;
					}
				if (keyUsageExt->IsSet(EX509NonRepudiation))
					{
					x509Usage |= EX509UsageNonRepudiation;
					}
				if (keyUsageExt->IsSet(EX509KeyEncipherment))
					{
					x509Usage |= EX509UsageKeyEncipherment;
					}
				if (keyUsageExt->IsSet(EX509DataEncipherment))
					{
					x509Usage |= EX509UsageDataEncipherment;
					}
				if (keyUsageExt->IsSet(EX509KeyAgreement))
					{
					x509Usage |= EX509UsageKeyAgreement;
					}
				if (keyUsageExt->IsSet(EX509KeyCertSign))
					{
					x509Usage |= EX509UsageKeyCertSign;
					}
				if (keyUsageExt->IsSet(EX509CRLSign))
					{
					x509Usage |= EX509UsageCRLSign;
					}
				if (keyUsageExt->IsSet(EX509EncipherOnly))
					{
					x509Usage |= EX509UsageEncipherOnly;
					}
				if (keyUsageExt->IsSet(EX509DecipherOnly))
					{
					x509Usage |= EX509UsageDecipherOnly;
					}

				CleanupStack::PopAndDestroy(keyUsageExt);
                }

			iKeyFilter.iUsage = KeyUsageX509ToPKCS15Private(x509Usage);
			
			iComputedSubjectKeyId.Zero();
			// For non-user ceriticates (i.e. CA certificates), we use the SubjectKeyIdentifier API, as it
			// tries to get the extension from cert., and calculates a value only if it is not found. This behaviour corresponds to the RFC.
			// For user ceritificates, the key identifier is used for matching key store with cert store, so we cannot use the value in the certificate itself.
			if (iCertificateOwnerType != EUserCertificate)
				{
				iComputedSubjectKeyId = cert->SubjectKeyIdentifierL();
				}
			else
				{
				// For non-CA certs, use the recommended method of computing it from RFC3280, section 4.2.1.2
				iComputedSubjectKeyId = cert->KeyIdentifierL();									
				}
			if (!iSubjectKeyId || *iSubjectKeyId == KNullDesC8)
                {
				iSubjectKeyId = &iComputedSubjectKeyId;
                }
			else if (iSubjectKeyId->Compare(iComputedSubjectKeyId)!=0)
                {//	Different subject ids
				User::Leave(KErrArgument);
                }
			
			CleanupStack::PopAndDestroy(cert);
            }	
            break;
	
        case EWTLSCertificate:
            {
            CCertificate* cert = CWTLSCertificate::NewLC(*iCertificate);
            iComputedSubjectKeyId = cert->KeyIdentifierL();
            if (!iSubjectKeyId || *iSubjectKeyId == KNullDesC8)
                {
                iSubjectKeyId = &iComputedSubjectKeyId;
                }
            else if (iSubjectKeyId->Compare(iComputedSubjectKeyId)!=0)
                {//	Different subject ids
                User::Leave(KErrArgument);
                }
	
            CleanupStack::PopAndDestroy(cert);
            }
            break;

        case EX509CertificateUrl:
            {
            iKeyFilter.iUsage = EPKCS15UsageAll;
		
            if (!iSubjectKeyId || *iSubjectKeyId == KNullDesC8)
                {
                User::Leave(KErrArgument);
                }
            }
            break;

        default:
            User::Leave(KErrNotSupported);	
            break;
        }

	iKeyFilter.iKeyId = *iSubjectKeyId;
    }

void CCheckedCertStore::Remove(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus)
    {
    assert(iWritableCertStore);
    assert(iState == EIdleState);
    aStatus = KRequestPending;
    iCallerStatus = &aStatus;
    iState = ERemove;
    iWritableCertStore->Remove(aCertInfo, iStatus);
    SetActive();
    }

void CCheckedCertStore::CancelRemove()
    {
    if (iState == ERemove)
        {
        Cancel();
        }
    }

void CCheckedCertStore::SetApplicability(const CCTCertInfo& aCertInfo, const RArray<TUid>& aApplications, TRequestStatus &aStatus)
    {
    assert(iWritableCertStore);
    assert(iState == EIdleState);
    aStatus = KRequestPending;
    iCallerStatus = &aStatus;
    iState = ESetApplicability;
    iWritableCertStore->SetApplicability(aCertInfo, aApplications, iStatus);
    SetActive();
    }
	
void CCheckedCertStore::CancelSetApplicability()
    {
    if (iState == ESetApplicability)
        {
        Cancel();
        }    
    }

void CCheckedCertStore::SetTrust(const CCTCertInfo& aCertInfo, TBool aTrusted, TRequestStatus& aStatus)
    {
    assert(iWritableCertStore);
    assert(iState == EIdleState);
    aStatus = KRequestPending;
    iCallerStatus = &aStatus;
    iState = ESetTrust;
    iWritableCertStore->SetTrust(aCertInfo, aTrusted, iStatus);
    SetActive();
    }

void CCheckedCertStore::CancelSetTrust()
    {
    if (iState == ESetTrust)
        {
        Cancel();
        }        
    }

TInt CCheckedCertStore::RunError(TInt aError)
    {
    Complete(aError);
	return KErrNone;
    }
	
void CCheckedCertStore::DoCancel()
    {
	// (see notes on cancellation in CUnifiedCertStore::DoCancel)

	switch (iState)
		{
        case EGetKeyInfosForList:
        case EAdd:
		case ERemove:
		case ESetApplicability:
		case ESetTrust:
			if (iStatus == KRequestPending)
				{
				// Attempt to cancel outstanding request and pass status back to
				// client
				CancelOutstandingRequest();
				Complete(iStatus.Int());
				}
			else
				{
				// We've already been completed - call RunL() to process results
				// and complete client
				TRAPD(err, RunL());
				if (err != KErrNone)
					{
					RunError(err);
					}
				}
			break;
			
		default:
			CancelOutstandingRequest();
			Complete(KErrCancel);
			break;
		}	
	}

void CCheckedCertStore::CancelOutstandingRequest()
	{
    switch (iState)
        {
        case EList:
            iCertStore.CancelList();
            break;

        case EInitKeyStoreForAdd:
        case EInitKeyStoreForList:
            assert(iUnifiedKeyStore);
            iUnifiedKeyStore->CancelInitialize();
            break;

        case EGetKeyInfosForAdd:
        case EGetKeyInfosForList:
            assert(iUnifiedKeyStore);
            iUnifiedKeyStore->CancelList();
            break;

        case EAdd:
        case EOldAdd:
            assert(iWritableCertStore);
            iWritableCertStore->CancelAdd();
            break;
            
        case ERemove:
		    assert(iWritableCertStore);
   			iWritableCertStore->CancelRemove();
			break;
			
        case ESetApplicability:
		    assert(iWritableCertStore);
   			iWritableCertStore->CancelSetApplicability();
			break;
			
        case ESetTrust:
		    assert(iWritableCertStore);
   			iWritableCertStore->CancelSetTrust();
			break;


        default:
            assert(EFalse);
            break;            
        }

    Complete(KErrCancel);
    }

void CCheckedCertStore::RunL()
    {
    assert(iCallerStatus);
    
	// we allow only KErrNone OR, possibly, KErrNotSupported after new Add()
	// otherwise - Leave!
	if (iStatus!=KErrNone &&
	    !(iStatus==KErrNotSupported && iState==EAdd) &&
	    !(iStatus == KErrNotFound && (iState==EList || iState==EGetKeyInfosForList || iState==EInitKeyStoreForList)))
		{
    	User::Leave(iStatus.Int());
    	}
		
    switch (iState)
        {
        case EList:
            if (iCallerFilter->iKeyUsage == EX509UsageAll)
                {
                // No key usage filter, so we're done
                Complete(KErrNone);
                }
            else
                {
                // Set up key filter according list cert parameters
                if (iCallerFilter->iSubjectKeyIdIsSet)
                    {
                    iKeyFilter.iKeyId = iCallerFilter->iSubjectKeyId;
                    }
                else
                    {
                    iKeyFilter.iKeyId = KNullDesC8;
                    }
                iKeyFilter.iUsage = KeyUsageX509ToPKCS15Private(iCallerFilter->iKeyUsage);
                InitialiseKeyStoreL(EInitKeyStoreForList);
                }
            break;

        case EInitKeyStoreForAdd:
        case EInitKeyStoreForList:
            assert(iUnifiedKeyStore);
            iState = (iState == EInitKeyStoreForAdd) ? EGetKeyInfosForAdd : EGetKeyInfosForList;
            iUnifiedKeyStore->List(iKeyInfos, iKeyFilter, iStatus);			
            SetActive();
            break;

        case EGetKeyInfosForList:
            BuildCheckedCertificateListL();	//	Not async
            Complete(KErrNone);
            break;

        case EGetKeyInfosForAdd:
            // We have a filter list of keys - there should be one with
            // the appropriate subject if we are to add it
            if (iKeyInfos.Count() == 0)
                {
                //	The private key can't be found in any key store
                Complete(KErrPrivateKeyNotFound); 
                }
            else
                {
                //	OK to go ahead and add the key
                assert(iWritableCertStore);
                iState = EAdd;
                
                // try to use new Add(.., TBool aDeletable, ..)
                // if it's not supported it will return with
                // iStatus set to KErrNotSupported
                iWritableCertStore->Add( *iCertLabel,	// call new Add() method
                						 iFormat,
                						 iCertificateOwnerType,
                                         iSubjectKeyId,
                                         iIssuerKeyId,
                                         *iCertificate,
                                         iDeletable,  	// with deletable param
                                         iStatus);
                SetActive();
                }
            break;
                
        case EAdd:
         	if (iStatus!=KErrNotSupported)
         		{
 	     		// Set published property
 		      	iPSCertstoreChangePropertyRef.Set(KUnifiedCertStorePropertyCat, // category
  												EUnifiedCertStoreFlag,        // key
  	    										1);                           // value
         		
		        // when here means MCTWritableCertStore was able to find 
		        // child's new Add(..,aDeletable,..) method
				// thus, ok and complete with whatever result it returned
			    Complete(iStatus.Int());
		        }
		    else
		       	{
				// here: call to the new Add() method above didn't work,
				// try to call old Add() method
			    iState = EOldAdd;
               	iStatus = KRequestPending;
               	iWritableCertStore->Add( *iCertLabel,	
                						 iFormat,
                						 iCertificateOwnerType,
                                         iSubjectKeyId,
                                         iIssuerKeyId,
                                         *iCertificate,
                                         iStatus);
                SetActive();                                         
               	} 
           	break;

        case EOldAdd:
        case ERemove:  
        case ESetApplicability:     
		case ESetTrust:              
      		// Set published property
 	      	iPSCertstoreChangePropertyRef.Set(KUnifiedCertStorePropertyCat, // category
											EUnifiedCertStoreFlag,        // key
 	      									1);                           // value      
            Complete(iStatus.Int());
            break;
            
        default:
            assert(EFalse);
            break;
        }
    }
	
void CCheckedCertStore::InitialiseKeyStoreL(TState aNextState)
    {
    assert(aNextState == EInitKeyStoreForAdd || aNextState == EInitKeyStoreForList);
	assert(!iUnifiedKeyStore);
    
	iUnifiedKeyStore = CUnifiedKeyStore::NewL(iFs);
    iUnifiedKeyStore->Initialize(iStatus);		
    iState = aNextState;
    SetActive();
    }

void CCheckedCertStore::BuildCheckedCertificateListL()
    {
	TInt certCount = iCallerCerts->Count();
    TInt keyCount = iKeyInfos.Count();
    
    // Iterate backwards through cert array so remove doesn't affect indicies
    for (TInt i = certCount - 1 ; i >= 0 ; --i)
        {
		CCTCertInfo* certInfo = (*iCallerCerts)[i];

        // It's problem in the certstore implementation if the list contains NULL pointers
        assert(certInfo); 

        // Check for key with corresponding id
		TBool accept = EFalse;
        for (TInt j = 0 ; j < keyCount ; ++j)
            {
            if (iKeyInfos[j]->ID()==certInfo->SubjectKeyId())
                {
                accept = ETrue;
                break;
                }
            }

        // If we don't have it, remove and release the cert info
        if (!accept)
            {
            iCallerCerts->Remove(i);
            certInfo->Release();
            }
        }
    }

void CCheckedCertStore::Complete(TInt aError)
    {
	if (iCallerStatus)
        {
		User::RequestComplete(iCallerStatus, aError);
        }
    Cleanup();
    }

void CCheckedCertStore::Cleanup()
    {
    //	Reset the state machine	
	iState = EIdleState;
	iKeyInfos.Close();
    iSubjectKeyId = NULL;
    iIssuerKeyId = NULL;

    delete iCertLabel;
    iCertLabel = NULL;

    delete iCertificate;
    iCertificate = NULL;

    delete iUnifiedKeyStore;
    iUnifiedKeyStore = 0;		

    iCallerCerts = NULL;
    iCallerFilter = NULL;
    }
