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

#ifndef OMXILCLIENTCLOCKPORT_H
#define OMXILCLIENTCLOCKPORT_H

#include <openmax/il/common/omxilotherport.h>
#include <openmax/il/common/omxilclockcomponentcmdsif.h>

NONSHARABLE_CLASS(COmxILClientClockPort) : public COmxILOtherPort, public MOmxILClockComponentCmdsIf
	{
public:
	IMPORT_C static COmxILClientClockPort* NewL(const TOmxILCommonPortData& aCommonPortData,
						    					const RArray<OMX_OTHER_FORMATTYPE>& aSupportedOtherFormats);

	// From MOmxILClockComponentCmdsIf
	virtual OMX_ERRORTYPE GetMediaTime(OMX_TICKS& aMediaTime) const;
	
	// From MOmxILClockComponentCmdsIf
	virtual OMX_ERRORTYPE GetWallTime(OMX_TICKS& aWallTime) const;

	// From MOmxILClockComponentCmdsIf
	virtual OMX_ERRORTYPE GetClockState(OMX_TIME_CONFIG_CLOCKSTATETYPE& aClockState) const;
	
	// From MOmxILClockComponentCmdsIf
	virtual OMX_ERRORTYPE MediaTimeRequest(const OMX_PTR apPrivate, const OMX_TICKS aMediaTime, const OMX_TICKS aOffset) const;
	
	// From MOmxILClockComponentCmdsIf
	virtual OMX_ERRORTYPE SetStartTime(const OMX_TICKS aStartTime) const;
	
	// From MOmxILClockComponentCmdsIf
	virtual OMX_ERRORTYPE SetVideoReference(const OMX_TICKS aVideRef) const;
	
	// From MOmxILClockComponentCmdsIf
	virtual OMX_ERRORTYPE SetAudioReference(const OMX_TICKS aAudioRef) const;
	
	// From MOmxILClockComponentCmdsIf
	virtual OMX_BOOL IsClockComponentAvailable() const;
	
	// From COmxILPort
	OMX_ERRORTYPE GetLocalOmxParamIndexes(RArray<TUint>& aIndexArray) const;

	// From COmxILPort
	OMX_ERRORTYPE GetLocalOmxConfigIndexes(RArray<TUint>& aIndexArray) const;

	// From COmxILPort
	OMX_ERRORTYPE GetParameter(OMX_INDEXTYPE aParamIndex,
	                                    TAny* apComponentParameterStructure) const;

	// From COmxILPort
	OMX_ERRORTYPE SetParameter(OMX_INDEXTYPE aParamIndex,
	                                    const TAny* apComponentParameterStructure,
	                                    TBool& aUpdateProcessingFunction);
										
protected:
	// From COmxILPort
	OMX_ERRORTYPE SetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE& aPortDefinition,
							 						TBool& aUpdateProcessingFunction);

	// From COmxILPort
	TBool IsTunnelledPortCompatible(const OMX_PARAM_PORTDEFINITIONTYPE& aPortDefinition) const;

private:
	void ConstructL(const TOmxILCommonPortData& aCommonPortData, const RArray<OMX_OTHER_FORMATTYPE>& aSupportedOtherFormats);
	
	COmxILClientClockPort();

	OMX_ERRORTYPE GetTime(OMX_TICKS& aTime, const OMX_INDEXTYPE aTimeIndex) const;

	OMX_ERRORTYPE SetTime(const OMX_TICKS aTime, const OMX_INDEXTYPE aTimeIndex) const;
	
	const OMX_VERSIONTYPE iSpecVersion;
	};

#endif // COMXILCLIENTCLOCKPORT_H
