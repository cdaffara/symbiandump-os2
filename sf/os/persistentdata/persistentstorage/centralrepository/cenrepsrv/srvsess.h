// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef SRVSESS_H
#define SRVSESS_H

#include <e32base.h>
#include "clientrequest.h"

class CSessionManager;
class CServerSubSession;

class CServerSession : public CSession2
	{
public:
	virtual ~CServerSession();

private:
	//methods inherited from CSession2
	virtual void ServiceError(const RMessage2 &aMessage, TInt aError);
	virtual void ServiceL(const RMessage2& aMessage);
	virtual void CreateL();

	//methods for handling subsessions
	CServerSubSession* NewSubSessionL(const RMessage2& aMessage);
	void DeleteSubSession(TInt aHandle);
	CServerSubSession* SubSessionFromHandle(const RMessage2& aMessage,TInt aHandle);
	
	inline CSessionManager* Server();
	
private:
	CObjectCon* iContainer;
	CObjectIx* iSubSessionIx;

//following methods and members are used for testing purposes only	
#if defined(__CENTREP_SERVER_PERFTEST__) || defined (__CENTREP_SERVER_MEMTEST__) || defined(__CENTREP_SERVER_CACHETEST__)
	TInt GetSetParameters(const TClientRequest& aMessage);
#endif // __CENTREP_SERVER_PERFTEST__ || __CENTREP_SERVER_MEMTEST__ || __CENTREP_SERVER_CACHETEST__

	};


#endif // SRVSESS_H

