/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pkcs12bags.h"

using namespace PKCS12;
///////////////////////// KeyBag ////////////////////////////////	

CDecPkcs12KeyBag::CDecPkcs12KeyBag()
	{
	}
	
EXPORT_C CDecPkcs12KeyBag* CDecPkcs12KeyBag::NewL(const TDesC8& aSafeBagData)
	{
	CDecPkcs12KeyBag* self = new(ELeave) CDecPkcs12KeyBag;
	CleanupStack::PushL(self);
	self->ConstructL(aSafeBagData);
	CleanupStack::Pop(self);
	return self;
	}

CDecPkcs12KeyBag::~CDecPkcs12KeyBag()
	{		
	}
	
void CDecPkcs12KeyBag::ConstructL(const TDesC8& aSafeBagData)
	{
	CDecPkcs12SafeBag::ConstructL(aSafeBagData);	
	}
	
EXPORT_C CDecPKCS8Data* CDecPkcs12KeyBag::PrivateKeyInfoL() const
	{
	TASN1DecGeneric seqGen(iBagValue);
	seqGen.InitL();
	if (seqGen.Tag() != EASN1Sequence || seqGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
			
	return TASN1DecPKCS8::DecodeDERL(seqGen.Encoding());
	}
		
////////////////////////// ShroudedKeyBag ////////////////////////	
CDecPkcs12ShroudedKeyBag::CDecPkcs12ShroudedKeyBag()
	{
	}

EXPORT_C CDecPkcs12ShroudedKeyBag* CDecPkcs12ShroudedKeyBag::NewL(const TDesC8& aSafeBagData)
	{
	CDecPkcs12ShroudedKeyBag* self = new(ELeave) CDecPkcs12ShroudedKeyBag;
	CleanupStack::PushL(self);
	self->ConstructL(aSafeBagData);
	CleanupStack::Pop(self);
	return self;
	}

CDecPkcs12ShroudedKeyBag::~CDecPkcs12ShroudedKeyBag()
	{
	}
	
void CDecPkcs12ShroudedKeyBag::ConstructL(const TDesC8& aSafeBagData)
	{
	CDecPkcs12SafeBag::ConstructL(aSafeBagData);
	TASN1DecGeneric seqGen(iBagValue);
	seqGen.InitL();
	if (seqGen.Tag() != EASN1Sequence || seqGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecSequence dec;
	CArrayPtrFlat<TASN1DecGeneric>* shroudedKeyBagSeq = dec.DecodeDERLC(seqGen);
	TInt seqIndex = 0;
	TInt shroudedKeyBagSeqCount = shroudedKeyBagSeq->Count();
	if (seqIndex >= shroudedKeyBagSeqCount)
		{
		User::Leave(KErrArgument);		
		}
	CleanupStack::PopAndDestroy(shroudedKeyBagSeq);
	}

EXPORT_C CDecPKCS8Data* CDecPkcs12ShroudedKeyBag::PrivateKeyInfoL(TDesC& aPassword) const
	{
	TASN1DecGeneric seqGen(iBagValue);
	seqGen.InitL();
	TASN1DecSequence dec;
	CArrayPtrFlat<TASN1DecGeneric>* shroudedKeyBagSeq = dec.DecodeDERLC(seqGen);
	// Get the first part of the sequence -> PKCS5 data
	const TASN1DecGeneric* shroudedKeyBagSeqAt0 = shroudedKeyBagSeq->At(0);
	TPtrC8 theContent(shroudedKeyBagSeqAt0->Encoding());//	expect this to be a sequence
	CPBEncryptParms* encryptParams =  TASN1DecPKCS5::DecodeDERL(theContent);
	CleanupStack::PushL(encryptParams);
	
	CPBEncryptElement* encryptElement;
	HBufC8* pkcs12Pwd = PKCS12KDF::GeneratePasswordLC(aPassword);
	if(encryptParams->Kdf() == CPBEncryptParms::EKdfPkcs12)
		{
	 	TPtrC8 iv = encryptParams->IV();
	    HBufC8* ivValue = HBufC8::NewMaxLC(iv.Length());
	    TPtr8 encryptKeyBuf = ivValue->Des();
	 		 
	 	switch(encryptParams->Cipher())	
	 		{
	 		case ECipherARC4_128:
	 		case ECipherARC4_40:
				{
	 		    //derive only key it is unnecessary to derive an IV for RC4
				break;
				}
			case ECipher3DES_CBC:
			case ECipher2Key3DES_CBC:
			case ECipherRC2_CBC_128_16:	 
			case ECipherRC2_CBC_40_5:	
				{
			    PKCS12KDF::DeriveKeyL(encryptKeyBuf, PKCS12KDF::EIDByteIV, *pkcs12Pwd, encryptParams->Salt(), encryptParams->Iterations());
				encryptParams->SetIV(encryptKeyBuf);
			    break;
				}
			default:
				{
			    User::Leave(KErrNotSupported);
			    break;
				}
	 		}
	 	CleanupStack::PopAndDestroy(ivValue);
		// Create the decryptor	
		encryptElement = CPBEncryptElement::NewLC(*pkcs12Pwd, *encryptParams);
		}
	else
		{
		TPBPassword password(aPassword);
		// Create the decryptor	
		encryptElement = CPBEncryptElement::NewLC(password.Password(), *encryptParams);	
		}
	CPBDecryptor* decryptor = encryptElement->NewDecryptLC();
	// Decrypt the final part of the sequence -> encrypted PKCS8 object
	const TASN1DecGeneric* shroudedKeyBagSeqAt1 = shroudedKeyBagSeq->At(1);
	if (shroudedKeyBagSeqAt1->Tag() != EASN1OctetString || shroudedKeyBagSeqAt1->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	TPtrC8 encryptedKey(shroudedKeyBagSeqAt1->GetContentDER());
	TUint encryptLength = encryptedKey.Length();
	TUint maxDecryptLength = decryptor->MaxOutputLength(encryptLength);
	if ( maxDecryptLength <= 0 )
		{
		User::Leave(KErrGeneral);		
		}
	HBufC8* decryptedContent = HBufC8::NewLC(encryptLength);
	TPtr8 dcDes(decryptedContent->Des());
	decryptor->Process(encryptedKey, dcDes);
	
	CDecPKCS8Data* privateKeyInfo = TASN1DecPKCS8::DecodeDERL(dcDes);	
		
	CleanupStack::PopAndDestroy(6,shroudedKeyBagSeq);//shroudedKeyBagSeq, encryptParams,pkcs12Pwd
													// encryptElement, decryptor, decryptedContent.
	return privateKeyInfo;
	}
	
///////////////////////////// CertBag ///////////////////////////////

CDecPkcs12CertBag::CDecPkcs12CertBag()
	{
	}
	
EXPORT_C CDecPkcs12CertBag* CDecPkcs12CertBag::NewL(const TDesC8& aSafeBagData) 
    {
    CDecPkcs12CertBag* self = new (ELeave) CDecPkcs12CertBag;
	CleanupStack::PushL(self);
	self->ConstructL(aSafeBagData);
	CleanupStack::Pop(self);
	return self;
	}

CDecPkcs12CertBag::~CDecPkcs12CertBag()
	{	
	delete iCertId;
	}

void CDecPkcs12CertBag::ConstructL(const TDesC8& aSafeBagData)
	{
	CDecPkcs12SafeBag::ConstructL(aSafeBagData);
	
	TASN1DecGeneric seqGen(iBagValue);
	seqGen.InitL();
		if (seqGen.Tag() != EASN1Sequence || seqGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	
    TASN1DecSequence seq;
	CArrayPtr<TASN1DecGeneric>* certBagSequence = seq.DecodeDERLC(seqGen);
  	const TASN1DecGeneric* certBagSequenceAt0 = certBagSequence->At(0);
  	if (certBagSequenceAt0->Tag() != EASN1ObjectIdentifier || certBagSequenceAt0->Class() != EUniversal)
  		{
		User::Leave(KErrArgument);
		}
	  			
  	// Obtain the CertId		
  	TASN1DecObjectIdentifier oid;
  	iCertId = oid.DecodeDERL(*certBagSequenceAt0);

	const TASN1DecGeneric* certBagSequenceAt1 = certBagSequence->At(1);
	if (certBagSequenceAt1->Tag() != EASN1EOC || certBagSequenceAt1->Class() != EContextSpecific)
  		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecGeneric certBagSeq(certBagSequenceAt1->GetContentDER());
	certBagSeq.InitL();
	if (certBagSeq.Tag() != EASN1OctetString || certBagSeq.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	else 
		{
		iCertValue.Set(certBagSeq.GetContentDER());
		}
    CleanupStack::PopAndDestroy(certBagSequence);
	}
	
EXPORT_C const TDesC& CDecPkcs12CertBag::CertId() const
	{
	return *iCertId;	
   	}

EXPORT_C const TDesC8& CDecPkcs12CertBag::CertValue() const
   	{
   	return iCertValue;		
   	}
   	
EXPORT_C CX509Certificate* CDecPkcs12CertBag::X509CertificateL() const
	{
	return (*iCertId == KX509CertificateOID) ? CX509Certificate::NewL(iCertValue) : NULL;
	}

/////////////////////// SafeContentsBag ///////////////////////

CDecPkcs12SafeContentsBag::CDecPkcs12SafeContentsBag()
	{
	}
	
EXPORT_C CDecPkcs12SafeContentsBag* CDecPkcs12SafeContentsBag::NewL(const TDesC8& aSafeContentsBagData)
	{
	CDecPkcs12SafeContentsBag* self = new(ELeave) CDecPkcs12SafeContentsBag;
	CleanupStack::PushL(self);
	self->ConstructL(aSafeContentsBagData);
	CleanupStack::Pop(self);
	return self;
	}

CDecPkcs12SafeContentsBag::~CDecPkcs12SafeContentsBag()
	{
	iSafeBags.ResetAndDestroy();
	iSafeBags.Close();
	}

void CDecPkcs12SafeContentsBag::ConstructL(const TDesC8& aSafeBagData)
	{
	CDecPkcs12SafeBag::ConstructL(aSafeBagData);
	// This is SafeBag Sequence containing a SafeContents Bag
	TASN1DecGeneric seqGen(iBagValue);
	seqGen.InitL();
	
	// Check if this is a Sequence
	if (seqGen.Tag() != EASN1Sequence || seqGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecSequence seq;
	CArrayPtrFlat<TASN1DecGeneric>* safeContentsBagSeq = seq.DecodeDERLC(seqGen);
			        
    // Find out the number of SafeBags present in the SafeContents Bag
    TInt safeContentsBagCount = safeContentsBagSeq->Count();
    const TASN1DecGeneric* safeContentsBagSeqAtPos;
    for (TInt index = 0; index < safeContentsBagCount; index++)
   		{
   		safeContentsBagSeqAtPos = safeContentsBagSeq->At(index);
   	 	if (safeContentsBagSeqAtPos->Tag() != EASN1Sequence || safeContentsBagSeqAtPos->Class() != EUniversal)
			{
			User::Leave(KErrArgument);
			}
					
		const TDesC8& safeBag(safeContentsBagSeqAtPos->Encoding());
  		// Decode this sequence, This is a SafeBag.
  		CDecPkcs12SafeBag* safeBagObject = CDecPkcs12SafeBag::NewL(safeBag);
  		CleanupStack::PushL(safeBagObject);
   	 	iSafeBags.AppendL(safeBagObject);
   	 	CleanupStack::Pop(safeBagObject);
   		}
   	CleanupStack::PopAndDestroy(safeContentsBagSeq);
    }

EXPORT_C const RPointerArray<CDecPkcs12SafeBag>& CDecPkcs12SafeContentsBag::SafeBags() const
	 {
	 return iSafeBags;
	 } 
	 
///////////////////////////// Safe Contents ///////////////////////////////
CDecPkcs12SafeContents::CDecPkcs12SafeContents()
	{
	}

CDecPkcs12SafeContents::~CDecPkcs12SafeContents()
	{
	delete iDecryptedData;
	iSafeBags.ResetAndDestroy();
	iSafeBags.Close();
	}

EXPORT_C CDecPkcs12SafeContents* CDecPkcs12SafeContents::NewL(const CPKCS7ContentInfo& aSafeContentsBagData)
	{
	CDecPkcs12SafeContents* self = new(ELeave) CDecPkcs12SafeContents;
	CleanupStack::PushL(self);
	self->ConstructL(aSafeContentsBagData.ContentData());
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDecPkcs12SafeContents* CDecPkcs12SafeContents::NewL(const CPKCS7ContentInfo& aSafeContentsBagData, const TDesC& aPassword)
	{
	CDecPkcs12SafeContents* self = new(ELeave) CDecPkcs12SafeContents;
	CleanupStack::PushL(self);
	self->DecodeEncryptedDataL(aSafeContentsBagData,aPassword);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CDecPkcs12SafeContents* CDecPkcs12SafeContents::NewL(const TDesC8& aSafeContent)
	{
	CDecPkcs12SafeContents* self = new(ELeave) CDecPkcs12SafeContents;
	CleanupStack::PushL(self);
	self->ConstructL(aSafeContent);
	CleanupStack::Pop(self);
	return self;
	}

void CDecPkcs12SafeContents::ConstructL(const TDesC8& aSafeContent)
	{
	TASN1DecGeneric decGen(aSafeContent);
    decGen.InitL(); 
			
	if(decGen.Tag() != EASN1Sequence || decGen.Class() != EUniversal)      
		{
		User::Leave(KErrArgument);
		}
          		 
    TASN1DecGeneric decGen2(decGen.GetContentDER());
	decGen2.InitL(); 
			
	if(decGen2.Tag() != EASN1Sequence || decGen.Class() != EUniversal)      
		{
		User ::Leave(KErrArgument);
	    }
				
	// Decode sequence
	TASN1DecSequence seq;
 	CArrayPtr<TASN1DecGeneric>* safeBagSequences = seq.DecodeDERLC(decGen);
 	
 	// A Sequence of SafeBags are present within the SafeContents Bag Sequence
 	TInt safeBagCount = safeBagSequences->Count();
 	
 	for(TInt index = 0; index < safeBagCount; index++)
 	  	{
 	  	CDecPkcs12SafeBag* safeBag = CDecPkcs12SafeBag::NewL(safeBagSequences->At(index)->Encoding());
 	  	CleanupStack::PushL(safeBag);
 	   	iSafeBags.AppendL(safeBag);
 	   	CleanupStack::Pop(safeBag);
 	   	}	
 	CleanupStack::PopAndDestroy(safeBagSequences); // safeBagSequences
	}

void CDecPkcs12SafeContents::DecodeEncryptedDataL(const CPKCS7ContentInfo& aContentInfo, const TDesC& aPassword)
	{
	CPKCS7EncryptedDataObject* pkcs7EncryptedData = CPKCS7EncryptedDataObject::NewL(aContentInfo);
	CleanupStack::PushL(pkcs7EncryptedData);
	iDecryptedData = pkcs7EncryptedData->DecryptDataL(aPassword);	
	ConstructL(iDecryptedData->Des());
	CleanupStack::PopAndDestroy(pkcs7EncryptedData); 
	}

EXPORT_C const RPointerArray<CDecPkcs12SafeBag>& CDecPkcs12SafeContents::SafeContentsBags() const
	{
	return iSafeBags;
	}

EXPORT_C const TDesC8* CDecPkcs12SafeContents::DecryptedData() const
	{
	return iDecryptedData;	
	}

