// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// UTrace Performance Tests Kernel-Side Timer.
//



/**
 @file te_perfsanitytimer.h
 @internalTechnology
 @prototype
*/
#ifndef TE_UPTSANITYTIMER_H_
#define TE_UPTSANITYTIMER_H_

#ifndef __KERNEL_MODE__
#include "timer/te_perfusertimer.h"
#else
#include "timer/te_perfkerneltimer.h"
#endif
#include "te_perfsanityhelpers.h"

class TTestTimer
{
private:
enum TTestMethodType
	{
	ESanityFoo,
	ESanityFooLong,
	EUtraceUsr,
	EUtraceKrn,
	};
	
public:
	TBool	TestUserTimerL(TUint32& aTestTime);
	TBool	TestUserLongTimerL(TUint32& aTestTime);
	TBool	TestKernelTimerL(TUint32& aTestTime);
	TBool	TestKernelLongTimerL(TUint32& aTestTime);
	TBool	TestUTraceUserTimerL(TUint32& aTestTime);
	TBool	TestUTraceKernelTimerL(TUint32& aTestTime);
	TUint32 Count(){return iCount;}
private:
	TBool	VerifyTime(TUint32 aTime);
	TBool	DoTestUTraceTimer(const TInt aApi, TUint32& aTestTime);
	TBool	DoTestTraceL(const TTestMethodType aMethod, TUint32& aTestTime);
private:
	#ifndef __KERNEL_MODE__
	CUptTimer		iTimer;
	#else
	TKernelTimer	iTimer;
	#endif
	/**
	 * In case you want the latest count...
	 */
	TUint32			iCount;
};

#endif /*TE_UPTSANITYTIMER_H_*/


