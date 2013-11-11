/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements the key stream decoder object of the streaming reference agent.
*
*/


#include "srakeystreamdecoder.h"


CSraKeyStreamDecoder::CSraKeyStreamDecoder()
/**
 	Constructor.
 */
	: CAgentKeyStreamDecoder()
	{
	//empty
	}

CSraKeyStreamDecoder::~CSraKeyStreamDecoder()
/**
 	Destructor.
 */
	{
	iAgentClient.Close();
	}

CSraKeyStreamDecoder* CSraKeyStreamDecoder::NewL(const CKeyStreamSink& aKeyStreamSink,
	    										 const CSdpMediaField& aSdpKeyStream,
	    										 const CSdpDocument& aSdpDoc)
/**
	Factory function allocates a new instance of CSraKeyStreamDecoder.
	@return		New instance of CSraKeyStreamDecoder
 */
	{
	CSraKeyStreamDecoder* self = CSraKeyStreamDecoder::NewLC(aKeyStreamSink, aSdpKeyStream, aSdpDoc);
	CleanupStack::Pop(self);
	return self;
	}

CSraKeyStreamDecoder* CSraKeyStreamDecoder::NewLC(const CKeyStreamSink& aKeyStreamSink,
	    										  const CSdpMediaField& aSdpKeyStream,
	    										  const CSdpDocument& aSdpDoc)
/**
	Factory function allocates a new instance of CSraKeyStreamDecoder 
	and places the object on the cleanup stack.
	@return		A new instance of CSraKeyStreamDecoder 
				which is left on the cleanup stack.
 */
	{
	CSraKeyStreamDecoder* self = new (ELeave) CSraKeyStreamDecoder();
	CleanupStack::PushL(self);
	self->ConstructL(aKeyStreamSink, aSdpKeyStream, aSdpDoc);
	return self;
	}

void CSraKeyStreamDecoder::ConstructL(const CKeyStreamSink& aKeyStreamSink,
									  const CSdpMediaField& aSdpKeyStream,
									  const CSdpDocument& aSdpDoc)
/**
 	Second phase constructor.
 */
	{
	User::LeaveIfError(iAgentClient.Connect());
	iAgentClient.SendKeyStreamSinkL(aKeyStreamSink);
	iAgentClient.SendSdpMediaFieldL(aSdpKeyStream);
	iAgentClient.SendSdpDocumentL(aSdpDoc);
	}

void CSraKeyStreamDecoder::GetAttributeL(const TAttribute& aAttribute, TBool& aValue)
/**
 	@see CAgentKeyStreamDecoder::GetAttributeL
 */ 	
	{
	iAgentClient.GetAttributeL(aAttribute, aValue);
	}

HBufC* CSraKeyStreamDecoder::GetStringAttributeLC(const TStringAttribute& aAttribute)
/**
 	@see CAgentKeyStreamDecoder::GetStringAttributeL
 */
	{
	return iAgentClient.GetStringAttributeLC(aAttribute);
	}
 
void CSraKeyStreamDecoder::GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType)
/**
 	@see CAgentKeyStreamDecoder::GetPostDeliveryRightsHandleL
 */
	{
	iAgentClient.GetPostDeliveryRightsL(aFile, aCid, aRightsMimeType, aContentMimeType);
	}
