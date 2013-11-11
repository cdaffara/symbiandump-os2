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
// Defines the Test 'Single Process' Stream Agent Factory
// 
//

/**
 @file
 @test
*/

#ifndef TESTSINGLEPROCESSAGENTFACTORY_H_
#define TESTSINGLEPROCESSAGENTFACTORY_H_

#include <caf/streaming/streamagentfactory.h>
#include "testsingleprocesskeystreamdecoder.h"
#include "tscaf_spsadefs.h"

class CSdpMediaField;
class CSdpDocument;
namespace StreamAccess
	{
	NONSHARABLE_CLASS(CTestSingleProcessAgentFactory) : public CStreamAgentFactory
		{
	public:
		static CStreamAgentFactory* CreateTestAgentL();
		CAgentKeyStreamDecoder* GetKeyStreamDecoderL(const CKeyStreamSink& aKeyStreamSink,
			    									 const CSdpMediaField& aSdpKeyStream,
			    									 const CSdpDocument& aSdpDoc);
		TBool IsKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream);
	protected:
		~CTestSingleProcessAgentFactory();
		
	private:
		CTestSingleProcessAgentFactory();
		};
	
	}//End of the namespace StreamAccess

#endif /*TESTSINGLEPROCESSAGENTFACTORY_H_*/
