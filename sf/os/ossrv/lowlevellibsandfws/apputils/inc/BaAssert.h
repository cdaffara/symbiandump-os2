// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BAASSERT_H__
#define __BAASSERT_H__

#include <baflpan.h>

/** TBaAssert class has two methods: AssertDebL and AssertRelL, which
can do a panic or leave depending on the iErrorAction value.
If the TBaAssert object is created with TBaAssert::ELeave
constant, then AssertDebL/AssertRelL methods will leave when called
(if aAssertExprVal argument is EFalse).
If the TBaAssert object is created with TBaAssert::EPanic
constant, then AssertDebL/AssertRelL methods will panic when called
(if aAssertExprVal argument is EFalse).
If the TBaAssert object is created using the default constructor - by default
the methods will panic.
The class size has to be exactly sizeof(enum), because objects of that class are
used in some legacy code - replacing an existing enum data member.
@internalComponent */
class TBaAssert
	{
public:
	enum TErrorAction
		{
		EPanic,
		ELeave
		};
public:
	TBaAssert();
	TBaAssert(TErrorAction aErrorAction);
	void AssertDebL(TBool aAssertExprVal, TBafPanic aPanic) const;
	void AssertRelL(TBool aAssertExprVal, TBafPanic aPanic) const;
private:
	void DoAssertL(TBool aAssertExprVal, TBool aDebug, TBafPanic aPanic) const;
	void DoLeaveL(TBafPanic aPanic) const;
	void DoAssert(TBool aDebug, TBafPanic aPanic) const;
private:
	TErrorAction iErrorAction;
	};

#endif//__BAASSERT_H__
