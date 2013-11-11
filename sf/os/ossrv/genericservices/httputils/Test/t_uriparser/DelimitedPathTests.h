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

#ifndef __DELIMITEDPATHTESTS_H__
#define __DELIMITEDPATHTESTS_H__

// System includes
//
#include <e32base.h>

//
//
// DelmiitedPath test data
//
//

_LIT(KPathParse0,		"/a/bc/def/ghij/");
_LIT(KPathParse0Seg0,	"a");
_LIT(KPathParse0Seg1,	"bc");
_LIT(KPathParse0Seg2,	"def");
_LIT(KPathParse0Seg3,	"ghij");
_LIT(KPathParse0Seg4,	"");

_LIT(KPathParse1,		"/hello:there;p1;p2;p3/index.wml");
_LIT(KPathParse1Seg0,	"hello:there;p1;p2;p3");
_LIT(KPathParse1Seg1,	"index.wml");

_LIT(KPathParse2,		"/hello/there;p1;p2;p3/is/there/any/filename/");
_LIT(KPathParse2Seg0,	"hello");
_LIT(KPathParse2Seg1,	"there;p1;p2;p3");
_LIT(KPathParse2Seg2,	"is");
_LIT(KPathParse2Seg3,	"there");
_LIT(KPathParse2Seg4,	"any");
_LIT(KPathParse2Seg5,	"filename");
_LIT(KPathParse2Seg6,	"");

_LIT(KPath_DelimPresence0, "/afrontdelim/abackdelim/");
TBool KPath_FrontDelim0 = ETrue;
TBool KPath_BackDelim0 = ETrue;

_LIT(KPath_DelimPresence1, "nofrontdelim/abackdelim/");
TBool KPath_FrontDelim1 = EFalse;
TBool KPath_BackDelim1 = ETrue;

_LIT(KPath_DelimPresence2, "/afrontdelim/nobackdelim");
TBool KPath_FrontDelim2 = ETrue;
TBool KPath_BackDelim2 = EFalse;

_LIT(KPath_DelimPresence3, "nofrontdelim/nobackdelim");
TBool KPath_FrontDelim3 = EFalse;
TBool KPath_BackDelim3 = EFalse;

_LIT(KPath_Rem_Forwards0_0, "/a/bc/def/ghikj/");
_LIT(KPath_Rem_Forwards0_1, "/bc/def/ghikj/");
_LIT(KPath_Rem_Forwards0_2, "/def/ghikj/");
_LIT(KPath_Rem_Forwards0_3, "/ghikj/");
_LIT(KPath_Rem_Forwards0_4, "/");

_LIT(KPath_Rem_Forwards1_0, "/a/bc/def/ghikj");
_LIT(KPath_Rem_Forwards1_1, "/bc/def/ghikj");
_LIT(KPath_Rem_Forwards1_2, "/def/ghikj");
_LIT(KPath_Rem_Forwards1_3, "/ghikj");

_LIT(KPath_Rem_Forwards2_0, "a/bc/def/ghikj");
_LIT(KPath_Rem_Forwards2_1, "/bc/def/ghikj");
_LIT(KPath_Rem_Forwards2_2, "/def/ghikj");
_LIT(KPath_Rem_Forwards2_3, "/ghikj");

_LIT(KPath_Rem_Backwards0_0, "/a/bc/def/ghikj/");
_LIT(KPath_Rem_Backwards0_1, "/a/bc/def/ghikj");
_LIT(KPath_Rem_Backwards0_2, "/a/bc/def");
_LIT(KPath_Rem_Backwards0_3, "/a/bc");
_LIT(KPath_Rem_Backwards0_4, "/a");

_LIT(KPath_Rem_Backwards1_0, "/a/bc/def/ghikj");
_LIT(KPath_Rem_Backwards1_1, "/a/bc/def");
_LIT(KPath_Rem_Backwards1_2, "/a/bc");
_LIT(KPath_Rem_Backwards1_3, "/a");

_LIT(KPath_Rem_Backwards2_0, "a/bc/def/ghikj");
_LIT(KPath_Rem_Backwards2_1, "a/bc/def");
_LIT(KPath_Rem_Backwards2_2, "a/bc");
_LIT(KPath_Rem_Backwards2_3, "a");

