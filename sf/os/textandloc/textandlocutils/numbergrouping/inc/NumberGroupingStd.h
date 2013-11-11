/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef NUMBERGROUPING_STD_H
#define NUMBERGROUPING_STD_H

// standard includes

#include <e32std.h>
#include <e32base.h>
#include <s32mem.h>

// number grouping constants

const TInt KSingleCharacter		= 1;

const TInt KErrInvalidChar		= -45;
const TInt KErrIndexOutOfRange	= -46;
const TInt KErrSyntaxError		= -47;

const TInt KStateMatched		= -1;
const TInt KStateNoMatch		= 0;

const TInt KCharacterDot		= 11;
const TInt KCharacterPlus		= 12;

// Panic Enumeration

enum TNumberGroupingPanic
	{
	ENumberGroupingInvalidSeparatorCharacterInFormat,
    ENumberGroupingMatchingPatternVersusFormatPatternMismatch,
	ENumberGroupingNoInitialDigitsInResource,
    ENumberGroupingBadMinMaxDigitRangeInResource,
    ENumberGroupingFormattedNumberAlreadyExists,
    ENumberGroupingNoSuchStateMachine,
    ENumberGroupingBadLengthToGroup
	};


#endif // NUMBERGROUPING_STD_H

// End of File
