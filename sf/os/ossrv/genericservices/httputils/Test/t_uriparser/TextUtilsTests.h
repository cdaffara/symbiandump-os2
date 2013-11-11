// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TEXTUTILSTESTS_H__
#define __TEXTUTILSTESTS_H__

// System includes
//
#include <e32base.h>

//
//
// TEXTUtils test data
//
//

_LIT(KRemove0_base,		"    Hello there   ");
_LIT(KRemove0_left,		"Hello there   ");
const TInt KRemove0_left_consumed = 4;
_LIT(KRemove0_right,		"    Hello there");
const TInt KRemove0_right_consumed = 3;
_LIT(KRemove0_both,		"Hello there");
const TInt KRemove0_both_consumed = 7;

const TInt KNumericValue0		= 1234567890;
_LIT(KDecRep0,					 "1234567890");
_LIT(KHexRep0,					 "499602d2");
const TBool KIsValid0			= ETrue;

const TInt KNumericValue1		= -543210;
_LIT(KDecRep1,					 "-543210");
_LIT(KHexRep1,					 "-849eA");
const TBool KIsValid1			= ETrue;

const TInt KNumericValue2		= 0;
_LIT(KDecRep2,					 "");
_LIT(KHexRep2,					 "");
const TBool KIsValid2			= EFalse;
	
const TInt KNumericValue3		= 0;
_LIT(KDecRep3,					 "t500");
_LIT(KHexRep3,					 "t1f4");
const TBool KIsValid3			= EFalse;

_LIT(KQuotedBuffer0,	"this is the \"quoted string\" and this is not");
_LIT(KQuotedString0,	"quoted string");
_LIT(KQuotedRemainder0,	" and this is not");
const TInt KExpectedError0	= KErrNone;

_LIT(KQuotedBuffer1,	"this is the \"quoted string\" and this is also \"string!\" but this is not");
_LIT(KQuotedString1,	"quoted string\" and this is also \"string!");
_LIT(KQuotedRemainder1,	" but this is not");
const TInt KExpectedError1	= KErrNone;

_LIT(KQuotedBuffer2,	"this has no quoted string");
_LIT(KQuotedString2,	"");
_LIT(KQuotedRemainder2,	"this has no quoted string");
const TInt KExpectedError2	= -5014;

_LIT(KQuotedBuffer3,	"this has no a malformed \"quoted string");
_LIT(KQuotedString3,	"");
_LIT(KQuotedRemainder3,	"this has no a malformed \"quoted string");
const TInt KExpectedError3	= -5014;

_LIT(KListBuffer0,		"token0, token1 / token 2 is very long    ");
_LIT(KListToken0_0,		"token0");
_LIT(KListToken0_1,		"token1");
_LIT(KListToken0_2,		"token 2 is very long");
_LIT(KListRemainder0,	"");
_LIT(KListSeparator0,	",/");

_LIT(KListBuffer1,		"token0, token1 / token 2 # very long    ");
_LIT(KListToken1_0,		"token0");
_LIT(KListToken1_1,		"token1");
_LIT(KListToken1_2,		"token 2");
_LIT(KListRemainder1,	" very long    ");
_LIT(KListSeparator1,	",/#");

_LIT(KListBuffer2,		"token0, token1 , token 2 is very long    ");
_LIT(KListToken2_0,		"token0");
_LIT(KListToken2_1,		"token1");
_LIT(KListToken2_2,		"token 2 is very long");
_LIT(KListRemainder2,	"");
const TUint KListSeparator2	= ',';

_LIT(KListBuffer3,		"token0/  token1 / token 2 / very long    ");
_LIT(KListToken3_0,		"token0");
_LIT(KListToken3_1,		"token1");
_LIT(KListToken3_2,		"token 2");
_LIT(KListRemainder3,	" very long    ");
const TUint KListSeparator3	= '/';

#endif // __TEXTUTILSTESTS_H__
