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


// ComponentTunnelRequest must be used in the following states:
// - Any state, (but the port will have to be disabled)
//
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::ComponentTunnelRequest(
	COmxILFsm& aFsm,
	OMX_U32 aPort,
	OMX_HANDLETYPE aTunneledComp,
	OMX_U32 aTunneledPort,
	OMX_TUNNELSETUPTYPE* apTunnelSetup)
	{
	return aFsm.iPortManager.TunnelRequest(
		aPort,
		aTunneledComp,
		aTunneledPort,
		apTunnelSetup,
		OMX_TRUE				// Port must be disabled
		);
	}

// ComponentTunnelRequestV2 must be used in the following states:
// - OMX_StateLoaded
//
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::ComponentTunnelRequestV2(
	COmxILFsm& aFsm,
	OMX_U32 aPort,
	OMX_HANDLETYPE aTunneledComp,
	OMX_U32 aTunneledPort,
	OMX_TUNNELSETUPTYPE* apTunnelSetup)
	{
	return aFsm.iPortManager.TunnelRequest(
		aPort,
		aTunneledComp,
		aTunneledPort,
		apTunnelSetup,
		OMX_FALSE				// Port does not need to be disabled
		);
	}

// This PopulateBuffer version must be used in the following states:
// - OMX_StateIdle,
// - OMX_StateExecuting,
// - OMX_StatePaused
//
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::PopulateBuffer(COmxILFsm& aFsm,
									   OMX_BUFFERHEADERTYPE** appBufferHdr,
									   OMX_U32 aPortIndex,
									   OMX_PTR apAppPrivate,
									   OMX_U32 aSizeBytes,
									   OMX_U8* apBuffer,
									   TBool& portPopulationCompleted)
	{
	return aFsm.iPortManager.PopulateBuffer(
		appBufferHdr,
		aPortIndex,
		apAppPrivate,
		aSizeBytes,
		apBuffer,
		portPopulationCompleted,
		OMX_TRUE				// Port must be disabled
		);
	}

// This PopulateBuffer version must be used in the following states:
// - OMX_StateLoaded,
// - OMX_StateWaitForResources
//
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::PopulateBufferV2(
	COmxILFsm& aFsm,
	OMX_BUFFERHEADERTYPE** appBufferHdr,
	OMX_U32 aPortIndex,
	OMX_PTR apAppPrivate,
	OMX_U32 aSizeBytes,
	OMX_U8* apBuffer,
	TBool& portPopulationCompleted)
	{
	return aFsm.iPortManager.PopulateBuffer(
		appBufferHdr,
		aPortIndex,
		apAppPrivate,
		aSizeBytes,
		apBuffer,
		portPopulationCompleted,
		OMX_FALSE				// Port does not need to be disabled
		);
	}


// This FreeBuffer version is used in the following states:
// - OMX_StateLoaded,
// - OMX_StateWaitForResources
// - OMX_StateExecuting,
// - OMX_StatePaused,
// AND the port must be disabled
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::FreeBuffer(COmxILFsm& aFsm,
								   OMX_U32 aPortIndex,
								   OMX_BUFFERHEADERTYPE* apBuffer,
								   TBool& aPortDepopulationCompleted)
	{
	return aFsm.iPortManager.FreeBuffer(
		aPortIndex,
		apBuffer,
		aPortDepopulationCompleted,
		OMX_TRUE				// Port should be disabled, otherwise
								// OMX_ErrorPortUnpopulated might be sent
		);
	}

// This FreeBuffer version is used in the following states:
// - OMX_StateIdle,
//
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::FreeBufferV2(COmxILFsm& aFsm,
									 OMX_U32 aPortIndex,
									 OMX_BUFFERHEADERTYPE* apBuffer,
									 TBool& aPortDepopulationCompleted)
	{
	return aFsm.iPortManager.FreeBuffer(
		aPortIndex,
		apBuffer,
		aPortDepopulationCompleted,
		OMX_FALSE				// Port does not need to be disabled
		);
	}

// This EmptyThisBuffer version must be used in the following states:
// - OMX_StateLoaded,
// - OMX_StateWaitForResources
// AND the port must be disabled
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::EmptyThisBuffer(COmxILFsm& aFsm,
										OMX_BUFFERHEADERTYPE* apBuffer)
	{
	return aFsm.iPortManager.BufferIndication(
		apBuffer,
		OMX_DirInput,
		OMX_TRUE				// Port must be disabled
		);
	}

// This EmptyThisBuffer version must be used in the following states:
// - OMX_StateIdle,
// - OMX_StateExecuting,
// - OMX_StatePaused
//
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::EmptyThisBufferV2(COmxILFsm& aFsm,
										  OMX_BUFFERHEADERTYPE* apBuffer)
	{
	return aFsm.iPortManager.BufferIndication(
		apBuffer,
		OMX_DirInput,
		OMX_FALSE				// Port does not need to be disabled
		);
	}

// This FillThisBuffer version must be used in the following states:
// - OMX_StateLoaded,
// - OMX_StateWaitForResources
// AND the port must be disabled
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::FillThisBuffer(COmxILFsm& aFsm,
									   OMX_BUFFERHEADERTYPE* apBuffer)
	{
	return aFsm.iPortManager.BufferIndication(
		apBuffer,
		OMX_DirOutput,
		OMX_TRUE				// Port must be disabled
		);
	}

// This FillThisBuffer version must be used in the following states:
// - OMX_StateIdle,
// - OMX_StateExecuting,
// - OMX_StatePaused
//
inline OMX_ERRORTYPE
COmxILFsm::COmxILState::FillThisBufferV2(COmxILFsm& aFsm,
										 OMX_BUFFERHEADERTYPE* apBuffer)
	{
	return aFsm.iPortManager.BufferIndication(
		apBuffer,
		OMX_DirOutput,
		OMX_FALSE				// Port does not need to be disabled
		);
	}

