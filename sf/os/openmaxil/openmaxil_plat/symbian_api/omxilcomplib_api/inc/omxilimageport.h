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
 * @file
 * @internalTechnology
 */

#ifndef OMXILIMAGEPORT_H
#define OMXILIMAGEPORT_H

#include <e32base.h>

#include <openmax/il/khronos/v1_x/OMX_Types.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

#include <openmax/il/common/omxilport.h>

class COmxILImagePortImpl;
/**
   Image Port Panic category
*/
_LIT(KOmxILImagePortPanicCategory, "OmxILImagePort");

class COmxILImagePort : public COmxILPort
	{

public:

	IMPORT_C ~COmxILImagePort();
	
	// From COmxILPort
	IMPORT_C OMX_ERRORTYPE GetLocalOmxParamIndexes(RArray<TUint>& aIndexArray) const = 0;

	// From COmxILPort
	IMPORT_C OMX_ERRORTYPE GetLocalOmxConfigIndexes(RArray<TUint>& aIndexArray) const = 0;

	// From COmxILPort
	IMPORT_C OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE aParamIndex,
							   TAny* apComponentParameterStructure) const = 0;
	// From COmxILPort
	IMPORT_C OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure,
							   TBool& aUpdateProcessingFunction) = 0;
	// From COmxILPort									
	IMPORT_C virtual OMX_ERRORTYPE GetConfig(OMX_INDEXTYPE aConfigIndex,
											TAny* apComponentConfigStructure) const;
	// From COmxILPort
	IMPORT_C virtual OMX_ERRORTYPE SetConfig(OMX_INDEXTYPE aConfigIndex,
									const TAny* apComponentConfigStructure,
									TBool& aUpdateProcessingFunction);
	
	// From COmxILPort
    IMPORT_C virtual OMX_ERRORTYPE GetExtensionIndex(
        OMX_STRING aParameterName,
        OMX_INDEXTYPE* apIndexType) const;

	// From COmxILPort
    IMPORT_C virtual OMX_ERRORTYPE PopulateBuffer(
        OMX_BUFFERHEADERTYPE** appBufferHdr,
        const OMX_PTR apAppPrivate,
        OMX_U32 aSizeBytes,
        OMX_U8* apBuffer,
        TBool& portPopulationCompleted);

	// From COmxILPort
    IMPORT_C virtual OMX_ERRORTYPE FreeBuffer(
        OMX_BUFFERHEADERTYPE* apBufferHeader,
        TBool& portDepopulationCompleted);

	// From COmxILPort
    IMPORT_C virtual OMX_ERRORTYPE TunnelRequest(
        OMX_HANDLETYPE aTunneledComp,
        OMX_U32 aTunneledPort,
        OMX_TUNNELSETUPTYPE* apTunnelSetup);

	// From COmxILPort
    IMPORT_C virtual OMX_ERRORTYPE PopulateTunnel(
        TBool& portPopulationCompleted);

	// From COmxILPort
    IMPORT_C virtual OMX_ERRORTYPE FreeTunnel(
        TBool& portDepopulationCompleted);

	// From COmxILPort
    IMPORT_C virtual TBool SetBufferSent(
        OMX_BUFFERHEADERTYPE* apBufferHeader,
        TBool& aBufferMarkedWithOwnMark);

	// From COmxILPort
    IMPORT_C virtual TBool SetBufferReturned(
        OMX_BUFFERHEADERTYPE* apBufferHeader);

	// From COmxILPort
    IMPORT_C virtual void SetTransitionToDisabled();

	// From COmxILPort	
    IMPORT_C virtual void SetTransitionToEnabled();

	// From COmxILPort	
    IMPORT_C virtual void SetTransitionToDisabledCompleted();
	
	// From COmxILPort
    IMPORT_C virtual void SetTransitionToEnabledCompleted();
	
	// From COmxILPort
    IMPORT_C virtual OMX_ERRORTYPE StoreBufferMark(
        const OMX_MARKTYPE* pMark);
		
	// From COmxILPort
    IMPORT_C virtual OMX_ERRORTYPE SetComponentRoleDefaults(
        TUint aComponentRoleIndex);
		
	// From COmxILPort
    IMPORT_C virtual OMX_ERRORTYPE DoPortReconfiguration(
        TUint aPortSettingsIndex,
        const TDesC8& aPortSettings,
        OMX_EVENTTYPE& aEventForILClient);
		
	// From COmxILPort
    IMPORT_C virtual TBool HasAllBuffersAtHome() const;
	
	// From COmxILPort	
    IMPORT_C virtual TBool IsBufferAtHome(
        OMX_BUFFERHEADERTYPE* apHeaderHeader) const;    
							  
