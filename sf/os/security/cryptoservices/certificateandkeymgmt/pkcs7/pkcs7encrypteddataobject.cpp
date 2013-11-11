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


#include "pkcs7encrypteddataobject.h"

/** The last character in the PKCS7 EncryptedData OID */
const TInt KPkcs7EncryptedData = 6;

EXPORT_C CPKCS7EncryptedDataObject* CPKCS7EncryptedDataObject::NewL(const CPKCS7ContentInfo& aContentInfo)
	{
	CPKCS7EncryptedDataObject* self = new (ELeave) CPKCS7EncryptedDataObject();
	CleanupStack::PushL(self);
	self->ConstructL(aContentInfo);
	CleanupStack::Pop(self);
	return self;	
	}

CPKCS7EncryptedDataObject::CPKCS7EncryptedDataObject(void)
	{
	}

CPKCS7EncryptedDataObject::~CPKCS7EncryptedDataObject()
	{
	delete iEncryptParams;
	}

EXPORT_C TInt CPKCS7EncryptedDataObject::Version() const
	{
	return iVersion;
	}
	
EXPORT_C CPKCS7EncryptedDataObject::TContentType CPKCS7EncryptedDataObject::ContentType() const
	{
	return iContentType;
	}
	
EXPORT_C const TDesC8& CPKCS7EncryptedDataObject::EncryptedContentInfoData() const
	{
	return iEncryptedContent;
	}	

EXPORT_C const CPBEncryptParms& CPKCS7EncryptedDataObject::EncryptParams() const
	{
	return *iEncryptParams;
	}

CPBEncryptParms* CPKCS7EncryptedDataObject::DecodeContentEncryptionAlgorithmL(const TDesC8& aBinaryData) const
	{
	TASN1DecGeneric seqGen(aBinaryData);
	seqGen.InitL();
	if (seqGen.Tag() != EASN1Sequence || seqGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	
    TPtrC8 theContent(seqGen.Encoding());
    CPBEncryptParms* encryptParams =  TASN1DecPKCS5::DecodeDERL(theContent);
    return encryptParams;
	}	

EXPORT_C HBufC8* CPKCS7EncryptedDataObject::DecryptDataL(const TDesC& aPassword) const
	{
	CPBEncryptElement* encryptElement;
    HBufC8* pkcs12Pwd = PKCS12KDF::GeneratePasswordLC(aPassword);
    if(iEncryptParams->Kdf() == CPBEncryptParms::EKdfPkcs12)
		{
	 	TPtrC8 iv = iEncryptParams->IV();
	    HBufC8* ivValue = HBufC8::NewMaxLC(iv.Length());
	    TPtr8 encryptKeyBuf = ivValue->Des();

	 	switch(iEncryptParams->Cipher())	
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
				PKCS12KDF::DeriveKeyL(encryptKeyBuf, PKCS12KDF::EIDByteIV, *pkcs12Pwd, iEncryptParams->Salt(), iEncryptParams->Iterations());
				iEncryptParams->SetIV(encryptKeyBuf);
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
		encryptElement = CPBEncryptElement::NewLC(*pkcs12Pwd, *iEncryptParams);
		}
	else
		{
		TPBPassword password(aPassword);
		// Create the decryptor	
		encryptElement = CPBEncryptElement::NewLC(password.Password(), *iEncryptParams);	
		}
	 
	CPBDecryptor* decryptor = encryptElement->NewDecryptLC();
   	
	TPtrC8 encryptedKey(iEncryptedContent);
	TUint encryptLength = encryptedKey.Length();
	TUint maxDecryptLength = decryptor->MaxOutputLength(encryptLength);
	
	if ( maxDecryptLength <= 0 )
		{
		User::Leave(KErrGeneral);		
		}
	
	HBufC8* decryptedContent = HBufC8::NewLC(maxDecryptLength);
	TPtr8 dcDes(decryptedContent->Des());
	decryptor->Process(encryptedKey, dcDes);
	CleanupStack::Pop(decryptedContent);
	CleanupStack::PopAndDestroy(3, pkcs12Pwd); // pkcs12Pwd, encryptElement, decryptor
   	return decryptedContent;
   	}

 void CPKCS7EncryptedDataObject::ConstructL(const CPKCS7ContentInfo& aContentInfo)
	{
	if(aContentInfo.ContentType() != KPkcs7EncryptedData)
		{
		User::Leave(KErrArgument);
		}
    
	// EncryptedData SEQUENCE
	TASN1DecGeneric decGen(aContentInfo.ContentData());
	decGen.InitL();
	if(decGen.Tag() != EASN1Sequence || decGen.Class() != EUniversal)
		{
	    User::Leave(KErrArgument);
		}
    TASN1DecSequence seq1;
    CArrayPtrFlat<TASN1DecGeneric>* EncryptedDataContents = seq1.DecodeDERLC(decGen);       					
			    
	// Both version and encryptedContentInfo should be present
	if (EncryptedDataContents->Count() != 2)
		{
		User::Leave(KErrArgument);
		}
		            
	// VERSION
	// version is the syntax version number. It shall be 0 for this version of the standard
	const TASN1DecGeneric* EncryptedDataContentsAt0 = EncryptedDataContents->At(0);
	if(EncryptedDataContentsAt0->Tag() != EASN1Integer || EncryptedDataContentsAt0->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	 
	TASN1DecInteger intDecoder;
	iVersion = intDecoder.DecodeDERShortL(*EncryptedDataContentsAt0);
	if (iVersion != 0)
		{
	    User::Leave(KErrArgument);		
	    }
		                
    // ENCRYPTED CONTENT INFO a SEQUENCE
    const TASN1DecGeneric* EncryptedDataContentsAt1 = EncryptedDataContents->At(1);
	if (EncryptedDataContentsAt1->Tag() != EASN1Sequence || EncryptedDataContentsAt1->Class() != EUniversal)
		{
	    User::Leave(KErrArgument);
	    }		               
	TASN1DecSequence seq2;
    CArrayPtrFlat<TASN1DecGeneric>* EncryptedContentInfo = seq2.DecodeDERLC(*EncryptedDataContentsAt1);       
		        
    // CONTENT TYPE IDENTIFIED BY AN OBJECT IDENTIFIER
	// Gets the oid
	TASN1DecObjectIdentifier ContentTypeoidDec;
	HBufC* contentType = ContentTypeoidDec.DecodeDERL(*EncryptedContentInfo->At(0));
	if(contentType->Compare(KPkcs7DataOID()) == 0)
		{
		iContentType = EPkcs7Data;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	delete contentType;	        
	// CONTENT ENCRYPTION ALGORITHM
	// Get the Cipher used and the encoded params in it
    iEncryptParams = DecodeContentEncryptionAlgorithmL(EncryptedContentInfo->At(1)->Encoding());
                
    // ENCRYPTED CONTENT
	// This is an OPTIONAL field
	TInt encryptedContentInfoCount = EncryptedContentInfo->Count();
	if(encryptedContentInfoCount == 3)
		{
		TASN1DecGeneric decOctetString(*EncryptedContentInfo->At(2));
		decOctetString.InitL();
	    iEncryptedContent.Set(decOctetString.GetContentDER());
	   	}
	else if(encryptedContentInfoCount > 3 || encryptedContentInfoCount < 2)
	    {
	    User::Leave(KErrArgument);
	    }
	else if(encryptedContentInfoCount == 2)
		{
		iEncryptedContent.Set(KNullDesC8());
		}

	CleanupStack::PopAndDestroy(2, EncryptedDataContents); //EncryptedDataContents, EncryptedContentInfo    	
	}

