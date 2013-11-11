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

#ifndef __DELIMITEDQUERYTESTS_H__
#define __DELIMITEDQUERYTESTS_H__

// System includes
//
#include <e32base.h>

//
//
// Delmiited Query test data
//
//

_LIT(KQueryParse0,		"a&bc&def&ghij");
_LIT(KQueryParse0Seg0,	"a");
_LIT(KQueryParse0Seg1,	"bc");
_LIT(KQueryParse0Seg2,	"def");
_LIT(KQueryParse0Seg3,	"ghij");

_LIT(KQueryParse1,		"name=dave&surname=cunado");
_LIT(KQueryParse1Seg0,	"name=dave");
_LIT(KQueryParse1Seg1,	"surname=cunado");

_LIT(KQueryParse2,		"hello&there=p1&is&there&any&stuff");
_LIT(KQueryParse2Seg0,	"hello");
_LIT(KQueryParse2Seg1,	"there=p1");
_LIT(KQueryParse2Seg2,	"is");
_LIT(KQueryParse2Seg3,	"there");
_LIT(KQueryParse2Seg4,	"any");
_LIT(KQueryParse2Seg5,	"stuff");

_LIT(KQuery_DelimPresence0, "&afrontdelim&abackdelim&");
TBool KQuery_FrontDelim0 = ETrue;
TBool KQuery_BackDelim0 = ETrue;

_LIT(KQuery_DelimPresence1, "nofrontdelim&abackdelim&");
TBool KQuery_FrontDelim1 = EFalse;
TBool KQuery_BackDelim1 = ETrue;

_LIT(KQuery_DelimPresence2, "&afrontdelim&nobackdelim");
TBool KQuery_FrontDelim2 = ETrue;
TBool KQuery_BackDelim2 = EFalse;

_LIT(KQuery_DelimPresence3, "nofrontdelim&nobackdelim");
TBool KQuery_FrontDelim3 = EFalse;
TBool KQuery_BackDelim3 = EFalse;

_LIT(KQuery_Rem_Forwards0_0, "a&bc&def&ghikj");
_LIT(KQuery_Rem_Forwards0_1, "&bc&def&ghikj");
_LIT(KQuery_Rem_Forwards0_2, "&def&ghikj");
_LIT(KQuery_Rem_Forwards0_3, "&ghikj");

_LIT(KQuery_Rem_Backwards0_0, "a&bc&def&ghikj");
_LIT(KQuery_Rem_Backwards0_1, "a&bc&def");
_LIT(KQuery_Rem_Backwards0_2, "a&bc");
_LIT(KQuery_Rem_Backwards0_3, "a");

_LIT(KQuery_Des0, "a&bc&def&ghij/");

_LIT(KQuery_PushPopFront0_0, "");
_LIT(KQuery_PushPopFront0_1, "&zero");
_LIT(KQuery_PushPopFront0_2, "&one&zero");
_LIT(KQuery_PushPopFront0_3, "&two&one&zero");
_LIT(KQuery_PushPopFront0_4, "&three&two&one&zero");
_LIT(KQuery_PushPopFront0_5, "&four&three&two&one&zero");

_LIT(KQuery_PushPopBack0_0, "");
_LIT(KQuery_PushPopBack0_1, "&zero");
_LIT(KQuery_PushPopBack0_2, "&zero&one");
_LIT(KQuery_PushPopBack0_3, "&zero&one&two");
_LIT(KQuery_PushPopBack0_4, "&zero&one&two&three");
_LIT(KQuery_PushPopBack0_5, "&zero&one&two&three&four");

_LIT(KQuery_Push0_a, "zero");
_LIT(KQuery_Push0_b, "one");
_LIT(KQuery_Push0_c, "two");
_LIT(KQuery_Push0_d, "three");
_LIT(KQuery_Push0_e, "four");

_LIT(KQuery_AddTrimDelim0, "zero&one&two&three&four");

_LIT(KQuery_InsertBackwards0_0, "some&rubbish&before&some&rubbish&after");
_LIT(KQuery_InsertBackwards0_1, "some&rubbish&before&zero&some&rubbish&after");
_LIT(KQuery_InsertBackwards0_2, "some&rubbish&before&one&zero&some&rubbish&after");
_LIT(KQuery_InsertBackwards0_3, "some&rubbish&before&two&one&zero&some&rubbish&after");
_LIT(KQuery_InsertBackwards0_4, "some&rubbish&before&three&two&one&zero&some&rubbish&after");
_LIT(KQuery_InsertBackwards0_5, "some&rubbish&before&four&three&two&one&zero&some&rubbish&after");
const TInt KQueryInsertBackwardsPos0 = 3;

_LIT(KQuery_RemoveBackwards0_5, "some&rubbish&before&some&rubbish&after");
_LIT(KQuery_RemoveBackwards0_4, "some&rubbish&before&zero&some&rubbish&after");
_LIT(KQuery_RemoveBackwards0_3, "some&rubbish&before&zero&one&some&rubbish&after");
_LIT(KQuery_RemoveBackwards0_2, "some&rubbish&before&zero&one&two&some&rubbish&after");
_LIT(KQuery_RemoveBackwards0_1, "some&rubbish&before&zero&one&two&three&some&rubbish&after");
_LIT(KQuery_RemoveBackwards0_0, "some&rubbish&before&zero&one&two&three&four&some&rubbish&after");
const TInt KQueryRemoveBackwardsPos0 = 3;

