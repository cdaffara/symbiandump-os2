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
* Describes the streaming reference agent factory plug-in.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRAFACTORY_H_
#define SRAFACTORY_H_

#include <caf/streaming/streamagentfactory.h>

using namespace StreamAccess;

static const TUint KStreamRefAgentFactoryImpId = 0x10285A35;
// Supported KMS Id's
_LIT8(KSraSupportedKmsIds,"2222;");

NONSHARABLE_CLASS(CSraFactory) : public CStreamAgentFactory
	{
public:
	static CStreamAgentFactory* CreateTestAgentL();
	CAgentKeyStreamDecoder* GetKeyStreamDecoderL(const CKeyStreamSink& aKeyStreamSink,
			    								 const CSdpMediaField& aSdpKeyStream,
			    								 const CSdpDocument& aSdpDoc);
	TBool IsKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream);
protected:
	~CSraFactory();
		
private:
	CSraFactory();
	HBufC8* ExtractKmsIdLC(const TDesC8& aValue);
	};

#endif /*SRAFACTORY_H_*/
