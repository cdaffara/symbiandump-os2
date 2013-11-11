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

#ifndef __DELIMITEDPATHSEGMENTTESTS_H__
#define __DELIMITEDPATHSEGMENTTESTS_H__

// System includes
//
#include <e32base.h>

//
//
// DelmiitedPathSegment test data
//
//

_LIT(KPathSegmentParse0,		"abc;def;ghij");
_LIT(KPathSegmentParse0Seg0,	"abc");
_LIT(KPathSegmentParse0Seg1,	"def");
_LIT(KPathSegmentParse0Seg2,	"ghij");

_LIT(KPathSegmentParse1,		"hello:there;p1;p2;p3");
_LIT(KPathSegmentParse1Seg0,	"hello:there");
_LIT(KPathSegmentParse1Seg1,	"p1");
_LIT(KPathSegmentParse1Seg2,	"p2");
_LIT(KPathSegmentParse1Seg3,	"p3");

_LIT(KPathSegmentParse2,		"hello:there;p1;p2;p3;is;there;any;stuff");
_LIT(KPathSegmentParse2Seg0,	"hello:there");
_LIT(KPathSegmentParse2Seg1,	"p1");
_LIT(KPathSegmentParse2Seg2,	"p2");
_LIT(KPathSegmentParse2Seg3,	"p3");
_LIT(KPathSegmentParse2Seg4,	"is");
_LIT(KPathSegmentParse2Seg5,	"there");
_LIT(KPathSegmentParse2Seg6,	"any");
_LIT(KPathSegmentParse2Seg7,	"stuff");


_LIT(KPathSegment_DelimPresence0, ";afrontdelim;abackdelim;");
TBool KPathSegment_FrontDelim0 = ETrue;
TBool KPathSegment_BackDelim0 = ETrue;

_LIT(KPathSegment_DelimPresence1, "nofrontdelim;abackdelim;");
TBool KPathSegment_FrontDelim1 = EFalse;
TBool KPathSegment_BackDelim1 = ETrue;

_LIT(KPathSegment_DelimPresence2, ";afrontdelim;nobackdelim");
TBool KPathSegment_FrontDelim2 = ETrue;
TBool KPathSegment_BackDelim2 = EFalse;

_LIT(KPathSegment_DelimPresence3, "nofrontdelim;nobackdelim");
TBool KPathSegment_FrontDelim3 = EFalse;
TBool KPathSegment_BackDelim3 = EFalse;

_LIT(KPathSegment_Rem_Forwards0_0, "a;bc;def;ghikj");
_LIT(KPathSegment_Rem_Forwards0_1, ";bc;def;ghikj");
_LIT(KPathSegment_Rem_Forwards0_2, ";def;ghikj");
_LIT(KPathSegment_Rem_Forwards0_3, ";ghikj");

_LIT(KPathSegment_Rem_Backwards0_0, "a;bc;def;ghikj");
_LIT(KPathSegment_Rem_Backwards0_1, "a;bc;def");
_LIT(KPathSegment_Rem_Backwards0_2, "a;bc");
_LIT(KPathSegment_Rem_Backwards0_3, "a");

_LIT(KPathSegment_Des0, "a;bc;def;ghij");

_LIT(KPathSegment_PushPopFront0_0, "");
_LIT(KPathSegment_PushPopFront0_1, ";zero");
_LIT(KPathSegment_PushPopFront0_2, ";one;zero");
_LIT(KPathSegment_PushPopFront0_3, ";two;one;zero");
_LIT(KPathSegment_PushPopFront0_4, ";three;two;one;zero");
_LIT(KPathSegment_PushPopFront0_5, ";four;three;two;one;zero");

_LIT(KPathSegment_PushPopBack0_0, "");
_LIT(KPathSegment_PushPopBack0_1, ";zero");
_LIT(KPathSegment_PushPopBack0_2, ";zero;one");
_LIT(KPathSegment_PushPopBack0_3, ";zero;one;two");
_LIT(KPathSegment_PushPopBack0_4, ";zero;one;two;three");
_LIT(KPathSegment_PushPopBack0_5, ";zero;one;two;three;four");

_LIT(KPathSegment_Push0_a, "zero");
_LIT(KPathSegment_Push0_b, "one");
_LIT(KPathSegment_Push0_c, "two");
_LIT(KPathSegment_Push0_d, "three");
_LIT(KPathSegment_Push0_e, "four");

_LIT(KPathSegment_AddTrimDelim0, "zero;one;two;three;four");

_LIT(KPathSegment_InsertBackwards0_0, "some;rubbish;before;some;rubbish;after");
_LIT(KPathSegment_InsertBackwards0_1, "some;rubbish;before;zero;some;rubbish;after");
_LIT(KPathSegment_InsertBackwards0_2, "some;rubbish;before;one;zero;some;rubbish;after");
_LIT(KPathSegment_InsertBackwards0_3, "some;rubbish;before;two;one;zero;some;rubbish;after");
_LIT(KPathSegment_InsertBackwards0_4, "some;rubbish;before;three;two;one;zero;some;rubbish;after");
_LIT(KPathSegment_InsertBackwards0_5, "some;rubbish;before;four;three;two;one;zero;some;rubbish;after");
const TInt KPathSegmentInsertBackwardsPos0 = 3;

_LIT(KPathSegment_RemoveBackwards0_5, "some;rubbish;before;some;rubbish;after");
_LIT(KPathSegment_RemoveBackwards0_4, "some;rubbish;before;zero;some;rubbish;after");
_LIT(KPathSegment_RemoveBackwards0_3, "some;rubbish;before;zero;one;some;rubbish;after");
_LIT(KPathSegment_RemoveBackwards0_2, "some;rubbish;before;zero;one;two;some;rubbish;after");
_LIT(KPathSegment_RemoveBackwards0_1, "some;rubbish;before;zero;one;two;three;some;rubbish;after");
_LIT(KPathSegment_RemoveBackwards0_0, "some;rubbish;before;zero;one;two;three;four;some;rubbish;after");
const TInt KPathSegmentRemoveBackwardsPos0 = 3;

