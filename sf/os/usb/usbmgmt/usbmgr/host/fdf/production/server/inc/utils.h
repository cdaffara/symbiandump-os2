/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef UTILS_H
#define UTILS_H

#include <e32base.h>
#include <ecom/ecom.h>
#include <usb/usblogger.h>

// In debug, using checking forms of CleanupStack::Pop. In release builds, 
// use the non-checking form to save a little bit of ROM.
#ifdef _DEBUG
#define CLEANUPSTACK_POP1(a)		CleanupStack::Pop(a);
#define CLEANUPSTACK_POP2(a, b) 	CleanupStack::Pop(a, b);
#else
#define CLEANUPSTACK_POP1(a)		CleanupStack::Pop();
#define CLEANUPSTACK_POP2(a, b) 	CleanupStack::Pop(2);
#endif // _DEBUG



// Undefine the e32def.h-defined ASSERT macro to make sure no-one uses it 
// under the mistaken impression that it's useful. Use our informative one 
// above instead!
#undef ASSERT

/**
Cleanup stack item to remove a given TUint from an RArray.
*/
struct TArrayRemove
	{
	TArrayRemove(RArray<TUint>& aDeviceIds, TUint aDeviceId);
	~TArrayRemove();
	
	RArray<TUint>& iDeviceIds;
	const TUint iDeviceId;
	};
void CleanupRemovePushL(TArrayRemove& aArrayRemove);
void Remove(TAny* aArrayRemove);

#endif // UTILS_H
