/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "wtlscertchainao.h"
#include <asymmetric.h>
#include <bigint.h>
#include <ccertattributefilter.h>
#include <cctcertinfo.h>

CWTLSCertChainAO* CWTLSCertChainAO::NewL(RFs& aFs, 
										 CWTLSCertChain& aWTLSCertChain,
										 const CArrayPtr<CWTLSCertificate>& aRootCerts)
	{
	CWTLSCertChainAO* self = new(ELeave) CWTLSCertChainAO(aFs, aWTLSCertChain);
	CleanupStack::PushL(self);
	self->ConstructL(aRootCerts);
	CleanupStack::Pop(self);
	return self;
	}

CWTLSCertChainAO* CWTLSCertChainAO::NewL(RFs& aFs, 
										 CWTLSCertChain& aWTLSCertChain,
										 const TUid aClient)
	{
	return new(ELeave) CWTLSCertChainAO(aFs, aWTLSCertChain, aClient);
	}

CWTLSCertChainAO::~CWTLSCertChainAO()
	{
	Cancel();
	delete iCertStoreManager;
	delete iFilter;
	delete iEncodedCertTemp;
	iRootSubjectClientHashList.ResetAndDestroy();
	iRootSubjectStoreHashList.Close();
	iCertInfos.Close(); //In an RMPointerArray Close deletes all elements as 
						//well as any personal allocated space
	iRootsFromStore.ResetAndDestroy();
	iRootsFromStore.Close();
	iRootsFromClient.ResetAndDestroy();
	}

CWTLSCertChainAO::CWTLSCertChainAO(RFs& aFs, 
								   CWTLSCertChain& aWTLSCertChain)
	: CActive(EPriorityNormal), iFs(aFs), iWTLSCertChain(aWTLSCertChain), iEncodedCert(NULL, 0)
	{
	CActiveScheduler::Add(this);
	}

CWTLSCertChainAO::CWTLSCertChainAO(RFs& aFs, 
								   CWTLSCertChain& aWTLSCertChain,
								   const TUid aClient)
	: CActive(0), iFs(aFs), iWTLSCertChain(aWTLSCertChain), iClient(aClient),
		iEncodedCert(NULL, 0)
	{
	CActiveScheduler::Add(this);
	}

void CWTLSCertChainAO::ConstructL(const CArrayPtr<CWTLSCertificate>& aRootCerts)
	{
	for(TInt i=0; i< aRootCerts.Count(); i++)
		{
		CWTLSCertificate* root = CWTLSCertificate::NewLC(*(aRootCerts[i]));
		User::LeaveIfError( iRootsFromClient.Append(root) );
		CleanupStack::Pop(); //root
		}
	}

void CWTLSCertChainAO::RunL()
	{
	//If any of my active objects complete with errors then we don't
	//want to proceed
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EStoreManagerInitialization:
			HandleEStoreManagerInitializationL();
			break;

		case EStoreManagerInitialized:
			HandleEStoreManagerInitializedL();
			break;

		case EGetCertHashes:
			HandleEGetCertHashesL();
			break;

		case EPruneList:
			HandleEPruneListL();
			break;

		case EPruneListDone:
			HandleEPruneListDoneL();
			break;

		case ECheckTCA:
			HandleECheckTCAL();
			break;

		case EIsChainSelfSigned:
			HandleEIsChainSelfSignedL();
			break;

		case ERetrieveRoots:
			HandleERetrieveRootsL();
			break;

		case EAddRootToList:
			HandleEAddRootToListL();
			break;

		case EFindRoot:
			HandleEFindRootL();
			break;

		case EValidateEnd:
			HandleEValidateEndL();
			break;
		
		default:
			__ASSERT_DEBUG(EFalse, User::Panic(_L("CWTLSCertChainAO"), 1));
			User::Leave(KErrArgument);
			break;
		}
	}

TInt CWTLSCertChainAO::RunError(TInt aError)
	{
	User::RequestComplete(iOriginalRequestStatus, aError);

	delete iCertStoreManager;
	iCertStoreManager = 0;

	return 0;
	}

