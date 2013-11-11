/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: An XmlSec interface to the Symbian Unified Key Store.       
*
*/

#include <mctkeystore.h>
#include <asymmetric.h>
#include <asn1dec.h>
#include <asn1enc.h>
#include <x509cert.h>
#include <charconv.h>
#include <utf.h>

#include "xmlsecmsymbiankeystore.h"

// -----------------------------------------------------------------------------
// FindMatchedKey Find the key that matched iKeyLabelToFind from iKeys. 
//				  The key found will be stored in iKey
// -----------------------------------------------------------------------------
//
void CSymbianKeyStore::FindMatchedKey()
    {

	TInt numKey = iKeys.Count();
	
	// Reset iKey
	if (iKey)
	    {
		iKey->Release();
		iKey = NULL;
	    }
	
	// No label to be found
	if (!iKeyLabelToFind)
		return;			

	for (int i=0;i<numKey;i++)
	    {
		CCTKeyInfo* key = (CCTKeyInfo *)iKeys[i];
		if (iKeyLabelToFind->Compare(key->Label()) == 0)
		    {
			iKey = key;
		    }			
	    }
	
    ResetAndDestroyKeysArray();     //iKeys
	}

// -----------------------------------------------------------------------------
// GetRSASignatureL
// Sets iSignature buffer
// -----------------------------------------------------------------------------
//
void CSymbianKeyStore::GetRSASignatureL()
    {
	if (iSignature)
	    {
	    delete iSignature;
	    iSignature = NULL;
	    }
	iSignature = iRSASignature->S().BufferLC();
	CleanupStack::Pop(iSignature);		// BufferLC
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// 
CSymbianKeyStore::CSymbianKeyStore()
:	CActive( EPriorityStandard ),
	iState( EUnitialized ),
	iVerifyResult( EFalse )
    {
    }    

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
// 
void CSymbianKeyStore::ConstructL()
    {
    User::LeaveIfError(iFs.Connect()); 
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
//   
EXPORT_C CSymbianKeyStore* CSymbianKeyStore::NewL()
    {
    CSymbianKeyStore* self = NewLC();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
//   
EXPORT_C CSymbianKeyStore* CSymbianKeyStore::NewLC()
    {
    CSymbianKeyStore* self = new( ELeave ) CSymbianKeyStore;
    
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CSymbianKeyStore::~CSymbianKeyStore()
    {
    Cancel();
    
    ResetAndDestroyKeysArray();     //iKeys
	
	// Free memory
		delete iDataToSign;
		delete iDataToVerify;
		delete iSignature;
		delete iPublicKeyData;
		delete iKeyLabelToFind;
		delete iRSASignature;
    if (iKey)
        {
        iKey->Release();
        }

    if (iRSASigner)
        {
        iRSASigner->Release();
        }	
    
		delete iKeyStore;
    
    iFs.Close();	
    }

// -----------------------------------------------------------------------------
// Release all resources kept in iKeys array (exept for iKey which is released 
// separetly) and empty iKeys array
// -----------------------------------------------------------------------------
//
void CSymbianKeyStore::ResetAndDestroyKeysArray()
    {    
    TInt count = iKeys.Count();
        
    for ( int i=0; i<count; ++i )
	    {
		CCTKeyInfo* key = iKeys[i];
		if ( key != iKey )
		    {
		    key->Release();
		    }
	    }	    
	iKeys.Reset();            
    }

// -----------------------------------------------------------------------------
// PerformRSASignOperation
// Sign data
// -----------------------------------------------------------------------------
//
void CSymbianKeyStore::PerformRSASignOperation()
{
    if (!iKey || !iKeyStore)
        {
    	return;
        }

    iRSASigner->Sign(*iDataToSign, iRSASignature, iStatus);
    iState = EPerformRSASignOperation;
    SetActive();

    // RunL called again when this completes
}

// -----------------------------------------------------------------------------
// ExportRSAPublicKeyL
// Export public key if none present
// -----------------------------------------------------------------------------
//
void CSymbianKeyStore::ExportRSAPublicKeyL()
{
    // iKey is a CCTKeyInfo*
    // iPublicKeyData is an HBufC8*
    __ASSERT_ALWAYS(iKey, User::Leave(KErrGeneral));
    
    if (iPublicKeyData)
        {
    	delete iPublicKeyData;
    	iPublicKeyData = NULL;
        }  

    iKeyStore->ExportPublic(*iKey, iPublicKeyData, iStatus);
    iState = EExportPublic;
    SetActive();
    
    }

// -----------------------------------------------------------------------------
// PerformRSAVerifyOperationL
// Verify an RSA signed data
// -----------------------------------------------------------------------------
//
void CSymbianKeyStore::PerformRSAVerifyOperationL()
    {
    // iRSAPublicKey is a CRSAPublicKey*
    if (!iPublicKeyData)
    {
    return;
    }
   	
    CX509SubjectPublicKeyInfo* ki = 
		CX509SubjectPublicKeyInfo::NewLC(*iPublicKeyData);    

    TAlgorithmId algorithmId = ESHA1;
    CAlgorithmIdentifier* digestId=CAlgorithmIdentifier::NewLC(algorithmId,KNullDesC8());

    TX509KeyFactory factory; 
    CRSAPublicKey *publicKey = factory.RSAPublicKeyL(ki->KeyData());
    CleanupStack::PushL(publicKey);
    
    CRSAPKCS1v15Verifier* verifier = CRSAPKCS1v15Verifier::NewLC(*publicKey);
    
    HBufC8* publicDecryptOutput = verifier->InverseSignLC(*iRSASignature);
    CRSASignatureResult* decoder = factory.RSASignatureResultL(*digestId, *iDataToVerify);
    CleanupStack::PushL(decoder);
	
    TPtr8 outputPtr(publicDecryptOutput->Des());
    iVerifyResult  = decoder->VerifyL(outputPtr);
   
    CleanupStack::PopAndDestroy(decoder);
    CleanupStack::PopAndDestroy(publicDecryptOutput);	
    CleanupStack::PopAndDestroy(verifier);	
    CleanupStack::PopAndDestroy(publicKey);
    CleanupStack::PopAndDestroy(digestId);
    CleanupStack::PopAndDestroy(ki);
    }

// -----------------------------------------------------------------------------
// CSymbianKeyStore::RunL
// Handles an active object's request completion event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSymbianKeyStore::RunL()
    {
    if (iStatus!=KErrNone) 
        {
    	User::Leave(iStatus.Int());
        }

    switch(iState)
        {
    	case EInitializingKeystore:
    		CActiveScheduler::Stop();
    		break;
        case EFindingKeys:
    	    FindMatchedKey();
    	 	CActiveScheduler::Stop();   
    		break;	
        case EImportKey:
    	case ECreateKey:
    		CActiveScheduler::Stop();
    		break;
    	case EOpenRSAKeyForSigning:
    	    PerformRSASignOperation();
    	    break;
        case EPerformRSASignOperation:
        	CActiveScheduler::Stop();
        	break;
        case EExportPublic:
            PerformRSAVerifyOperationL();
            CActiveScheduler::Stop();
            break;

        }

    }
  
// -----------------------------------------------------------------------------
// CSymbianKeyStore::DoCancel
// This function is called as part of the active object's Cancel().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
void CSymbianKeyStore::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CSymbianKeyStore::RunError
// Handles Leaves from RunL function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CSymbianKeyStore::RunError(TInt aError)
	{
	iError=aError;
	CActiveScheduler::Stop();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSymbianKeyStore::CreateUnifiedKeyStoreL
// Create the Unified Key Store structure iKeyStore
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianKeyStore::CreateUnifiedKeyStoreL()
    {

	if (iKeyStore)
	    {
	    delete iKeyStore;    
	    iKeyStore = NULL;
	    }
    iKeyStore = CUnifiedKeyStore::NewL(iFs);
    iKeyStore->Initialize(iStatus);
    iState = EInitializingKeystore;
    SetActive();

    // RunL() called when this completes

    }

// -----------------------------------------------------------------------------
// FindKey
// Lists keys from Unified Key Store
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianKeyStore::FindKey(
	const TDesC8 &aLabel, CKeyInfoBase::EKeyAlgorithm aAlgo)
    {
    // KApplicationUID is the UID of the key owner application
    // iKeys is an RMPointerArray<CCTKeyInfo> that is filled with the keys found

    TCTKeyAttributeFilter filter;
    filter.iUsage = EPKCS15UsageSign;
    if (aAlgo!=CCTKeyInfo::EInvalidAlgorithm)
    	filter.iKeyAlgorithm = aAlgo;		
    
    // Store aLabel
    if (iKeyLabelToFind)
        {
    	delete iKeyLabelToFind;
    	iKeyLabelToFind = NULL;
        }
    TRAPD(err,iKeyLabelToFind = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aLabel));
    if (err != KErrNone)
        {
        iError=err;
        return;
        }

    iKeyStore->List(iKeys, filter, iStatus);
    iState = EFindingKeys;
    SetActive();
    // RunL() called when this completes
    }

// -----------------------------------------------------------------------------
// hasKey
// Check if a key is found in the Unified Key Store
// Returns: KErrNone if the key is found
//                  KErrNotFound if the key is not found
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSymbianKeyStore::hasKey()
    {
	if (iKey)
		return KErrNone;
	else
		return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// GetKeySize
// Get the size of the key stored
// Returns: Size of the key
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CSymbianKeyStore::GetKeySize()
    {
	// assert iKey
	return iKey->Size();
    }
    
// -----------------------------------------------------------------------------
// GetKeyAlgorithm
// Get the algorithm of the key stored
// Returns: CCTKeyInfo::EKeyAlgorithm
// -----------------------------------------------------------------------------
//
EXPORT_C CCTKeyInfo::EKeyAlgorithm CSymbianKeyStore::GetKeyAlgorithm()
    {
	// assert iKey
	return iKey->Algorithm();
    }

// -----------------------------------------------------------------------------
// CreateRSAKey
// Creates RSA key and adds it to Unified Key Store
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianKeyStore::CreateRSAKey(
    const TUint aSize, const TDesC8 &aKeyName)
    {
    HBufC16* unicodeKeyName=NULL;
    // iKey is a CCTKeyInfo* that will be set if the function succeeds
    if (iKey)
        {
		//delete iKey;
		iKey->Release();
		iKey = NULL;
        }      
    // Convert key name from TDesC8 to TDesC16
    TRAPD(err,unicodeKeyName=CnvUtfConverter::ConvertToUnicodeFromUtf8L(aKeyName));
    if (err != KErrNone)
        {
        iError=err;
        return;
        }
    
    // Find the number of file key stores present
    TInt num = iKeyStore->KeyStoreManagerCount();    
    TBool found = EFalse;
 	TInt keyStoreIndex = 0;  
 	TInt index;

	// Find the Symbian file key store index 
	for (index = 0;index < num;index++)
	{
 		MCTKeyStoreManager& manager = iKeyStore->KeyStoreManager(index);
        MCTToken& token = manager.Token();
		TUid tokenuid = token.Handle().iTokenTypeUid;

   		if ( tokenuid == TUid::Uid(KTokenTypeFileKeystore) ) // Symbian's file key store, defined in mctkeystore.h
		{
   			found = ETrue;
			break;
   	    }
	}	

	if ( found )
	{
		// If found, then store in the place pointed by the index else it shall take the first key store
		keyStoreIndex = index;
	}
	    
    TTime startDate, endDate;
    startDate.UniversalTime();
    endDate.UniversalTime();
    endDate += TTimeIntervalYears(1); // key valid for a year

    iKeyStore->CreateKey(
                    keyStoreIndex,
                    EPKCS15UsageSign,
                    aSize,
                    *unicodeKeyName,
                    CCTKeyInfo::ERSA,
                    CCTKeyInfo::EExtractable,
                    startDate,
                    endDate,
                    iKey,
                    iStatus);
    delete unicodeKeyName;
    iState = ECreateKey;
    SetActive();

    // RunL() called when this completes
    }

// -----------------------------------------------------------------------------
// ImportKey
// Import Key from Unified Key Store
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianKeyStore::ImportKey(
    const TDesC8 &aKeyData, 
    const TDesC8 &aKeyName)		
    {
    HBufC16* unicodeKeyName=NULL;	
    // iKey is a CCTKeyInfo* that will be set if the function succeeds
    if (iKey)
        { 
		//delete iKey;
		iKey->Release();
		iKey = NULL;
        }
    
    // Convert key name from TDesC8 to TDesC16
    TRAPD(err,unicodeKeyName=CnvUtfConverter::ConvertToUnicodeFromUtf8L(aKeyName));
    if (err != KErrNone)
        {
        iError=err;
        return;
        }	
    
    // Find the number of file key stores present
    TInt num = iKeyStore->KeyStoreManagerCount();    
    TBool found = EFalse;
 	TInt keyStoreIndex = 0;  
 	TInt index;

	// Find the Symbian file key store index 
	for (index = 0;index < num;index++)
	{
 		MCTKeyStoreManager& manager = iKeyStore->KeyStoreManager(index);
        MCTToken& token = manager.Token();
		TUid tokenuid = token.Handle().iTokenTypeUid;

   		if ( tokenuid == TUid::Uid(KTokenTypeFileKeystore) ) // Symbian's file key store, defined in mctkeystore.h
		{
   			found = ETrue;
			break;
   	    }
	}	

	if ( found )
	{
		// If found, then store in the place pointed by the index else it shall take the first key store
		keyStoreIndex = index;
	}
	
	    
    TTime startDate, endDate;
    startDate.UniversalTime();
    endDate.UniversalTime();
    endDate += TTimeIntervalYears(1); // key valid for a year

    iKeyStore->ImportKey(
                    keyStoreIndex,
                    aKeyData,
                    EPKCS15UsageSign,                   
                    *unicodeKeyName,                    
                    CCTKeyInfo::EExtractable,
                    startDate,
                    endDate,
                    iKey,
                    iStatus);
    iState = EImportKey;
    delete unicodeKeyName;
    SetActive();

    // RunL() called when this completes
    }
    
// -----------------------------------------------------------------------------
// RSASignL
// Opens RSA key for signing the data 
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianKeyStore::RSASignL(
	const TUint8* aDataToSign, TUint aLen)
	{
    // iRSASigner is an MRSASigner* object returned from Open()
    // iDataToSign is a HBufC8* containing data to be signed
    // iRSASignature is a CRSASignature* which will contain the result
    __ASSERT_ALWAYS(iKey, User::Leave(KErrGeneral));
    
    if (iDataToSign)
        {
    	delete iDataToSign;
    	iDataToSign = NULL;
        }
    
    if (iRSASigner)
        {
    	iRSASigner->Release();		
    	iRSASigner = NULL;
        }
    
    if (iRSASignature)
        {
    	delete iRSASignature;
    	iRSASignature = NULL;
        }
    
	TPtrC8 dataPtr(aDataToSign, aLen);
    // Build ASN1 encoding of digestAlgId and digest..
	CASN1EncSequence* encAll = CASN1EncSequence::NewLC();

	// Build AlgID encoder (for SHA1)
	CASN1EncSequence* encAlgId = CASN1EncSequence::NewLC();

	CASN1EncObjectIdentifier* encObjId = CASN1EncObjectIdentifier::NewLC(KSHA1);
	encAlgId->AddChildL(encObjId);
	CleanupStack::Pop(encObjId); // encObjId, now owned by endAlgId

	CASN1EncNull* encNull = CASN1EncNull::NewLC();
	encAlgId->AddChildL(encNull);
	CleanupStack::Pop(encNull); // encNull, now owned by endAlgId

	encAll->AddChildL(encAlgId);
	CleanupStack::Pop(encAlgId); // encAlgId, now owned by encAll

	CASN1EncOctetString* encDigest = CASN1EncOctetString::NewLC(dataPtr);
	encAll->AddChildL(encDigest);
	CleanupStack::Pop(encDigest); // encDigest, now owned by encAll

	iDataToSign = HBufC8::NewMaxL(encAll->LengthDER());
	TUint pos = 0;
	TPtr8 digestInfoPtr = iDataToSign->Des();
	encAll->WriteDERL(digestInfoPtr, pos);
    CleanupStack::PopAndDestroy(encAll);
        
    iKeyStore->Open(*iKey, iRSASigner, iStatus);	
    iState = EOpenRSAKeyForSigning;
    SetActive();
    
	}

// -----------------------------------------------------------------------------
// RSAVerifyL
// Verify an RSA signature with a self-created private key in Unified Key Store
// -----------------------------------------------------------------------------
//

EXPORT_C void CSymbianKeyStore::RSAVerifyL(
	const TUint8* aDataToVerify, 			// Data to be verified with the signature
	TUint aDataLen, 						// Length of the data to be verified
	const TUint8* aSig, 					// A reference to the signature that signed the data
	TUint aSigLen)							// Length of the signature
	{
	iOutOfMemoryFlag = EFalse;
    if (iDataToVerify)
        {
    	delete iDataToVerify;
    	iDataToVerify = NULL;
        }	
    
    // Store the data
	
	TPtrC8 ptr(aDataToVerify, aDataLen);
	iDataToVerify = ptr.AllocL();
	// Store the signature
	ptr.Set(aSig, aSigLen);
	RInteger sigInt = RInteger::NewL(ptr);
	CleanupClosePushL(sigInt);
	if (iRSASignature)
	    {
	    delete iRSASignature;
	    iRSASignature = NULL;
	    }
	iRSASignature = CRSASignature::NewL(sigInt);
	CleanupStack::Pop(&sigInt);

    // Export public key if none present
    // iPublicKeyData is an HBufC8*
    ExportRSAPublicKeyL();
    
	}
	
// -----------------------------------------------------------------------------
// RSAVerifyWithPublicKeyL
// Verify an RSA signed data with a public key passed from a certificate
// Returns: ETrue The verification is succeeded
//                  EFalse The verification is failed
// -----------------------------------------------------------------------------
//

EXPORT_C TBool CSymbianKeyStore::RSAVerifyWithPublicKeyL(
	const TUint8* aDataToVerify, 			// Signed data to be verified 
	TUint aDataLen, 						// Length of the signed data
	const TUint8* aSig, 					// A reference to the signature that signed the data
	TUint aSigLen,							// Length of the signature
	CSubjectPublicKeyInfo *aSubPubKeyInfo)	// A handle to the public key passed from a certificate
	{
	iOutOfMemoryFlag = EFalse;
    if (iDataToVerify)
        {
    	delete iDataToVerify;
    	iDataToVerify = NULL;
        }	
    
    // Store the data
	TPtrC8 ptr(aDataToVerify, aDataLen);
	iDataToVerify = ptr.AllocL();
	
	// Store the signature
	ptr.Set(aSig, aSigLen);
	RInteger sigInt = RInteger::NewL(ptr);
	CleanupClosePushL(sigInt);
	if (iRSASignature)
	    {
	    delete iRSASignature;
	    iRSASignature = NULL;
	    }
	iRSASignature = CRSASignature::NewL(sigInt);
	CleanupStack::Pop(&sigInt);

    TAlgorithmId algorithmId = ESHA1;
    CAlgorithmIdentifier* digestId=CAlgorithmIdentifier::NewLC(algorithmId,KNullDesC8());
    TX509KeyFactory factory; 
    CRSAPublicKey *publicKey = factory.RSAPublicKeyL(aSubPubKeyInfo->KeyData());
    CleanupStack::PushL(publicKey);
    
	CRSAPKCS1v15Verifier* verifier = CRSAPKCS1v15Verifier::NewLC(*publicKey);
    HBufC8* publicDecryptOutput = verifier->InverseSignLC(*iRSASignature);
    CRSASignatureResult* decoder = factory.RSASignatureResultL(*digestId, *iDataToVerify);
    CleanupStack::PushL(decoder);	
    TPtr8 outputPtr(publicDecryptOutput->Des());
    iVerifyResult  = decoder->VerifyL(outputPtr);
       
    CleanupStack::PopAndDestroy(decoder);
    CleanupStack::PopAndDestroy(publicDecryptOutput);		
    CleanupStack::PopAndDestroy(verifier);
    CleanupStack::PopAndDestroy(publicKey);
    CleanupStack::PopAndDestroy(digestId);

	return iVerifyResult;
	}
	
// -----------------------------------------------------------------------------
// GetSignedData
// Get signed data
// Returns: length of signed data
// -----------------------------------------------------------------------------
//
EXPORT_C const TUint8* CSymbianKeyStore::GetSignedData(TUint *aLen)
    {
	TInt leaveValue;
	
	if (iRSASignature)
	    {
	    if (iSignature)
		    {
			delete iSignature;
			iSignature = NULL;
		    }

		TRAP(leaveValue, GetRSASignatureL()) ;
		if ( leaveValue != KErrNone ) 
		    {
		    iError = leaveValue;
		    }
		if (iSignature)	
		    {
			*aLen = iSignature->Length();	
			return (iSignature->Ptr());		
		    }
        }

	// in case of errors
	*aLen = 0;
	return NULL;
	}
	
// -----------------------------------------------------------------------------
// GetVerifyResult
// Returns verification result
// Returns: ETrue The verification is succeeded
//                  EFalse The verification is failed
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSymbianKeyStore::GetVerifyResult()
    {
	return iVerifyResult;
    }
    
// -----------------------------------------------------------------------------
// GetError
// Get the error flag
// Returns: error code
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSymbianKeyStore::GetError()
    {
    return iError;
    }    
