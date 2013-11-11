// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LOGSERVEROPERATIONS.H
// 
//

#ifndef __LOGSERVEROPERATIONS_H__
#define __LOGSERVEROPERATIONS_H__

#include "LogServOperationBase.h"

// Classes referenced
class CLogEvent;
class CLogServViewBase;
class CLogServViewRecent;

/**
CLogServOperationBase derived class that initiates the "add event" operation.  
 
@see CLogServOperationBase
@see CLogAddEvent
@internalComponent
*/
class CLogServOpEventAdd : public CLogServOperationBase
	{
public:
	CLogServOpEventAdd(MLogServTaskInterface& aTaskInterface, 
					   MLogServOperationManager& aOperationManager, 
					   const RMessage2& aMessage, 
					   CLogPackage& aPackage,
					   const TLogClientServerData& aCliServData,
					   TLogServSessionId aSessionId);
	~CLogServOpEventAdd();
	void StartL(TRequestStatus& aStatus);
	TCompletionStatus CompleteProcessingL(TInt aFinalCompletionCode);
	void WriteL(const RMessage2& aMessage);
	//
private:
	CLogEvent* iEvent;
	};

/**
CLogServOperationBase derived class that initiates the "change event" operation.  
 
@see CLogServOperationBase
@see CLogChangeEvent
@internalComponent
*/
class CLogServOpEventChange : public CLogServOperationBase
	{
public:
	CLogServOpEventChange(MLogServTaskInterface& aTaskInterface, 
						  MLogServOperationManager& aOperationManager, 
						  const RMessage2& aMessage, 
						  CLogPackage& aPackage,
						  const TLogClientServerData& aCliServData,
						  TLogServSessionId aSessionId);
	~CLogServOpEventChange();
	void StartL(TRequestStatus& aStatus);
	//
private:
	CLogEvent* iEvent;
	};

/**
CLogServOperationBase derived class that initiates the "get event" operation.  
 
@see CLogServOperationBase
@see CLogGetEvent
@internalComponent
*/
class CLogServOpEventGet : public CLogServOperationBase
	{
public:
	CLogServOpEventGet(MLogServTaskInterface& aTaskInterface, 
					   MLogServOperationManager& aOperationManager, 
					   const RMessage2& aMessage, 
					   CLogPackage& aPackage,
					   const TLogClientServerData& aCliServData,
					   TLogServSessionId aSessionId);
	~CLogServOpEventGet();
	void StartL(TRequestStatus& aStatus);
	TCompletionStatus CompleteProcessingL(TInt aFinalCompletionCode);
	void WriteL(const RMessage2& aMessage);
	//
private:
	CLogEvent* iEvent;
	};

