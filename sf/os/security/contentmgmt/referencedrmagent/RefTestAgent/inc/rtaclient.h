/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* RTA - client Reference Test Server interface
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#ifndef __RTACLIENT_H__
#define __RTACLIENT_H__

#include <e32std.h>

namespace ReferenceTestAgent
	{
	const TInt KDefaultBufferSize = 2048;
	
	class RRtaClient : public RSessionBase
		{
	protected:
		// protected constructor so only derived classes can access it
		RRtaClient();
		TInt Connect();
		TBool IsDrmCapabilityEnforced() const;
	private:
		static TInt StartRTAServer();
		};
	} // namespace
#endif
