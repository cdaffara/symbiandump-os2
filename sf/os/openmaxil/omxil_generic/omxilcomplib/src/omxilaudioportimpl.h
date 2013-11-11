// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef OMXILAUDIOPORTIMPL_H
#define OMXILAUDIOPORTIMPL_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Types.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <openmax/il/common/omxilport.h>

class COmxILAudioPortImpl : public CBase
	{
public:
	static COmxILAudioPortImpl* NewL(const RArray<OMX_AUDIO_CODINGTYPE>& aSupportedAudioFormats,
									const TOmxILCommonPortData& aCommonPortData);
	~COmxILAudioPortImpl();
	OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE aParamIndex,
								TAny* apComponentParameterStructure) const;
	OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex,
								const TAny* apComponentParameterStructure,
								TBool& aUpdateProcessingFunction);

private:
	COmxILAudioPortImpl();
	void ConstructL(const RArray<OMX_AUDIO_CODINGTYPE>& aSupportedAudioFormats,
									const TOmxILCommonPortData& aCommonPortData);
private:
	RArray<OMX_AUDIO_CODINGTYPE> iSupportedAudioFormats;
	OMX_AUDIO_PARAM_PORTFORMATTYPE iParamAudioPortFormat;
	};

#endif // OMXILAUDIOPORTIMPL_H

