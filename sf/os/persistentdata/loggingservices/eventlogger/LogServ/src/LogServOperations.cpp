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
//

#include "LogServOperations.h"
#include "logservpanic.h"
#include "LogServView.h"
#include "LogServTaskInterface.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpEventAdd   //////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Create an operation to add an event to the log
//
CLogServOpEventAdd::CLogServOpEventAdd(MLogServTaskInterface& aTaskInterface, 
					   MLogServOperationManager& aOperationManager, 
					   const RMessage2& aMessage, 
					   CLogPackage& aPackage,
					   const TLogClientServerData& aCliServData,
					   TLogServSessionId aSessionId):
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

CLogServOpEventAdd::~CLogServOpEventAdd()
	{
	delete iEvent;
	}

//
//	Get stuff from the client and start doing the work
//
void CLogServOpEventAdd::StartL(TRequestStatus& aStatus)
	{
	// Read stuff from the client
	const TInt length = Message().GetDesLengthL(1);

	Package().ResizeL(length);
	TPtr8 pPackage(Package().Ptr());
	Message().ReadL(1,pPackage);

	// Decode the parameters we've read from the client
	iEvent = CLogEvent::NewL();
	Package().GetLogEventL(*iEvent);

	// Do the actual work
	TaskInterface().TaskEventAddL(aStatus, *iEvent, Message());
	}

//
//	Complete the client request - we need to wait for the client to request a response
//
CLogServOperationBase::TCompletionStatus CLogServOpEventAdd::CompleteProcessingL(TInt /*aFinalCompletionCode*/)
	{
	Package().SetLogEventL(*iEvent);
	const TInt size = Package().Ptr().Size();
	Complete(size);
	//
	return CLogServOperationBase::EOperationCompleteWaitForClient;
	}

