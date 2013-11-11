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

#ifndef OMXILCALLBACKMANAGERIF_H
#define OMXILCALLBACKMANAGERIF_H

#include <openmax/il/khronos/v1_x/OMX_Component.h>

#include <openmax/il/common/omxilcallbacknotificationif.h>

// Forward declarations
class MOmxILPortManagerIf;
class COmxILFsm;

/**
   CallBack Manager Interface used by Callback Manager implementations.
 */
class MOmxILCallbackManagerIf : public MOmxILCallbackNotificationIf
	{

public:

	virtual ~MOmxILCallbackManagerIf();

	/**
	   Method to update the Port Manager reference

	   @param [in] aPortManager The Port Manager

	   @return OMX_ERRORTYPE
	*/
	virtual void SetPortManager(MOmxILPortManagerIf& aPortManager) = 0;

	/**
	   Method to update the FSM reference

	   @param [in] aFsm The FSM

	   @return OMX_ERRORTYPE
	*/
	virtual void SetFsm(COmxILFsm& aFsm) = 0;


	//
	// Methods for Callback Registration/Un-registration
	//

	/**
	   Method to register the component handle

	   @param [in] aComponentHandle The component handle

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE RegisterComponentHandle(
		OMX_HANDLETYPE aComponentHandle) = 0;

	/**
	   Method to register the IL Client callbacks

	   @param [in] apCallbacks The IL Client callback pointers

	   @param [in] apAppData The IL Client data used to provide
	   component-specific context

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE RegisterILClientCallbacks(
		const OMX_CALLBACKTYPE* apCallbacks,
		const OMX_PTR apAppData) = 0;

	/**
	   Method to register a tunnel callback

	   @param [in] aLocalPortIndex The tunnel's local port index

	   @param [in] aLocalPortDirection The direction of the tunnel's local port

	   @param [in] aTunnelledComponentHandle Tunnelled component handle

	   @param [in] aTunnelledPortIndex Index of the tunnelled port

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE RegisterTunnelCallback(
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection,
		OMX_HANDLETYPE aTunnelledComponentHandle,
		OMX_U32 aTunnelledPortIndex) = 0;

	/**
	   Method to un-register a tunnel callback

	   @param [in] aLocalPortIndex The tunnel's local port index

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE DeregisterTunnelCallback(
		OMX_U32 aLocalPortIndex) = 0;

	/**
	   Method to register buffer mark propagation ports

	   @param [in] aPortIndex The index of the input port that receives buffer
	   marks

	   @param [in] aPropagationPortIndex An index of the ouput port where buffer marks
	   will be propagated

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE RegisterBufferMarkPropagationPort(
		OMX_U32 aPortIndex,
		OMX_U32 aPropagationPortIndex) = 0;

	/**
	   Method to remove a buffer done commands from the Callback Manager
	   internal queues, if queueing is in use

	   @param [in] apBufferHeader The buffer header contained in the buffer indication

	   @param [in] aDirection The direction of the port that handles the buffer

	   @return TBool
	*/
	virtual TBool BufferRemovalIndication(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection) = 0;

	//
	// Methods for Callback Notification
	//

	/**
	   Method to notify the IL Client of the completion of an OpenMAX IL state
	   transition

	   @param [in] aOmxState The new state

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE TransitionCompleteNotification(
		OMX_STATETYPE aOmxState) = 0;

	/**
	   Method to notify the IL Client of the completion of a OpenMAX IL command

	   @param [in] aOmxCommand The command that has been completed

	   @param [in] aOmxPortIndex The index of the port where the command
	   applies, if any

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE CommandCompleteNotification(
		OMX_COMMANDTYPE aOmxCommand,
		OMX_U32 aOmxPortIndex) = 0;

#ifdef _OMXIL_COMMON_IL516C_ON
	/**
	   Method to request from the tunnelled component the ejection of the
	   buffers present in one of its ports

	   @param [in] aLocalOmxPortIndex The index of the local port that is
	   tunnelled to the component that the ejection request applies to

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE EjectBuffersRequest(
		OMX_U32 aLocalOmxPortIndex) = 0;
#endif

	};

#include "omxilcallbackmanagerif.inl"

#endif // OMXILCALLBACKMANAGERIF_H
