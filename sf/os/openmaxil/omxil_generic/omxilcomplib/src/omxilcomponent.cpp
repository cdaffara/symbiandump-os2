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

#include <openmax/il/common/omxilcomponent.h>
#include "omxilcomponentimpl.h"
#include "log.h"

EXPORT_C
COmxILComponent::COmxILComponent()
	{
    DEBUG_PRINTF(_L8("COmxILComponent::COmxILComponent"));
	}

EXPORT_C
COmxILComponent::~COmxILComponent()
	{
    DEBUG_PRINTF(_L8("COmxILComponent::~COmxILComponent"));
	delete ipImpl;
	}

EXPORT_C void 
COmxILComponent::ConstructL(OMX_HANDLETYPE aComponent)
	{
    DEBUG_PRINTF(_L8("COmxILComponent::ConstructL"));
	ipImpl = COmxILComponentImpl::NewL(this, aComponent);
	}
	
EXPORT_C void
COmxILComponent::InitComponentL()
	{
    DEBUG_PRINTF(_L8("COmxILComponent::InitComponent"));
	__ASSERT_ALWAYS(ipImpl, User::Panic(KOmxILComponentPanicCategory, 1));
	ipImpl->InitComponentL();
	}

EXPORT_C 
OMX_ERRORTYPE COmxILComponent::SymbianErrorToGetHandleError(TInt aSymbianError)
	{
	switch(aSymbianError)
		{
	case KErrNone:
		return OMX_ErrorNone;
	case KErrNoMemory:
		return OMX_ErrorInsufficientResources;
	case KErrArgument:
		return OMX_ErrorBadParameter;
		};
	return OMX_ErrorUndefined;	
	}

EXPORT_C	
MOmxILCallbackNotificationIf* COmxILComponent::CreateCallbackManagerL(TCallbackManagerType aCallbackManagerType)
	{
	__ASSERT_ALWAYS(ipImpl, User::Panic(KOmxILComponentPanicCategory, 1));
	return ipImpl->CreateCallbackManagerL(aCallbackManagerType);
	}
	
EXPORT_C
TInt COmxILComponent::AddPort(const COmxILPort* aPort, OMX_DIRTYPE aDirection)
	{
    __ASSERT_ALWAYS(ipImpl, User::Panic(KOmxILComponentPanicCategory, 1));
	return ipImpl->AddPort(aPort, aDirection);
	}
	
EXPORT_C	
void COmxILComponent::CreatePortManagerL(TPortManagerType aType,
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
    __ASSERT_ALWAYS(ipImpl, User::Panic(KOmxILComponentPanicCategory, 1));
	ipImpl->CreatePortManagerL(aType, 
									aOmxVersion,
									aNumberOfAudioPorts,
									aStartAudioPortNumber,
									aNumberOfImagePorts,
									aStartImagePortNumber,
									aNumberOfVideoPorts,
									aStartVideoPortNumber,
									aNumberOfOtherPorts,
									aStartOtherPortNumber,
									aImmediateReturnTimeBuffer);
	}

EXPORT_C
void COmxILComponent::RegisterProcessingFunction(const COmxILProcessingFunction* apProcessingFunction)
	{
    __ASSERT_ALWAYS(ipImpl && apProcessingFunction, User::Panic(KOmxILComponentPanicCategory, 1));
	ipImpl->RegisterProcessingFunction(apProcessingFunction);
	}

EXPORT_C
void COmxILComponent::RegisterConfigurationManager(const COmxILConfigManager* apConfigManager)
	{
    __ASSERT_ALWAYS(ipImpl && apConfigManager, User::Panic(KOmxILComponentPanicCategory, 1));
	ipImpl->RegisterConfigurationManager(apConfigManager);
	}

	
EXPORT_C
OMX_COMPONENTTYPE* COmxILComponent::GetHandle() const
	{
    __ASSERT_ALWAYS(ipImpl, User::Panic(KOmxILComponentPanicCategory, 1));
	return ipImpl->GetHandle();
	}
	
EXPORT_C	
OMX_PTR COmxILComponent::GetAppData() const
	{
    __ASSERT_ALWAYS(ipImpl, User::Panic(KOmxILComponentPanicCategory, 1));
	return ipImpl->GetAppData();
	}
	
EXPORT_C	
OMX_CALLBACKTYPE* COmxILComponent::GetCallbacks() const
	{
    __ASSERT_ALWAYS(ipImpl, User::Panic(KOmxILComponentPanicCategory, 1));
	return ipImpl->GetCallbacks();
	}
	
EXPORT_C
COmxILConfigManager* COmxILComponent::GetConfigManager() const
	{
    __ASSERT_ALWAYS(ipImpl, User::Panic(KOmxILComponentPanicCategory, 1));
	return ipImpl->GetConfigManager();
	}
	
EXPORT_C
COmxILProcessingFunction* COmxILComponent::GetProcessingFunction() const
	{
    __ASSERT_ALWAYS(ipImpl, User::Panic(KOmxILComponentPanicCategory, 1));
	return ipImpl->GetProcessingFunction();
	}
	
EXPORT_C
TInt COmxILComponent::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)
	{
	return CBase::Extension_(aExtensionId, a0, a1);
	}
	
	
