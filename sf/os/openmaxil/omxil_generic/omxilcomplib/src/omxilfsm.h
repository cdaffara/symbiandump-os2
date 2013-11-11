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

#ifndef OMXILFSM_H
#define OMXILFSM_H


#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <e32msgqueue.h>
#include <openmax/il/common/omxilstatedefs.h>

// forward declarations
class MOmxILPortManagerIf;
class COmxILConfigManager;
class MOmxILCallbackManagerIf;
class COmxILComponent;
class COmxILProcessingFunction;

/**
   FSM Panic category
*/
_LIT(KOmxILFsmPanicCategory, "OmxILFsm");


/**
   FSM class that represents the main FSM context object of the State
   pattern. It delegates most events for processing to the current state
   object.
 */
NONSHARABLE_CLASS(COmxILFsm) : public CBase
	{

public:

	static const TInt KMaxMsgQueueEntries = 10;
public:

	static COmxILFsm* NewL(COmxILComponent& aComponent,
							COmxILProcessingFunction& aProcFunction,
							MOmxILPortManagerIf& aPortManager,
							COmxILConfigManager& aConfigManager,
							MOmxILCallbackManagerIf& aCallbacks);

	~COmxILFsm();

	OMX_ERRORTYPE InitFsm();

	COmxILComponent* GetComponent() const;

	//
	// OpenMAX IL API calls
	//

	OMX_ERRORTYPE GetComponentVersion(OMX_STRING aComponentName,
									  OMX_VERSIONTYPE* pComponentVersion,
									  OMX_VERSIONTYPE* pSpecVersion,
									  OMX_UUIDTYPE* pComponentUUID) const;

	OMX_ERRORTYPE SendCommand(OMX_COMMANDTYPE Cmd,
							  TUint32 nParam1,
							  TAny* apCmdData);

	OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE aParamIndex,
							   TAny* apComponentParameterStructure) const;

	OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure);

	OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE aConfigIndex,
							TAny* apComponentConfigStructure) const;

	OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE aConfigIndex,
							const TAny* apComponentConfigStructure);

	OMX_ERRORTYPE GetExtensionIndex(OMX_STRING aParameterName,
									OMX_INDEXTYPE* apIndexType) const;

	OMX_ERRORTYPE GetState(OMX_STATETYPE* apState) const;

	OMX_ERRORTYPE ComponentTunnelRequest(OMX_U32 aPort,
										 OMX_HANDLETYPE aTunneledComp,
										 OMX_U32 aTunneledPort,
										 OMX_TUNNELSETUPTYPE* apTunnelSetup);

	OMX_ERRORTYPE UseBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
							OMX_U32 aPortIndex,
							OMX_PTR apAppPrivate,
							OMX_U32 aSizeBytes,
							OMX_U8* apBuffer);

	OMX_ERRORTYPE AllocateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes);

	OMX_ERRORTYPE FreeBuffer(OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE EmptyThisBuffer(OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE FillThisBuffer(OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE SetCallbacks(const OMX_CALLBACKTYPE* apCallbacks,
							   const OMX_PTR apAppData);

    OMX_ERRORTYPE UseEGLImage(OMX_BUFFERHEADERTYPE** appBufferHdr,
							  OMX_U32 aPortIndex,
							  OMX_PTR aAppPrivate,
							  void* eglImage);

    OMX_ERRORTYPE ComponentRoleEnum(OMX_U8* aRole,
									OMX_U32 aIndex) const;

private:

	// Declaration of nested private state classes
	class COmxILState;
	class COmxILStateInvalid;
	class COmxILStateLoaded;
	class COmxILStateLoadedToIdle;
	class COmxILStateWaitForResources;
	class COmxILStateIdle;
	class COmxILStateIdleToLoaded;
	class COmxILStateExecuting;
	class COmxILStateExecutingToIdle;
	class COmxILStatePause;
	class COmxILStatePauseToIdle;
	class MOmxILPauseOrExecutingToIdle;

	// Note that the following friends don't break COmxILFsm's interface as all
	// friends below are COmxILFsm's private nested classes and therefore they
	// are logically part of COmxILFsm implementation
	friend class COmxILState;
	friend class COmxILStateInvalid;
	friend class COmxILStateLoaded;
	friend class COmxILStateLoadedToIdle;
	friend class COmxILStateWaitForResources;
	friend class COmxILStateIdle;
	friend class COmxILStateIdleToLoaded;
	friend class COmxILStateExecuting;
	friend class COmxILStateExecutingToIdle;
	friend class COmxILStatePause;
	friend class COmxILStatePauseToIdle;
	friend class MOmxILPauseOrExecutingToIdle;

private:

	COmxILFsm(COmxILComponent& aComponent,
			  COmxILProcessingFunction& aProcFunction,
			  MOmxILPortManagerIf& aPortManager,
			  COmxILConfigManager& aConfigManager,
			  MOmxILCallbackManagerIf& aCallbacks);

	void ConstructL();

	OMX_ERRORTYPE PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer);

	OMX_ERRORTYPE FsmTransition(TStateIndex aNewState);

	OMX_ERRORTYPE FsmTransition(TUint32 aNewState);

	OMX_ERRORTYPE SendOmxErrorEventIfNeeded(OMX_ERRORTYPE aError) const;

	OMX_ERRORTYPE SendOmxErrorEventIfNeeded(OMX_ERRORTYPE aError);

private:

	COmxILComponent& iComponent;
	COmxILProcessingFunction& iProcFunction;
	MOmxILPortManagerIf& iPortManager;
	COmxILConfigManager& iConfigManager;
	MOmxILCallbackManagerIf& iCallbacks;

	RPointerArray<COmxILState> iStates;
	TStateIndex iCurrentStateIndex;
	COmxILState* ipCurrentState;

	};


#endif // OMXILFSM_H
