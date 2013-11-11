// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_sqloom.h"

RTest TheTest(_L("t_sqloom1 test"));

///////////////////////////////////////////////////////////////////////////////////////
///////////////         Non-secure database OOM tests         /////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void DoTestsL()
	{
	TheTest.Start(_L("SQL OOM tests - 1"));
	DbOomTestsL(ENonSecureDb);
	}

TInt E32Main()
	{
	TheTest.Title();

	CTrapCleanup* tc = CTrapCleanup::New();

	__UHEAP_MARK;

	CreateTestDir();
	DeleteTestFiles();

	TRAPD(err, DoTestsL());
	DeleteTestFiles();
	TEST2(err, KErrNone);

	__UHEAP_MARKEND;

	TheTest.End();
	TheTest.Close();

	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
