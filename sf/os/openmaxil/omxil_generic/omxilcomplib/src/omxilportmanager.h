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

#ifndef OMXILPORTMANAGER_H
#define OMXILPORTMANAGER_H

#include <openmax/il/khronos/v1_x/OMX_Component.h>

#include <openmax/il/common/omxilport.h>
#include "omxilindexmanager.h"
#include "omxilportmanagerif.h"

/**
   Port Manager Panic category
*/
_LIT(KOmxILPortManagerPanicCategory, "OmxILPortManager");


// Forward declarations
class COmxILProcessingFunction;
class MOmxILCallbackManagerIf;

/**
   OpenMAX IL Port Manager.

   It keeps and manages a list of the ports configured in the component. The
   Port Manager is used by the FSM object (@see COmxILFsm) to communicate with
   the ports.

*/
NONSHARABLE_CLASS(COmxILPortManager) : public COmxILIndexManager,
									   public MOmxILPortManagerIf
	{

public:

	static COmxILPortManager* NewL(
		COmxILProcessingFunction& aProcessingFunction,
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
		OMX_BOOL aImmediateReturnTimeBuffer = OMX_TRUE);

	~COmxILPortManager();

	TInt AddPort(const COmxILPort* aPort,
						  OMX_DIRTYPE aDirection);

	OMX_ERRORTYPE GetParameter(
		OMX_INDEXTYPE aParamIndex,
		TAny* apComponentParameterStructure) const;

	OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure,
							   OMX_BOOL aPortIsDisabled = OMX_FALSE);

	OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE aConfigIndex,
							TAny* apComponentConfigStructure) const;

	OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE aConfigIndex,
							const TAny* apComponentConfigStructure);

	OMX_ERRORTYPE GetExtensionIndex(OMX_STRING aParameterName,
									OMX_INDEXTYPE* apIndexType) const;

	OMX_ERRORTYPE PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer,
								 TBool& portPopulationCompleted,
								 OMX_BOOL aPortIsDisabled = OMX_FALSE);

	OMX_ERRORTYPE FreeBuffer(OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBufferHeader,
							 TBool& portDepopulationCompleted,
							 OMX_BOOL aPortIsDisabled = OMX_FALSE);

	OMX_ERRORTYPE TunnelRequest(OMX_U32 aPortIndex,
								OMX_HANDLETYPE aTunneledComp,
								OMX_U32 aTunneledPort,
								OMX_TUNNELSETUPTYPE* apTunnelSetup,
								OMX_BOOL aPortIsDisabled = OMX_FALSE);

	OMX_ERRORTYPE TunnellingBufferAllocation(
		TBool& aComponentPopulationCompleted,
		TUint32 aPortIndex = OMX_ALL);

	OMX_ERRORTYPE TunnellingBufferDeallocation(
		TBool& aComponentDePopulationCompleted);

	OMX_ERRORTYPE InitiateTunnellingDataFlow(
		OMX_U32 aPortIndex = OMX_ALL,
		OMX_BOOL aSuppliersAndNonSuppliers = OMX_FALSE);

	OMX_ERRORTYPE BufferIndication(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection,
		OMX_BOOL aPortIsDisabled = OMX_FALSE);

	OMX_ERRORTYPE BufferReturnIndication(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection,
		TBool& aAllBuffersReturned);

	OMX_ERRORTYPE BufferFlushIndicationFlushCommand(
		TUint32 aPortIndex, TBool aEjectBuffers = ETrue);

	OMX_ERRORTYPE BufferFlushIndicationPauseOrExeToIdleCommand(
		TBool& aAllBuffersReturnedToSuppliers);

	OMX_ERRORTYPE PortEnableIndication(
		TUint32 aPortIndex,
		TBool aIndicationIsFinal);

	OMX_ERRORTYPE PortDisableIndication(
		TUint32 aPortIndex);

	OMX_ERRORTYPE BufferMarkIndication(OMX_U32 aPortIndex,
									   OMX_PTR ipMarkData);

#ifdef _OMXIL_COMMON_IL516C_ON
	OMX_ERRORTYPE BufferEjectIndication(
		TUint32 aPortIndex);
#endif

	OMX_ERRORTYPE ComponentRoleIndication(TUint aComponentRoleIndex);

	OMX_ERRORTYPE PortSettingsChangeIndication(OMX_U32 aPortIndex,
											   TUint aPortSettingsIndex,
											   const TDesC8& aPortSettings,
											   OMX_EVENTTYPE& aEventForILClient);

	//
	//
	//
	TBool AllPortsPopulated() const;

	TBool AllPortsDePopulated() const;

	TBool AllBuffersAtHome() const;


private:

	COmxILPortManager(COmxILProcessingFunction& aProcessingFunction,
					  MOmxILCallbackManagerIf& aCallbacks);

	// From MOmxILPortManagerIf
	void ConstructL(COmxILProcessingFunction& aProcessingFunction,
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
					OMX_BOOL aImmediateReturnTimeBuffer = OMX_TRUE);


	void AppendPortL(const COmxILPort* aPort);
	void RemoveLastAppendedPort();

	inline OMX_ERRORTYPE CheckPortIndex(OMX_U32 aPortIndex) const;

	inline OMX_ERRORTYPE GetPortIndexFromOmxStruct(
		const TAny*& apComponentParameterStructure,
		OMX_U32& aIndex) const;

	inline OMX_ERRORTYPE GetPortIndexFromOmxStruct(
		TAny*& apComponentParameterStructure,
		OMX_U32& aIndex) const;

	TBool RemoveBuffersFromPfOrCm(COmxILPort* apPort,
								  OMX_BOOL aRemoveFromPfOnly = OMX_FALSE) const;

private:

	COmxILProcessingFunction& iProcessingFunction;
	MOmxILCallbackManagerIf& iCallbacks;

	RPointerArray<COmxILPort> iAllPorts;
	RArray<TBool> iTimePorts;		// Indicate the corresponding port is OMX_OTHER_FormatTime

	OMX_PORT_PARAM_TYPE iAudioParamInit;
	OMX_PORT_PARAM_TYPE iImageParamInit;
	OMX_PORT_PARAM_TYPE iVideoParamInit;
	OMX_PORT_PARAM_TYPE iOtherParamInit;
	OMX_BOOL iImmediateReturnTimeBuffer;
	};

#include "omxilportmanager.inl"

#endif // OMXILPORTMANAGER_H
