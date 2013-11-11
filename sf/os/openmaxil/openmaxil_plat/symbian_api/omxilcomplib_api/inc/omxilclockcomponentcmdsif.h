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

#ifndef OMXILCLOCKCOMPONENTCMDSIF_H
#define OMXILCLOCKCOMPONENTCMDSIF_H

#include <openmax/il/khronos/v1_x/OMX_Component.h>

class MOmxILClockComponentCmdsIf
	{
public:
	/**
	   This method is used to query for the current media clock time of the
	   clock component.

	   @param aMediaTime The clock component media time in OMX_TICKS.
	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE GetMediaTime(OMX_TICKS& aMediaTime) const = 0;
	
	/**
	   This method is used to query for the current wall clock of the clock
	   component.

	   @param aWallTime The clock component wall clock in OMX_TICKS.
	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE GetWallTime(OMX_TICKS& aWallTime) const = 0;
	

	/**
	   This method is used to query for the current state of the clock
	   component.

	   @param aClockState A OMX_TIME_CONFIG_CLOCKSTATETYPE structure.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE GetClockState(OMX_TIME_CONFIG_CLOCKSTATETYPE& aClockState) const = 0;

	/**
	   This method allows a client to request a particular timestamp
	   
	   @param apPrivate  A pointer to any private data that the client wants to
	                     associate with the request.
	   @param aMediaTime A timestamp, which is associated with some 
	                     operation (e.g., the presentation of a frame) that
			     		 the client shall execute at the time requested.
	   @param aOffset    Specifies the desired difference between the wall time
	                     when the timestamp actually occurs and the wall time
			     	 	 when the request is to be fulfilled.
	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE MediaTimeRequest(const OMX_PTR apPrivate, const OMX_TICKS aMediaTime, const OMX_TICKS aOffset) const = 0;
	
	/**
	   This method sends the start time to the clock component
	   
	   @param aStartTime The starting timestamp of the stream
	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE SetStartTime(const OMX_TICKS aStartTime) const = 0;
	
	/**
	   This method updates the clock component's video reference clock.
	   
	   @param aVideoRef  The media time of the video component.
	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE SetVideoReference(const OMX_TICKS aVideoRef) const = 0;
	
	/**
	   This method updates the clock component's audio reference clock.
	   
	   @param aAudioRef  The media time of the audio component.
	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE SetAudioReference(const OMX_TICKS aAudioRef) const = 0;
	
	/**
		   This method checks the clock component availability
		   
		   @return TBool
	*/
	virtual OMX_BOOL IsClockComponentAvailable() const = 0;
	};

#endif // OMXILCLOCKCOMPONENTCMDSIF_H