protected:

	IMPORT_C COmxILImagePort();
				
	// From COmxILPort
	OMX_ERRORTYPE SetFormatInPortDefinition(
		const OMX_PARAM_PORTDEFINITIONTYPE& aPortDefinition,
		TBool& aUpdateProcessingFunction) = 0;

	// From COmxILPort
	TBool IsTunnelledPortCompatible(
		const OMX_PARAM_PORTDEFINITIONTYPE& aPortDefinition) const = 0;
		
	IMPORT_C void ConstructL(const TOmxILCommonPortData& aCommonPortData,
					const RArray<OMX_IMAGE_CODINGTYPE>& aSupportedImageFormats,
					const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats);
					
	IMPORT_C virtual OMX_ERRORTYPE DoBufferAllocation(
		OMX_U32 aSizeBytes,
		OMX_U8*& apPortSpecificBuffer,
		OMX_PTR& apPortPrivate,
		OMX_PTR& apPlatformPrivate,
		OMX_PTR apAppPrivate = 0);

	IMPORT_C virtual void DoBufferDeallocation(
		OMX_PTR apPortSpecificBuffer,
		OMX_PTR apPortPrivate,
		OMX_PTR apPlatformPrivate,
		OMX_PTR apAppPrivate = 0);

	IMPORT_C virtual OMX_ERRORTYPE DoBufferWrapping(
		OMX_U32 aSizeBytes,
		OMX_U8* apBuffer,
		OMX_PTR& apPortPrivate,
		OMX_PTR& apPlatformPrivate,
		OMX_PTR apAppPrivate = 0);

	IMPORT_C virtual void DoBufferUnwrapping(
		OMX_PTR apPortSpecificBuffer,
		OMX_PTR apPortPrivate,
		OMX_PTR apPlatformPrivate,
		OMX_PTR apAppPrivate = 0);

	IMPORT_C virtual OMX_ERRORTYPE DoOmxUseBuffer(
		OMX_HANDLETYPE aTunnelledComponent,
		OMX_BUFFERHEADERTYPE** appBufferHdr,
		OMX_U32 aTunnelledPortIndex,
		OMX_PTR apPortPrivate,
		OMX_PTR apPlatformPrivate,
		OMX_U32 aSizeBytes,
		OMX_U8* apBuffer);

	IMPORT_C const RArray<OMX_IMAGE_CODINGTYPE>& GetSupportedImageFormats() const;
	IMPORT_C const RArray<OMX_COLOR_FORMATTYPE>& GetSupportedColorFormats() const;
	IMPORT_C const OMX_IMAGE_PARAM_PORTFORMATTYPE& GetParamImagePortFormat() const;		

	IMPORT_C RArray<OMX_IMAGE_CODINGTYPE>& GetSupportedImageFormats();
	IMPORT_C RArray<OMX_COLOR_FORMATTYPE>& GetSupportedColorFormats();
	IMPORT_C OMX_IMAGE_PARAM_PORTFORMATTYPE& GetParamImagePortFormat();		
	
	//From CBase
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny *&a0, TAny *a1);
					
private:
	COmxILImagePortImpl* ipImagePortImpl;
	};

#endif // OMXILIMAGEPORT_H