_LIT(KQuery_InsertForwards0_0, "some&rubbish&before&some&rubbish&after");
_LIT(KQuery_InsertForwards0_1, "some&rubbish&before&zero&some&rubbish&after");
_LIT(KQuery_InsertForwards0_2, "some&rubbish&before&zero&one&some&rubbish&after");
_LIT(KQuery_InsertForwards0_3, "some&rubbish&before&zero&one&two&some&rubbish&after");
_LIT(KQuery_InsertForwards0_4, "some&rubbish&before&zero&one&two&three&some&rubbish&after");
_LIT(KQuery_InsertForwards0_5, "some&rubbish&before&zero&one&two&three&four&some&rubbish&after");
const TInt KQueryInsertForwardsPos0 = 3;

_LIT(KQuery_RemoveForwards0_5, "some&rubbish&before&some&rubbish&after");
_LIT(KQuery_RemoveForwards0_4, "some&rubbish&before&zero&some&rubbish&after");
_LIT(KQuery_RemoveForwards0_3, "some&rubbish&before&one&zero&some&rubbish&after");
_LIT(KQuery_RemoveForwards0_2, "some&rubbish&before&two&one&zero&some&rubbish&after");
_LIT(KQuery_RemoveForwards0_1, "some&rubbish&before&three&two&one&zero&some&rubbish&after");
_LIT(KQuery_RemoveForwards0_0, "some&rubbish&before&four&three&two&one&zero&some&rubbish&after");
const TInt KQueryRemoveForwardsPos0 = 3;

_LIT(KQuery_Insert0_a, "zero");
_LIT(KQuery_Insert0_b, "one");
_LIT(KQuery_Insert0_c, "two");
_LIT(KQuery_Insert0_d, "three");
_LIT(KQuery_Insert0_e, "four");

_LIT(KQuery_InsertEscapeBackwards0_0, "some&rubbish&before&some&rubbish&after");
_LIT(KQuery_InsertEscapeBackwards0_1, "some&rubbish&before&a%2Fa&some&rubbish&after");
_LIT(KQuery_InsertEscapeBackwards0_2, "some&rubbish&before&a%3Ba&a%2Fa&some&rubbish&after");
_LIT(KQuery_InsertEscapeBackwards0_3, "some&rubbish&before&a%3Da&a%3Ba&a%2Fa&some&rubbish&after");
_LIT(KQuery_InsertEscapeBackwards0_4, "some&rubbish&before&a%3Fa&a%3Da&a%3Ba&a%2Fa&some&rubbish&after");
_LIT(KQuery_InsertEscapeBackwards0_5, "some&rubbish&before&a%20a&a%3Fa&a%3Da&a%3Ba&a%2Fa&some&rubbish&after");
const TInt KQueryInsertEscapeBackwardsPos0 = 3;

_LIT(KQuery_InsertEscapeForwards0_0, "some&rubbish&before&some&rubbish&after");
_LIT(KQuery_InsertEscapeForwards0_1, "some&rubbish&before&a%2Fa&some&rubbish&after");
_LIT(KQuery_InsertEscapeForwards0_2, "some&rubbish&before&a%2Fa&a%3Ba&some&rubbish&after");
_LIT(KQuery_InsertEscapeForwards0_3, "some&rubbish&before&a%2Fa&a%3Ba&a%3Da&some&rubbish&after");
_LIT(KQuery_InsertEscapeForwards0_4, "some&rubbish&before&a%2Fa&a%3Ba&a%3Da&a%3Fa&some&rubbish&after");
_LIT(KQuery_InsertEscapeForwards0_5, "some&rubbish&before&a%2Fa&a%3Ba&a%3Da&a%3Fa&a%20a&some&rubbish&after");
const TInt KQueryInsertEscapeForwardsPos0 = 3;

_LIT(KQuery_InsertEscape0_a, "a/a");
_LIT(KQuery_InsertEscape0_b, "a;a");
_LIT(KQuery_InsertEscape0_c, "a=a");
_LIT(KQuery_InsertEscape0_d, "a?a");
_LIT(KQuery_InsertEscape0_e, "a a");

_LIT(KQuery_PushEscapeFront0_0, "");
_LIT(KQuery_PushEscapeFront0_1, "&a%2Fa");
_LIT(KQuery_PushEscapeFront0_2, "&a%3Ba&a%2Fa");
_LIT(KQuery_PushEscapeFront0_3, "&a%3Da&a%3Ba&a%2Fa");
_LIT(KQuery_PushEscapeFront0_4, "&a%3Fa&a%3Da&a%3Ba&a%2Fa");
_LIT(KQuery_PushEscapeFront0_5, "&a%20a&a%3Fa&a%3Da&a%3Ba&a%2Fa");

_LIT(KQuery_PushEscapeBack0_0, "");
_LIT(KQuery_PushEscapeBack0_1, "&a%2Fa");
_LIT(KQuery_PushEscapeBack0_2, "&a%2Fa&a%3Ba");
_LIT(KQuery_PushEscapeBack0_3, "&a%2Fa&a%3Ba&a%3Da");
_LIT(KQuery_PushEscapeBack0_4, "&a%2Fa&a%3Ba&a%3Da&a%3Fa");
_LIT(KQuery_PushEscapeBack0_5, "&a%2Fa&a%3Ba&a%3Da&a%3Fa&a%20a");

_LIT(KQuery_PushEscape0_a, "a/a");
_LIT(KQuery_PushEscape0_b, "a;a");
_LIT(KQuery_PushEscape0_c, "a=a");
_LIT(KQuery_PushEscape0_d, "a?a");
_LIT(KQuery_PushEscape0_e, "a a");

#endif	// __DELIMITEDQUERYTESTS_H__