/**
CLogServOperationBase derived class that initiates the "delete event" operation.  
 
@see CLogServOperationBase
@see CLogDeleteEvent
@internalComponent
*/
class CLogServOpEventDelete : public CLogServOperationBase
	{
public:
	CLogServOpEventDelete(MLogServTaskInterface& aTaskInterface, 
						  MLogServOperationManager& aOperationManager, 
						  const RMessage2& aMessage, 
						  CLogPackage& aPackage,
						  const TLogClientServerData& aCliServData,
						  TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	};

/**
CLogServOperationBase derived class that initiates the "add event type" operation.  
 
@see CLogServOperationBase
@see CLogServCacheTypes
@internalComponent
*/
class CLogServOpTypeAdd : public CLogServOperationBase
	{
public:
	CLogServOpTypeAdd(MLogServTaskInterface& aTaskInterface, 
					  MLogServOperationManager& aOperationManager, 
					  const RMessage2& aMessage, 
					  CLogPackage& aPackage,
					  const TLogClientServerData& aCliServData,
					  TLogServSessionId aSessionId);
	~CLogServOpTypeAdd();
	void StartL(TRequestStatus& aStatus);
	//
private:
	CLogEventType* iEventType;
	};

/**
CLogServOperationBase derived class that initiates the "get event type" operation.  
 
@see CLogServOperationBase
@see CLogServCacheTypes
@internalComponent
*/
class CLogServOpTypeGet : public CLogServOperationBase
	{
public:
	CLogServOpTypeGet(MLogServTaskInterface& aTaskInterface, 
					  MLogServOperationManager& aOperationManager, 
					  const RMessage2& aMessage, 
					  CLogPackage& aPackage,
					  const TLogClientServerData& aCliServData,
					  TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	TCompletionStatus CompleteProcessingL(TInt aFinalCompletionCode);
	void WriteL(const RMessage2& aMessage);
	//
private:
	CLogEventType* iEventType;
	};

/**
CLogServOperationBase derived class that initiates the "change event type" operation.  
 
@see CLogServOperationBase
@see CLogServCacheTypes
@internalComponent
*/
class CLogServOpTypeChange : public CLogServOperationBase
	{
public:
	CLogServOpTypeChange(MLogServTaskInterface& aTaskInterface, 
						 MLogServOperationManager& aOperationManager, 
						 const RMessage2& aMessage, 
						 CLogPackage& aPackage,
						  const TLogClientServerData& aCliServData,
						  TLogServSessionId aSessionId);
	~CLogServOpTypeChange();
	void StartL(TRequestStatus& aStatus);
	//
private:
	CLogEventType* iEventType;
	};

/**
CLogServOperationBase derived class that initiates the "delete event type" operation.  
 
@see CLogServOperationBase
@see CLogServCacheTypes
@internalComponent
*/
class CLogServOpTypeDelete : public CLogServOperationBase
	{
public:
	CLogServOpTypeDelete(MLogServTaskInterface& aTaskInterface, 
						 MLogServOperationManager& aOperationManager, 
						 const RMessage2& aMessage, 
						 CLogPackage& aPackage,
						  const TLogClientServerData& aCliServData,
						  TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	};

/**
CLogServOperationBase derived class that initiates the "get config" operation.  
 
@see CLogServOperationBase
@see CLogServCacheConfig
@internalComponent
*/
class CLogServOpConfigGet : public CLogServOperationBase
	{
public:
	CLogServOpConfigGet(MLogServTaskInterface& aTaskInterface, 
						MLogServOperationManager& aOperationManager, 
						const RMessage2& aMessage, 
						CLogPackage& aPackage,
						  const TLogClientServerData& aCliServData,
						  TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	TCompletionStatus CompleteProcessingL(TInt aFinalCompletionCode);
	void WriteL(const RMessage2& aMessage);

	//
private:
	TLogConfig iConfig;
	};

/**
CLogServOperationBase derived class that initiates the "change config" operation.  
 
@see CLogServOperationBase
@see CLogServCacheConfig
@internalComponent
*/
class CLogServOpConfigChange : public CLogServOperationBase
	{
public:
	CLogServOpConfigChange(MLogServTaskInterface& aTaskInterface, 
						   MLogServOperationManager& aOperationManager, 
						   const RMessage2& aMessage, 
						   CLogPackage& aPackage,
						   const TLogClientServerData& aCliServData,
						   TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	//
private:
	TLogConfig iConfig;
	};

/**
CLogServOperationBase derived class that initiates the "clear events log" operation.  
 
@see CLogServOperationBase
@see CLogServDatabaseDriver
@internalComponent
*/
class CLogServOpClearLog : public CLogServOperationBase
	{
public:
	CLogServOpClearLog(MLogServTaskInterface& aTaskInterface, 
					   MLogServOperationManager& aOperationManager, 
					   const RMessage2& aMessage, 
					   CLogPackage& aPackage,
					   const TLogClientServerData& aCliServData,
					   TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	};

/**
CLogServOperationBase derived class that initiates the "clear recent list" operation.  
 
@see CLogServOperationBase
@see CLogServDatabaseDriver
@internalComponent
*/
class CLogServOpClearRecent : public CLogServOperationBase
	{
public:
	CLogServOpClearRecent(MLogServTaskInterface& aTaskInterface, 
						  MLogServOperationManager& aOperationManager, 
						  const RMessage2& aMessage, 
						  CLogPackage& aPackage,
						  const TLogClientServerData& aCliServData,
						  TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	};

/**
CLogServOperationBase derived class that initiates database maintenance tasks such as:
database recover, database compaction, etc.  

@see CLogServOperationBase
@see CLogMaintenance
@internalComponent
*/
class CLogServOpMaintenance : public CLogServOperationBase
	{
public:
	CLogServOpMaintenance(MLogServTaskInterface& aTaskInterface, 
						  MLogServOperationManager& aOperationManager, 
						  const RMessage2& aMessage, 
						  CLogPackage& aPackage,
						  const TLogClientServerData& aCliServData,
						  TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	};

/**
CLogServOperationBase derived class that initiates the "setup view" operation.

@see CLogServOperationBase
@see CLogServViewBase
@see CLogServViewLockObserver
@see CLogServViewEvent
@see CLogServViewRecent
@see CLogServViewDuplicate
@internalComponent
*/
class CLogServOpViewSetup : public CLogServOperationBase
	{
public:
	CLogServOpViewSetup(MLogServTaskInterface& aTaskInterface, 
			            MLogServOperationManager& aOperationManager, 
                        const RMessage2& aMessage, 
                        CLogPackage& aPackage, 
                        CLogServViewBase& aView,
 					    const TLogClientServerData& aCliServData,
 					    TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	//
	TCompletionStatus CompleteProcessingL(TInt aFinalCompletionCode);
	//
private:
	CLogServViewBase& iView;
	};

/**
CLogServOperationBase derived class that initiates the "remove event from view" operation.

@see CLogServOperationBase
@see CLogServViewBase
@see CLogServViewLockObserver
@see CLogServViewEvent
@see CLogServViewRecent
@see CLogServViewDuplicate
@internalComponent
*/
class CLogServOpViewEventRemove : public CLogServOperationBase
	{
public:
	CLogServOpViewEventRemove(MLogServTaskInterface& aTaskInterface, 
							  MLogServOperationManager& aOperationManager, 
							  const RMessage2& aMessage, 
							  CLogPackage& aPackage, 
							  CLogServViewBase& aView,
							  const TLogClientServerData& aCliServData,
							  TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	//
private:
	CLogServViewBase& iView;
	};

/**
CLogServOperationBase derived class that initiates the "clear duplicates from view" operation.

@see CLogServOperationBase
@see CLogServViewBase
@see CLogServViewLockObserver
@see CLogServViewEvent
@see CLogServViewRecent
@see CLogServViewDuplicate
@internalComponent
*/
class CLogServOpViewClearDuplicates : public CLogServOperationBase
	{
public:
	CLogServOpViewClearDuplicates(MLogServTaskInterface& aTaskInterface, 
								  MLogServOperationManager& aOperationManager, 
								  const RMessage2& aMessage, 
								  CLogPackage& aPackage, 
								  CLogServViewBase& aView,
								  const TLogClientServerData& aCliServData,
								  TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	//
private:
	CLogServViewBase& iView;
	};

/**
CLogServOperationBase derived class that initiates the "set view events flags" operation.

@see CLogServOperationBase
@see CLogServViewBase
@see CLogServViewLockObserver
@see CLogServViewEvent
@see CLogServViewRecent
@see CLogServViewDuplicate
@internalComponent
*/
class CLogServOpViewSetFlags : public CLogServOperationBase
	{
public:
	CLogServOpViewSetFlags(MLogServTaskInterface& aTaskInterface, 
						   MLogServOperationManager& aOperationManager, 
						   const RMessage2& aMessage, 
						   CLogPackage& aPackage, 
						   CLogServViewBase& aView,
						   const TLogClientServerData& aCliServData,
						   TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	//
private:
	CLogServViewBase& iView;
	};

/**
CLogServOperationBase derived class that initiates the "get events window" operation.

@see CLogServOperationBase
@see CLogServViewBase
@see CLogServViewLockObserver
@see CLogServViewEvent
@see CLogServViewRecent
@see CLogServViewDuplicate
@internalComponent
*/
class CLogServOpViewWindowFetcher : public CLogServOperationBase
	{
public:
	CLogServOpViewWindowFetcher(MLogServTaskInterface& aTaskInterface, 
								MLogServOperationManager& aOperationManager, 
								const RMessage2& aMessage, 
								CLogPackage& aPackage, 
								CLogServViewBase& aView,
								const TLogClientServerData& aCliServData,
								TLogServSessionId aSessionId);
	void StartL(TRequestStatus& aStatus);
	TCompletionStatus CompleteProcessingL(TInt aFinalCompletionCode);
	//
private:
	CLogServViewBase& iView;
	};


#endif