_LIT(KPathSegment_InsertForwards0_0, "some;rubbish;before;some;rubbish;after");
_LIT(KPathSegment_InsertForwards0_1, "some;rubbish;before;zero;some;rubbish;after");
_LIT(KPathSegment_InsertForwards0_2, "some;rubbish;before;zero;one;some;rubbish;after");
_LIT(KPathSegment_InsertForwards0_3, "some;rubbish;before;zero;one;two;some;rubbish;after");
_LIT(KPathSegment_InsertForwards0_4, "some;rubbish;before;zero;one;two;three;some;rubbish;after");
_LIT(KPathSegment_InsertForwards0_5, "some;rubbish;before;zero;one;two;three;four;some;rubbish;after");
const TInt KPathSegmentInsertForwardsPos0 = 3;

_LIT(KPathSegment_RemoveForwards0_5, "some;rubbish;before;some;rubbish;after");
_LIT(KPathSegment_RemoveForwards0_4, "some;rubbish;before;zero;some;rubbish;after");
_LIT(KPathSegment_RemoveForwards0_3, "some;rubbish;before;one;zero;some;rubbish;after");
_LIT(KPathSegment_RemoveForwards0_2, "some;rubbish;before;two;one;zero;some;rubbish;after");
_LIT(KPathSegment_RemoveForwards0_1, "some;rubbish;before;three;two;one;zero;some;rubbish;after");
_LIT(KPathSegment_RemoveForwards0_0, "some;rubbish;before;four;three;two;one;zero;some;rubbish;after");
const TInt KPathSegmentRemoveForwardsPos0 = 3;

_LIT(KPathSegment_Insert0_a, "zero");
_LIT(KPathSegment_Insert0_b, "one");
_LIT(KPathSegment_Insert0_c, "two");
_LIT(KPathSegment_Insert0_d, "three");
_LIT(KPathSegment_Insert0_e, "four");

_LIT(KPathSegment_InsertEscapeBackwards0_0, "some;rubbish;before;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeBackwards0_1, "some;rubbish;before;a%2Fa;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeBackwards0_2, "some;rubbish;before;a%3Ba;a%2Fa;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeBackwards0_3, "some;rubbish;before;a%3Da;a%3Ba;a%2Fa;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeBackwards0_4, "some;rubbish;before;a%3Fa;a%3Da;a%3Ba;a%2Fa;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeBackwards0_5, "some;rubbish;before;a%20a;a%3Fa;a%3Da;a%3Ba;a%2Fa;some;rubbish;after");
const TInt KPathSegmentInsertEscapeBackwardsPos0 = 3;

_LIT(KPathSegment_InsertEscapeForwards0_0, "some;rubbish;before;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeForwards0_1, "some;rubbish;before;a%2Fa;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeForwards0_2, "some;rubbish;before;a%2Fa;a%3Ba;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeForwards0_3, "some;rubbish;before;a%2Fa;a%3Ba;a%3Da;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeForwards0_4, "some;rubbish;before;a%2Fa;a%3Ba;a%3Da;a%3Fa;some;rubbish;after");
_LIT(KPathSegment_InsertEscapeForwards0_5, "some;rubbish;before;a%2Fa;a%3Ba;a%3Da;a%3Fa;a%20a;some;rubbish;after");
const TInt KPathSegmentInsertEscapeForwardsPos0 = 3;

_LIT(KPathSegment_InsertEscape0_a, "a/a");
_LIT(KPathSegment_InsertEscape0_b, "a;a");
_LIT(KPathSegment_InsertEscape0_c, "a=a");
_LIT(KPathSegment_InsertEscape0_d, "a?a");
_LIT(KPathSegment_InsertEscape0_e, "a a");

_LIT(KPathSegment_PushEscapeFront0_0, "");
_LIT(KPathSegment_PushEscapeFront0_1, ";a%2Fa");
_LIT(KPathSegment_PushEscapeFront0_2, ";a%3Ba;a%2Fa");
_LIT(KPathSegment_PushEscapeFront0_3, ";a%3Da;a%3Ba;a%2Fa");
_LIT(KPathSegment_PushEscapeFront0_4, ";a%3Fa;a%3Da;a%3Ba;a%2Fa");
_LIT(KPathSegment_PushEscapeFront0_5, ";a%20a;a%3Fa;a%3Da;a%3Ba;a%2Fa");

_LIT(KPathSegment_PushEscapeBack0_0, "");
_LIT(KPathSegment_PushEscapeBack0_1, ";a%2Fa");
_LIT(KPathSegment_PushEscapeBack0_2, ";a%2Fa;a%3Ba");
_LIT(KPathSegment_PushEscapeBack0_3, ";a%2Fa;a%3Ba;a%3Da");
_LIT(KPathSegment_PushEscapeBack0_4, ";a%2Fa;a%3Ba;a%3Da;a%3Fa");
_LIT(KPathSegment_PushEscapeBack0_5, ";a%2Fa;a%3Ba;a%3Da;a%3Fa;a%20a");

_LIT(KPathSegment_PushEscape0_a, "a/a");
_LIT(KPathSegment_PushEscape0_b, "a;a");
_LIT(KPathSegment_PushEscape0_c, "a=a");
_LIT(KPathSegment_PushEscape0_d, "a?a");
_LIT(KPathSegment_PushEscape0_e, "a a");

#endif	// __DELIMITEDPATHSEGMENTTESTS_H__
