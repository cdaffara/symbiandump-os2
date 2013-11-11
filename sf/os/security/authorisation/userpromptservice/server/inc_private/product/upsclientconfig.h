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
* User Prompt Service client side (system server0 configuration classes for
* upsclient.dll
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef UPSCLIENTCONFIG_H
#define UPSCLIENTCONFIG_H

#include <e32base.h>
#include <e32cmn.h>
#include <ups/upstypes.h>
#include "serviceconfig.h"

namespace UserPromptService
{

/**
Hold configuration information in the system server that determines
whether the user prompt service must be invoked regardless of whether
the client passed the system server's security check.
*/
NONSHARABLE_CLASS(CUpsClientConfig) : public CBase
	{
public:
	static CUpsClientConfig* NewL(TInt aCount, TServiceConfig *aRawServiceConfig);	

	enum TQueryUpsResult { EAllow, EQueryUps, EReject };
	
	TQueryUpsResult QueryUps(TBool aServerChecksPassed,
								const TServiceId &aServiceId, 
								const TSecureId &aClientSid,
								const TProcessId &aClientProcessId) const;
	
	~CUpsClientConfig();
private:
	void ConstructL(TInt aCount, TServiceConfig *aRawServiceConfig);
	CUpsClientConfig();
	RArray<TServiceConfig> iServiceConfig;
	};

} // End of namespace UserPromptService

#endif // UPSCLIENTCONFIG_H
