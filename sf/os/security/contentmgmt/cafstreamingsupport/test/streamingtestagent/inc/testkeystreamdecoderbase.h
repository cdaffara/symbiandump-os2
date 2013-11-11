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
// Defines the base class for a test key stream decoder object.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TESTKEYSTREAMDECODERBASE_H_
#define TESTKEYSTREAMDECODERBASE_H_

#include <caf/streaming/streamagentinterface.h>

namespace StreamAccess
	{
	NONSHARABLE_CLASS(CTestKeyStreamDecoderBase) : public CAgentKeyStreamDecoder
		{
	public:
		~CTestKeyStreamDecoderBase();
		
		//From CAgentKeyStreamDecoder
		IMPORT_C virtual void GetAttributeL(const TAttribute& aAttribute, TBool& aValue) = 0;
		IMPORT_C virtual HBufC* GetStringAttributeLC(const TStringAttribute& aAttribute) = 0;
		IMPORT_C virtual void GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType) = 0;
		
		IMPORT_C virtual void SendKeyStreamL(const TDesC8& aKey) const = 0;
		IMPORT_C virtual void SendIpSecAssociationL(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey) const = 0;
	
	protected:
		CTestKeyStreamDecoderBase();
		};
	}//End of the namespace StreamAccess

#endif /*TESTKEYSTREAMDECODERBASE_H_*/
