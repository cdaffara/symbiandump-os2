// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "log.h"
#include "omxilportimpl.h"
#include <openmax/il/common/omxilport.h>
#include <openmax/il/common/omxilutil.h>

const TInt COmxILPort::KBufferMarkPropagationPortNotNeeded;

EXPORT_C
TOmxILCommonPortData::TOmxILCommonPortData(
	OMX_VERSIONTYPE aOmxVersion,
	OMX_U32 aPortIndex,
	OMX_DIRTYPE aDirection,
	OMX_U32 aBufferCountMin,
	OMX_U32 aBufferSizeMin,
	OMX_PORTDOMAINTYPE aPortDomain,
	OMX_BOOL aBuffersContiguous,
	OMX_U32 aBufferAlignment,
	OMX_BUFFERSUPPLIERTYPE aBufferSupplier,
	OMX_U32 aBufferMarkPropagationPortIndex)
	:
	iOmxVersion(aOmxVersion),
	iPortIndex(aPortIndex),
	iDirection(aDirection),
	iBufferCountMin(aBufferCountMin),
	iBufferSizeMin(aBufferSizeMin),
	iPortDomain(aPortDomain),
	iBuffersContiguous(aBuffersContiguous),
	iBufferAlignment(aBufferAlignment),
	iBufferSupplier(aBufferSupplier),
	iBufferMarkPropagationPortIndex(aBufferMarkPropagationPortIndex)
	{
	}


COmxILPort::COmxILPort()
	{
    DEBUG_PRINTF(_L8("COmxILPort::COmxILPort"));
	}


COmxILPort::~COmxILPort()
	{
    DEBUG_PRINTF(_L8("COmxILPort::~COmxILPort"));
	delete ipPortImpl;
	}

void COmxILPort::ConstructL(const TOmxILCommonPortData& aCommonPortData)
	{
    __ASSERT_DEBUG(aCommonPortData.iDirection == OMX_DirInput ||
                   aCommonPortData.iDirection == OMX_DirOutput,
                   User::Panic(KOmxILPortPanicCategory, 1));

    // From Section 3.1.2.12.1 .
    //
    // nBufferCountMin : "The component shall define this non-zero default
    // value."
    //
    // nBufferCountActual : "The component shall set a default value no less
    // than nBufferCountMin for this field"
    __ASSERT_DEBUG(aCommonPortData.iBufferCountMin > 0,
                   User::Panic(KOmxILPortPanicCategory, 1));
    
	ipPortImpl=COmxILPortImpl::NewL(aCommonPortData, *this);
	}
	
