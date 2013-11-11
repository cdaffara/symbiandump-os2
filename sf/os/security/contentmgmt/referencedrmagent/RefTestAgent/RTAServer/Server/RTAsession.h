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
#ifndef __RTASESSION_H__
#define __RTASESSION_H__

#include <e32base.h>
#include "clientserver.h"
#include "RTAserver.h"

namespace ReferenceTestAgent
	{
	class CRtaMessageHandler;
	
	class CRtaSession : public CSession2
		{
	public:
		CRtaSession();

		void ServiceL(const RMessage2& aMessage);
		static void PanicClient(const RMessagePtr2& aMessage, TRtaServerPanic aPanic);
					
	private:
		virtual ~CRtaSession();
		virtual void CreateL();
		
		CRtaServer& Server();
		virtual void ServiceError(const RMessage2& aMessage,TInt aError);
 		
	private:
		CRtaMessageHandler* iMessageHandler;
		};
	
inline CRtaServer& CRtaSession::Server()
	{
	return *static_cast<CRtaServer*>(const_cast<CServer2*>(CSession2::Server()));
	}


	} //namespace

#endif // __SISREGISTRYSERVERSESSION_H__
