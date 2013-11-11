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
// Implements tthe test agent key stream decoder object.
// 
//

#include "testagentkeystreamdecoder.h"

using namespace StreamAccess;

CTestAgentKeyStreamDecoder::CTestAgentKeyStreamDecoder()
/**
 	Constructor.
 */
	: CTestKeyStreamDecoderBase()
	{
	//empty
	}

CTestAgentKeyStreamDecoder::~CTestAgentKeyStreamDecoder()
/**
 	Destructor.
 */
	{
	iAgentClient.Close();
	}

EXPORT_C CTestAgentKeyStreamDecoder* CTestAgentKeyStreamDecoder::NewL(const CKeyStreamSink& aKeyStreamSink,
	    															  const CSdpMediaField& aSdpKeyStream,
	    					    									  const CSdpDocument& aSdpDoc)
/**
	Factory function allocates a new instance of CTestAgentKeyStreamDecoder.
	@return		New instance of CTestAgentKeyStreamDecoder
 */
	{
	//TInt count = 	aSdpKeyStream.AttributeFields().Count();
	CTestAgentKeyStreamDecoder* self = CTestAgentKeyStreamDecoder::NewLC(aKeyStreamSink, aSdpKeyStream, aSdpDoc);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CTestAgentKeyStreamDecoder* CTestAgentKeyStreamDecoder::NewLC(const CKeyStreamSink& aKeyStreamSink,
	    															   const CSdpMediaField& aSdpKeyStream,
	    					    									   const CSdpDocument& aSdpDoc)
/**
	Factory function allocates a new instance of CTestAgentKeyStreamDecoder 
	and places the object on the cleanup stack.
	@return		A new instance of CTestAgentKeyStreamDecoder 
				which is left on the cleanup stack.
 */
	{
	CTestAgentKeyStreamDecoder* self = new (ELeave) CTestAgentKeyStreamDecoder();
	CleanupStack::PushL(self);
	self->ConstructL(aKeyStreamSink, aSdpKeyStream, aSdpDoc);
	return self;
	}

void CTestAgentKeyStreamDecoder::ConstructL(const CKeyStreamSink& aKeyStreamSink,
											const CSdpMediaField& aSdpKeyStream,
											const CSdpDocument& aSdpDoc)
/**
 	Second phase constructor.
 */
	{
	User::LeaveIfError(iAgentClient.Connect());
	iAgentClient.SetKeyStreamSinkL(aKeyStreamSink);
	iAgentClient.SetSdpMediaFieldL(aSdpKeyStream);
	iAgentClient.SetSdpDocumentL(aSdpDoc);
	}

EXPORT_C void CTestAgentKeyStreamDecoder::GetAttributeL(const TAttribute& aAttribute, TBool& aValue)
/**
 	@see CAgentKeyStreamDecoder::GetAttributeL
 */ 	
	{
	iAgentClient.GetAttributeL(aAttribute, aValue);
	}

EXPORT_C HBufC* CTestAgentKeyStreamDecoder::GetStringAttributeLC(const TStringAttribute& aAttribute)
/**
 	@see CAgentKeyStreamDecoder::GetStringAttributeL
 */
	{
	return iAgentClient.GetStringAttributeLC(aAttribute);
	}
 
EXPORT_C void CTestAgentKeyStreamDecoder::GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType)
/**
 	@see CAgentKeyStreamDecoder::GetPostDeliveryRightsHandleL
 */
	{
	iAgentClient.GetPostDeliveryRightsL(aFile, aCid, aRightsMimeType, aContentMimeType);
	}

EXPORT_C void CTestAgentKeyStreamDecoder::SendKeyStreamL(const TDesC8& aKey) const
/**
 	Sends an encrypted short-term key to the test agent server.
 	@param aKey An encrypted key.
 */
	{
	iAgentClient.SendKeyStreamL(aKey);
	}

EXPORT_C void CTestAgentKeyStreamDecoder::SendIpSecAssociationL(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey) const
	{
	iAgentClient.SendIpSecAssociationL(aSpi, aEncryptionKey, aAuthenticationKey);
	}
