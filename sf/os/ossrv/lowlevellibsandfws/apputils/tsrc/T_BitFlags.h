// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TBITFLAGS.H
// 
//

#ifndef __TBITFLAGS_H__
#define __TBITFLAGS_H__

// System includes
#include <e32std.h>
#include <babitflags.h>

class TestBitFlags
	{
public:
	enum TFavColor
		{
		EBlue	= 0,
		ERed	= 1,
		EGreen	= 2,
		EYellow	= 3,
		EPurple	= 4,
		EBlack	= 5,
		EWhite	= 6,
		EGrey   = 7
		};
public:
	void	TestSetAll();
	void	TestClearAll();
	void	TestSetL(TInt aColor);
	void	TestClearL(TInt aColor);
	void	TestAssign(TInt aColor, TBool aSetOrClear);
	void	TestToggleL(TInt aColor);
	TBool	TestOperator1(TInt aColor); //operator []
	void	TestOperator2(); //operator =
	void	TestOperator3(); //operator ==
	TBool	TestIsSet(TInt aColor);
	TBool	TestIsClear(TInt aColor);
	TInt	TestValue();

private:
	TBitFlags iTestFlags;
	TBitFlags iFlagA;
	TBitFlags iFlagB;
	};

enum TTestEnum
	{
	ETest0		= 0x00000000,
	ETest1		= 0x00000001,
	ETest2		= 0x00000002,
	ETest3		= 0x00000004,
	ETest4		= 0x00000008,
	ETest5		= 0x00000010,
	ETest6		= 0x00000020,
	ETest7		= 0x00000040,
	ETest8		= 0x00000080,
	ETest9		= 0x00000100,
	ETest10		= 0x00000200,
	ETest11		= 0x00000400,
	ETest12		= 0x00000800,
	ETest13		= 0x00001000,
	ETest14		= 0x00002000,
	ETest15		= 0x00004000,
	ETest16		= 0x00008000,
	ETest17		= 0x00010000,
	ETest18		= 0x00020000,
	ETest19		= 0x00040000,
	ETest20		= 0x00080000,
	ETest21		= 0x00100000,
	ETest22		= 0x00200000,
	ETest23		= 0x00400000,
	ETest24		= 0x00800000,
	ETest25		= 0x01000000,
	ETest26		= 0x02000000,
	ETest27		= 0x04000000,
	ETest28		= 0x08000000,
	ETest29		= 0x10000000,
	ETest30		= 0x20000000,
	ETest31		= 0x40000000,
	ETest32		= 0x80000000,
	};

#endif
