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
#include <charconv.h>

#include <xml/parserfeature.h>
#include <xml/xmlframeworkerrors.h>
#include <xml/plugins/charsetconverter.h>

using namespace Xml;

static RTest test(_L("t_parserucs4test"));

/**
@SYMTestCaseID 		 		SYSLIB-XML-UT-3717
@SYMTestCaseDesc		    Converting character set.
@SYMTestPriority 		    Medium
@SYMTestActions  		    Converting the character to unicode.
@SYMTestExpectedResults 	The expected conversion result and the actual conversion result match.
@SYMREQ 		 		 	REQ0000
*/
static void ParserTestsL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-XML-UT-3717 ParserTestsL tests... "));

	__UHEAP_MARK;


	// supplementary character
	//    2        9        8        7        6 decimal
	// 0010     1001     1000     0111     0110 hex
	// 11110000 10101001 10100001 10110110      utf-8
	//    F   0    A   9    A   1    B   6

	TUint32 ucs4Char = 0x29876;
	HBufC8* result = NULL;

	CCharSetConverter* charset = CCharSetConverter::NewL();
	CleanupStack::PushL(charset);

	// read supplementary character and convert to utf8
	charset->ConvertUcs4CharactersToEncodingL(&ucs4Char, 1, KCharacterSetIdentifierUtf8, result);

	TBuf8<10> compare;
	compare.Append(0xF0);
	compare.Append(0xA9);
	compare.Append(0xA1);
	compare.Append(0xB6);

	test(*result == compare);

	delete result;
	CleanupStack::PopAndDestroy(charset);


	__UHEAP_MARKEND;
	}

static void MainL()
	{
	ParserTestsL();
	}

TInt E32Main()
	{

	__UHEAP_MARK;
	test.Title();
	test.Start(_L("initialising"));

	CTrapCleanup* c=CTrapCleanup::New();

	// start the loader
	RFs fs;
	test (fs.Connect()==KErrNone);
	fs.Close();

	test (c!=0);
	TRAPD(r,MainL());
	test (r==KErrNone);
	delete c;
	test.End();
	test.Close();
	__UHEAP_MARKEND;

	return KErrNone;
	}

