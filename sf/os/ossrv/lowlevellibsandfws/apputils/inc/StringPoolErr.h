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

#ifndef __STRINGPOOLERR_H
#define __STRINGPOOLERR_H

#include <e32std.h>
#include <e32base.h>

class StringPoolPanic
/**
@internalComponent
*/
	{
public:
	enum TStringPoolPanic
		{
		EComparisonBetweenTwoStringPoolsNotAllowed=0,// We are trying to compare a string from one string pool to a string from another string pool
		EIllegalUseOfNullString,
		ECreatingStringWithWrongCase,
        EStringNotTableEntry,
        EStringTableNotFound
		};
	static void Panic(TStringPoolPanic aPanic);
	};

inline void StringPoolPanic::Panic(StringPoolPanic::TStringPoolPanic aPanic)
	{
	_LIT(KStringPoolPanic,"StringPool");
	User::Panic(KStringPoolPanic,aPanic);
	}

#endif // __STRINGPOOLERR_H