_LIT(KPath_Des0, "/a/bc/def/ghij/");

_LIT(KPath_PushPopFront0_0, "");
_LIT(KPath_PushPopFront0_1, "/zero");
_LIT(KPath_PushPopFront0_2, "/one/zero");
_LIT(KPath_PushPopFront0_3, "/two/one/zero");
_LIT(KPath_PushPopFront0_4, "/three/two/one/zero");
_LIT(KPath_PushPopFront0_5, "/four/three/two/one/zero");

_LIT(KPath_PushPopBack0_0, "");
_LIT(KPath_PushPopBack0_1, "/zero");
_LIT(KPath_PushPopBack0_2, "/zero/one");
_LIT(KPath_PushPopBack0_3, "/zero/one/two");
_LIT(KPath_PushPopBack0_4, "/zero/one/two/three");
_LIT(KPath_PushPopBack0_5, "/zero/one/two/three/four");

_LIT(KPath_Push0_a, "zero");
_LIT(KPath_Push0_b, "one");
_LIT(KPath_Push0_c, "two");
_LIT(KPath_Push0_d, "three");
_LIT(KPath_Push0_e, "four");

_LIT(KPath_AddTrimDelim0, "zero/one/two/three/four");

_LIT(KPath_InsertBackwards0_0, "/some/rubbish/before/some/rubbish/after");
_LIT(KPath_InsertBackwards0_1, "/some/rubbish/before/zero/some/rubbish/after");
_LIT(KPath_InsertBackwards0_2, "/some/rubbish/before/one/zero/some/rubbish/after");
_LIT(KPath_InsertBackwards0_3, "/some/rubbish/before/two/one/zero/some/rubbish/after");
_LIT(KPath_InsertBackwards0_4, "/some/rubbish/before/three/two/one/zero/some/rubbish/after");
_LIT(KPath_InsertBackwards0_5, "/some/rubbish/before/four/three/two/one/zero/some/rubbish/after");
const TInt KPathInsertBackwardsPos0 = 3;

_LIT(KPath_RemoveBackwards0_5, "/some/rubbish/before/some/rubbish/after");
_LIT(KPath_RemoveBackwards0_4, "/some/rubbish/before/zero/some/rubbish/after");
_LIT(KPath_RemoveBackwards0_3, "/some/rubbish/before/zero/one/some/rubbish/after");
_LIT(KPath_RemoveBackwards0_2, "/some/rubbish/before/zero/one/two/some/rubbish/after");
_LIT(KPath_RemoveBackwards0_1, "/some/rubbish/before/zero/one/two/three/some/rubbish/after");
_LIT(KPath_RemoveBackwards0_0, "/some/rubbish/before/zero/one/two/three/four/some/rubbish/after");
const TInt KPathRemoveBackwardsPos0 = 3;

_LIT(KPath_InsertForwards0_0, "/some/rubbish/before/some/rubbish/after");
_LIT(KPath_InsertForwards0_1, "/some/rubbish/before/zero/some/rubbish/after");
_LIT(KPath_InsertForwards0_2, "/some/rubbish/before/zero/one/some/rubbish/after");
_LIT(KPath_InsertForwards0_3, "/some/rubbish/before/zero/one/two/some/rubbish/after");
_LIT(KPath_InsertForwards0_4, "/some/rubbish/before/zero/one/two/three/some/rubbish/after");
_LIT(KPath_InsertForwards0_5, "/some/rubbish/before/zero/one/two/three/four/some/rubbish/after");
const TInt KPathInsertForwardsPos0 = 3;

_LIT(KPath_RemoveForwards0_5, "/some/rubbish/before/some/rubbish/after");
_LIT(KPath_RemoveForwards0_4, "/some/rubbish/before/zero/some/rubbish/after");
_LIT(KPath_RemoveForwards0_3, "/some/rubbish/before/one/zero/some/rubbish/after");
_LIT(KPath_RemoveForwards0_2, "/some/rubbish/before/two/one/zero/some/rubbish/after");
_LIT(KPath_RemoveForwards0_1, "/some/rubbish/before/three/two/one/zero/some/rubbish/after");
_LIT(KPath_RemoveForwards0_0, "/some/rubbish/before/four/three/two/one/zero/some/rubbish/after");
const TInt KPathRemoveForwardsPos0 = 3;