//
//	Write stuff back to the client
//
void CLogServOpEventAdd::WriteL(const RMessage2& aMessage)
	{
	aMessage.WriteL(1, Package().Ptr());
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpEventChange   ///////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Create an operation to change an event in the log
//
CLogServOpEventChange::CLogServOpEventChange(MLogServTaskInterface& aTaskInterface, 
					  MLogServOperationManager& aOperationManager, 
					  const RMessage2& aMessage, 
					  CLogPackage& aPackage,
					  const TLogClientServerData& aCliServData,
					  TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

CLogServOpEventChange::~CLogServOpEventChange()
	{
	delete iEvent;
	}

//
//	Get stuff from the client and start doing the work
//
void CLogServOpEventChange::StartL(TRequestStatus& aStatus)
	{
	// Read stuff from the client
	Package().ResizeL(Message().GetDesLengthL(1));
	Message().ReadL(1, Package().Ptr());

	// Decode the parameters we've read from the client
	iEvent = CLogEvent::NewL();
	Package().GetLogEventL(*iEvent);

	// Do the actual work
	TaskInterface().TaskEventChangeL(aStatus, *iEvent, Message());
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpEventGet   //////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Create an operation to get an event in the log
//
CLogServOpEventGet::CLogServOpEventGet(MLogServTaskInterface& aTaskInterface, 
				   MLogServOperationManager& aOperationManager, 
				   const RMessage2& aMessage, 
				   CLogPackage& aPackage,
				   const TLogClientServerData& aCliServData,
				   TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

CLogServOpEventGet::~CLogServOpEventGet()
	{
	delete iEvent;
	}

//
//	Get stuff from the client and start doing the work
//
void CLogServOpEventGet::StartL(TRequestStatus& aStatus)
	{
	// Setup the event
	iEvent = CLogEvent::NewL();
	iEvent->SetId((TLogId)Message().Ptr1());

	// Do the actual work
	TaskInterface().TaskEventGetL(aStatus, *iEvent, Message());
	}

//
//	Complete the client request - we need to wait for the client to request a response
//
CLogServOperationBase::TCompletionStatus CLogServOpEventGet::CompleteProcessingL(TInt /*aFinalCompletionCode*/)
	{
	Package().SetLogEventL(*iEvent);
	const TInt size = Package().Ptr().Size();
	Complete(size);
	//
	return CLogServOperationBase::EOperationCompleteWaitForClient;
	}

//
//	Write stuff back to the client
//
void CLogServOpEventGet::WriteL(const RMessage2& aMessage)
	{
	aMessage.WriteL(1, Package().Ptr());
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpEventDelete   ///////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Create an operation to delete an event from the log
//
CLogServOpEventDelete::CLogServOpEventDelete(MLogServTaskInterface& aTaskInterface, 
					  MLogServOperationManager& aOperationManager, 
					  const RMessage2& aMessage, 
					  CLogPackage& aPackage,
					  const TLogClientServerData& aCliServData,
					  TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

void CLogServOpEventDelete::StartL(TRequestStatus& aStatus)
	{
	// Do the actual work
	const TLogId eventId = static_cast<TLogId>(Message().Int1());
	TaskInterface().TaskEventDeleteL(aStatus, eventId, Message());
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpTypeAdd   ///////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Add a new event type to the database
//
CLogServOpTypeAdd::CLogServOpTypeAdd(MLogServTaskInterface& aTaskInterface, 
				  MLogServOperationManager& aOperationManager, 
				  const RMessage2& aMessage, 
				  CLogPackage& aPackage,
				  const TLogClientServerData& aCliServData,
				  TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

CLogServOpTypeAdd::~CLogServOpTypeAdd()
	{
	delete iEventType;
	}

//
//	Get stuff from the client and start doing the work
//
void CLogServOpTypeAdd::StartL(TRequestStatus& aStatus)
	{
	// Read stuff from the client
	Package().ResizeL(Message().GetDesLengthL(1));
	Message().ReadL(1, Package().Ptr());
	

	// Decode the parameters we've read from the client
	iEventType = CLogEventType::NewL();
	Package().GetLogEventTypeL(*iEventType);

	// Do the actual work
	TaskInterface().TaskEventTypeAddL(aStatus, *iEventType);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpTypeGet   ///////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Get event type details from the database
//
CLogServOpTypeGet::CLogServOpTypeGet(MLogServTaskInterface& aTaskInterface, 
				  MLogServOperationManager& aOperationManager, 
				  const RMessage2& aMessage, 
				  CLogPackage& aPackage,
				  const TLogClientServerData& aCliServData,
				  TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

//
//	Get stuff from the client and start doing the work
//
void CLogServOpTypeGet::StartL(TRequestStatus& aStatus)
	{
	// Do the actual work
	const TUid eventTypeUid = { Message().Int1() };
 	const CLogEventType *eventType;
	TaskInterface().TaskEventTypeGetL(aStatus, eventType, eventTypeUid);
 	iEventType = const_cast<CLogEventType*>(eventType);
	}

//
//	Complete the client request - we need to wait for the client to request a response
//
CLogServOperationBase::TCompletionStatus CLogServOpTypeGet::CompleteProcessingL(TInt /*aFinalCompletionCode*/)
	{
	Package().SetLogEventTypeL(*iEventType);
	const TInt size = Package().Ptr().Size();
	Complete(size);
	//
	return CLogServOperationBase::EOperationCompleteWaitForClient;
	}

//
//	Write stuff back to the client
//
void CLogServOpTypeGet::WriteL(const RMessage2& aMessage)
	{
	aMessage.WriteL(1, Package().Ptr());
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpTypeChange   ////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Change event type details in the database
//
CLogServOpTypeChange::CLogServOpTypeChange(MLogServTaskInterface& aTaskInterface, 
					 MLogServOperationManager& aOperationManager, 
					 const RMessage2& aMessage, 
					 CLogPackage& aPackage,
					  const TLogClientServerData& aCliServData,
					  TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

CLogServOpTypeChange::~CLogServOpTypeChange()
	{
	delete iEventType;
	}

//
//	Get stuff from the client and start doing the work
//
void CLogServOpTypeChange::StartL(TRequestStatus& aStatus)
	{
	// Read stuff from the client
	Package().ResizeL(Message().GetDesLengthL(1));
	Message().ReadL(1, Package().Ptr());

	// Decode the parameters we've read from the client
	iEventType = CLogEventType::NewL();
	Package().GetLogEventTypeL(*iEventType);

	// Do the actual work
	TaskInterface().TaskEventTypeChangeL(aStatus, *iEventType);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpTypeDelete   ////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Delete event type details from the database
//
CLogServOpTypeDelete::CLogServOpTypeDelete(MLogServTaskInterface& aTaskInterface, 
					 MLogServOperationManager& aOperationManager, 
					 const RMessage2& aMessage, 
					 CLogPackage& aPackage,
					 const TLogClientServerData& aCliServData,
					 TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

//
//	Get stuff from the client and start doing the work
//
void CLogServOpTypeDelete::StartL(TRequestStatus& aStatus)
	{
	// Do the actual work
	const TUid eventTypeUid = { Message().Int1() };
	TaskInterface().TaskEventTypeDeleteL(aStatus, eventTypeUid);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpConfigGet   /////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Get database config
//
CLogServOpConfigGet::CLogServOpConfigGet(MLogServTaskInterface& aTaskInterface, 
					MLogServOperationManager& aOperationManager, 
					const RMessage2& aMessage, 
					CLogPackage& aPackage,
				    const TLogClientServerData& aCliServData,
					TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

//
//	Start doing the work
//
void CLogServOpConfigGet::StartL(TRequestStatus& aStatus)
	{
	// Do the actual work
	TaskInterface().TaskConfigGetL(aStatus, iConfig);
	}

//
//	Complete the client request - we need to wait for the client to request a response
//
CLogServOperationBase::TCompletionStatus CLogServOpConfigGet::CompleteProcessingL(TInt /*aFinalCompletionCode*/)
	{
	Package().SetLogConfigL(iConfig);
	const TInt size = Package().Ptr().Size();
	Complete(size);
	//
	return CLogServOperationBase::EOperationCompleteWaitForClient;
	}

// Write stuff back to the client
void CLogServOpConfigGet::WriteL(const RMessage2& aMessage)
	{
	aMessage.WriteL(1, Package().Ptr());
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpConfigChange   //////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Change database config
//
CLogServOpConfigChange::CLogServOpConfigChange(MLogServTaskInterface& aTaskInterface, 
					   MLogServOperationManager& aOperationManager, 
					   const RMessage2& aMessage, 
					   CLogPackage& aPackage,
					   const TLogClientServerData& aCliServData,
					   TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

//
//	Decode parameters from a client and start doing the work
//
void CLogServOpConfigChange::StartL(TRequestStatus& aStatus)
	{
	// Read stuff from the client
	Package().ResizeL(Message().GetDesLengthL(1));
	Message().ReadL(1, Package().Ptr()); 

	// Decode the parameters we've read from the client
	Package().GetLogConfigL(iConfig);

	// Do the actual work
	TaskInterface().TaskConfigChangeL(aStatus, iConfig);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpClearLog   //////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Clear the event log
//
CLogServOpClearLog::CLogServOpClearLog(MLogServTaskInterface& aTaskInterface, 
				   MLogServOperationManager& aOperationManager, 
				   const RMessage2& aMessage, 
				   CLogPackage& aPackage,
				   const TLogClientServerData& aCliServData,
				   TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

//
//	Decode parameters from the client and start doing the work
//
void CLogServOpClearLog::StartL(TRequestStatus& aStatus)
	{
	// Do the actual work
	TUint p2 = (TUint)Message().Ptr2();
	TUint p1 = (TUint)Message().Ptr1();
	const TTime time(MAKE_TINT64(p2, p1));
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TSimId simId = (TSimId)Message().Int3();	
	TaskInterface().TaskClearLogL(aStatus, time, simId);
#else
	TaskInterface().TaskClearLogL(aStatus, time);
#endif
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpClearRecent   ///////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Clear a recent list
//
CLogServOpClearRecent::CLogServOpClearRecent(MLogServTaskInterface& aTaskInterface, 
					  MLogServOperationManager& aOperationManager, 
					  const RMessage2& aMessage, 
					  CLogPackage& aPackage,
					  const TLogClientServerData& aCliServData,
					  TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

//
//	Decode parameters from the client and start doing the work
//
void CLogServOpClearRecent::StartL(TRequestStatus& aStatus)
	{
	// Do the actual work
	const TLogRecentList recentList = static_cast<TLogRecentList>(Message().Int1());
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TSimId simId = (TSimId)Message().Int2();
	TaskInterface().TaskClearRecentL(aStatus, recentList, simId);
#else
	TaskInterface().TaskClearRecentL(aStatus, recentList);
#endif
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpMaintenance   ///////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Kicks the server to perform a maintain on the database. 
//	Always done after an op as necessary
//
CLogServOpMaintenance::CLogServOpMaintenance(MLogServTaskInterface& aTaskInterface, 
					  MLogServOperationManager& aOperationManager, 
					  const RMessage2& aMessage, 
					  CLogPackage& aPackage,
					  const TLogClientServerData& aCliServData,
					  TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId)
	{
	}

//
//	Just complete ourselves
//
void CLogServOpMaintenance::StartL(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpViewSetup   /////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Setup a view
CLogServOpViewSetup::CLogServOpViewSetup(MLogServTaskInterface& aTaskInterface, 
		            MLogServOperationManager& aOperationManager, 
                    const RMessage2& aMessage, 
                    CLogPackage& aPackage, 
                    CLogServViewBase& aView,
					const TLogClientServerData& aCliServData,
					TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId),
	iView(aView)
	{
	}

//
//	Decode parameters from the client and start doing the work
//
void CLogServOpViewSetup::StartL(TRequestStatus& aStatus)
	{
	// Read the filter construction type from the shared data slot
	const TInt filterConstructionType = ClientServerData().iDataSlot1;
	if	(filterConstructionType == ELogFilterConstructFilterByFilterFieldByField || filterConstructionType == ELogFilterConstructFieldByFieldFilterByFilter)
		{
		// Do the actual work
		iView.SetupL(Message(), static_cast<TLogFilterConstructionType>(filterConstructionType));
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		}
	else
		::PanicClientL(Message(), ELogBadFilterConstructionType);
	}

//
//	Complete the client request telling it the number of entries in the view
//
CLogServOperationBase::TCompletionStatus CLogServOpViewSetup::CompleteProcessingL(TInt /*aFinalCompletionCode*/)
	{
	const TInt viewCount = iView.Count();
	Complete(viewCount);
	//
	return CLogServOperationBase::EOperationComplete;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpViewEventRemove   ///////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
//	Remove an event from a view
//
CLogServOpViewEventRemove::CLogServOpViewEventRemove(MLogServTaskInterface& aTaskInterface, 
						  MLogServOperationManager& aOperationManager, 
						  const RMessage2& aMessage, 
						  CLogPackage& aPackage, 
						  CLogServViewBase& aView,
						  const TLogClientServerData& aCliServData,
						  TLogServSessionId aSessionId):
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId),
	iView(aView)
	{
	}

//
//	Decode parameters from the client and start doing the work
//
void CLogServOpViewEventRemove::StartL(TRequestStatus& aStatus)
	{
	iView.RemoveL(Message());
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpViewClearDuplicates   ///////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLogServOpViewClearDuplicates::CLogServOpViewClearDuplicates(MLogServTaskInterface& aTaskInterface, 
							  MLogServOperationManager& aOperationManager, 
							  const RMessage2& aMessage, 
							  CLogPackage& aPackage, 
							  CLogServViewBase& aView,
							  const TLogClientServerData& aCliServData,
							  TLogServSessionId aSessionId):
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId),
	iView(aView)
	{
	}

void CLogServOpViewClearDuplicates::StartL(TRequestStatus& aStatus)
	{
	iView.ClearDuplicatesL(Message());
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpViewSetFlags   //////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLogServOpViewSetFlags::CLogServOpViewSetFlags(MLogServTaskInterface& aTaskInterface, 
					   MLogServOperationManager& aOperationManager, 
					   const RMessage2& aMessage, 
					   CLogPackage& aPackage, 
					   CLogServViewBase& aView,
					   const TLogClientServerData& aCliServData,
					   TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId),
	iView(aView)
	{
	}

void CLogServOpViewSetFlags::StartL(TRequestStatus& aStatus)
	{
	iView.SetFlagsL(Message());
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   CLogServOpViewWindowFetcher   /////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLogServOpViewWindowFetcher::CLogServOpViewWindowFetcher(MLogServTaskInterface& aTaskInterface, 
							MLogServOperationManager& aOperationManager, 
							const RMessage2& aMessage, 
							CLogPackage& aPackage, 
							CLogServViewBase& aView,
							const TLogClientServerData& aCliServData,
							TLogServSessionId aSessionId) :
	CLogServOperationBase(aTaskInterface, aOperationManager, aMessage, aPackage, aCliServData, aSessionId),
	iView(aView)
	{
	}

void CLogServOpViewWindowFetcher::StartL(TRequestStatus& aStatus)
	{
	// Get the window
	TLogTransferWindow window;
	TPckg<TLogTransferWindow> pWindow(window);
	Message().ReadL(2, pWindow);

	// Do the actual work
	TaskInterface().TaskBuildWindowL(aStatus, iView, window, Message());
	}

//
//	Complete the client request - the final completion code tells us, in this instance, how many records were
//	read from the view.
//
CLogServOperationBase::TCompletionStatus CLogServOpViewWindowFetcher::CompleteProcessingL(TInt aFinalCompletionCode)
	{
	Complete(aFinalCompletionCode);

	// Don't need to wait for anything - we've already written back to the client's address space
	// when we built the window.
	return CLogServOperationBase::EOperationComplete;
	}
