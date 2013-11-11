// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SRVSUBSESS_H
#define SRVSUBSESS_H

#include <e32base.h>
#include "shrepos.h"
#include "srvrepos_noc.h"
#include "clientrequest.h"
#include "sessnotf.h"

class CServerSession;
class CSessionManager;

class CServerSubSession : public CObject
	{
friend class CServerSession;	
public:
	CServerSubSession(CServerSession* aSession);
	~CServerSubSession();
	void ServiceError(TInt aError);
	TInt ServiceL(const RMessage2& aMessage);
public:
	enum { KDontCompleteMessage = 1 };

private:
	typedef TInt (CServerSubSession::*TServerFunc)(const TClientRequest&);

	TInt GeneralOperationsL(const TClientRequest& aMessage, TServerFunc aFunc);
	TInt ReadOperationsL(const TClientRequest& aMessage, TServerFunc aFunc);
	TInt WriteOperationsL(const TClientRequest& aMessage, TServerFunc aFunc);
	TInt ResetOperationsL(const TClientRequest& aMessage, TServerFunc aFunc);

	TInt InitialiseL(const TClientRequest& aMessage);
	TInt CreateIntL(const TClientRequest& aMessage);
	TInt CreateRealL(const TClientRequest& aMessage);
	TInt CreateStringL(const TClientRequest& aMessage);
	TInt DeleteL(const TClientRequest& aMessage);
	TInt DeleteRangeL(const TClientRequest& aMessage);
	TInt GetIntL(const TClientRequest& aMessage);
	TInt SetIntL(const TClientRequest& aMessage);
	TInt GetRealL(const TClientRequest& aMessage);
	TInt SetRealL(const TClientRequest& aMessage);
	TInt GetStringL(const TClientRequest& aMessage);
	TInt SetStringL(const TClientRequest& aMessage);
	TInt FindL(const TClientRequest& aMessage);
	TInt FindEqIntL(const TClientRequest& aMessage);
	TInt FindEqRealL(const TClientRequest& aMessage);
	TInt FindEqStringL(const TClientRequest& aMessage);
	TInt FindNeqIntL(const TClientRequest& aMessage);
	TInt FindNeqRealL(const TClientRequest& aMessage);
	TInt FindNeqStringL(const TClientRequest& aMessage);
	TInt GetFindResultL(const TClientRequest& aMessage);
	TInt NotifyRequestCheck(const TClientRequest& aMessage);
	TInt NotifyRequest(const TClientRequest& aMessage);
	TInt NotifyCancel(const TClientRequest& aMessage);
	TInt NotifyCancelAll(const TClientRequest& aMessage);
	TInt GroupNotifyRequest(const TClientRequest& aMessage);
	TInt GroupNotifyCancel(const TClientRequest& aMessage);
	TInt ResetL(const TClientRequest& aMessage);
	TInt ResetAllL(const TClientRequest& aMessage);
	TInt TransactionStart(const TClientRequest& aMessage);
	TInt TransactionCancel(const TClientRequest& aMessage);
	TInt TransactionCommitL(const TClientRequest& aMessage);
	TInt MoveL(const TClientRequest& aMessage) ;
	TInt TransactionStateL(const TClientRequest& aMessage);
	TInt TransactionFail(const TClientRequest& aMessage);
	TInt GetMeta(const TClientRequest& aMessage);
	
	template <class T> TInt FindValueL(const TClientRequest& aMessage, const T& aVal, TComparison aComparison=EEqual);
	inline CSessionManager* Server();	
	TInt CheckPolicy(const TClientRequest& msg,const TSecurityPolicy& aPolicy,
				const char *aDiagnostic);

private:
	CServerSession* iSession;
	CServerRepository iRepository;
	CSessionNotifier iNotifier;
	RArray<TUint32> iFindResult;
	TBool iInitialised;
#if defined (__CENTREP_SERVER_PERFTEST__) || defined(SRVSUBSESS_TRACE) || defined(CENTREP_TRACE)

   	TUid iRepositoryUid;
 #endif	


#if defined (__CENTREP_SERVER_PERFTEST__) || defined(SRVSUBSESS_TRACE) || defined(CENTREP_TRACE)
// For testing purposes only
public:
	inline TUid RepositoryUid()
	{
	return iRepositoryUid;
	}
#endif
	};


#endif // SRVSUBSESS_H