void CWTLSCertChainAO::DoCancel()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrCancel);
	if (iOriginalRequestStatus)
		{
		User::RequestComplete(iOriginalRequestStatus, KErrCancel);
		}
	}

void CWTLSCertChainAO::Validate(CWTLSValidationResult& aValidationResult,
								const TTime& aValidationTime,								 
								TRequestStatus& aStatus)
	{
	iValidationResult = &aValidationResult;
	iValidationResult->Reset();
	iValidationTime = &aValidationTime;
	iOriginalRequestStatus = &aStatus;
	aStatus = KRequestPending;

	__ASSERT_DEBUG(!IsActive(), User::Panic(_L("CWTLSCertChainAO"), 1));
	__ASSERT_DEBUG(!iCertStoreManager, User::Panic(_L("CWTLSCertChainAO"), 1));
	
	iState = EStoreManagerInitialization;
	TRequestStatus *status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

TBool CWTLSCertChainAO::CheckSignatureAndNameL(const CWTLSCertificate& aCert, 
											   CWTLSValidationResult& aResult,
											   TInt aPos) const
	{
	TInt issuerPos = aPos + 1;
	TBool res = EFalse;
	if (issuerPos == iWTLSCertChain.iChain->Count())
		//then it's the root
		{
		if (aCert.IssuerName().ExactMatchL(aCert.SubjectName()))
			//then it claims to be self signed, sig must verify
			{
			if (aCert.VerifySignatureL(aCert.PublicKey().KeyData()))
				{
				res = ETrue;
				}
			else 
				{
				aResult.SetError(ESignatureInvalid, aPos);
				}
			}
		else
			{
			aResult.AppendWarningL(TWTLSValidationStatus(ERootCertNotSelfSigned, aPos));
			res = ETrue; //if its a warning we continue the validation process with the 
						//warning duly noted so we can check for further warn/errors
			}
		}
	else
		//then it isn't the root: so names must chain & sigs must verify
		{
		const CWTLSCertificate* issuer = iWTLSCertChain.iChain->At(issuerPos);
		TBool subject = EFalse;
		TBool signature = EFalse;
		subject = aCert.IssuerName().ExactMatchL(issuer->SubjectName());
		if( !subject ) 
			{
			aResult.SetError(ENamesDontChain, aPos);
			return EFalse;
			}
		signature = aCert.VerifySignatureL(issuer->PublicKey().KeyData());
		if( !signature )
			{
			aResult.SetError(ESignatureInvalid, aPos);
			return EFalse;
			}
		res = subject && signature;
		}
	return res;
	}

TBool CWTLSCertChainAO::CheckValidityPeriod(const CWTLSCertificate& aCert,
											CWTLSValidationResult& aResult, 
											const TTime aTime,
											TInt aPos) const
	{
	if (aCert.ValidityPeriod().Valid(aTime))
		{
		return ETrue;
		}
	aResult.SetError(EDateOutOfRange, aPos);
	return EFalse;
	}

void CWTLSCertChainAO::HandleEStoreManagerInitializationL()
	{
	iFilter = CCertAttributeFilter::NewL();
	iFilter->SetFormat(EWTLSCertificate);
	iFilter->SetUid(iClient);
	iFilter->SetOwnerType(ECACertificate);

	iCertStoreManager = CUnifiedCertStore::NewL(iFs, EFalse);
	iCertStoreManager->Initialize(iStatus);

	iState = EStoreManagerInitialized;
	SetActive();
	}

void CWTLSCertChainAO::HandleEStoreManagerInitializedL()
	{
	iCertStoreManager->List(iCertInfos, *iFilter, iStatus);

	iState = EGetCertHashes;
	SetActive();
	}

void CWTLSCertChainAO::HandleEGetCertHashesL()
	{
	for(TInt i=0; i<iRootsFromClient.Count(); i++)
		{
		HBufC8* hash = &GeneratePublicKeyHashL( *(iRootsFromClient[i]));
		CleanupStack::PushL(hash);
		User::LeaveIfError( iRootSubjectClientHashList.Append(hash) );
		CleanupStack::Pop(); //hash
		}
	for(TInt j=0; j < iCertInfos.Count(); j++ )
		{
		User::LeaveIfError( iRootSubjectStoreHashList.Append( &((iCertInfos[j])->SubjectKeyId()) ) );
		}

	iPruned = EFalse;	
	iPrunedChainLength = iWTLSCertChain.iChain->Count();
	iIndex = -1;

	iState = EPruneList;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

/* Walk through the canadiate list and compare the hash of the subjects with the previously
 * computed subject hash of certs from the CertStore and certs supplied by the client
 */
void CWTLSCertChainAO::HandleEPruneListL()
	{
	iIndex++;
	if(iIndex < iWTLSCertChain.iChain->Count() )
		{
		CWTLSCertificate* cert = iWTLSCertChain.iChain->At(iIndex);
		HBufC8* hash = &GeneratePublicKeyHashL(*cert);
		CleanupStack::PushL(hash);

		for(TInt i=0; i < iRootSubjectClientHashList.Count(); i++)
			{
			if( (iRootSubjectClientHashList[i])->Compare(*hash) == 0 )
				{
				iPrunedChainLength = iIndex;
				iPruned = ETrue;
				break;
				}
			}
		if(!iPruned)
			{
			for(TInt j=0; j<iRootSubjectStoreHashList.Count(); j++) 
				{
				if( (iRootSubjectStoreHashList[j])->Compare(*hash) == 0 )
					{
					iPrunedChainLength = iIndex;
					iPruned = ETrue;
					break;
					}
				}
			}
		CleanupStack::PopAndDestroy(hash);
		if(iPruned)
			{
			iState = EPruneListDone;
			}
		else 
			{
			iState = EPruneList;
			}
		}
	else
		{
		iState = EPruneListDone;
		}
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CWTLSCertChainAO::HandleEPruneListDoneL()
	{
	if(iPruned) 
		{
		TInt count = iWTLSCertChain.iChain->Count();
		for( TInt i=count - 1; i > iPrunedChainLength; i-- )
			{
			delete iWTLSCertChain.iChain->At(i);
			iWTLSCertChain.iChain->Delete(i);
			}
		iWTLSCertChain.iChain->Compress();
		}
	iState = ECheckTCA;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

//checks to see if each certificate in a chain has the authority to sign other certificates
void CWTLSCertChainAO::HandleECheckTCAL()
	{
	TBool validChain = ETrue;
	for( TInt i = 1; i < iWTLSCertChain.iChain->Count(); i++ ) 
		//all intermediate certs (ie not EE certs and not self signed) need
		// to have a field T=ca indicating that they can sign other certs
		{
		if( (iWTLSCertChain.iChain)->At(i)->IsTCAL() == EFalse && 
			(iWTLSCertChain.iChain)->At(i)->IsSelfSignedL() == EFalse ) 
			{
			iValidationResult->SetError(ENotCACert, i);
			User::RequestComplete(iOriginalRequestStatus, KErrNone);
			validChain = EFalse;
			break;
			}
		}
	if(validChain && iPruned) 
		{
		//if we've pruned the list and the chain we have is valid,
		//then our chain already has a root that we trust.
		//therefore there is no need to retrieve one :)
		//therefore goto validation
		iState = EValidateEnd;
		TRequestStatus *status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	else if(validChain) // ie && !iPruned
		{
		//if we haven't pruned but chain is valid then we're back a square one.
		iState = EIsChainSelfSigned;
		TRequestStatus *status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	}

void CWTLSCertChainAO::HandleEIsChainSelfSignedL()
	{

	TInt last = iWTLSCertChain.iChain->Count() - 1;
	if( iWTLSCertChain.iChain->At(last)->IsSelfSignedL() )
		{
		//if chained is self signed, and no earlier cert in the sequence was trusted
		//then this is going to fail validation
		//This is just an optimisation to avoid retrieving all the roots from the store

		iValidationResult->SetError(EChainHasNoRoot, last);
		User::RequestComplete(iOriginalRequestStatus, KErrNone);
		}
	else 
		{
		//standard chain -> need to find the appropriate trusted root for chain if it exists
		iState = ERetrieveRoots;
		iIndex = -1;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	}

void CWTLSCertChainAO::HandleERetrieveRootsL()
	{
	iIndex++;	
	if(iIndex < iCertInfos.Count() )
		{
		if( iEncodedCertTemp != NULL ) 
			{
			delete iEncodedCertTemp;
			}
		iEncodedCertTemp = HBufC8::NewMaxL( (iCertInfos[iIndex])->Size() );
		iEncodedCert.Set( iEncodedCertTemp->Des() );
		iCertStoreManager->Retrieve( *(iCertInfos[iIndex]), iEncodedCert, iStatus );
		iState = EAddRootToList;
		}
	else 
		{
		iState = EFindRoot;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}	
	SetActive();
	}

void CWTLSCertChainAO::HandleEAddRootToListL()
	{
	//are we guarenteed that a cert from the store is a valid WTLScert?
	//ie is this going to leave for reasons other than OOM?
	CWTLSCertificate *cert = CWTLSCertificate::NewL( iEncodedCert );
	User::LeaveIfError( iRootsFromStore.Append(cert) );

	iState = ERetrieveRoots;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CWTLSCertChainAO::HandleEFindRootL()
	{
	TInt last = iWTLSCertChain.iChain->Count() - 1;
	const CWTLSName* issuerName = &(iWTLSCertChain.iChain->At(last)->IssuerName());

	iFoundRoot = EFalse;
	for(TInt i=0; i<iRootsFromClient.Count(); i++) 
		{
		if( issuerName->ExactMatchL( (iRootsFromClient[i])->SubjectName() ) )
			{
			iFoundRoot = ETrue;
			CWTLSCertificate* cert = CWTLSCertificate::NewLC( *(iRootsFromClient[i]) );
			iWTLSCertChain.iChain->AppendL( cert ); 
			CleanupStack::Pop(cert);
			break;
			}
		}
	if(!iFoundRoot) 
		{
		for(TInt j=0; j<iRootsFromStore.Count(); j++)
			{
			if( issuerName->ExactMatchL( (iRootsFromStore[j])->SubjectName() ) )
				{
				iFoundRoot = ETrue;
				CWTLSCertificate* cert = CWTLSCertificate::NewLC( *(iRootsFromStore[j]) );
				iWTLSCertChain.iChain->AppendL( cert );
				CleanupStack::Pop(cert);
				break;
				}
			}
		}
	if(!iFoundRoot)
		{
		iValidationResult->SetError(EChainHasNoRoot, last);
		User::RequestComplete(iOriginalRequestStatus, KErrNone);
		}
	else
		{
		iState = EValidateEnd;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	}

void CWTLSCertChainAO::HandleEValidateEndL()
	{
	TInt i = iWTLSCertChain.iChain->Count() -1;//we can guarantee that chain has at least 1 cert
	for (; i >= 0; i--)
		{
		const CWTLSCertificate* current = iWTLSCertChain.iChain->At(i);
		if ((!CheckSignatureAndNameL(*current, *iValidationResult, i))	||
			(!CheckValidityPeriod(*current, *iValidationResult, *iValidationTime, i)))
			{
			//these functions set the error internally if there is one
			break;
			}
		}		

	User::RequestComplete(iOriginalRequestStatus, KErrNone);
	}

HBufC8& CWTLSCertChainAO::GeneratePublicKeyHashL(const CWTLSCertificate& aCert) const
	{
	TWTLSKeyFactory keyFactory;
	CRSAPublicKey* key = keyFactory.RSAPublicKeyL( aCert.PublicKey().KeyData() );
	CleanupStack::PushL(key);
	HBufC8* modulusBuffer = key->N().BufferLC();
	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);
	TPtrC8 hash = sha1->Final(*modulusBuffer);
	HBufC8* permHash = hash.AllocL();
	CleanupStack::PopAndDestroy(3); //sha1, modulusBuffer, key
	return *permHash;
	}

