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
// Implements the test stream agent factory plug-in.
// 
//

#include "teststreamagentfactory.h"
#include "srautils.h"

using namespace StreamAccess;


static const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KTestStreamAgentFactoryImpId, CTestStreamAgentFactory::CreateTestAgentL)
	};
	
	
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
/**
	Standard ECOM factory
*/
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
	
//
//CTestStreamAgentFactory
//

CTestStreamAgentFactory::CTestStreamAgentFactory()
/**
	Constructor.
 */
:	CStreamAgentFactory()
 	{
 	//empty
 	}
 	

CTestStreamAgentFactory::~CTestStreamAgentFactory()
/**
	Destructor
 */
	{
	//empty
	}
	

CStreamAgentFactory* CTestStreamAgentFactory::CreateTestAgentL()
/**
	Factory method that instantiates a new test stream agent factory ECOM plug-in.

	@return A pointer to the new test stream agent factory object.
*/
	{
	CTestStreamAgentFactory *self = new(ELeave) CTestStreamAgentFactory();
	return self;
	}

CAgentKeyStreamDecoder* CTestStreamAgentFactory::GetKeyStreamDecoderL(const CKeyStreamSink& aKeyStreamSink,
		 															  const CSdpMediaField& aSdpKeyStream,
		 				    										  const CSdpDocument& aSdpDoc)
/**
 	@see CStreamAgentFactory::GetKeyStreamDecoderL
 */
	{
	return CTestAgentKeyStreamDecoder::NewL(aKeyStreamSink, aSdpKeyStream, aSdpDoc); 
	}

TBool CTestStreamAgentFactory::IsKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream)
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
