// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
*/

#ifndef OMXILCALLBACKMANAGER_H
#define OMXILCALLBACKMANAGER_H

#include <e32base.h>

#include "omxilcallbackmanagerif.h"
#include "omxilcallbackmanagerifimpl.h"
#include "primsgqueue.h"


/**
   Call Back Manager Panic category
*/
_LIT(KOmxILCallbackManagerPanicCategory, "OmxILCallbackManager");


/**
   OpenMAX IL call backs and buffer marks manager. This is an asynchronous
   implementation of MOmxILCallbackManagerIf.  It keeps and manages the IL
   Client callback information as well as the buffer mark propagation info.

 */
NONSHARABLE_CLASS(COmxILCallbackManager) :
	public CActive,
	public MOmxILCallbackManagerIf,
	private XOmxILCallbackManagerIfImpl
	{

	// Declaration of nested private command classes
	class CCallbackCommand;
	class CCompHandleRegistrationCommand;
	class CClientCallbacksRegistrationCommand;
	class CTunnelCallbackRegistrationCommand;
	class CBufferMarkPropagationRegistrationCommand;
	class CBufferRemovalCommand;
	class CEventCallbackCommand;
	class CBufferDoneCallbackCommand;
	class CPortSettingsChangeCommand;
#ifdef _OMXIL_COMMON_IL516C_ON
	class CEjectBuffersRequestCommand;
#endif
	// Note that the following friends don't break COmxILCallbackManager's
	// interface as all friends below are COmxILCallbackManager's private
	// nested classes and therefore they are logically part of
	// COmxILCallbackManager implementation
	friend class CCompHandleRegistrationCommand;
	friend class CClientCallbacksRegistrationCommand;
	friend class CTunnelCallbackRegistrationCommand;
	friend class CBufferMarkPropagationRegistrationCommand;
	friend class CBufferRemovalCommand;
	friend class CEventCallbackCommand;
	friend class CBufferDoneCallbackCommand;
	friend class CPortSettingsChangeCommand;
#ifdef _OMXIL_COMMON_IL516C_ON
	friend class CEjectBuffersRequestCommand;
#endif

public:

	static COmxILCallbackManager* NewL(
		OMX_HANDLETYPE apComponentHandle,
		OMX_PTR apAppData,
		OMX_CALLBACKTYPE* apCallbacks);

	~COmxILCallbackManager();

	void SetPortManager(MOmxILPortManagerIf& apPortManager);

	void SetFsm(COmxILFsm& apFsm);


	//
	// Methods for Callback Registration (from MOmxILCallbackManagerIf)
	//

	OMX_ERRORTYPE RegisterComponentHandle(
		OMX_HANDLETYPE aComponentHandle);

	OMX_ERRORTYPE RegisterILClientCallbacks(
		const OMX_CALLBACKTYPE* apCallbacks,
		const OMX_PTR apAppData);

	OMX_ERRORTYPE RegisterTunnelCallback(
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection,
		OMX_HANDLETYPE aTunnelledComponentHandle,
		OMX_U32 aTunnelledPortIndex
		);

	OMX_ERRORTYPE DeregisterTunnelCallback(
		OMX_U32 aLocalPortIndex);

	OMX_ERRORTYPE RegisterBufferMarkPropagationPort(
		OMX_U32 aPortIndex,
		OMX_U32 aPropagationPortIndex);

	TBool BufferRemovalIndication(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection);


	//
	// Methods for Callback Notification (from MOmxILCallbackManagerIf)
	//

	OMX_ERRORTYPE TransitionCompleteNotification(
		OMX_STATETYPE aOmxState);

	OMX_ERRORTYPE CommandCompleteNotification(
		OMX_COMMANDTYPE aOmxCommand,
		OMX_U32 aOmxPortIndex);

#ifdef _OMXIL_COMMON_IL516C_ON
	OMX_ERRORTYPE EjectBuffersRequest(
		OMX_U32 aLocalOmxPortIndex);
#endif

	//
	// Methods for Callback Notification (from MOmxILCallbackManagerIf)
	//

	OMX_ERRORTYPE ErrorEventNotification(
		OMX_ERRORTYPE aOmxError);

	OMX_ERRORTYPE EventNotification(
		OMX_EVENTTYPE aEvent,
		TUint32 aData1,
		TUint32 aData2,
		OMX_STRING aExtraInfo);

	OMX_ERRORTYPE BufferDoneNotification(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection);

	OMX_ERRORTYPE ClockBufferDoneNotification(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection);

	OMX_ERRORTYPE PortSettingsChangeNotification(
		OMX_U32 aLocalPortIndex,
		TUint aPortSettingsIndex,
		const TDesC8& aPortSettings);

	//
	// From CActive
	//
private:	
	void RunL();
	void DoCancel();

private:

	// Convenience typedef
	typedef RPriMsgQueue<CCallbackCommand*> RCbCmdPriQue;

	/**
	   COmxILCallbackManager's priority-based command queue.

	   This queue is based on RPriMsgQueue but it is specialized to support the
	   search and deletion of Buffer Done Callback Commands using some specific
	   search criteria like buffer headers or port indexes.
	 */
	class RCallbackManagerQueue : public RCbCmdPriQue
		{

	public:

		TBool RemoveBufferDoneCbCommandsByBufferHeader(
			COmxILCallbackManager& aCbMgr,
			const OMX_BUFFERHEADERTYPE* apBufferHeader,
			OMX_DIRTYPE aDirection);

		TBool RemoveBufferDoneCbCommandsByPortIndex(
			COmxILCallbackManager& aCbMgr,
			OMX_U32 aLocalPortIndex);

		TBool ExecuteBufferDoneCbCommandsByPortIndex(
			COmxILCallbackManager& aCbMgr,
			OMX_U32 aLocalPortIndex);


    };

private:

	static const TInt KMaxMsgQueueEntries = 30;

private:

	COmxILCallbackManager(OMX_HANDLETYPE apComponentHandle,
						  OMX_PTR apAppData,
						  OMX_CALLBACKTYPE* apCallbacks);
	void ConstructL();


	OMX_ERRORTYPE SendBufferDoneNotification(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection,
		TInt aPriority);

	void ProcessQueue(RCallbackManagerQueue& aQueue);

	void CleanUpQueue(RCallbackManagerQueue& aQueue);
	
	OMX_ERRORTYPE EventNotificationImpl(
		OMX_EVENTTYPE aEvent,
		TUint32 aData1,
		TUint32 aData2,
		OMX_STRING aExtraInfo);

private:

	// COmxILCallbackManager priority queue.
	RCallbackManagerQueue iCommandQueue;

	// Queue of buffer done notifications that need to be queued during
	// OMX_StatePaused state
	RCallbackManagerQueue iPendingQueue;

	// Flag to enable unconditional flushing of buffer done notifications
	TBool iFlushPendingQueue;

	OMX_STATETYPE iCurrentState;
	OMX_STATETYPE iPreviousState;

	};


