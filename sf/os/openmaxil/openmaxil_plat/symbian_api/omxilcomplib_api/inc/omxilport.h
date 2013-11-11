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

#ifndef OMXILPORT_H
#define OMXILPORT_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <openmax/il/common/omxilspecversion.h>


/**
   Base Port Panic category
*/
_LIT(KOmxILPortPanicCategory, "OmxILPort");

// Forward declarations
class COmxILPortImpl;

/**
   Structure used to pass the common information that an OpenMAX IL port needs
   at instantiation time.
*/
class TOmxILCommonPortData
	{

public:

	// OMX specification version information
	OMX_VERSIONTYPE iOmxVersion;
	// Port number the structure applies to
	OMX_U32 iPortIndex;
	// Direction (input or output) of this port
	OMX_DIRTYPE iDirection;
    // The minimum number of buffers this port requires
	OMX_U32 iBufferCountMin;
	// Minimum size, in bytes, for buffers to be used for this port
	OMX_U32 iBufferSizeMin;
	// Domain of the port
	OMX_PORTDOMAINTYPE iPortDomain;
	// Buffers contiguous requirement (true or false)
	OMX_BOOL iBuffersContiguous;
	// Buffer aligment requirements
	OMX_U32 iBufferAlignment;
	// Port supplier preference when tunneling between two ports
	OMX_BUFFERSUPPLIERTYPE iBufferSupplier;

	// \brief This is the associated port that will be used to propagate buffer marks
	// found in incoming buffer headers.
	//
	// In general, for any output port, this parameter should be configured
	// with constant COmxILPort::KBufferMarkPropagationPortNotNeeded except
	// for an output port of a source component, in which case it must be used
	// the same index of the port that this structure applies to. For an input
	// port of a sink component,
	// COmxILPort::KBufferMarkPropagationPortNotNeeded must also be used.
	OMX_U32 iBufferMarkPropagationPortIndex;

	// Constructor
	IMPORT_C TOmxILCommonPortData(
		OMX_VERSIONTYPE aOmxVersion,
		OMX_U32 aPortIndex,
		OMX_DIRTYPE aDirection,
		OMX_U32 aBufferCountMin,
		OMX_U32 aBufferSizeMin,
		OMX_PORTDOMAINTYPE aPortDomain,
		OMX_BOOL aBuffersContiguous,
		OMX_U32 aBufferAlignment,
		OMX_BUFFERSUPPLIERTYPE aBufferSupplier,
		OMX_U32 aBufferMarkPropagationPortIndex);

	};


