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

#include <e32test.h>
#include <eiksrvc.h>
#include <ecom/ecomerrorcodes.h>

_LIT(KTitle,"ECom Error code lookup");

class TEikEcomErrorTest: public RTest
	{
public:
	TEikEcomErrorTest();
	void ResolveError();
	void End();
private:
	REikAppUiSession iSession;
	};


TEikEcomErrorTest::TEikEcomErrorTest():
	RTest(KTitle)
	{

	}
/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0654
@SYMTestCaseDesc	    ECom Error code REikAppUiSession lookup test
@SYMTestPriority 	    High
@SYMTestActions  	    Call up REikAppUiSession::ResolveError
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void TEikEcomErrorTest::ResolveError()
	{
	TInt err;
	Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0654 REikAppUiSession Connecting.. "));
	err=iSession.Connect();
	(*this)(err==KErrNone);

	Next(_L("ECom Error code REikAppUiSession lookup test"));
	TBuf<256> tmp;

	//ECom Error -17000 -> -17030 defined in EcomErr.ra
	for (TInt i=KEComErrBase;i>=KEComErrLast;i--)
		{
		Printf(_L("Message Number %d => "),i);
		err=iSession.ResolveError(tmp,i,TUid::Null());

		(*this)(err==KErrNone);
		(*this)(tmp.Length() > 0 );

		Printf(_L("%S \n"),&tmp);
		tmp.FillZ();
		}
	}


void TEikEcomErrorTest::End()
	{
	iSession.Close();
	//Printf(_L("Press  Any Key to continue"));
	//TKeyCode a=Getch(); a=a;
	RTest::End();
	Close();
	}


TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();

	TEikEcomErrorTest test;
	test.Title();
	test.Start(_L("Starting tests.."));

	test.ResolveError();
	test.End();

	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;

	}


#if defined(__WINS__)
EXPORT_C TInt EntryPoint(TAny*) {return E32Main();}
#endif


