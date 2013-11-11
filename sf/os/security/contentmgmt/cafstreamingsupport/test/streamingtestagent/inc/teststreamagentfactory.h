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
// Implements the base test stream agent factory plug-in.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TESTSTREAMAGENTFACTORY_H_
#define TESTSTREAMAGENTFACTORY_H_

#include <caf/streaming/streamagentfactory.h>
#include "testagentkeystreamdecoder.h"
#include "tscaf_streamagentdefs.h"

namespace StreamAccess
	{
	NONSHARABLE_CLASS(CTestStreamAgentFactory) : public CStreamAgentFactory
		{
	public:
		static CStreamAgentFactory* CreateTestAgentL();
		CAgentKeyStreamDecoder* GetKeyStreamDecoderL(const CKeyStreamSink& aKeyStreamSink,
			    									 const CSdpMediaField& aSdpKeyStream,
			    									 const CSdpDocument& aSdpDoc);
		TBool IsKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream);
	protected:
		~CTestStreamAgentFactory();
		
	private:
		CTestStreamAgentFactory();
		HBufC8* ExtractKmsIdLC(const TDesC8& aValue);
		};
	
	}//End of the namespace StreamAccess

#endif /*TESTSTREAMAGENTFACTORY_H_*/
