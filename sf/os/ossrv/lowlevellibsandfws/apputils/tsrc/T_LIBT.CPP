// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by Brendan, Dec 1996
// Test library unloader
// 
//

#include "T_LIBT.H"

IMPORT_C CLibTest* LibEntry();	// Force export 

NONSHARABLE_CLASS(CMyLibTest) : public CLibTest
	{
public:
	CMyLibTest();
	TInt Test1() const;
	TInt Test2() const;
	TInt Test3();
	};

CMyLibTest::CMyLibTest()
	{
	__DECLARE_NAME(_S("CMyLibTest"));
	}

TInt CMyLibTest::Test1() const
	{
	return(1);
	}
	
TInt CMyLibTest::Test2() const
	{
	return(2);
	}

TInt CMyLibTest::Test3()
	{
	return(3);
	}

EXPORT_C CLibTest* LibEntry()
//
// Lib main entry point
//
	{
	return(new(ELeave) CMyLibTest);
	}



