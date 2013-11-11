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
#ifndef __RTARIGHTSHANDLER_H__
#define __RTARIGHTSHANDLER_H__

#include <e32base.h>
#include "rtamessagehandler.h"
#include "clientserver.h"
#include "RTAserver.h"

namespace ReferenceTestAgent
	{
	class CRtaRightsHandler: public CRtaMessageHandler
		{
	public:
		static CRtaRightsHandler* NewL(const CSession2& aSession);
		
		virtual void ServiceL(const RMessage2& aMessage);
		virtual ~CRtaRightsHandler();
	private:
		CRtaRightsHandler(const CSession2& aSession);
		
		// RRtaRights functions
		void AddRightsL(const RMessage2& aMessage);
		void ClearAllRightsL(const RMessage2& aMessage);
		void DumpRightsDatabaseL(const RMessage2& aMessage);
		void DeleteRightsByKeyL(const RMessage2& aMessage);
		void EraseDeleteKeyL(const RMessage2& aMessage);
		void ReloadRightsDatabaseL(const RMessage2& aMessage);
		};
		
	} //namespace

#endif // __SISREGISTRYSERVERSESSION_H__
