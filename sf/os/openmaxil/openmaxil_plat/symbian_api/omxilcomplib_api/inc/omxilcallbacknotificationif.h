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

#ifndef OMXILCALLBACKNOTIFICATIONIF_H
#define OMXILCALLBACKNOTIFICATIONIF_H

#include <e32def.h>

#include <openmax/il/khronos/v1_x/OMX_Core.h>


/**
   Portion of CallBack Manager Interface used by Processing Function objects
 */
class MOmxILCallbackNotificationIf
	{

public:

	/**
	   OpenMAX IL Error notification method that must be implemented by the
	   Callback manager object.

	   @param [in] aOmxError The error event.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE ErrorEventNotification(
		OMX_ERRORTYPE aOmxError) = 0;

	/**
	   OpenMAX IL Buffer Done notification method that must be implemented by
	   the Callback manager object.

	   @param [in] apBufferHeader The header of the buffer processed.

	   @param [in] aLocalPortIndex Index of the port that processed the buffer.

	   @param [in] aLocalPortDirection The direction of the port.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE BufferDoneNotification(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection) = 0;

	/**
	   OpenMAX IL Clock Buffer Done notification method that must be
	   implemented by the Callback manager object. If queueing is implemented,
	   clock buffers will be returned with higher priority than normal buffers.

	   @param [in] apBufferHeader The header of the clock buffer processed.

	   @param [in] aLocalPortIndex Index of the port that processed the buffer.

	   @param [in] aLocalPortDirection The direction of the port.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE ClockBufferDoneNotification(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_U32 aLocalPortIndex,
		OMX_DIRTYPE aLocalPortDirection) = 0;

	/**
	   Generic OpenMAX IL Event method notification method that must be
	   implemented by the Callback manager object.

	   @param [in] aEvent The event to be notified.

	   @param [in] aData1 An integer with additional event information.

	   @param [in] aData2 An integer with additional event information.

	   @param [in] aExtraInfo An optional text.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE EventNotification(
		OMX_EVENTTYPE aEvent,
		TUint32 aData1,
		TUint32 aData2,
		OMX_STRING aExtraInfo) = 0;

	/**
	   Notification method that must be implemented by the Callback manager
	   object. It is used by Processing Function objects to notify that some
	   port setting has changed during the processing of a buffer.

	   @param [in] aLocalPortIndex The index of the local port that needs to be
	   notified of the change.

	   @param [in] aPortSettingsIndex An implementation-specific identifier
	   that the implementation associates to the setting(s) that need(s)
	   updating in the port.

	   @param [in] aPortSettings A buffer descriptor that contains an
	   implementation-specific structure with the new setting(s) that need to
	   be updated in the port.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE PortSettingsChangeNotification(
		OMX_U32 aLocalPortIndex,
		TUint aPortSettingsIndex,
		const TDesC8& aPortSettings) = 0;

	};

#endif // OMXILCALLBACKNOTIFICATIONIF_H


