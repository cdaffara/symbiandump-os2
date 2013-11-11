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

#ifndef OMXILIMAGEPORTIMPL_H
#define OMXILIMAGEPORTIMPL_H

#include <e32base.h>

#include <openmax/il/khronos/v1_x/OMX_Types.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

#include <openmax/il/common/omxilport.h>

class COmxILImagePortImpl : public CBase
	{
public:
	static COmxILImagePortImpl* NewL(const TOmxILCommonPortData& aCommonPortData,
							const RArray<OMX_IMAGE_CODINGTYPE>& aSupportedImageFormats,
							const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats);
	~COmxILImagePortImpl();

	OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE aParamIndex,
							   TAny* apComponentParameterStructure) const;

	OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure,
							   TBool& aUpdateProcessingFunction);
							   
	const RArray<OMX_IMAGE_CODINGTYPE>& GetSupportedImageFormats() const;
	const RArray<OMX_COLOR_FORMATTYPE>& GetSupportedColorFormats() const;
	const OMX_IMAGE_PARAM_PORTFORMATTYPE& GetParamImagePortFormat() const;
	RArray<OMX_IMAGE_CODINGTYPE>& GetSupportedImageFormats();
	RArray<OMX_COLOR_FORMATTYPE>& GetSupportedColorFormats();
	OMX_IMAGE_PARAM_PORTFORMATTYPE& GetParamImagePortFormat();
	
private:
	COmxILImagePortImpl();
	void ConstructL(const TOmxILCommonPortData& aCommonPortData,
					const RArray<OMX_IMAGE_CODINGTYPE>& aSupportedImageFormats,
					const RArray<OMX_COLOR_FORMATTYPE>& aSupportedColorFormats);

private:
	RArray<OMX_IMAGE_CODINGTYPE> iSupportedImageFormats;
	RArray<OMX_COLOR_FORMATTYPE> iSupportedColorFormats;
	OMX_IMAGE_PARAM_PORTFORMATTYPE iParamImagePortFormat;
	};
#endif // OMXILIMAGEPORTIMPL_H
