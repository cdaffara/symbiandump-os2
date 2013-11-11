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

#include <openmax/il/khronos/v1_x/OMX_Types.h>
#include "omxilindexmanager.h"
#include "omxilportmanager.h"
#include "omxilcallbackmanager.h"
#include "omxilincontextcallbackmanager.h"
#include <openmax/il/common/omxilconfigmanager.h>
#include <openmax/il/common/omxilprocessingfunction.h>
#include <openmax/il/common/omxilstatedefs.h>
#include <openmax/il/common/omxilcomponent.h>
#include "omxilcomponentimpl.h"
#include "omxilfsm.h"
#include "log.h"

COmxILComponentImpl* COmxILComponentImpl::NewL(COmxILComponent* aComponent, OMX_HANDLETYPE aHandle)
	{
	if (aComponent==0 || aHandle==0)
		{
		User::Leave(KErrArgument);
		}
	COmxILComponentImpl* self=new(ELeave) COmxILComponentImpl(aComponent, aHandle);
	return self;		
	}

COmxILComponentImpl::COmxILComponentImpl(COmxILComponent* aComponent, OMX_HANDLETYPE aHandle)
	: 	ipComponent(aComponent),
		iComponentName(0),
		ipHandle(static_cast<OMX_COMPONENTTYPE*>(aHandle)),
		ipAppData(0),
		ipCallbacks(0),
		ipFsm(0),
		ipConfigManager(0),
		ipCallbackManager(0),
		ipPortManager(0),
		ipProcessingFunction(0)
	{
	DEBUG_PRINTF3(_L8("COmxILComponentImpl::COmxILComponentImpl : Comp[%X], Handle[%X]"), aComponent, ipHandle);
	}

COmxILComponentImpl::~COmxILComponentImpl()
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::~COmxILComponentImpl : Handle[%X]"), ipHandle);
	delete ipProcessingFunction;
	delete ipPortManager;
	delete ipConfigManager;
	delete ipFsm;
	delete ipCallbackManager;	
	iComponentName.Close();
	}