_LIT(KPath_Insert0_a, "zero");
_LIT(KPath_Insert0_b, "one");
_LIT(KPath_Insert0_c, "two");
_LIT(KPath_Insert0_d, "three");
_LIT(KPath_Insert0_e, "four");

_LIT(KPath_InsertEscapeBackwards0_0, "/some/rubbish/before/some/rubbish/after");
_LIT(KPath_InsertEscapeBackwards0_1, "/some/rubbish/before/a%2Fa/some/rubbish/after");
_LIT(KPath_InsertEscapeBackwards0_2, "/some/rubbish/before/a%3Ba/a%2Fa/some/rubbish/after");
_LIT(KPath_InsertEscapeBackwards0_3, "/some/rubbish/before/a%3Da/a%3Ba/a%2Fa/some/rubbish/after");
_LIT(KPath_InsertEscapeBackwards0_4, "/some/rubbish/before/a%3Fa/a%3Da/a%3Ba/a%2Fa/some/rubbish/after");
_LIT(KPath_InsertEscapeBackwards0_5, "/some/rubbish/before/a%20a/a%3Fa/a%3Da/a%3Ba/a%2Fa/some/rubbish/after");
const TInt KPathInsertEscapeBackwardsPos0 = 3;

_LIT(KPath_InsertEscapeForwards0_0, "/some/rubbish/before/some/rubbish/after");
_LIT(KPath_InsertEscapeForwards0_1, "/some/rubbish/before/a%2Fa/some/rubbish/after");
_LIT(KPath_InsertEscapeForwards0_2, "/some/rubbish/before/a%2Fa/a%3Ba/some/rubbish/after");
_LIT(KPath_InsertEscapeForwards0_3, "/some/rubbish/before/a%2Fa/a%3Ba/a%3Da/some/rubbish/after");
_LIT(KPath_InsertEscapeForwards0_4, "/some/rubbish/before/a%2Fa/a%3Ba/a%3Da/a%3Fa/some/rubbish/after");
_LIT(KPath_InsertEscapeForwards0_5, "/some/rubbish/before/a%2Fa/a%3Ba/a%3Da/a%3Fa/a%20a/some/rubbish/after");
const TInt KPathInsertEscapeForwardsPos0 = 3;

_LIT(KPath_InsertEscape0_a, "a/a");
_LIT(KPath_InsertEscape0_b, "a;a");
_LIT(KPath_InsertEscape0_c, "a=a");
_LIT(KPath_InsertEscape0_d, "a?a");
_LIT(KPath_InsertEscape0_e, "a a");

_LIT(KPath_PushEscapeFront0_0, "");
_LIT(KPath_PushEscapeFront0_1, "/a%2Fa");
_LIT(KPath_PushEscapeFront0_2, "/a%3Ba/a%2Fa");
_LIT(KPath_PushEscapeFront0_3, "/a%3Da/a%3Ba/a%2Fa");
_LIT(KPath_PushEscapeFront0_4, "/a%3Fa/a%3Da/a%3Ba/a%2Fa");
_LIT(KPath_PushEscapeFront0_5, "/a%20a/a%3Fa/a%3Da/a%3Ba/a%2Fa");

_LIT(KPath_PushEscapeBack0_0, "");
_LIT(KPath_PushEscapeBack0_1, "/a%2Fa");
_LIT(KPath_PushEscapeBack0_2, "/a%2Fa/a%3Ba");
_LIT(KPath_PushEscapeBack0_3, "/a%2Fa/a%3Ba/a%3Da");
_LIT(KPath_PushEscapeBack0_4, "/a%2Fa/a%3Ba/a%3Da/a%3Fa");
_LIT(KPath_PushEscapeBack0_5, "/a%2Fa/a%3Ba/a%3Da/a%3Fa/a%20a");

_LIT(KPath_PushEscape0_a, "a/a");
_LIT(KPath_PushEscape0_b, "a;a");
_LIT(KPath_PushEscape0_c, "a=a");
_LIT(KPath_PushEscape0_d, "a?a");
_LIT(KPath_PushEscape0_e, "a a");

#endif	// __DELIMITEDPATHTESTS_H__
