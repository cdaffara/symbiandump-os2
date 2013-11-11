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


#ifndef OMXILPORTMANAGERIF_H
#define OMXILPORTMANAGERIF_H

#include <openmax/il/khronos/v1_x/OMX_Component.h>

// Forward declarations
class COmxILProcessingFunction;
class MOmxILCallbackManagerIf;
class COmxILPort;

/**
   OpenMAX IL Port Manager Interface to be used by Port Manager
   implementations.
 */
class MOmxILPortManagerIf
	{

public:

	virtual ~MOmxILPortManagerIf();

	/**
	   This method is used at component's construction time, i.e., in a factory
	   method of the final component implementation. The main component object
	   uses this method to add the component's ports to its port manager
	   instance. This is the only method that needs to be exported by Port
	   Manager's implementations. All other Port Managers public methods are
	   for internal use in the component framework.

	   @param aPort The pointer of the port instance to be added.

	   @param aDirection The direction of the port being added.

	   @return A Symbian error code indicating if the function call was
	   successful.  KErrNone on success, otherwise another of the system-wide
	   error codes.
	*/
	virtual TInt AddPort(const COmxILPort* aPort,
						 OMX_DIRTYPE aDirection) = 0;


/**
   Port Manager's interface for OpenMAX IL Standard API component
   functions. These are for interal use in the component framework and should
   not be exported by Port Manager implementations.

   */


	/**
	   Port Manager's version of the GetParameter OpenMAX IL component
	   API

	   @param aParamIndex The index of the OpenMAX IL param structure.

	   @param apComponentParameterStructure The pointer to the IL
	   client-allocated param structure to be filled by the Port Manager
	   (tipically delegated to the port)

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE GetParameter(
		OMX_INDEXTYPE aParamIndex,
		TAny* apComponentParameterStructure) const = 0;

	/**
	   Port Manager's version of the SetParameter OpenMAX IL component
	   API

	   @param aParamIndex The index of the OpenMAX IL param structure.

	   @param apComponentParameterStructure The pointer to the IL
	   client-allocated param structure to be set into the port

	   @param aPortIsDisabled This is an indication from the FSM to the Port
	   Manager of whether the port should or should not be currently disabled
	   for this OpenMAX IL API to succeed. If aPortIsDisabled is OMX_TRUE and
	   the port is enabled this API is not allowed in the current state and the
	   Port Manager must return OMX_ErrorIncorrectStateOperation. Default value
	   is OMX_FALSE.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure,
							   OMX_BOOL aPortIsDisabled = OMX_FALSE) = 0;

	/**
	   Port Manager's version of the GetConfig OpenMAX IL component
	   API

	   @param aParamIndex The index of the OpenMAX IL config structure.

	   @param apComponentParameterStructure The pointer to the config structure
	   to be filled by the Port Manager (tipically delegated to the port)

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE aConfigIndex,
							TAny* apComponentConfigStructure) const = 0;

	/**
	   Port Manager's version of the SetConfig OpenMAX IL component
	   API

	   @param aParamIndex The index of the OpenMAX IL config structure.

	   @param apComponentParameterStructure The pointer to the config structure
	   to be set into the port

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE aConfigIndex,
							const TAny* apComponentConfigStructure) = 0;

	/**
	   Port Manager's version of the GetExtensionIndex OpenMAX IL component
	   API

	   @param aParameterName An OMX_STRING value that shall be less than 128
	   characters long including the trailing null byte. The Port Manager will
	   translate this string into a configuration index.

	   @param apIndexType A pointer to the OMX_INDEXTYPE structure that is to
	   receive the index value.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE GetExtensionIndex(OMX_STRING aParameterName,
									OMX_INDEXTYPE* apIndexType) const = 0;


	/**
	   Port Manager's version of the AllocateBuffer/UseBuffer OpenMAX IL component
	   APIs

	   @param appBufferHdr A pointer to a pointer of an OMX_BUFFERHEADERTYPE
	   structure that receives the pointer to the buffer header.

	   @param aPortIndex The index of the port that will use the specified
	   buffer.

	   @param apAppPrivate A pointer that refers to an implementation-specific
	   memory area that is under responsibility of the supplier of the buffer

	   @param aSizeBytes The buffer size in bytes

	   @param apBuffer A pointer to the memory buffer area to be used

	   @param [out] aPortPopulationCompleted This is an indication from the
	   Port Manager to the FSM of whether the port population has completed or
	   not.

	   @param aPortIsDisabled This is an indication from the FSM to the Port
	   Manager of whether the port should or should not be currently disabled
	   for this OpenMAX IL API to succeed. If aPortIsDisabled is OMX_TRUE and
	   the port is enabled this API is not allowed in the current state and the
	   Port Manager must return OMX_ErrorIncorrectStateOperation. Default value
	   is OMX_FALSE.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer,
								 TBool& aPortPopulationCompleted,
								 OMX_BOOL aPortIsDisabled = OMX_FALSE) = 0;

	/**
	   Port Manager's version of the FreeBuffer OpenMAX IL component
	   API

	   @param aPortIndex The index of the port that is using the specified
	   buffer

	   @param apBufferHeader A pointer to an OMX_BUFFERHEADERTYPE structure

	   @param aPortDepopulationCompleted This is a g

	   @param aPortIsDisabled This is an indication from the FSM to the Port
	   Manager of whether the port should or should not be currently disabled
	   for this OpenMAX IL API to succeed. If aPortIsDisabled is OMX_TRUE and
	   the port is enabled this API is not allowed in the current state and the
	   Port Manager must return OMX_ErrorIncorrectStateOperation. Default value
	   is OMX_FALSE.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE FreeBuffer(OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBufferHeader,
							 TBool& aPortDepopulationCompleted,
							 OMX_BOOL aPortIsDisabled = OMX_FALSE) = 0;

	/**
	   Port Manager's version of the EmptyThisBuffer/FillThisBuffer OpenMAX IL
	   component APIs. This is to be used by the FSM in any state except
	   substates OMX_StateExecuting to OMX_StateIdle and OMX_StatePause to
	   OMX_StateIdle, in which case BufferReturnIndication should be used.

	   @param apBufferHeader A pointer to an OMX_BUFFERHEADERTYPE structure

	   @param aDirection The direction of the port that received the
	   EmptyThisBuffer/FillThisBuffer call

	   @param aPortIsDisabled This is an indication from the FSM to the Port
	   Manager of whether the port should or should not be currently disabled
	   for this OpenMAX IL API to succeed. If aPortIsDisabled is OMX_TRUE and
	   the port is enabled this API is not allowed in the current state and the
	   Port Manager must return OMX_ErrorIncorrectStateOperation. Default value
	   is OMX_FALSE.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE BufferIndication(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection,
		OMX_BOOL aPortIsDisabled = OMX_FALSE) = 0;

	/**
	   Port Manager's version of the EmptyThisBuffer/FillThisBuffer OpenMAX IL
	   component APIs. This is to be used by the FSM when the component is in
	   substates OMX_StateExecuting to OMX_StateIdle and OMX_StatePause to
	   OMX_StateIdle.

	   @param apBufferHeader A pointer to an OMX_BUFFERHEADERTYPE structure

	   @param aDirection The direction of the port that received the
	   EmptyThisBuffer/FillThisBuffer call

	   @param [out] aAllBuffersReturned ETrue if all buffers have already been
	   returned to ports managed by the Port Manager. This is an indication to
	   the FSM that the component is ready to be complete the transition to
	   OMX_StateIdle.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE BufferReturnIndication(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection,
		TBool& aAllBuffersReturned) = 0;

	/**
	   Port Manager's version of the ComponentTunnelRequest OpenMAX IL
	   component API. This is to be used by the FSM when the component is in
	   substates OMX_StateExecuting to OMX_StateIdle and OMX_StatePause to
	   OMX_StateIdle.

	   @param aPortIndex The index of the local port that is going to be tunnelled

	   @param aTunneledComp The handle of the other component that participates
	   in the tunnel. When this parameter is NULL, the port specified in aPortIndex
	   should be configured for non-tunneled communication with the IL client.

	   @param aTunneledPort The index of the remote port to be tunnelled with

	   @param apTunnelSetup The structure that contains data for the tunneling
	   negotiation between components.

	   @param aPortIsDisabled This is an indication from the FSM to the Port
	   Manager of whether the port should or should not be currently disabled
	   for this OpenMAX IL API to succeed. If aPortIsDisabled is OMX_TRUE and
	   the port is enabled this API is not allowed in the current state and the
	   Port Manager must return OMX_ErrorIncorrectStateOperation. Default value
	   is OMX_FALSE.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE TunnelRequest(OMX_U32 aPortIndex,
										OMX_HANDLETYPE aTunneledComp,
										OMX_U32 aTunneledPort,
										OMX_TUNNELSETUPTYPE* apTunnelSetup,
										OMX_BOOL aPortIsDisabled = OMX_FALSE) = 0;


/**
   Other Port Manager's interface, with no direct mapping to OpenMAX IL
   APIs. These are also for interal use in the component framework and should
   not be exported by Port Manager implementations.

   */

	/**
	   This is and indication from the FSM to the Port Manager that the
	   component is transitioning from OMX_StateLoaded to OMX_StateIdle and
	   that the Port Manager shall initiate the allocation of buffers in
	   tunnelled supplier ports.

	   @param [out] aComponentPopulationCompleted This is an indication from
	   the Port Manager to the FSM of whether the population has completed in
	   all ports, suppliers and non-suppliers.

	   @param aPortIndex The index of the port to start populating. OMX_ALL by default.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE TunnellingBufferAllocation(
		TBool& aComponentPopulationCompleted,
		TUint32 aPortIndex = OMX_ALL) = 0;

	/**
	   This is and indication from the FSM to the Port Manager that the
	   component is transitioning to from OMX_StateExecuting or OMX_StatePause
	   to OMX_StateIdle and that the Port Manager shall initiate the de-allocation
	   of buffers in tunnelled supplier ports.

	   @param [out] aComponentDePopulationCompleted This is an indication from
	   the Port Manager to the FSM of whether the de-population has completed in
	   all ports, suppliers and non-suppliers.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE TunnellingBufferDeallocation(
		TBool& aComponentDePopulationCompleted) = 0;

	/**
	   This is and indication from the FSM to the Port Manager that the
	   component is transitioning to from OMX_StateIdle to OMX_StatePause or
	   OMX_StateExecuting and that the Port Manager shall initiate the data
	   flow in the tunnel (supplier input ports buffers are sent to the Callback Manager
	   and supplier ouput ports buffers to the Processing Function)

	   @param aPortIndex The index of the port that this function applies
	   to. OMX_ALL if applies to all tunnelled supplier ports in the component.

	   @param aSuppliersAndNonSuppliers This overrides the default behaviour of
	   this function, i.e., initiating the data flow for the supplier
	   ports. This is an indication to the Port Manager that non-supplier ports
	   should be included too.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE InitiateTunnellingDataFlow(
		OMX_U32 aPortIndex = OMX_ALL,
		OMX_BOOL aSuppliersAndNonSuppliers = OMX_FALSE) = 0;


	/**
	   This is and indication from the FSM to the Port Manager that the
	   component has received a command to flush buffers.

	   @param aPortIndex The index of the port that this function applies
	   to. OMX_ALL if applies to all tunnelled supplier ports in the component.

	   @param aEjectBuffers This is an indication to the Port Manager of
	   whether owned buffers should be circulated or not as a consequence of
	   flushing. The default is ETrue (buffers are ejected, ie sent to the
	   Processing Function or the Callback Manager as needed). The FSM may
	   override this default behaviour by using EFalse, e.g. if a flush command
	   is received when the component is already in OMX_StateIdle state.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE BufferFlushIndicationFlushCommand(
		TUint32 aPortIndex, TBool aEjectBuffers = ETrue) = 0;

	/**
	   This is and indication from the FSM to the Port Manager that the
	   component needs to flush buffers as a consequence of a transition to
	   OMX_StateIdle from OMX_StateExecuting or OMX_StatePause.

	   @param [out] aAllBuffersReturnedToSuppliers This is an indication from
	   the Port Manager to the FSM that all buffers have been sent/received
	   to/from suppliers/non-suppliers and that the component is ready to
	   complete the transition to OMX_StateIdle.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE BufferFlushIndicationPauseOrExeToIdleCommand(
		TBool& aAllBuffersReturnedToSuppliers) = 0;

	/**
	   This is and indication from the FSM to the Port Manager that it needs to
	   enable some port.

	   @param aPortIndex The index of the port to be enabled. OMX_ALL if all ports.

	   @param aIndicationIsFinal When this is ETrue, this is an indication from
	   the FSM to the Port Manager that the current state of the component does
	   not require the allocation of buffers (e.g. OMX_StateLoaded) and the
	   port should complete the transition to enabled and forward the command
	   complete event to the IL Client.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE PortEnableIndication(
		TUint32 aPortIndex,
		TBool aIndicationIsFinal) = 0;

	/**
	   This is and indication from the FSM to the Port Manager that it needs to
	   disable some port.

	   @param aPortIndex The index of the port to be disabled. OMX_ALL if all ports.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE PortDisableIndication(
		TUint32 aPortIndex) = 0;

	/**
	   This is and indication from the FSM to the Port Manager that there is a
	   buffer mark command to be applied to some port.

	   @param aPortIndex The index of the port that receives the buffer mark command.

	   @param ipMarkData The pointer to the buffer mark data

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE BufferMarkIndication(OMX_U32 aPortIndex,
									   OMX_PTR ipMarkData) = 0;

	/**
	   This is an indication to the Port Manager that there is a request to
	   change the role of the component and that the component's ports need to
	   be adjusted to it.

	   @param aComponentRoleIndex This is the index from the internal list of
	   roles supported by the component

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE ComponentRoleIndication(
		TUint aComponentRoleIndex) = 0;

	/**
	   This is an indication to the Port Manager that there is a port that
	   needs to be reconfigured.

	   @param aPortIndex The index of the port to be reconfigured.

	   @param aPortSettingsIndex This is an index of the port structure to be
	   reconfigured

	   @param aPortSettings This is a descriptor that contains the information
	   to be passed to the port that is to be reconfigured

	   @param [out] aEventForILClient This is the event returned by the port
	   after reconfiguration so the caller can forward that to the IL Client.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE PortSettingsChangeIndication(
		OMX_U32 aPortIndex,
		TUint aPortSettingsIndex,
		const TDesC8& aPortSettings,
		OMX_EVENTTYPE& aEventForILClient) = 0;

	/**
	   Returns ETrue is all ports are currently populated
	*/
	virtual TBool AllPortsPopulated() const = 0;

	/**
	   Returns ETrue is all ports are currently depopulated
	*/
	virtual TBool AllPortsDePopulated() const = 0;

	/**
	   Returns ETrue is all buffer headers are currently with the Port Manager
	*/
	virtual TBool AllBuffersAtHome() const = 0;


private:

	/**
	   Virtual constructor. This is mostly for reference. Port Managers should
	   follow the below contruction interface.

	   @param   aProcessingFunction The component's processing function

	   @param   aCallbacks The component's callback manager

	   @param   aOmxVersion The IL Spec version in use

	   @param   aNumberOfAudioPorts Number of audio ports in the component

	   @param   aStartAudioPortNumber The start index for audio ports

	   @param   aNumberOfImagePorts Number of image ports in the component

	   @param   aStartImagePortNumber The start index for image ports

	   @param   aNumberOfVideoPorts Number of video ports in the component

	   @param   aStartVideoPortNumber The start index for video ports

	   @param   aNumberOfOtherPorts Number of other ports in the component

	   @param   aStartOtherPortNumber The start index for other ports

	   @param aImmediateReturnTimeBuffer This only applies to components with a
	   clock client port. Indicates whether the Port Manager must forward an
	   arriving clock buffer to the Callback Manager (ETrue) or to the
	   Processing Function (EFalse) . If the clock buffer is to be forwarded to
	   the Processing Function, this will happen using the BufferIndication
	   function of the component's PF. Otherwise, PF's MediaTimeIndication is
	   used instead.
	*/
	virtual void ConstructL(COmxILProcessingFunction& aProcessingFunction,
							MOmxILCallbackManagerIf& aCallbacks,
							const OMX_VERSIONTYPE& aOmxVersion,
							OMX_U32 aNumberOfAudioPorts,
							OMX_U32 aStartAudioPortNumber,
							OMX_U32 aNumberOfImagePorts,
							OMX_U32 aStartImagePortNumber,
							OMX_U32 aNumberOfVideoPorts,
							OMX_U32 aStartVideoPortNumber,
							OMX_U32 aNumberOfOtherPorts,
							OMX_U32 aStartOtherPortNumber,
							OMX_BOOL aImmediateReturnTimeBuffer = OMX_TRUE) = 0;

	};

#include "omxilportmanagerif.inl"

#endif // OMXILPORTMANAGERIF_H
