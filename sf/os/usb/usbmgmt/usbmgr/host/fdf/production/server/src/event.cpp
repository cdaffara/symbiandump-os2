/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <usb/usblogger.h>
#include "event.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eventTraces.h"
#endif


TDeviceEvent::TDeviceEvent()
	{
    OstTraceFunctionEntry0( TDEVICEEVENT_TDEVICEEVENT_CONS_ENTRY );
 	}

TDeviceEvent::~TDeviceEvent()
	{
    OstTraceFunctionEntry0( TDEVICEEVENT_TDEVICEEVENT_DES_ENTRY );
	}

void TDeviceEvent::Log() const
	{
    OstTrace1( TRACE_DUMP, TDEVICEEVENT_LOG, "\tLogging event 0x%08x", this );
    OstTrace1( TRACE_DUMP, TDEVICEEVENT_LOG_DUP1, "\t\tdevice ID = %d", iInfo.iDeviceId );
    OstTrace1( TRACE_DUMP, TDEVICEEVENT_LOG_DUP2, "\t\tevent type = %d", iInfo.iEventType );
        
	switch ( iInfo.iEventType )
		{
	case EDeviceAttachment:
	    OstTrace1( TRACE_DUMP, TDEVICEEVENT_LOG_DUP3, "\t\terror = %d", iInfo.iError );
	        
	    if ( !iInfo.iError )
			{
            OstTrace1( TRACE_DUMP, TDEVICEEVENT_LOG_DUP4, "\t\tVID = 0x%04x", iInfo.iVid );
            OstTrace1( TRACE_DUMP, TDEVICEEVENT_LOG_DUP5, "\t\tPID = 0x%04x", iInfo.iPid );
			}
		break;

	case EDriverLoad:
	    OstTrace1( TRACE_DUMP, TDEVICEEVENT_LOG_DUP6, "\t\terror = %d", iInfo.iError );
	    OstTrace1( TRACE_DUMP, TDEVICEEVENT_LOG_DUP7, "\t\t\tdriver load status = %d", iInfo.iDriverLoadStatus);
	                
		break;

	case EDeviceDetachment: // No break deliberate.
	default:
		break;
		}
	}

