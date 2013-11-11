// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALLBACK_H__
#define __CALLBACK_H__

/**
@file
@internalTechnology
*/

#include <e32cmn.h>

/** enum to identify why callback is invoked. */
enum TCallBackId
	{
	ECallBackId_None = 0,
	ECallBackId_ImplUpgrade,
	ECallBackId_SwiEvent,
	ECallBackId_BurEvent
	};

/** enum to generalize start and end of SWI or BUR */
enum TCallBackState
	{
	ECallBackState_EventEnd,
	ECallBackState_EventStart
	};

/**
This description is cloned from TCallBack in e32std.h. The only difference
between TCallBack and TCallBackWithArg is that the callback of the
latter takes 3 arguments.
TCallBackWithArg encapsulates a general call-back function.
The class encapsulates:
1. a pointer to a function which takes 3 arguments, TAny*, TInt, and TAny*
   and returns a TInt.
2. a pointer which is passed to the function every time it is called.
   The pointer can point to any object. This pointer is required in the constructor
   but can be NULL if not used.
3. The TInt and the other TAny* arguments are passed in when invoking the callback
   function.
Do not let the names of the 3 callback arguments dictate their uses. Feel free
to store anything you want or ignore anyone that is not needed.

The callback function can be a static function of a class,
e.g. static TInt X::Foo(TAny*, TInt, TAny*) or
it can be a function which is not a member of any class, e.g.
TInt Foo(TAny *, TInt, TAny*).
*/
class TCallBackWithArg
	{
public:
	// default constructor
	inline TCallBackWithArg();

	// Real constructor requires the callback function and a pointer.
	inline TCallBackWithArg(TInt (*aFunction)(TAny* aObj, TInt aEvent, TAny* aData), TAny* aObj);

	inline TInt CallBack(TInt aEvent = ECallBackId_None,
						 TAny* aData = NULL) const;
public:
	/** A pointer to the callback function. */
	TInt (*iFunction)(TAny* aObj, TInt aEvent, TAny* aData);
	
	/** the first argument in the callback function. */
	TAny* iObj;
	};

//
// Implementation of TCallBackWithArg
//

/** Default contructor sets the callback function
to NULL. This way a not yet initialized callback object will just
do nothing. */
inline TCallBackWithArg::TCallBackWithArg()
	: iFunction(NULL), iObj(NULL)
	{ }

inline TCallBackWithArg::TCallBackWithArg(TInt (*aFunction)(TAny*, TInt, TAny*), TAny* aObj)
	: iFunction(aFunction), iObj(aObj)
	{ }

inline TInt TCallBackWithArg::CallBack(TInt aEvent, TAny* aData) const
	{ 
	return (iFunction ? (*iFunction)(iObj, aEvent, aData) : 0);
	}

#endif
