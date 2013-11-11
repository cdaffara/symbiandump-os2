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

#ifndef __T_COMPONENTTESTS_H__
#define __T_COMPONENTTESTS_H__

#include <stringpool.h>


//
// The other classes are tested as part of the FrameworkTests, 
// or can only be tested when they are derived from, i.e. the Parser.
//

class CComponentTests : public CBase
	{
public:
	static CComponentTests* NewL();
	virtual ~CComponentTests();

	// The tests to run

	// Component tests
	void Test1L();
	void Test2L();
	void Test3L();
	void Test4L();
	void Test5L();
	void Test6L();
	void Test7L();
	void Test8L();
	void Test9L();
	void Test10L();
	void Test11L();
	void Test12L();
	
private:
	CComponentTests();

private:

	RStringPool iStringPool;
	};

#endif // __T_COMPONENTTESTS_H__
