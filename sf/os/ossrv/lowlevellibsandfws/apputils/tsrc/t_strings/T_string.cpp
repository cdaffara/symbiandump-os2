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
// Tests for RString, RstringF, RStringTokenF and RStringToken class
// 
//

#include <e32test.h>
#include <e32base.h>
#include <e32svr.h>
#include <stringpool.h>
#include "stringtablef.h"
#include "stringtablec.h"

RTest test(_L("T_String"));

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-1708
@SYMTestCaseDesc        Testing the functions of RString, RstringF, RStringTokenF and RStringToken class
						Tests for RString::Copy(), RString::operator RStringToken(), RString::operator!=(), RString::operator==().Tests the same functions of the other classes specified above.
@SYMTestPriority        Medium
@SYMTestActions         Tests by first creating the string table for case-sensitive and case-insensitive strings and then comparing the data in the tables
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/

void DoTestsL()
	{
	RStringPool pool;
	pool.OpenL(StringTableF::Table);
	
	test.Printf(_L("Test for RStringF: case-insensitive strings"));
	
	RStringF objF1 = pool.StringF(StringTableF::EBanana,StringTableF::Table);
	TBuf<100> buf;
	buf.Copy(objF1.DesC());
	test.Printf(buf);
		
	RStringF objF2 = pool.OpenFStringL(_L8("Apple"));
	test(objF1!=objF2);
	test(!(objF1==objF2));

	objF2 = objF1.Copy();
	test(objF1==objF2);
	test(!(objF1!=objF2));
	
	test.Next(_L("Test for RStringTokenF"));
	
	RStringTokenF objF3 = objF2;
	test(objF3==objF2);
	
	test.Next(_L("Test for RString: case-sensitive strings'"));
	
	pool.OpenL(StringTableC::Table);
	RString objC1 = pool.String(StringTableC::EApple,StringTableC::Table);
	
	RString objC2 = pool.OpenStringL(_L8("BaNana"));
	test(!(objC1==objC2));
	test(objC1!=objC2);
	
	RString objC3 = objC2.Copy();
	test(!(objC3!=objC2));
	test(objC3!=objC1);
	test(objC3==objC2);
	
	test.Printf(_L("Test for RStringToken"));
	
	RStringToken objC4 = objC3;
	test(objC4==objC3);
	test(!(objC4==objC1));

	objF1.Close();
	objF2.Close();	
	objC1.Close();
	objC2.Close();
	objC3.Close();
	
	pool.Close();
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanup=CTrapCleanup::New(); 
	if(cleanup == NULL)
	{
		return KErrNoMemory;
	}
	test.Title();
	test.Start(_L("String Tests"));
	TRAPD(error,DoTestsL());
	test( error == KErrNone );
	test.End();
	test.Close();
	delete cleanup;
	__UHEAP_MARKEND;

	return KErrNone; 
	}