OMX_ERRORTYPE
COmxILComponentImpl::GetComponentVersion(
	OMX_HANDLETYPE aComponent,
	OMX_STRING aComponentName,
	OMX_VERSIONTYPE* apComponentVersion,
	OMX_VERSIONTYPE* apSpecVersion,
	OMX_UUIDTYPE* apComponentUUID)
	{
	DEBUG_PRINTF2(_L8("COmxILComponentImpl::GetComponentVersion : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
    omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		GetComponentVersion(aComponentName, apComponentVersion,
							apSpecVersion, apComponentUUID);
    return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::SendCommand(
	OMX_HANDLETYPE aComponent,
	OMX_COMMANDTYPE aCmd,
	OMX_U32 aParam1,
	OMX_PTR aCmdData)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::SendCommand : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
	omxError =  (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		SendCommand(aCmd ,aParam1, aCmdData);
	
	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::GetParameter(
	OMX_HANDLETYPE aComponent,
	OMX_INDEXTYPE aParamIndex,
	OMX_PTR aComponentParameterStructure)
	{
    DEBUG_PRINTF3(_L8("COmxILComponentImpl::GetParameter : Handle[%X]; ParamIndex[0x%X]"), aComponent, aParamIndex);
    
    OMX_ERRORTYPE omxError;
	omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		GetParameter(aParamIndex, aComponentParameterStructure);
	
	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::SetParameter(
	OMX_HANDLETYPE aComponent,
	OMX_INDEXTYPE aIndex,
	OMX_PTR aComponentParameterStructure)
	{
    DEBUG_PRINTF3(_L8("COmxILComponentImpl::SetParameter : Handle[%X]; ParamIndex[0x%X]"), aComponent, aIndex);
    
    OMX_ERRORTYPE omxError;
	omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		SetParameter(aIndex,
					 const_cast<const TAny*>(aComponentParameterStructure));

	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::GetConfig(
	OMX_HANDLETYPE aComponent,
	OMX_INDEXTYPE aIndex,
	OMX_PTR aComponentParameterStructure)
	{
    DEBUG_PRINTF3(_L8("COmxILComponentImpl::GetConfig : Handle[%X]; ConfigIndex[0x%X]"), aComponent, aIndex);
    OMX_ERRORTYPE omxError;
	omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		GetConfig(aIndex, aComponentParameterStructure);
	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::SetConfig(
	OMX_HANDLETYPE aComponent,
	OMX_INDEXTYPE aIndex,
	OMX_PTR aComponentConfigStructure)
	{
    DEBUG_PRINTF3(_L8("COmxILComponentImpl::SetConfig : Handle[%X]; ConfigIndex[0x%X]"), aComponent, aIndex);
    OMX_ERRORTYPE omxError;
	omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		SetConfig(aIndex, const_cast<const TAny*>(aComponentConfigStructure));
	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::GetExtensionIndex(
	OMX_HANDLETYPE aComponent,
	OMX_STRING aParameterName,
	OMX_INDEXTYPE* aIndexType)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::GetExtensionIndex : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
	omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		GetExtensionIndex(aParameterName, aIndexType);
	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::GetState(
	OMX_HANDLETYPE aComponent,
	OMX_STATETYPE* aState)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::GetState : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
    omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		GetState(aState);
    return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::ComponentTunnelRequest(
	OMX_HANDLETYPE aComponent,
	OMX_U32 aPort,
	OMX_HANDLETYPE aTunneledComp,
	OMX_U32 aTunneledPort,
	OMX_TUNNELSETUPTYPE* aTunnelSetup)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::ComponentTunnelRequest : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
	omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		ComponentTunnelRequest(aPort, aTunneledComp,
							   aTunneledPort, aTunnelSetup);
	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::UseBuffer(
	OMX_HANDLETYPE aComponent,
	OMX_BUFFERHEADERTYPE** appBufferHdr,
	OMX_U32 aPortIndex,
	OMX_PTR apAppPrivate,
	OMX_U32 aSizeBytes,
	OMX_U8* aBuffer)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::UseBuffer : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
    omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		UseBuffer(appBufferHdr, aPortIndex, apAppPrivate, aSizeBytes, aBuffer);
    return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::AllocateBuffer(
	OMX_HANDLETYPE aComponent,
	OMX_BUFFERHEADERTYPE** apBuffer,
	OMX_U32 aPortIndex,
	OMX_PTR aAppData,
	OMX_U32 aSizeBytes)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::AllocateBuffer : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
    omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		AllocateBuffer(apBuffer, aPortIndex, aAppData, aSizeBytes);
    return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::FreeBuffer(
	OMX_HANDLETYPE aComponent,
	OMX_U32 aPortIndex,
	OMX_BUFFERHEADERTYPE* aBuffer)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::FreeBuffer : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
    omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		FreeBuffer(aPortIndex, aBuffer);
    return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::EmptyThisBuffer(
	OMX_HANDLETYPE aComponent,
	OMX_BUFFERHEADERTYPE* aBuffer)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::EmptyThisBuffer : Handle[%X]"), aComponent);
#ifdef _OMXIL_COMMON_BUFFER_TRACING_ON
	RDebug::Print(_L("COmxILComponentImpl::EmptyThisBuffer component=0x%08X header=0x%08X port=%d flags=0x%X filledLen=%d timeStamp=%Ld"),
		aComponent, aBuffer, aBuffer->nInputPortIndex, aBuffer->nFlags, aBuffer->nFilledLen, aBuffer->nTimeStamp);
#endif
	OMX_ERRORTYPE omxError;
	omxError =  (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		EmptyThisBuffer(aBuffer);
	
	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::FillThisBuffer(
	OMX_HANDLETYPE aComponent,
	OMX_BUFFERHEADERTYPE* aBuffer)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::FillThisBuffer : Handle[%X]"), aComponent);
#ifdef _OMXIL_COMMON_BUFFER_TRACING_ON
	RDebug::Print(_L("COmxILComponentImpl::FillThisBuffer component=0x%08X header=0x%08X port=%d"), aComponent, aBuffer, aBuffer->nOutputPortIndex);
#endif
	
	OMX_ERRORTYPE omxError;
	omxError =  (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		FillThisBuffer(aBuffer);
	
	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::SetCallbacks(
	OMX_HANDLETYPE aComponent,
	OMX_CALLBACKTYPE* aCallbacks,
	OMX_PTR aAppData)
	{	
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::SetCallbacks : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
	omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		SetCallbacks(const_cast<const OMX_CALLBACKTYPE*>(aCallbacks), aAppData);
	if (OMX_ErrorNone==omxError)
		{
		(static_cast<OMX_COMPONENTTYPE*>(aComponent))->pApplicationPrivate = aAppData;
		}
	return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::ComponentDeInit(
	OMX_HANDLETYPE aComponent)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::ComponentDeInit : Handle[%X]"), aComponent);
	delete (static_cast<COmxILComponent*>(
				(static_cast<COmxILFsm*>(
					(static_cast<OMX_COMPONENTTYPE*>(aComponent))->
					pComponentPrivate))->GetComponent()));
	return OMX_ErrorNone;
	}

OMX_ERRORTYPE
COmxILComponentImpl::UseEGLImage(
		OMX_HANDLETYPE aComponent,
		OMX_BUFFERHEADERTYPE** appBufferHdr,
		OMX_U32 aPortIndex,
		OMX_PTR aAppPrivate,
		void* eglImage)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::UseEGLImage : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
    omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		UseEGLImage(appBufferHdr, aPortIndex, aAppPrivate, eglImage);
    return omxError;
	}

OMX_ERRORTYPE
COmxILComponentImpl::ComponentRoleEnum(
	OMX_HANDLETYPE aComponent,
	OMX_U8* aRole,
	OMX_U32 aIndex)
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::ComponentRoleEnum : Handle[%X]"), aComponent);
    OMX_ERRORTYPE omxError;
    omxError = (static_cast<COmxILFsm*>
			((static_cast<OMX_COMPONENTTYPE*>(aComponent))->pComponentPrivate))->
		ComponentRoleEnum(aRole, aIndex);
    return omxError;
	}

void
COmxILComponentImpl::InitComponentL()
	{
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::InitComponent : Handle[%X]"), ipHandle);
	__ASSERT_ALWAYS(ipHandle && ipProcessingFunction && ipCallbackManager && ipPortManager && ipConfigManager, User::Panic(KOmxILComponentPanicCategory, 1));
	ipFsm = COmxILFsm::NewL(*ipComponent,
							*ipProcessingFunction,
							*ipPortManager,
							*ipConfigManager,
							*ipCallbackManager);
	
	char componentNamebuffer[OMX_MAX_STRINGNAME_SIZE];
	OMX_VERSIONTYPE componentVersion;
	OMX_VERSIONTYPE specVersion;
	OMX_UUIDTYPE componentUid;

	ipConfigManager->GetComponentVersion(componentNamebuffer,
										 &componentVersion,
										 &specVersion,
										 &componentUid);
	TBuf8<128> componentNameBuf8;
	componentNameBuf8 = const_cast<const TUint8*>(reinterpret_cast<TUint8*>(componentNamebuffer));
	iComponentName.CreateL(componentNameBuf8, componentNameBuf8.Length() + 1);
	iComponentName.PtrZ();
    DEBUG_PRINTF2(_L8("COmxILComponentImpl::InitComponent : [%S]"), &iComponentName);

	// Fill in the component handle
	ipHandle->nVersion				 = componentVersion;
	// The FSM will take care of all the API calls
	ipHandle->pComponentPrivate		 = ipFsm;
	ipHandle->pApplicationPrivate	 = 0;
	ipHandle->GetComponentVersion	 = COmxILComponentImpl::GetComponentVersion;
	ipHandle->SendCommand			 = COmxILComponentImpl::SendCommand;
	ipHandle->GetParameter			 = COmxILComponentImpl::GetParameter;
	ipHandle->SetParameter			 = COmxILComponentImpl::SetParameter;
	ipHandle->GetConfig				 = COmxILComponentImpl::GetConfig;
	ipHandle->SetConfig				 = COmxILComponentImpl::SetConfig;
	ipHandle->GetExtensionIndex		 = COmxILComponentImpl::GetExtensionIndex;
	ipHandle->GetState				 = COmxILComponentImpl::GetState;
	ipHandle->ComponentTunnelRequest = COmxILComponentImpl::ComponentTunnelRequest;
	ipHandle->UseBuffer				 = COmxILComponentImpl::UseBuffer;
	ipHandle->AllocateBuffer		 = COmxILComponentImpl::AllocateBuffer;
	ipHandle->FreeBuffer			 = COmxILComponentImpl::FreeBuffer;
	ipHandle->EmptyThisBuffer		 = COmxILComponentImpl::EmptyThisBuffer;
	ipHandle->FillThisBuffer		 = COmxILComponentImpl::FillThisBuffer;
	ipHandle->SetCallbacks			 = COmxILComponentImpl::SetCallbacks;
	ipHandle->ComponentDeInit		 = COmxILComponentImpl::ComponentDeInit;
	ipHandle->UseEGLImage			 = COmxILComponentImpl::UseEGLImage;
	ipHandle->ComponentRoleEnum		 = COmxILComponentImpl::ComponentRoleEnum;

	OMX_ERRORTYPE omxRetValue =
		ipCallbackManager->RegisterComponentHandle(ipHandle);
	if (OMX_ErrorNone != omxRetValue)
		{
		if (OMX_ErrorInsufficientResources == omxRetValue)
			{
			User::Leave(KErrNoMemory);
			}
		User::Leave(KErrGeneral);
		}

	// Let's init the FSM...
	ipFsm->InitFsm();
	}

MOmxILCallbackNotificationIf* COmxILComponentImpl::CreateCallbackManagerL(COmxILComponent::TCallbackManagerType aCallbackManagerType)
	{
	if (aCallbackManagerType==COmxILComponent::EInContext)
		{
		ipCallbackManager = COmxILInContextCallbackManager::NewL(ipHandle, ipAppData, ipCallbacks);
		}
	else if (aCallbackManagerType==COmxILComponent::EOutofContext)
		{
		ipCallbackManager = COmxILCallbackManager::NewL(ipHandle, ipAppData, ipCallbacks);
		}
		
	return ipCallbackManager;
	}

TInt COmxILComponentImpl::AddPort(const COmxILPort* aPort, OMX_DIRTYPE aDirection)
	{
	return ipPortManager->AddPort(aPort, aDirection);
	}

void COmxILComponentImpl::CreatePortManagerL(COmxILComponent::TPortManagerType /*aType*/,
											const OMX_VERSIONTYPE& aOmxVersion,
											OMX_U32 aNumberOfAudioPorts,
											OMX_U32 aStartAudioPortNumber,
											OMX_U32 aNumberOfImagePorts,
											OMX_U32 aStartImagePortNumber,
											OMX_U32 aNumberOfVideoPorts,
											OMX_U32 aStartVideoPortNumber,
											OMX_U32 aNumberOfOtherPorts,
											OMX_U32 aStartOtherPortNumber,
											OMX_BOOL aImmediateReturnTimeBuffer)
	{
	__ASSERT_ALWAYS(ipProcessingFunction && ipCallbackManager, User::Panic(KOmxILComponentPanicCategory, 1));
	ipPortManager = COmxILPortManager::NewL(*ipProcessingFunction,       // The component's processing function
											*ipCallbackManager,          // The call back manager object
											aOmxVersion,                  // Component's OMX Version
											aNumberOfAudioPorts,		 	 // The number of audio ports in this component
											aStartAudioPortNumber, 		 // The starting audio port index
											aNumberOfImagePorts,		 	 // The number of image ports in this component
											aStartImagePortNumber,		 // The starting image port index
											aNumberOfVideoPorts,		 	 // The number of video ports in this component
											aStartVideoPortNumber,		 // The starting video port index
											aNumberOfOtherPorts,		 	 // The number of other ports in this component
											aStartOtherPortNumber,	 	 // The starting other port index
											aImmediateReturnTimeBuffer
											);	
	}

void COmxILComponentImpl::RegisterProcessingFunction(const COmxILProcessingFunction* apProcessingFunction)
	{
	__ASSERT_ALWAYS(apProcessingFunction, User::Panic(KOmxILComponentPanicCategory, 1));
	ipProcessingFunction = const_cast<COmxILProcessingFunction*>(apProcessingFunction);
	}

void COmxILComponentImpl::RegisterConfigurationManager(const COmxILConfigManager* apConfigManager)
	{
	__ASSERT_ALWAYS(apConfigManager, User::Panic(KOmxILComponentPanicCategory, 1));
	ipConfigManager = const_cast<COmxILConfigManager*>(apConfigManager);
	SetPortManagerForConfigManager();
	}	
	
OMX_COMPONENTTYPE* COmxILComponentImpl::GetHandle() const
	{
	return ipHandle;
	}
OMX_PTR COmxILComponentImpl::GetAppData() const
	{
	return ipAppData;
	}
OMX_CALLBACKTYPE* COmxILComponentImpl::GetCallbacks() const
	{
	return ipCallbacks;
	}
COmxILConfigManager* COmxILComponentImpl::GetConfigManager() const
	{
	return ipConfigManager;
	}
COmxILProcessingFunction* COmxILComponentImpl::GetProcessingFunction() const
	{
	return ipProcessingFunction;
	}
MOmxILCallbackNotificationIf* COmxILComponentImpl::GetCallbackNotificationIf() const
	{
	return ipCallbackManager;
	}
void COmxILComponentImpl::SetPortManagerForConfigManager()
	{
	__ASSERT_ALWAYS(ipPortManager && ipConfigManager, User::Panic(KOmxILComponentPanicCategory, 1));
	ipConfigManager->SetPortManager(ipPortManager);
	}