//
// COmxILCallbackManager nested classes
//

/**
  Abstract Callback Manager's command class.
 */
class COmxILCallbackManager::CCallbackCommand : public CBase
	{

public:

	enum TCommandPriority
		{
		EPriorityLow,
		EPriorityNormal,
		EPriorityHigh,
		EPriorityVeryHigh
		};

public:

	inline CCallbackCommand(TInt aPriority);

	/**
	   Operator() method that must be implemented to perform the specific
	   command logic.

	   @param [in] aCbMgr The Callback Manager object used as context to the
	   command operation
	*/
	virtual void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred) = 0;

	virtual TBool DoRemoveBufferDoneCbCommandByBufferHeader(
		COmxILCallbackManager& aCbMgr,
		const OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection);

	virtual TBool DoRemoveBufferDoneCbCommandByPortIndex(
		COmxILCallbackManager& aCbMgr,
		OMX_U32 aLocalPortIndex);

	virtual TBool DoExecuteBufferDoneCbCommandByPortIndex(
		COmxILCallbackManager& aCbMgr,
		OMX_U32 aLocalPortIndex);

	inline TInt Priority();

public:

	TInt iPriority;

	};

/**
  Callback Manager's command class for registering component handles.
 */
class COmxILCallbackManager::CCompHandleRegistrationCommand :
	public COmxILCallbackManager::CCallbackCommand
	{

public:

	inline CCompHandleRegistrationCommand(OMX_HANDLETYPE aComponentHandle);

	void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred);

private:

	OMX_COMPONENTTYPE* ipHandle; // Not owned

	};

/**
  Callback Manager's command class for registering IL Client callbacks .
 */
class COmxILCallbackManager::CClientCallbacksRegistrationCommand :
	public COmxILCallbackManager::CCallbackCommand
	{

public:

	inline CClientCallbacksRegistrationCommand(
		const OMX_CALLBACKTYPE*& apCallbacks,
		const OMX_PTR& apAppData);

	void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred);