/**
   OpenMAX IL component port base class.
*/
class COmxILPort : public CBase
	{
friend class COmxILPortImpl;

public:
	static const TInt	KBufferMarkPropagationPortNotNeeded = 0x7FFFFFFF;

	~COmxILPort();

	/**
	   This method is used to retrieve the local OpenMAX IL parameter indexes
	   managed by the specific port. The implementation should collect indexes
	   from the parent class. Local indexes should be appended in order to the
	   list received as parameter. The implementation should handle index
	   duplication as a non-error situation (i.e., the resulting list will
	   contain unique indexes).

	   @param aIndexArray An array of parameter indexes where the local
	   indexes are to be appended.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE GetLocalOmxParamIndexes(
		RArray<TUint>& aIndexArray) const = 0;

	/**
	   This method is used to retrieve the local OpenMAX IL config indexes
	   managed by the specific port. The implementation should collect indexes
	   from the parent class. Local indexes should be appended in order to the
	   list received as parameter. The implementation should handle index
	   duplication as a non-error situation (i.e., the resulting list will
	   contain unique indexes).

	   @param aIndexArray An array of config indexes where the local indexes
	   are to be appended.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE GetLocalOmxConfigIndexes(
		RArray<TUint>& aIndexArray) const = 0;

	/**
	   Port-specific version of the OpenMAX IL GetParameter API.

	   @param aParamIndex The index of the structure that is to be filled.
	   @param apComponentParameterStructure A pointer to the IL structure.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE GetParameter(
		OMX_INDEXTYPE aParamIndex,
		TAny* apComponentParameterStructure) const = 0;

	/**
	   Port-specific version of the OpenMAX IL SetParameter API.

	   @param aParamIndex The index of the structure that is to be set.
	   @param apComponentParameterStructure A pointer to the IL structure.
	   @param aUpdateProcessingFunction A flag to signal that the component's
	   processing function needs to be updated with this structure.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE SetParameter(
		OMX_INDEXTYPE aParamIndex,
		const TAny* apComponentParameterStructure,
		TBool& aUpdateProcessingFunction) = 0;

	virtual OMX_ERRORTYPE GetConfig(
		OMX_INDEXTYPE aConfigIndex,
		TAny* apComponentConfigStructure) const;

	virtual OMX_ERRORTYPE SetConfig(
		OMX_INDEXTYPE aConfigIndex,
		const TAny* apComponentConfigStructure,
		TBool& aUpdateProcessingFunction);

	virtual OMX_ERRORTYPE GetExtensionIndex(
		OMX_STRING aParameterName,
		OMX_INDEXTYPE* apIndexType) const;

	virtual OMX_ERRORTYPE PopulateBuffer(
		OMX_BUFFERHEADERTYPE** appBufferHdr,
		const OMX_PTR apAppPrivate,
		OMX_U32 aSizeBytes,
		OMX_U8* apBuffer,
		TBool& portPopulationCompleted);

	virtual OMX_ERRORTYPE FreeBuffer(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		TBool& portDepopulationCompleted);

	virtual OMX_ERRORTYPE TunnelRequest(
		OMX_HANDLETYPE aTunneledComp,
		OMX_U32 aTunneledPort,
		OMX_TUNNELSETUPTYPE* apTunnelSetup);

	virtual OMX_ERRORTYPE PopulateTunnel(
		TBool& portPopulationCompleted);

	virtual OMX_ERRORTYPE FreeTunnel(
		TBool& portDepopulationCompleted);

	virtual TBool SetBufferSent(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		TBool& aBufferMarkedWithOwnMark);

	virtual TBool SetBufferReturned(
		OMX_BUFFERHEADERTYPE* apBufferHeader);

	virtual void SetTransitionToDisabled();

	virtual void SetTransitionToEnabled();

	virtual void SetTransitionToDisabledCompleted();

	virtual void SetTransitionToEnabledCompleted();

	virtual OMX_ERRORTYPE StoreBufferMark(
		const OMX_MARKTYPE* pMark);

	virtual OMX_ERRORTYPE SetComponentRoleDefaults(
		TUint aComponentRoleIndex);

	virtual OMX_ERRORTYPE DoPortReconfiguration(
		TUint aPortSettingsIndex,
		const TDesC8& aPortSettings,
		OMX_EVENTTYPE& aEventForILClient);

	OMX_DIRTYPE Direction() const;

	IMPORT_C TBool IsEnabled() const;

	TBool IsPopulated() const;

	TBool IsDePopulated() const;

	TBool IsTunnelled() const;

	TBool IsTunnelledAndBufferSupplier() const;

	virtual TBool HasAllBuffersAtHome() const;

	TBool IsTransitioningToEnabled() const;

	TBool IsTransitioningToDisabled() const;

	virtual TBool IsBufferAtHome(
		OMX_BUFFERHEADERTYPE* apHeaderHeader) const;

	IMPORT_C OMX_U32 Index() const;

	OMX_PORTDOMAINTYPE Domain() const;

	OMX_U32 Count() const;

	OMX_BUFFERHEADERTYPE* const& operator[](TInt anIndex) const;

	OMX_BUFFERHEADERTYPE*& operator[](TInt anIndex);

	OMX_U32 BufferMarkPropagationPort() const;

protected:

	COmxILPort();
	void ConstructL(const TOmxILCommonPortData& aCommonPortData);
	/**
	   When the IL Client calls SetParameter with index
	   OMX_IndexParamPortDefinition, this method is called to set the fields in
	   the OMX_PARAM_PORTDEFINITIONTYPE (@see iParamPortDefinition) structure
	   that are specific to this port's domain.

	   @param aPortDefinition The OpenMAX IL structure received from the IL Client.
	   @param aUpdateProcessingFunction A flag to signal that the component's
	   processing function needs to be updated with the this structure.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE SetFormatInPortDefinition(
		const OMX_PARAM_PORTDEFINITIONTYPE& aPortDefinition,
		TBool& aUpdateProcessingFunction) = 0;

	/**
	   On a tunnel request, this method is called to check the compatibility of
	   the fields in the OMX_PARAM_PORTDEFINITIONTYPE structure of the
	   tunnelled component that are specific to this port's domain.

	   @param aPortDefinition The OpenMAX IL structure received from a
	   tunnelled component.

	   @return ETrue if compatible, EFalse otherwise.
	*/
	virtual TBool IsTunnelledPortCompatible(
		const OMX_PARAM_PORTDEFINITIONTYPE& aPortDefinition) const = 0;

	virtual OMX_ERRORTYPE DoBufferAllocation(
		OMX_U32 aSizeBytes,
		OMX_U8*& apPortSpecificBuffer,
		OMX_PTR& apPortPrivate,
		OMX_PTR& apPlatformPrivate,
		OMX_PTR apAppPrivate = 0);

	virtual void DoBufferDeallocation(
		OMX_PTR apPortSpecificBuffer,
		OMX_PTR apPortPrivate,
		OMX_PTR apPlatformPrivate,
		OMX_PTR apAppPrivate = 0);

	virtual OMX_ERRORTYPE DoBufferWrapping(
		OMX_U32 aSizeBytes,
		OMX_U8* apBuffer,
		OMX_PTR& apPortPrivate,
		OMX_PTR& apPlatformPrivate,
		OMX_PTR apAppPrivate = 0);

	virtual void DoBufferUnwrapping(
		OMX_PTR apPortSpecificBuffer,
		OMX_PTR apPortPrivate,
		OMX_PTR apPlatformPrivate,
		OMX_PTR apAppPrivate = 0);

	virtual OMX_ERRORTYPE DoOmxUseBuffer(
		OMX_HANDLETYPE aTunnelledComponent,
		OMX_BUFFERHEADERTYPE** appBufferHdr,
		OMX_U32 aTunnelledPortIndex,
		OMX_PTR apPortPrivate,
		OMX_PTR apPlatformPrivate,
		OMX_U32 aSizeBytes,
		OMX_U8* apBuffer);

	template<typename T>
	inline OMX_ERRORTYPE GetParamStructureFromTunnel(
		T& aComponentConfigStructure, OMX_INDEXTYPE aParamIndex) const;

	OMX_ERRORTYPE StoreBufferMark(
		OMX_HANDLETYPE& ipMarkTargetComponent,
		OMX_PTR& ipMarkData);

	IMPORT_C void CleanUpPort();
	
protected:
	IMPORT_C const OMX_PARAM_PORTDEFINITIONTYPE& GetParamPortDefinition() const;
	IMPORT_C const OMX_HANDLETYPE& GetTunnelledComponent() const;
	IMPORT_C const OMX_U32& GetTunnelledPort() const;

	IMPORT_C OMX_PARAM_PORTDEFINITIONTYPE& GetParamPortDefinition();
	IMPORT_C OMX_HANDLETYPE& GetTunnelledComponent();
	IMPORT_C OMX_U32& GetTunnelledPort();
	//From CBase
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny *&a0, TAny *a1);
	
private:
	COmxILPortImpl* ipPortImpl;
	
	};

#include <openmax/il/common/omxilport.inl>

#endif // OMXILPORT_H
