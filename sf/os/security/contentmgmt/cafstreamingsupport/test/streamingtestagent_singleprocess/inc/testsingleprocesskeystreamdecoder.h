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
// Defines the Test 'Single Process' Stream Agent Key Stream Decoder object.
// 
//

/**
 @file
 @test
*/

#ifndef TESTSINGLEPROCESSKEYSTREAMDECODER_H
#define TESTSINGLEPROCESSKEYSTREAMDECODER_H

#include <s32file.h>

#include "testkeystreamdecoderbase.h"
#include "stacommon.h"
#include "srarightsobject.h"
#include "srautils.h"

#include "testkeyassociation.h"
#include "testkeystreamsink.h"

class CSdpDocument;
class CSdpDocument;
namespace StreamAccess
	{
	NONSHARABLE_CLASS(CTestSingleProcessKeyStreamDecoder) : public CTestKeyStreamDecoderBase
		{
	public:
		IMPORT_C static CTestSingleProcessKeyStreamDecoder* NewL(const CKeyStreamSink& aKeyStreamSink,
			    										 const CSdpMediaField& aSdpKeyStream,
			    										 const CSdpDocument& aSdpDoc);
		IMPORT_C static CTestSingleProcessKeyStreamDecoder* NewLC(const CKeyStreamSink& aKeyStreamSink,
			    										 const CSdpMediaField& aSdpKeyStream,
			    										 const CSdpDocument& aSdpDoc);
		~CTestSingleProcessKeyStreamDecoder();
		
		//From CAgentKeyStreamDecoder
		IMPORT_C void GetAttributeL(const TAttribute& aAttribute, TBool& aValue);
		IMPORT_C HBufC* GetStringAttributeLC(const TStringAttribute& aAttribute);
		IMPORT_C void GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType);
	
		//From CTestKeyStreamDecoderBase
		IMPORT_C void SendKeyStreamL(const TDesC8& aKey) const;
		IMPORT_C void SendIpSecAssociationL(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey) const;
		
	protected:
		void SetRoL();
		void VerifyRoL();
		
	private:
		CTestSingleProcessKeyStreamDecoder();
		
		void ConstructL(const CKeyStreamSink& aKeyStreamSink,  
						const CSdpMediaField& aSdpKeyStream,
						const CSdpDocument& aSdpDoc);
		
	private:
		RFs iFs;
	
		CKeyStreamSink* iKeyStreamSink;
		CSdpMediaField* iSdp;
		CSdpDocument* iSdpDoc;
		CSraRightsObject* iRo;
		};
		
	}//End of the namespace StreamAccess
#endif /*TESTSINGLEPROCESSKEYSTREAMDECODER_H*/
