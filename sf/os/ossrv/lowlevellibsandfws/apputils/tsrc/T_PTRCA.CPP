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
// Started by DWW, April 1996
// Tests TPtrC arrays
// 
//

#include <e32test.h>
#include <badesca.h>

LOCAL_D RTest test(_L("T_PTRCA"));

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0425
@SYMTestCaseDesc        Testing descriptor arrays
						Tests for CPtrCArray::MdcaPoint(),CPtrCArray::MdcaCount() functions
@SYMTestPriority        Medium
@SYMTestActions         Tests by adding descriptors to the arrays and then comparing both CPtrCArray & CDesCArray arrays
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testPtrCDesC()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0425 PtrC and DesC "));
	CPtrCArray* ptrA=new(ELeave) CPtrCArray(8);
	CDesCArray* desA=new(ELeave) CDesCArrayFlat(8);
	desA->AppendL(_L("one"));
	ptrA->AppendL(desA->MdcaPoint(0));
	test(ptrA->MdcaPoint(0)==desA->MdcaPoint(0));
	TBuf<32> buffer=_L("two");
	ptrA->InsertL(0,buffer);
	test(ptrA->MdcaPoint(0)!=desA->MdcaPoint(0));
	buffer=_L("onetwothree");
	test(ptrA->MdcaPoint(0)==desA->MdcaPoint(0)); // TPtrC length is unchanged
	//
	desA->AppendL(_L("two"));
	desA->AppendL(_L("three"));
	test(ptrA->MdcaPoint(1)!=desA->MdcaPoint(1));
	*ptrA=*desA;
	test(ptrA->MdcaCount()==desA->MdcaCount());
	test(ptrA->MdcaPoint(1)==desA->MdcaPoint(1));
	buffer=ptrA->MdcaPoint(0);
	test(buffer==_L("one"));
	//
	ptrA->Reset();
	desA->Reset();
	buffer=_L("new test");
	desA->AppendL(buffer);
	ptrA->AppendL(buffer);
	test(ptrA->MdcaPoint(0)==buffer);
	test(desA->MdcaPoint(0)==buffer);
	buffer=_L("hello");
	desA->AppendL(buffer);
	ptrA->AppendL(buffer);
	test(ptrA->MdcaPoint(0)==_L("helloest"));
	test(ptrA->MdcaPoint(1)==buffer);
	test(desA->MdcaPoint(1)==buffer);
	//
	delete(ptrA);
	delete(desA);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0426
@SYMTestCaseDesc        CPtrCArray class functionality test
@SYMTestPriority        Medium
@SYMTestActions         Tests for CPtrCArray::InsertIsqL(),CPtrCArray::InsertIsqAllowDuplicatesL() functions
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void testFindIsq()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0426 Findin in sequence "));
	CPtrCArray* ptrA=new(ELeave) CPtrCArray(8);
	//
	TPtrC hello(_S("Hello"));
	TPtrC bye(_S("Bye"));
	TPtrC world(_S("World"));
	TPtrC middle(_S("middle"));
	TPtrC extra(_S("extra"));
	TInt pos;
	ptrA->AppendL(hello);
	ptrA->AppendL(bye);
	ptrA->AppendL(world);
	ptrA->AppendL(middle);
	//
	TKeyArrayFix key(0,ECmpFolded);
	//
	ptrA->Find(world,key,pos);
	test(pos==2);
	ptrA->Sort(key);
	ptrA->Find(world,key,pos);
	test(pos==3);
	//
	pos=ptrA->InsertIsqL(extra,key);
	test(pos==1);
	TRAP(pos,ptrA->InsertIsqL(extra,key));
	test(pos==KErrAlreadyExists);
	pos=ptrA->InsertIsqAllowDuplicatesL(extra,key);
	test(pos==2);
	//
	delete(ptrA);
	}

GLDEF_C TInt E32Main()
	{
    CTrapCleanup *cleanup=CTrapCleanup::New();
	test.Title();
	test.Start(_L("Testing descriptor arrays "));
    __UHEAP_MARK;
    TRAPD(err,testPtrCDesC());
    test(err==KErrNone);
    TRAP(err,testFindIsq());
    test(err==KErrNone);
    __UHEAP_MARKEND;
	test.End();
    test.Close();
    delete cleanup;
	return(0);
    }