private:

	const OMX_CALLBACKTYPE* ipCallbacks; // not owned
	const OMX_PTR ipAppData; // not owned

	};

/**
  Callback Manager's command class for registering tunnel callbacks.
 */
class COmxILCallbackManager::CTunnelCallbackRegistrationCommand :
	public COmxILCallbackManager::CCallbackCommand
	{

public:

	inline CTunnelCallbackRegistrationCommand(
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection,
		OMX_HANDLETYPE aTunnelledComponentHandle,
		OMX_U32 aTunnelledPortIndex);

	void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred);


public:

	TTunnelRegistrationInfo iTunnelInfo;

	};

/**
  Callback Manager's command class for registering buffer marks info .
 */
class COmxILCallbackManager::CBufferMarkPropagationRegistrationCommand :
	public COmxILCallbackManager::CCallbackCommand
	{

public:

	inline CBufferMarkPropagationRegistrationCommand(
		OMX_U32 aPortIndex,
		OMX_U32 aPropagationPortIndex);

	void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred);

public:

	TBufferMarkPropagationInfo iMarkPropagationInfo;

	};

/**
  Callback Manager's command class for buffer header removal.
 */
class COmxILCallbackManager::CBufferRemovalCommand :
	public COmxILCallbackManager::CCallbackCommand
	{

public:

	inline CBufferRemovalCommand(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection);

	void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred);

public:

	OMX_BUFFERHEADERTYPE* ipBufferHeader;
	OMX_DIRTYPE iDirection;

	};

/**
  Callback Manager's command class for notification of OpenMAX IL events .
 */
class COmxILCallbackManager::CEventCallbackCommand :
	public COmxILCallbackManager::CCallbackCommand
	{

public:

	inline CEventCallbackCommand(OMX_EVENTTYPE aEvent,
								 TUint32 aData1,
								 TUint32 aData2,
								 OMX_STRING aExtraInfo);

	void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred);

private:

	OMX_EVENTTYPE iEvent;
	TUint32 iData1;
	TUint32 iData2;
	OMX_STRING iExtraInfo;

	};

/**
  Callback Manager's command class for notification of buffer done events .
 */
class COmxILCallbackManager::CBufferDoneCallbackCommand :
	public COmxILCallbackManager::CCallbackCommand
	{

public:

	inline CBufferDoneCallbackCommand(OMX_BUFFERHEADERTYPE* apBufferHeader,
									  OMX_U32 aLocalPortIndex,
									  OMX_DIRTYPE aLocalPortDirection,
									  TInt aPriority = CCallbackCommand::EPriorityNormal);

	void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred);

	TBool DoRemoveBufferDoneCbCommandByBufferHeader(
		COmxILCallbackManager& aCbMgr,
		const OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection);

	TBool DoRemoveBufferDoneCbCommandByPortIndex(
		COmxILCallbackManager& aCbMgr,
		OMX_U32 aLocalPortIndex);

	TBool DoExecuteBufferDoneCbCommandByPortIndex(
		COmxILCallbackManager& aCbMgr,
		OMX_U32 aLocalPortIndex);

private:

	OMX_BUFFERHEADERTYPE* ipBufferHeader; // not owned
	OMX_U32 iLocalPortIndex;
	OMX_DIRTYPE iLocalPortDirection;

	};

/**
  Callback Manager's command class for notification of port settings change
  events .
 */
class COmxILCallbackManager::CPortSettingsChangeCommand :
	public COmxILCallbackManager::CCallbackCommand
	{

public:

	inline CPortSettingsChangeCommand(OMX_U32 aLocalPortIndex,
									  TUint aPortSettingsIndex,
									  HBufC8*& apPortSettings);

	~CPortSettingsChangeCommand();

	void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred);

private:

	OMX_U32 iLocalPortIndex;
	TUint iPortSettingsIndex;
	HBufC8* ipPortSettings; // This is owned by this class

	};

#ifdef _OMXIL_COMMON_IL516C_ON
/**
  Callback Manager's command class for requesting buffer ejection from tunnelled component.
 */
class COmxILCallbackManager::CEjectBuffersRequestCommand :
	public COmxILCallbackManager::CCallbackCommand
	{

public:

	inline CEjectBuffersRequestCommand(OMX_U32 aLocalOmxPortIndex);

	void operator()(COmxILCallbackManager& aCbMgr, TBool& aHasBeenDeferred);

private:

	OMX_U32 iLocalOmxPortIndex;

	};
#endif


#include "omxilcallbackmanager.inl"

#endif // OMXILCALLBACKMANAGER_H