OMX_ERRORTYPE
COmxILPort::GetLocalOmxParamIndexes(RArray<TUint>& aIndexArray) const
	{
    DEBUG_PRINTF(_L8("COmxILPort::GetLocalOmxParamIndexes"));
	
	TInt err = aIndexArray.InsertInOrder(OMX_IndexParamPortDefinition);

	// Note that index duplication is OK.
	if (KErrNone == err || KErrAlreadyExists == err)
		{
		err = aIndexArray.InsertInOrder(OMX_IndexParamCompBufferSupplier);
		}

	if (KErrNone != err && KErrAlreadyExists != err)
		{
		return OMX_ErrorInsufficientResources;
		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILPort::GetLocalOmxConfigIndexes(RArray<TUint>& /*aIndexArray*/) const
	{
    DEBUG_PRINTF(_L8("COmxILPort::GetLocalOmxConfigIndexes"));

	return OMX_ErrorNone;

	}


OMX_ERRORTYPE
COmxILPort::GetParameter(OMX_INDEXTYPE aParamIndex,
						 TAny* apComponentParameterStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILPort::GetParameter"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->GetParameter(aParamIndex, apComponentParameterStructure);
	}

OMX_ERRORTYPE
COmxILPort::SetParameter(OMX_INDEXTYPE aParamIndex,
						 const TAny* apComponentParameterStructure,
						 TBool& aUpdateProcessingFunction)
	{
    DEBUG_PRINTF(_L8("COmxILPort::SetParameter"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->SetParameter(aParamIndex, apComponentParameterStructure, aUpdateProcessingFunction);
	}


OMX_ERRORTYPE
COmxILPort::GetConfig(OMX_INDEXTYPE /*aConfigIndex*/,
					  TAny* /*apComponentConfigStructure*/) const
	{
    DEBUG_PRINTF(_L8("COmxILPort::GetConfig"));
	return OMX_ErrorUnsupportedIndex;
	}

OMX_ERRORTYPE
COmxILPort::SetConfig(OMX_INDEXTYPE /*aConfigIndex*/,
					  const TAny* /*apComponentConfigStructure*/,
					  TBool& /*aUpdateProcessingFunction*/)
	{
    DEBUG_PRINTF(_L8("COmxILPort::SetConfig"));
	return OMX_ErrorUnsupportedIndex;
	}

OMX_ERRORTYPE
COmxILPort::GetExtensionIndex(OMX_STRING /* aParameterName */,
							  OMX_INDEXTYPE* /* apIndexType */) const
	{
	// This method is intended to be overridden by the concrete ports when
	// needed...
	return OMX_ErrorUnsupportedIndex;
	}

OMX_ERRORTYPE
COmxILPort::PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
						   const OMX_PTR apAppPrivate,
						   OMX_U32 aSizeBytes,
						   OMX_U8* apBuffer,
						   TBool& aPortPopulationCompleted)
	{
    DEBUG_PRINTF2(_L8("COmxILPort::PopulateBuffer : pBuffer [%X]"), apBuffer);
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->PopulateBuffer(appBufferHdr, apAppPrivate, aSizeBytes, apBuffer, aPortPopulationCompleted);
	}


OMX_ERRORTYPE
COmxILPort::FreeBuffer(OMX_BUFFERHEADERTYPE* apBufferHeader,
					   TBool& aPortDepopulationCompleted)
	{
    DEBUG_PRINTF2(_L8("COmxILPort::FreeBuffer : BUFFER [%X]"), apBufferHeader);
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->FreeBuffer(apBufferHeader, aPortDepopulationCompleted);
	}


OMX_ERRORTYPE
COmxILPort::TunnelRequest(OMX_HANDLETYPE aTunneledComp,
						  OMX_U32 aTunneledPort,
						  OMX_TUNNELSETUPTYPE* apTunnelSetup)
	{
    DEBUG_PRINTF(_L8("COmxILPort::TunnelRequest"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->TunnelRequest(aTunneledComp, aTunneledPort, apTunnelSetup);
	}


OMX_ERRORTYPE
COmxILPort::PopulateTunnel(TBool& portPopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILPort::PopulateTunnel"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->PopulateTunnel(portPopulationCompleted);
	}


OMX_ERRORTYPE
COmxILPort::FreeTunnel(TBool& portDepopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILPort::FreeTunnel"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->FreeTunnel(portDepopulationCompleted);
	}



TBool
COmxILPort::SetBufferSent(OMX_BUFFERHEADERTYPE* apBufferHeader,
						  TBool& aBufferMarkedWithOwnMark)
	{
    DEBUG_PRINTF(_L8("COmxILPort::SetBufferSent"));
	__ASSERT_DEBUG(apBufferHeader, User::Panic(KOmxILPortPanicCategory, 1));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->SetBufferSent(apBufferHeader, aBufferMarkedWithOwnMark);
	}

TBool
COmxILPort::SetBufferReturned(OMX_BUFFERHEADERTYPE* apBufferHeader)
	{
    DEBUG_PRINTF(_L8("COmxILPort::SetBufferReturned"));

	__ASSERT_DEBUG(apBufferHeader, User::Panic(KOmxILPortPanicCategory, 1));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->SetBufferReturned(apBufferHeader);
	}

void
COmxILPort::SetTransitionToEnabled()
	{
    DEBUG_PRINTF(_L8("COmxILPort::SetTransitionToEnabled"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->SetTransitionToEnabled();
	}

void
COmxILPort::SetTransitionToDisabled()
	{
    DEBUG_PRINTF(_L8("COmxILPort::SetTransitionToDisabled"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->SetTransitionToDisabled();
	}

void
COmxILPort::SetTransitionToDisabledCompleted()
	{
    DEBUG_PRINTF(_L8("COmxILPort::SetTransitionToDisabledCompleted"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->SetTransitionToDisabledCompleted();
	}

void
COmxILPort::SetTransitionToEnabledCompleted()
	{
    DEBUG_PRINTF(_L8("COmxILPort::SetTransitionToEnabledCompleted"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->SetTransitionToEnabledCompleted();
	}

OMX_ERRORTYPE
COmxILPort::StoreBufferMark(const OMX_MARKTYPE* apMark)
	{
    DEBUG_PRINTF(_L8("COmxILPort::StoreBufferMark"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->StoreBufferMark(apMark);
	}


/**
   This utility method may be called from the most derived port
   class' destructor to delete any buffer header and or buffer that may remain
   allocated in the port. This typically happens when the component is unloaded
   without being properly transitioned from OMX_StateIdle to OMX_StateLoaded.

 */
EXPORT_C void
COmxILPort::CleanUpPort()
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->CleanUpPort();
	}

/**
   This is to be overriden by concrete ports that either support more than
   one role or that want to let the IL Client to reset the component to
   some default state...

   @param aComponentRoleIndex The index of the role that is to be assumed by
   the component.

   @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE
COmxILPort::SetComponentRoleDefaults(TUint /*aComponentRoleIndex*/)
	{
    DEBUG_PRINTF(_L8("COmxILPort::SetComponentRoleDefaults"));

	return OMX_ErrorNotImplemented;
	}

/**
   This is to be overriden by concrete ports that either support more than one
   role or that want to let the IL Client to reset the component to some
   default state...

   @param aPortSettingsIndex An implementation-specific identifier that the
   implementation associates to the setting(s) that need(s) updating in the
   port.

   @param aPortSettings A buffer descriptor that contains an
   implementation-specific structure with the new setting(s) that need to
   be updated in the port.

   @param [output] aEventForILClient An event that the port may choose to
   deliver to the IL Client. This should be one of OMX_EventPortSettingsChanged
   or OMX_EventPortFormatDetected. Use OMX_EventMax if no event need to be
   sent.
 */
OMX_ERRORTYPE
COmxILPort::DoPortReconfiguration(TUint /* aPortSettingsIndex */,
								  const TDesC8& /* aPortSettings */,
								  OMX_EVENTTYPE& aEventForILClient)
	{
    DEBUG_PRINTF(_L8("COmxILPort::DoPortReconfiguration"));

	// This means no need to notify the IL Client...
	aEventForILClient = OMX_EventMax;

	return OMX_ErrorNotImplemented;
	}

OMX_ERRORTYPE
COmxILPort::StoreBufferMark(OMX_HANDLETYPE& ipMarkTargetComponent,
							OMX_PTR& ipMarkData)
	{
    DEBUG_PRINTF(_L8("COmxILPort::StoreBufferMark"));
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->StoreBufferMark(ipMarkTargetComponent, ipMarkData);
	}

TBool
COmxILPort::HasAllBuffersAtHome() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	__ASSERT_ALWAYS(ipPortImpl->IsTunnelledAndBufferSupplier(),
					User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->HasAllBuffersAtHome();
	}

TBool
COmxILPort::IsBufferAtHome(OMX_BUFFERHEADERTYPE* apBufferHeader) const
	{
	DEBUG_PRINTF2(_L8("COmxILPort::IsBufferAtHome : [%X]"), apBufferHeader);
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->IsBufferAtHome(apBufferHeader);
	}


/**
   This method provides a mechanism for ports to place the port-specific
   buffer allocation logic (typically used in an OMX_AllocateBuffer
   scenario).

   @param aSizeBytes The size in bytes of the buffer to be allocated.

   @param [OUT] apPortSpecificBuffer A pointer to the memory area allocated by
   the port.

   @param [OUT] apPortPrivate A pointer that refers to an optional
   port-specific structure.

   @apPlatformPrivate[OUT] A pointer to an optional platform-specific
   structure.

   @param apAppPrivate A pointer that refers to a buffer supplier-specific
   structure.

   @return OMX_ERRORTYPE
*/
OMX_ERRORTYPE
COmxILPort::DoBufferAllocation(OMX_U32 aSizeBytes,
							   OMX_U8*& apPortSpecificBuffer,
							   OMX_PTR& /*apPortPrivate*/,
							   OMX_PTR& /* apPlatformPrivate */,
							   OMX_PTR /* apAppPrivate = 0 */)
	{
	DEBUG_PRINTF2(_L8("COmxILPort::DoBufferAllocation : aSizeBytes[%u]"), aSizeBytes);

	__ASSERT_DEBUG(aSizeBytes > 0, User::Panic(KOmxILPortPanicCategory, 1));

	apPortSpecificBuffer = new OMX_U8[aSizeBytes];
	if (!apPortSpecificBuffer)
		{
		return OMX_ErrorInsufficientResources;
		}

	return OMX_ErrorNone;
	}

/**
   This method provides a mechanism for ports to place the port-specific
   buffer deallocation logic (typically used in an OMX_FreeBuffer
   scenario).

   @param apPortSpecificBuffer A pointer to the memory area to be deallocated
   by the port.

   @param apPortPrivate A pointer that refers to a port-specific structure.

   @param apPlatformPrivate A pointer to an optional platform-specific
   structure.

   @param apAppPrivate A pointer that refers to a buffer supplier-specific
   structure.

*/
void
COmxILPort::DoBufferDeallocation(OMX_PTR apPortSpecificBuffer ,
								 OMX_PTR /*apPortPrivate*/,
								 OMX_PTR /* apPlatformPrivate */,
								 OMX_PTR /* apAppPrivate = 0 */)
	{
	DEBUG_PRINTF(_L8("COmxILPort::DoBufferDeallocation"));

	__ASSERT_DEBUG(apPortSpecificBuffer, User::Panic(KOmxILPortPanicCategory, 1));

	delete[] apPortSpecificBuffer;
	}

/**
   This method provides a mechanism for ports to place the port-specific buffer
   wrapping logic (typically used in an OMX_UseBuffer scenario).

   @param aSizeBytes The size in bytes of the buffer to be wrapped.

   @param apBuffer A pointer received from the IL Client or another
   component that references the allocated memory area .

   @param [OUT] apPortPrivate A pointer that refers to a port-specific
   structure.

   @param [OUT] apPlatformPrivate A pointer to an optional platform-specific
   structure.

   @param [OUT] apAppPrivate A pointer that refers to a buffer
   supplier-specific structure.

   @return OMX_ERRORTYPE
*/
OMX_ERRORTYPE
COmxILPort::DoBufferWrapping(OMX_U32 /*aSizeBytes*/,
							 OMX_U8* /*apBuffer*/,
							 OMX_PTR& /*apPortPrivate*/,
							 OMX_PTR& /* apPlatformPrivate */,
							 OMX_PTR /* apAppPrivate = 0 */)
	{
	DEBUG_PRINTF(_L8("COmxILPort::DoBufferWrapping"));
	// Does nothing, to be overloaded by Components if they require 
	return OMX_ErrorNone;
	}

/**
   This method provides a mechanism for ports to place the port-specific buffer
   unwrapping logic (typically used in an OMX_FreeBuffer scenario).

   @param apPortSpecificBuffer A pointer to the allocated memory area.

   @param apPortPrivate A pointer that refers to a port-specific structure.

   @param apPlatformPrivate A pointer to an optional platform-specific
   structure.

   @param apAppPrivate A pointer that refers to a buffer supplier-specific
   structure.

*/
void
COmxILPort::DoBufferUnwrapping(OMX_PTR /* apBuffer*/,
							   OMX_PTR /*appPortPrivate*/,
							   OMX_PTR /* apPlatformPrivate */,
							   OMX_PTR /* apAppPrivate = 0 */)
	{
	DEBUG_PRINTF(_L8("COmxILPort::DoBufferUnwrapping"));
	// Does nothing, to be overloaded by Components if they have their own 
	// DoBufferWrapping function 
	}

/**
   This method gets called during tunnelled buffer allocation (i.e.,
   PopulateTunnel). This is to allow component implementations to override this
   default implementation in a way that the pAppPrivate pointer parameter of
   OMX_UseBuffer can be used to convey pPortPrivate or pPlatformPrivate to the
   tunnelled component.

   @param aTunnelledComponent Handle to the tunnelled component

   @param [OUT] appBufferHdr The buffer header that will be allocated by the
   tunnelled component

   @param aTunnelledPortIndex The index of the tunnelled port

   @param apPortPrivate pPortPrivate pointer as returned by DoBufferAllocation

   @param apPlatformPrivate pPlatformPrivate pointer as returned by
   DoBufferAllocation

   @aSizeBytes The size in bytes of the to be allocated buffer

   @apBuffer A pointer to the allocated buffer

   @return OMX_ERRORTYPE

*/
OMX_ERRORTYPE
COmxILPort::DoOmxUseBuffer(OMX_HANDLETYPE aTunnelledComponent,
						   OMX_BUFFERHEADERTYPE** appBufferHdr,
						   OMX_U32 aTunnelledPortIndex,
						   OMX_PTR /* apPortPrivate */,
						   OMX_PTR /* apPlatformPrivate */,
						   OMX_U32 aSizeBytes,
						   OMX_U8* apBuffer)
	{

	DEBUG_PRINTF(_L8("COmxILPort::DoOmxUseBuffer"));

	return OMX_UseBuffer(
		aTunnelledComponent,
		appBufferHdr,
		aTunnelledPortIndex,
		0,
		aSizeBytes,
		apBuffer);

	}


OMX_DIRTYPE
COmxILPort::Direction() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->Direction();
	}

EXPORT_C TBool
COmxILPort::IsEnabled() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->IsEnabled();
	}

TBool
COmxILPort::IsPopulated() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->IsPopulated();
	}

TBool
COmxILPort::IsDePopulated() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->IsDePopulated();
	}

TBool
COmxILPort::IsTunnelled() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->IsTunnelled();
	}

TBool
COmxILPort::IsTunnelledAndBufferSupplier() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->IsTunnelledAndBufferSupplier();
	}

TBool
COmxILPort::IsTransitioningToEnabled() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->IsTransitioningToEnabled();
	}

TBool
COmxILPort::IsTransitioningToDisabled() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->IsTransitioningToDisabled();
	}

EXPORT_C OMX_U32
COmxILPort::Index() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->Index();
	}

OMX_PORTDOMAINTYPE
COmxILPort::Domain() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->Domain();
	}

OMX_U32
COmxILPort::Count() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->Count();
	}

OMX_BUFFERHEADERTYPE* const&
COmxILPort::operator[](TInt anIndex) const
	{
	return this->operator[](anIndex);
	}

OMX_BUFFERHEADERTYPE*&
COmxILPort::operator[](TInt anIndex)
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->operator[](anIndex);
	}


OMX_U32
COmxILPort::BufferMarkPropagationPort() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->BufferMarkPropagationPort();
	}

EXPORT_C const OMX_PARAM_PORTDEFINITIONTYPE& COmxILPort::GetParamPortDefinition() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->GetParamPortDefinition();
	}
	
EXPORT_C const OMX_HANDLETYPE& COmxILPort::GetTunnelledComponent() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->GetTunnelledComponent();
	}
	
EXPORT_C const OMX_U32& COmxILPort::GetTunnelledPort() const
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->GetTunnelledPort();
	}

EXPORT_C OMX_PARAM_PORTDEFINITIONTYPE& COmxILPort::GetParamPortDefinition()
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->GetParamPortDefinition();
	}
	
EXPORT_C OMX_HANDLETYPE& COmxILPort::GetTunnelledComponent()
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->GetTunnelledComponent();
	}
	
EXPORT_C OMX_U32& COmxILPort::GetTunnelledPort()
	{
	__ASSERT_ALWAYS(ipPortImpl, User::Panic(KOmxILPortPanicCategory, 1));
	return ipPortImpl->GetTunnelledPort();
	}

EXPORT_C
TInt COmxILPort::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)
	{
	return CBase::Extension_(aExtensionId, a0, a1);
	}	




