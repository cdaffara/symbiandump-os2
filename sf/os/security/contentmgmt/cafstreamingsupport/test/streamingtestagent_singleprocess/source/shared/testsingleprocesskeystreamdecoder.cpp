// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements the Test 'Single Process' Stream Agent Key Stream Decoder object.
// 
//

/**
 @file
 @test
*/

#include "testsingleprocesskeystreamdecoder.h"

using namespace StreamAccess;

CTestSingleProcessKeyStreamDecoder::CTestSingleProcessKeyStreamDecoder()
/**
 	Constructor.
 */
	: CTestKeyStreamDecoderBase()
	{
	//empty
	}

CTestSingleProcessKeyStreamDecoder::~CTestSingleProcessKeyStreamDecoder()
/**
 	Destructor.
 */
	{	
	iFs.Close();
	
	delete iKeyStreamSink;
	delete iSdp;
	delete iSdpDoc;
	delete iRo;
	}

EXPORT_C CTestSingleProcessKeyStreamDecoder* CTestSingleProcessKeyStreamDecoder::NewL(const CKeyStreamSink& aKeyStreamSink,
	    															  const CSdpMediaField& aSdpKeyStream,
	    															  const CSdpDocument& aSdpDoc)
/**
	Factory function allocates a new instance of CTestSingleProcessKeyStreamDecoder.
	@return		New instance of CTestSingleProcessKeyStreamDecoder
 */
	{
	CTestSingleProcessKeyStreamDecoder* self = CTestSingleProcessKeyStreamDecoder::NewLC(aKeyStreamSink, aSdpKeyStream, aSdpDoc);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CTestSingleProcessKeyStreamDecoder* CTestSingleProcessKeyStreamDecoder::NewLC(const CKeyStreamSink& aKeyStreamSink,
	    															   const CSdpMediaField& aSdpKeyStream,
	    															   const CSdpDocument& aSdpDoc)
/**
	Factory function allocates a new instance of CTestSingleProcessKeyStreamDecoder 
	and places the object on the cleanup stack.
	@return		A new instance of CTestSingleProcessKeyStreamDecoder 
				which is left on the cleanup stack.
 */
	{
	CTestSingleProcessKeyStreamDecoder* self = new (ELeave) CTestSingleProcessKeyStreamDecoder();
	CleanupStack::PushL(self);
	self->ConstructL(aKeyStreamSink, aSdpKeyStream, aSdpDoc);
	return self;
	}

void CTestSingleProcessKeyStreamDecoder::ConstructL(const CKeyStreamSink& aKeyStreamSink,
											const CSdpMediaField& aSdpKeyStream,
											const CSdpDocument& aSdpDoc)
/**
 	Second phase constructor.
 */
	{
	User::LeaveIfError(iFs.Connect());
	
	iKeyStreamSink = aKeyStreamSink.CloneLC();
	CleanupStack::Pop(iKeyStreamSink);
	
	// Set the encryption and authentication algorithm of the key stream sink
	iKeyStreamSink->SetEncryptionAlgorithmL(EAES_128_CBC);
	iKeyStreamSink->SetAuthenticationAlgorithmL(EHMAC_SHA1);

	iSdp = aSdpKeyStream.CloneL();
	iSdpDoc = aSdpDoc.CloneL();
	SetRoL();
	}

EXPORT_C void CTestSingleProcessKeyStreamDecoder::GetAttributeL(const TAttribute& aAttribute, TBool& aValue)
/**
 	@see CAgentKeyStreamDecoder::GetAttributeL
 */ 	
	{
	VerifyRoL();
	
	aValue = EFalse;
	
	switch(aAttribute)
		{
		case EIsProgramProtected:
			aValue = iRo->AccessRights() & EProgramProtected;
			break;
		case EIsServiceProtected:
			aValue = iRo->AccessRights() & EServiceProtected;
			break;
		case ECanExport:
			aValue = iRo->AccessRights() & ECanExportContent;
			break;
		case EMustProtectIfRecording:
			aValue = iRo->AccessRights() & EMustProtectContentIfRecording;
			break;
		case ECanPlay:
			aValue = iRo->AccessRights() & ECanPlayContent;
			break;
		default:
			User::Leave(KErrCAOutOfRange);
		}
	
	// If the the return value is greater than 0, set the boolean value to ETrue
	if(aValue)
		{
		aValue = ETrue;
		}
	}

EXPORT_C HBufC* CTestSingleProcessKeyStreamDecoder::GetStringAttributeLC(const TStringAttribute& aAttribute)
/**
 	@see CAgentKeyStreamDecoder::GetStringAttributeL
 */
	{
	VerifyRoL();
	
	HBufC8* str8 = NULL;
	
	switch(aAttribute)
		{
		case EContentID:
			str8 = iRo->ContentId();
			break;
		case ERightsIssuerURI:
			str8 = iRo->RightsIssuer();
			break;
		default:
			User::Leave(KErrCAOutOfRange);
		}
	
	//Convert 8-bit to 16-bit
	HBufC* value = HBufC::NewL(str8->Des().Size());
	TPtr ptr(value->Des());
	ptr.Copy(str8->Des());

	CleanupStack::PushL(value);
	return value;
	}
 
EXPORT_C void CTestSingleProcessKeyStreamDecoder::GetPostDeliveryRightsL(RFile& /*aFile*/, TDes8& /*aCid*/, TDes8& /*aRightsMimeType*/, TDes8& /*aContentMimeType*/)
/**
 	@see CAgentKeyStreamDecoder::GetPostDeliveryRightsHandleL
 */
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CTestSingleProcessKeyStreamDecoder::SendKeyStreamL(const TDesC8& aKey) const
/**
 	Sends an encrypted short-term key to the key stream sink
 	@param aKey An encrypted key.
 */
	{
	HBufC8* shortTermKey = aKey.AllocLC();
	
	//Check if the long-term key key exists in the short-term key
	TInt ret = shortTermKey->Find(*iRo->Key());
	if(ret == KErrNotFound)
		{
		User::Leave(KErrGeneral);
		}
	
	//Get the plain string (key)
	HBufC8* decryptedKey = HBufC8::NewLC(ret);
	TPtr8 ptrKey(decryptedKey->Des());
	ptrKey.Copy(shortTermKey->Des().Ptr(), ret);
	
	CTestKeyAssociation* decryptedAssoc = CTestKeyAssociation::NewL(ptrKey);
	CleanupStack::PushL(decryptedAssoc);
	
	iKeyStreamSink->ProcessNewKeyAssociationL(*decryptedAssoc);
	
	CleanupStack::PopAndDestroy(3, shortTermKey); //< shortTermKey, decryptedKey, decryptedAssoc
	}

EXPORT_C void CTestSingleProcessKeyStreamDecoder::SendIpSecAssociationL(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey) const
	{
	CKeyAssociation* ipSecKeyAssociation = CIpSecKeyAssociation::NewLC(aSpi, aEncryptionKey, aAuthenticationKey);			
	iKeyStreamSink->ProcessNewKeyAssociationL(*ipSecKeyAssociation);
	CleanupStack::PopAndDestroy(ipSecKeyAssociation);
	}

void CTestSingleProcessKeyStreamDecoder::SetRoL()
	{
	_LIT(KDriveFormat,"!:");
	TBuf<50> privateFolder;
	iFs.PrivatePath(privateFolder);
	privateFolder.Insert(0,KDriveFormat);
	
	DoSetRightsObjectL(iFs, *iSdp, iRo, privateFolder);
	}

void CTestSingleProcessKeyStreamDecoder::VerifyRoL()
/**
 	Verifies that the rights object exists.
 */
	{
	if(!iRo)
		{//The RO defined in the SDP does not exist
		User::Leave(KErrCANoRights);
		}
	if(iRo->IsExpired())
		{
		User::Leave(KErrCANoPermission);
		}
	}
