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

#ifndef OMXILINCONTEXTCALLBACKMANAGER_H
#define OMXILINCONTEXTCALLBACKMANAGER_H

#include "omxilcallbackmanager.h"

/**
   OpenMAX IL call backs and buffer marks manager. This is a synchronous
   implementation of MOmxILCallbackManagerIf.

 */
NONSHARABLE_CLASS(COmxILInContextCallbackManager) :
	public CBase,
	public MOmxILCallbackManagerIf,
	private XOmxILCallbackManagerIfImpl
	{

public:

	static COmxILInContextCallbackManager* NewL(
		OMX_HANDLETYPE apComponentHandle,
		OMX_PTR apAppData,
		OMX_CALLBACKTYPE* apCallbacks);

	~COmxILInContextCallbackManager();

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


private:

	class TOmxILBuffer
		{

	public:

		OMX_BUFFERHEADERTYPE* ipBufferHeader;
		OMX_U32 iLocalPortIndex;
		OMX_DIRTYPE iLocalPortDirection;

	// Default Constructor
	inline TOmxILBuffer();

	// Constructor
	inline TOmxILBuffer(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection);

		};

private:

	// Convenience typedef
	typedef RArray<TOmxILBuffer> RCbMgrBufferQueue;


private:

	COmxILInContextCallbackManager(OMX_HANDLETYPE apComponentHandle,
								   OMX_PTR apAppData,
								   OMX_CALLBACKTYPE* apCallbacks);
	void ConstructL();

	void LockCallbackManager();
	void UnlockCallbackManager();

	void SignalOrPropagateBufferMarks(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_U32 aLocalPortIndex);

	void ProcessBufferDoneNotification(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection);

	void FlushQueue(RCbMgrBufferQueue& aQueue);

	TBool RemoveBuffersByBufferHeader(
		RCbMgrBufferQueue& aQueue,
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		const OMX_DIRTYPE aDirection);

	void FlushBuffersByPortIndex(RCbMgrBufferQueue& aQueue,
								 const OMX_U32 aLocalPortIndex);


private:

	RFastLock iLock;

	// Queue of buffer done notifications that need to be queued during
	// OMX_StatePaused state
	RCbMgrBufferQueue iPendingQueue;

	// Flag to enable unconditional flushing of buffer done notifications
	TBool iFlushPendingQueue;

	OMX_STATETYPE iCurrentState;
	OMX_STATETYPE iPreviousState;

	};

inline
COmxILInContextCallbackManager::TOmxILBuffer::TOmxILBuffer()
	:
	ipBufferHeader(0),
	iLocalPortIndex(0),
	iLocalPortDirection(OMX_DirMax)
	{
	}

inline
COmxILInContextCallbackManager::TOmxILBuffer::TOmxILBuffer(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_U32 aLocalPortIndex,
	OMX_DIRTYPE aLocalPortDirection
	)
	:
	ipBufferHeader(apBufferHeader),
	iLocalPortIndex(aLocalPortIndex),
	iLocalPortDirection(aLocalPortDirection)
	{
	}


#endif // OMXILINCONTEXTCALLBACKMANAGER_H
