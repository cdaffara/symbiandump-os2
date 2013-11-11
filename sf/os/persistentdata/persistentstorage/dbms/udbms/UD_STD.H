// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "U32STD.H"
#include <d32dbms.h>
#include "D32DBAS.H"
#include "D32DRVR.H"

/**
@internalComponent
*/
enum TDbPanic
	{
	EDbUnimplemented,
	EDbInvalidColumn,
	EDbUninitialised,
	EDbRowLengthOverflow,
	EDbTooManyKeys,
	EDbInvalidViewWindowParameters,
	EDbWrongType,
	EDbInvalidIncrementalStep,
	EDbNoColumnsInSeekKey
	};
/**
@internalComponent
*/
GLREF_C void Panic(TDbPanic aPanic);

/**
@internalComponent
*/
GLREF_D const TDbDriver KBuiltinDriver;

#include "UD_STD.INL"
