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
// Implements the Test 'Single Process' Stream Agent Factory
// 
//

/**
 @file
 @test
*/

#include "testsingleprocessagentfactory.h"
#include "srautils.h"

using namespace StreamAccess;


static const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KTestStreamAgentFactoryImpId, CTestSingleProcessAgentFactory::CreateTestAgentL)
	};
	
	
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
/**
	Standard ECOM factory
*/
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
	

// CTestSingleProcessAgentFactory


CTestSingleProcessAgentFactory::CTestSingleProcessAgentFactory()
/**
	Constructor.
 */
:	CStreamAgentFactory()
 	{
 	//empty
 	}
 	

CTestSingleProcessAgentFactory::~CTestSingleProcessAgentFactory()
/**
	Destructor
 */
	{
	//empty
	}
	

CStreamAgentFactory* CTestSingleProcessAgentFactory::CreateTestAgentL()
/**
	Factory method that instantiates a new test stream agent factory ECOM plug-in.

	@return A pointer to the new test stream agent factory object.
*/
	{
	CTestSingleProcessAgentFactory *self = new(ELeave) CTestSingleProcessAgentFactory();
	return self;
	}

CAgentKeyStreamDecoder* CTestSingleProcessAgentFactory::GetKeyStreamDecoderL(const CKeyStreamSink& aKeyStreamSink,
		 															  const CSdpMediaField& aSdpKeyStream,
		 															  const CSdpDocument& aSdpDoc)
/**
 	@see CStreamAgentFactory::GetKeyStreamDecoderL
 */
	{
	return CTestSingleProcessKeyStreamDecoder::NewL(aKeyStreamSink, aSdpKeyStream, aSdpDoc); 
	}

TBool CTestSingleProcessAgentFactory::IsKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream)
/**
 	Function to determine whether the test stream agent supports the key management scheme 
 	specified in the SDP key stream definition
 	@param aSdpKeyStream Contains the metadata for the SDP key management scheme of the stream
 	@return ETrue if the test stream agent recognises the SDP format and is able to decode the key stream
	@return EFalse if the test stream agent fails to recofnise the SDP format or is unable to decode the key stream
 */
	{
	return CheckKeyStreamSupportedL(aSdpKeyStream, KSupportedKmsIds());
	}
