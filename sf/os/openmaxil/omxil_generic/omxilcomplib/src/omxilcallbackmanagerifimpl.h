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

#ifndef OMXILCALLBACKMANAGERIFIMPL_H
#define OMXILCALLBACKMANAGERIFIMPL_H

#include <openmax/il/khronos/v1_x/OMX_Component.h>

/**
   Call Back ManagerIf Impl Panic category
*/
_LIT(KOmxILCallbackManagerIfImplPanicCategory, "OmxILCallbackManagerIfImpl");

// Forward declarations
class MOmxILPortManagerIf;
class COmxILFsm;

/**
   This is a generic implementation of the Callback Manager functionality.

   This implementation is meant to be reused totally or partially by specilized
   implementations of a Callback Manager object.
 */
class XOmxILCallbackManagerIfImpl
	{

protected:

	XOmxILCallbackManagerIfImpl(OMX_HANDLETYPE apComponentHandle,
										 OMX_PTR apAppData,
										 OMX_CALLBACKTYPE* apCallbacks);

	virtual ~XOmxILCallbackManagerIfImpl();

	virtual void DoSetPortManager(MOmxILPortManagerIf& apPortManager);

	virtual void DoSetFsm(COmxILFsm& apFsm);

	//
	// Methods for Callback Registration
	//
	virtual OMX_ERRORTYPE DoRegisterComponentHandle(
		OMX_HANDLETYPE aComponentHandle);

	virtual OMX_ERRORTYPE DoRegisterILClientCallbacks(
		const OMX_CALLBACKTYPE* apCallbacks,
		const OMX_PTR apAppData);

	virtual OMX_ERRORTYPE DoRegisterTunnelCallback(
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection,
		OMX_HANDLETYPE aTunnelledComponentHandle,
		OMX_U32 aTunnelledPortIndex);

	virtual OMX_ERRORTYPE DoRegisterBufferMarkPropagationPort(
		OMX_U32 aPortIndex,
		OMX_U32 aPropagationPortIndex);

	//
	// Methods for Callback Notification
	//

	virtual OMX_ERRORTYPE DoTransitionCompleteNotification(
		OMX_STATETYPE aOmxState);

	virtual OMX_ERRORTYPE DoCommandCompleteNotification(
		OMX_COMMANDTYPE aOmxCommand,
		OMX_U32 aOmxPortIndex);

	//
	// Methods for Callback Notification
	//

	virtual OMX_ERRORTYPE DoErrorEventNotification(
		OMX_ERRORTYPE aOmxError);

	virtual OMX_ERRORTYPE DoEventNotification(
		OMX_EVENTTYPE aEvent,
		TUint32 aData1,
		TUint32 aData2,
		OMX_STRING aExtraInfo);

	virtual OMX_ERRORTYPE DoBufferDoneNotification(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection);

	virtual OMX_ERRORTYPE DoPortSettingsChangeNotification(
		OMX_U32 aLocalPortIndex,
		TUint aPortSettingsIndex,
		const TDesC8& aPortSettings);

#ifdef _OMXIL_COMMON_IL516C_ON
	virtual OMX_ERRORTYPE DoEjectBuffersRequest(
		OMX_U32 aLocalPortIndex);
#endif

	virtual void SignalOrPropagateBufferMarks(OMX_BUFFERHEADERTYPE* apBufferHeader,
													   OMX_U32 aLocalPortIndex);

	virtual void HandleInsufficientResources();

protected:

	// Declaration of nested protected classes
	class TTunnelRegistrationInfo;
	class TBufferMarkPropagationInfo;
	class TOutputPortBufferMarkInfo;

protected:

	// \defgroup IL Client callbacks data
    //@{
	OMX_COMPONENTTYPE* ipHandle; // not owned
	OMX_PTR ipAppData; // not owned
	OMX_CALLBACKTYPE* ipCallbacks; // not owned
    //@}

	// Tunnelled ports callback data
	RArray<TTunnelRegistrationInfo> iRegisteredTunnels;

	// Associations between input and output ports to be used in buffer marks
	// propagation
	RArray<TBufferMarkPropagationInfo> iBufferMarkPropagationPorts;

	// Temporary store of buffer marks waiting to go out in the next output
	// buffer
	RArray<TOutputPortBufferMarkInfo> iBufferMarks;

	// Port manager
	MOmxILPortManagerIf* ipPortManager;

	// FSM
	COmxILFsm* ipFsm;

	};

/**
  Structure used to hold the tunnel information needed for callback handling
  in tunnelled communication.
 */
class XOmxILCallbackManagerIfImpl::TTunnelRegistrationInfo
	{

public:

	inline TTunnelRegistrationInfo(
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection,
		OMX_HANDLETYPE aTunnelledComponentHandle,
		OMX_U32 aTunnelledPortIndex);

public:

	OMX_U32 iLocalPortIndex;
	OMX_DIRTYPE iLocalPortDirection;
	OMX_HANDLETYPE iTunnelledComponentHandle;
	OMX_U32 iTunnelledPortIndex;

	};


/**
  Structure used to hold the association of marked port and propagation port.
 */
class XOmxILCallbackManagerIfImpl::TBufferMarkPropagationInfo
	{

public:

	inline TBufferMarkPropagationInfo(
		OMX_U32 aPortIndex,
		OMX_U32 aPropagationPortIndex);

	// This constructor should only be used for array look-ups
	inline explicit TBufferMarkPropagationInfo(OMX_U32 aPortIndex);

	static TBool Compare(const TBufferMarkPropagationInfo& aBmpi1,
						 const TBufferMarkPropagationInfo& aBmpi2);

public:

	OMX_U32 iPortIndex;
	OMX_U32 iPropagationPortIndex;

	};

/**
  Structure used to hold the buffer marking info needed to propagate buffer
  marks on output ports.
 */
class XOmxILCallbackManagerIfImpl::TOutputPortBufferMarkInfo
	{

public:

	inline TOutputPortBufferMarkInfo(
		OMX_U32 aPortIndex,
		OMX_HANDLETYPE apMarkTargetComponent,
		OMX_PTR apMarkData);

	// This constructor should only be used for array look-ups
	inline explicit TOutputPortBufferMarkInfo(OMX_U32 aPortIndex);

	static TBool Compare(const TOutputPortBufferMarkInfo& aOpbmi1,
						 const TOutputPortBufferMarkInfo& aOpbmi2);

public:

	OMX_U32 iPortIndex;
	OMX_HANDLETYPE ipMarkTargetComponent;
	OMX_PTR ipMarkData;

	};

#include "omxilcallbackmanagerifimpl.inl"

#endif // OMXILCALLBACKMANAGERIFIMPL_H
