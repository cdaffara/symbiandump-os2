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
* Defines the key stream decoder object of the streaming reference agent.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRAKEYSTREAMDECODER_H
#define SRAKEYSTREAMDECODER_H

#include <caf/streaming/streamagentinterface.h>
#include "sraclient.h"


class CSraKeyStreamDecoder : public CAgentKeyStreamDecoder
	{
public:
	static CSraKeyStreamDecoder* NewL(const CKeyStreamSink& aKeyStreamSink,
			    					  const CSdpMediaField& aSdpKeyStream,
			    					  const CSdpDocument& aSdpDoc);
	static CSraKeyStreamDecoder* NewLC(const CKeyStreamSink& aKeyStreamSink,
			    					   const CSdpMediaField& aSdpKeyStream,
			    					   const CSdpDocument& aSdpDoc);
	~CSraKeyStreamDecoder();
		
	//From CAgentKeyStreamDecoder
	void GetAttributeL(const TAttribute& aAttribute, TBool& aValue);
	HBufC* GetStringAttributeLC(const TStringAttribute& aAttribute);
	void GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType);
	
private:
	CSraKeyStreamDecoder();
	void ConstructL(const CKeyStreamSink& aKeyStreamSink,  const CSdpMediaField& aSdpKeyStream, const CSdpDocument& aSdpDoc);
		
private:
RSraClient iAgentClient;
	};

#endif /*SRAKEYSTREAMDECODER_H*/
