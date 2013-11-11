// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests for CFindFileByType class
// 
//

#include <bafindf.h>
#include <e32test.h>
#include <basched.h>
#include <f32file.h>

RTest test(_L("T_FindFileByType"));
RFs theFs;

_LIT(KLitWildCard, "*.rsc");
_LIT(KPath, "\\system\\data\\");

/*
@SYMTestCaseID          SYSLIB-BAFL-CT-1758
@SYMTestCaseDesc        CFindFileByType class test
                        Tests for CFindFileByType::FindFirst(), CFindFileByType::FindNext()
@SYMTestPriority        Low
@SYMTestActions         Searching for the occurance of the specified file
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void DoTestsL()
{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-1758 Testing .... "));
	TBuf<64> buf;
	buf = KPath;

	TUidType uid1;
	TUidType uid2(uid1.MostDerived());

	theFs.Connect();
	CleanupClosePushL(theFs);

	CFindFileByType* obj = new(ELeave) CFindFileByType(theFs);
	TInt err = obj->FindFirst(KLitWildCard,buf,uid2);//calls FindNext()
	test(err==KErrNone);

 	delete obj;
 	CleanupStack::PopAndDestroy(); // theFs
}

TInt E32Main()
{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	test.Title();
	test.Start(_L("Testing CFindFileByType Class"));
	TRAPD(err,DoTestsL());
	test(err==KErrNone);
	test.End();
	test.Close();
	delete cleanup;

	__UHEAP_MARKEND;
	return(0);
}
