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

#ifndef OMXILOTHERPORTIMPL_H
#define OMXILOTHERPORTIMPL_H

#include <openmax/il/common/omxilport.h>
#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Types.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

class COmxILOtherPortImpl : public CBase
	{
public:
	static COmxILOtherPortImpl* NewL(const RArray<OMX_OTHER_FORMATTYPE>& aSupportedOtherFormats, 
									const TOmxILCommonPortData& aCommonPortData);
	~COmxILOtherPortImpl();
	OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE aParamIndex, 
										TAny* apComponentParameterStructure) const;
	
	OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex, 
										const TAny* apComponentParameterStructure,
										TBool& aUpdateProcessingFunction);
							  
	const RArray<OMX_OTHER_FORMATTYPE>& GetSupportedOtherFormats() const;
	const OMX_OTHER_PARAM_PORTFORMATTYPE& GetParamOtherPortFormat() const;
    RArray<OMX_OTHER_FORMATTYPE>& GetSupportedOtherFormats();
    OMX_OTHER_PARAM_PORTFORMATTYPE& GetParamOtherPortFormat();

	
protected:
	COmxILOtherPortImpl(const TOmxILCommonPortData& aCommonPortData);
	void ConstructL(const RArray<OMX_OTHER_FORMATTYPE>& aSupportedOtherFormats);
		
private:
	// To indicate the supported formats
	RArray<OMX_OTHER_FORMATTYPE> iSupportedOtherFormats;
	OMX_OTHER_PARAM_PORTFORMATTYPE iParamOtherPortFormat;
	};

#endif // OMXILOTHERPORTIMPL_H
