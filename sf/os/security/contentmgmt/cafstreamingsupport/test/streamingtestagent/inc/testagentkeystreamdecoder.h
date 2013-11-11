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
// Defines the test agent key stream decoder object.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TESTAGENTKEYSTREAMDECODER_H
#define TESTAGENTKEYSTREAMDECODER_H

#include "testkeystreamdecoderbase.h"
#include "staclient.h"

namespace StreamAccess
	{
	NONSHARABLE_CLASS(CTestAgentKeyStreamDecoder) : public CTestKeyStreamDecoderBase
		{
	public:
		IMPORT_C static CTestAgentKeyStreamDecoder* NewL(const CKeyStreamSink& aKeyStreamSink,
			    										 const CSdpMediaField& aSdpKeyStream,
			    										 const CSdpDocument& aSdpDoc);
		IMPORT_C static CTestAgentKeyStreamDecoder* NewLC(const CKeyStreamSink& aKeyStreamSink,
			    										 const CSdpMediaField& aSdpKeyStream,
			    										 const CSdpDocument& aSdpDoc);
		~CTestAgentKeyStreamDecoder();
		
		//From CAgentKeyStreamDecoder
		IMPORT_C void GetAttributeL(const TAttribute& aAttribute, TBool& aValue);
		IMPORT_C HBufC* GetStringAttributeLC(const TStringAttribute& aAttribute);
		IMPORT_C void GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType);
		
		//From CTestKeyStreamDecoderBase
		IMPORT_C void SendKeyStreamL(const TDesC8& aKey) const;
		IMPORT_C void SendIpSecAssociationL(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey) const;
	
	private:
		CTestAgentKeyStreamDecoder();
		void ConstructL(const CKeyStreamSink& aKeyStreamSink,  const CSdpMediaField& aSdpKeyStream, const CSdpDocument& aSdpDoc);
		
	private:
		RStaClient iAgentClient;
		};
	}//End of the namespace StreamAccess
#endif /*TESTAGENTKEYSTREAMDECODER_H*/
