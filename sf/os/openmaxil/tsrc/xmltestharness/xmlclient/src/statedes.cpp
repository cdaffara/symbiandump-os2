/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@internalComponent
*/

#include "statedes.h"

_LIT(KStateLoaded, "LOADED");
_LIT(KStateIdle, "IDLE");
_LIT(KStateExecuting, "EXECUTING");
_LIT(KStatePause, "PAUSE");
_LIT(KStateWaitForResources, "WAIT_FOR_RESOURCES");
_LIT(KStateStateInvalid, "STATEINVALID");
_LIT(KBadState, "?");

_LIT(KClockStateRunning, "CLOCKSTATE_RUNNING");
_LIT(KClockStateWaitingForStartTime, "CLOCKSTATE_WAITINGFORSTARTTIME");
_LIT(KClockStateStopped, "CLOCKSTATE_STOPPED");
_LIT(KClockStateUnknown, "CLOCKSTATE_UNKNOWN");

const TDesC* StateDes(OMX_STATETYPE aState)
	{
	switch(aState)
		{
	case OMX_StateLoaded:
		return &KStateLoaded;
	case OMX_StateIdle:
		return &KStateIdle;
	case OMX_StateExecuting:
		return &KStateExecuting;
	case OMX_StatePause:
		return &KStatePause;
	case OMX_StateWaitForResources:
		return &KStateWaitForResources;
    case OMX_StateInvalid:
        return &KStateStateInvalid;		
	default:
		return &KBadState;
		}
	}

const TDesC* ClockStateDes(OMX_TIME_CLOCKSTATE aClockState)
    {
    switch(aClockState)
        {
    case OMX_TIME_ClockStateRunning:
        return &KClockStateRunning;
    case OMX_TIME_ClockStateWaitingForStartTime:
        return &KClockStateWaitingForStartTime;
    case OMX_TIME_ClockStateStopped:
        return &KClockStateStopped;
    default:
        return &KClockStateUnknown;
        }
    }
