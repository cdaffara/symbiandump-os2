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
* Server-side classes which are used to implement the UPS server.
*
*/


/**
 @file
 @internalComponent
 @released
*/
#ifndef UPSSERVER_H
#define UPSSERVER_H

#include <ups/upstypes.h>
#include <f32file.h>
#include <scs/scsserver.h>
#include "upscommon.h"
#include "serviceconfig.h"
#include "upsdbmanager.h"
#include "upspolicycachehandle.h"

namespace UserPromptService
{

// forward declarations
class CUpsServer;
class CUpsSession;
class CUpsSubsession;
class CPolicyCache;
class CPromptRequest;
class CDecisionDbW;
class CAuthoriserFifo;

class CUpsSubsession : public CScsSubsession
/**
	This subsession object is created for each of the system server's
	clients.  It will be used to decide whether a client can perform
	the requested action.
 */
	{
public:
	static CUpsSubsession* NewL(CUpsSession &aSession, const RMessage2& aMessage);
	virtual ~CUpsSubsession();

	// implement CScsSubsession
	virtual TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);

private:
	CUpsSubsession(CUpsSession &aSession);
	void ConstructL(const RMessage2& aMessage);

	void PreparePromptL(const RMessage2& aMessage);
	void ExecutePromptL(const RMessage2& aMessage);

private:
	TThreadId iClientTid;		///< Handle to the system server's client.
	TProcessId iClientPid;		///< Handle to the system server's client.
	TServiceId iServiceId;
	RBuf iDestination;
	RBuf8 iOpaqueData;
	};

class CDecisionView;
class CUpsSession : public CScsSession, public MDbHandleClient
/**
	This session object is only used to generate subsessions for each
	of the system server's clients.
 */
	{
public:
	static CUpsSession* NewL(CUpsServer &aServer);
	virtual ~CUpsSession();

	// implement CScsSession
	virtual TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
	// override CScsSession
	virtual CScsSubsession* DoCreateSubsessionL(TInt aFunction, const RMessage2& aMessage);

	inline CUpsServer *UpsServer();

private:
	CUpsSession(CUpsServer &aServer);
	RArray<TServiceConfig> iServiceConfig;

	void PrefetchRecordAndWriteLengthToClientL(const RMessagePtr2& aMessage);
	void CleanupView();

	// Implement MDbHandleClient
	virtual void DbHandleAboutToBeDeleted();

	friend class CViewEvaluator;
	RUpsDbHandleSlave iDbViewHandle;
	CDecisionView *iManagementView; /// Used by CViewEvaluator and NextMatch functions
	CDecisionRecord *iRecord;
	};




class CPluginManager;
class CAuthoriser;
class CSwiWatcher;
class CUpsServer : public CScsServer
/**
	A single instance of this object is used to generate server-side
	sessions for the UPS client.
 */
	{
public:
	static CUpsServer* NewLC();
	virtual ~CUpsServer();

	// implement CScsServer
	virtual CScsSession* DoNewSessionL(const RMessage2& aMessage);
	// implement CScsServer
	virtual void DoPreHeapMarkOrCheckL();
	// implement CScsServer
	virtual void DoPostHeapMarkOrCheckL();

	void GateKeeperL(CAuthoriser *aAuthoriser);
	void AuthoriserDone(CAuthoriser *aAuthoriser);
	void WakeupNextPendingL();

	void DisputeRecordIdL(TUint32 aRecordId);
	void UnDisputeRecordIdL(TUint32 aRecordId);
	TBool IsRecordIdDisputed(TUint32 aRecordId) const;

	RFs iFs;
	RPolicyCacheCountedHandle iPolicyCache;
	CPluginManager* iPluginManager;

	RUpsDbHandleMaster iDbHandle;
	
private:
	CUpsServer();
	void ConstructL();
	void SetupL();
	void FreeUncompressableMemory();

	void AuthoriserDoneL(CAuthoriser *aAuthoriser);

	CSwiWatcher *iSwiWatcher;

	CAuthoriser *iCurrentDialog;
	CAuthoriserFifo *iFlurryQueue;
	CAuthoriserFifo *iFlurryQueueBeingProcessed;

	RArray<TUint> iDisputed;
	};

//
// Inline functions
//
inline CUpsServer *CUpsSession::UpsServer()
	{
	// Calls base class function and cast to correct type.
	return &static_cast<CUpsServer &>(iServer);
	}

} // End of namespace UserPromptService


#endif	// #ifndef UPSSERVER_H

