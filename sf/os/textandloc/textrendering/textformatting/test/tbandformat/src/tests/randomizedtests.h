/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __RANDOMIZEDTESTS_H
#define __RANDOMIZEDTESTS_H

#include <e32std.h>
#include <e32math.h>

const TInt KMaxStringToInsert = 200;

//this version includes bidirectional markup characters
/*
_LIT(KAvailableCharacters, "         ?   abcaaabceeejklooopsstttrrruuabcdefgh\
ijkelmopqrstuvwxyzABCDEFGHIJKLMNO0123456789!@#$%^&*()\x202a\x202b\x202c\x202d\
\x202e\x627\x633\x627\x644\x62D\x631\x64A\x629\x648\x623\x635\x644\x629\x641\
\x64A\x627\x644\x623\x633\x631\x629\x2029\x2028             ");
*/

//this contains LTR and RTL, but no bidirectional markup
_LIT(KAvailableCharacters, "         ?   abcaaabceeejklooopsstttrrruuabcdefgh\
ijkelmopqrstuvwxyzABCDEFGHIJKLMNO0123456789!@#$%^&*()\
\x627\x633\x627\x644\x62D\x631\x64A\x629\x648\x623\x635\x644\x629\x641\
\x64A\x627\x644\x623\x633\x631\x629\x2029\x2028             ");


//this version includes no RTL characters
/*
_LIT(KAvailableCharacters, "         ?   abcaaabceeejklooopsstttrrruuabcdefgh\
ijkelmopqrstuvwxyzABCDEFGHIJKLMNO0123456789!@#$%^&*()\x2029\x2028             ");
*/


TInt BoundedRandom(TInt aMin, TInt aMax);
void RandomStringLC(TInt aMin, TInt aMax, RBuf& aString);

#endif
