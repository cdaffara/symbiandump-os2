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
 @internalTechnology
*/

#ifndef OMXILVIDEOPORTIMPL_H
#define OMXILVIDEOPORTIMPL_H

#include <e32base.h>

#include <openmax/il/khronos/v1_x/OMX_Types.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

#include <openmax/il/common/omxilport.h>

class COmxILVideoPortImpl : public CBase
	{
public:
	static COmxILVideoPortImpl* NewL(const RArray<OMX_VIDEO_CODINGTYPE>& aSupportedVideoFormats,
	                                 const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormatsconst,
                                     const TOmxILCommonPortData& aCommonPortData);
	~COmxILVideoPortImpl();
	
	OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE aParamIndex,
							   TAny* apComponentParameterStructure) const;
							   
	OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure,
							   TBool& aUpdateProcessingFunction);
							  
	TBool UpdateColorFormat(OMX_COLOR_FORMATTYPE& aOldColor, OMX_COLOR_FORMATTYPE aNewColor, TBool& aUpdated);
	TBool UpdateCodingType(OMX_VIDEO_CODINGTYPE& aOldCodingType, OMX_VIDEO_CODINGTYPE aNewCodingType, TBool& aUpdated);
	RArray<OMX_VIDEO_CODINGTYPE>& GetSupportedVideoFormats();
	RArray<OMX_COLOR_FORMATTYPE>& GetSupportedColorFormats();
	OMX_VIDEO_PARAM_PORTFORMATTYPE& GetParamVideoPortFormat();
private:
	COmxILVideoPortImpl();
	void ConstructL(const RArray<OMX_VIDEO_CODINGTYPE>& aSupportedVideoFormats,
                    const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats,
                    const TOmxILCommonPortData& aCommonPortData);
	
private:
	RArray<OMX_VIDEO_CODINGTYPE> iSupportedVideoFormats;
	RArray<OMX_COLOR_FORMATTYPE> iSupportedColorFormats;
	RArray<OMX_U32> iSupportedFrameRates;
	OMX_VIDEO_PARAM_PORTFORMATTYPE iParamVideoPortFormat;
	};

#endif // OMXILVIDEOPORT_H
