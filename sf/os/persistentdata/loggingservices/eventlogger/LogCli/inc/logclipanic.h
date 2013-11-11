// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGCLIPANIC_H__
#define __LOGCLIPANIC_H__

// System includes
#include <e32def.h>

/**
Enumerations
@internalComponent
*/
enum TLogCliPanic
	{
	ELogAlreadyActive1						=0,
	ELogAlreadyActive2						=1,
	ELogAlreadyActive3						=2,
	ELogAlreadyActive4						=3,
	ELogAlreadyActive5						=4,
	ELogAlreadyActive6						=5,
	ELogAlreadyActive7						=6,
	ELogNotValid1							=7,
	ELogNotValid2							=8,
	ELogNotValid3							=9,
	ELogNotValid4							=10,
	ELogAlreadyObserving					=11,
	ELogUndefinedChangeType					=12,
	ELogUnexpectedChangeType				=13,
	ELogWindowNoChangeObserver				=14,
	ELogWindowStateMachineNavigationError1	=15,
	ELogWindowStateMachineNavigationError2	=16, 
	ELogWindowCursorCalculationOutOfBounds	=17,
	ELogWindowNavigationOutsideWindow		=18,
	ELogWindowStateMachineCancelError		=19,
	ELogCouldNotReconnectToLogServer		=20,
	ELogNullRcFile							=21,
	ELogDualSimNotSupported					=22
	};

/**
@internalComponent
*/
GLREF_C void Panic(TLogCliPanic aPanic);

#endif
