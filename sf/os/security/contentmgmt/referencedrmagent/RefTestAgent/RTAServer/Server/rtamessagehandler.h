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
* sisregistry - server classes
*
*/


/**
 @file 
 @test
 @internalComponent
*/
#ifndef __RTAMESSAGEHANDLER_H__
#define __RTAMESSAGEHANDLER_H__

#include <e32base.h>
#include "clientserver.h"
#include "RTAserver.h"

namespace ReferenceTestAgent
	{
	class CRtaMessageHandler : public CBase
		{
	public:	
		virtual ~CRtaMessageHandler();
		
		// derived clients must implement a ServiceL function
		virtual void ServiceL(const RMessage2& aMessage) = 0;
		
	protected:
		CRtaMessageHandler(const CSession2& aSession);

		CRtaServer& Server();
		
		// Utility functions
		static HBufC16* ReadDes16LC(const RMessage2& aMessage, TInt aParam);
		static HBufC8* ReadDes8LC(const RMessage2& aMessage, TInt aParam);
		static HBufC16* ReadDesC16LC(const RMessage2& aMessage, TInt aParam);
		static HBufC8* ReadDesC8LC(const RMessage2& aMessage, TInt aParam);
		static void WriteL(const RMessage2& aMessage, TInt aParam, const TDesC8& aBuffer);
		static void WriteL(const RMessage2& aMessage, TInt aParam, const TDesC16& aBuffer);
		
		const CSession2& iSession;
		};
		
	} //namespace

#endif 
