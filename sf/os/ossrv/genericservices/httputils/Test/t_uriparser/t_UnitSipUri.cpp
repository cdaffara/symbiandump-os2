// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Unit tests for PREQ748 - Adding support for the SIP scheme in URIs.
// Plus adding two new methods validate and equivalent
// 
//


#include <e32base.h>
#include <e32test.h>

#include <uri8.h>
#include <uri16.h>
#include <uriutils.h>
#include <uriutilscommon.h>
#include "t_UnitSipUri.h"

TBuf16<256> gFullUri16;
TBuf16<256> gFullUriRhs16;
TBuf16<256> gUriComponent16;

_LIT(KTestName,"SIP Uri Unit Tests");
LOCAL_D RTest test(KTestName);

void ParseUriTests16BitL()
	{	
	test.Next(_L("Parsing 16 bit URIs"));
	gFullUri16.Copy(KUri0);
	CUri8* uri = UriUtils::CreateUriL(gFullUri16);
	CleanupStack::PushL(uri);
	
	gUriComponent16.Copy(KScheme0);
	HBufC* text = uri->Uri().DisplayFormL(EUriScheme);
	TInt result = text->Compare(gUriComponent16) == 0? KErrNone : KUriUtilsErrDifferentScheme;
	delete text;
	User::LeaveIfError(result);

	gUriComponent16.Copy(KUserInfo0);
	text = uri->Uri().DisplayFormL(EUriUserinfo);
	result = text->Compare(gUriComponent16) == 0? KErrNone : KUriUtilsErrDifferentUserInfo;
	delete text;
	User::LeaveIfError(result);
	
	gUriComponent16.Copy(KHost0);
	text = uri->Uri().DisplayFormL(EUriHost);
	result = text->Compare(gUriComponent16) == 0? KErrNone : KUriUtilsErrDifferentHost;
	delete text;
	User::LeaveIfError(result);
	
	gUriComponent16.Copy(KPort0);
	text = uri->Uri().DisplayFormL(EUriPort);
	result = text->Compare(gUriComponent16) == 0? KErrNone : KUriUtilsErrDifferentPort;
	delete text;
	User::LeaveIfError(result);

	gUriComponent16.Copy(KParams0);
	text = uri->Uri().DisplayFormL(EUriPath);
	result = text->Compare(gUriComponent16) == 0? KErrNone : KUriUtilsErrDifferentPath;
	delete text;
	User::LeaveIfError(result);

	gUriComponent16.Copy(KHeaders0);
	text = uri->Uri().DisplayFormL(EUriQuery);
	result = text->Compare(gUriComponent16) == 0? KErrNone : KUriUtilsErrDifferentQuery;
	delete text;
	User::LeaveIfError(result);
	CleanupStack::PopAndDestroy(uri);
	}
	
void ParseUriTestsDeprecated()
	{	
	test.Next(_L("Parsing 16 bit URIs (deprecated)"));
	gFullUri16.Copy(KUri0);
	TUriParser16 parser16;
	parser16.Parse(gFullUri16);
	gUriComponent16.Copy(KScheme0);
	TInt result = parser16.Extract(EUriScheme).Compare(gUriComponent16);
	test(!result);
	gUriComponent16.Copy(KUserInfo0);
	result = parser16.Extract(EUriUserinfo).Compare(gUriComponent16);
	test(!result);
	gUriComponent16.Copy(KHost0);
	result = parser16.Extract(EUriHost).Compare(gUriComponent16);
	test(!result);
	gUriComponent16.Copy(KPort0);
	result = parser16.Extract(EUriPort).Compare(gUriComponent16);
	test(!result);
	gUriComponent16.Copy(KParams0);
	result = parser16.Extract(EUriPath).Compare(gUriComponent16);
	test(!result);
	gUriComponent16.Copy(KHeaders0);
	result = parser16.Extract(EUriQuery).Compare(gUriComponent16);
	test(!result);
	}

void ParseUriTests()
	{
	test.Next(_L("@SYMTestCaseID IWS-APPPROTOCOLS-INETURILIST-TESTUTILS-0001 Parsing 8 bit URIs"));
	TUriParser8 parser8;
	parser8.Parse(KUri0);
	TInt result = parser8.Extract(EUriScheme).Compare(KScheme0);
	test(!result);
	result = parser8.Extract(EUriUserinfo).Compare(KUserInfo0);
	test(!result);
	result = parser8.Extract(EUriHost).Compare(KHost0);
	test(!result);
	result = parser8.Extract(EUriPort).Compare(KPort0);
	test(!result);
	result = parser8.Extract(EUriPath).Compare(KParams0);
	test(!result);
	result = parser8.Extract(EUriQuery).Compare(KHeaders0);
	test(!result);
	result =parser8.Parse(KParseUri);
	test(!result);
	ParseUriTestsDeprecated();
	TRAP (result ,ParseUriTests16BitL());
	test(result == KErrNone);
	}

void GenarateUriTestsL()
	{
	test.Next(_L("@SYMTestCaseID IWS-APPPROTOCOLS-INETURILIST-TESTUTILS-0002 Generating 8 bit URIs"));
	CUri8* uri8 = CUri8::NewLC();
	uri8->SetComponentL(KScheme0, EUriScheme);
	uri8->SetComponentL(KHost0, EUriHost);
	uri8->SetComponentL(KUserInfo0, EUriUserinfo);
	uri8->SetComponentL(KPort0, EUriPort);
	uri8->SetComponentL(KParams0, EUriPath);
	uri8->SetComponentL(KHeaders0, EUriQuery);
	const TDesC8& des8 = uri8->Uri().UriDes();
	TInt result = des8.Compare(KUri0);
	test(!result);
	CleanupStack::PopAndDestroy(uri8);
	
	test.Next(_L("Generating 16 bit URIs"));
	CUri16* uri16 = CUri16::NewLC();
	gUriComponent16.Copy(KScheme0);
	uri16->SetComponentL(gUriComponent16, EUriScheme);
	gUriComponent16.Copy(KHost0);
	uri16->SetComponentL(gUriComponent16, EUriHost);
	gUriComponent16.Copy(KUserInfo0);
	uri16->SetComponentL(gUriComponent16, EUriUserinfo);
	gUriComponent16.Copy(KPort0);
	uri16->SetComponentL(gUriComponent16, EUriPort);
	gUriComponent16.Copy(KParams0);
	uri16->SetComponentL(gUriComponent16, EUriPath);
	gUriComponent16.Copy(KHeaders0);
	uri16->SetComponentL(gUriComponent16, EUriQuery);
	const TDesC16& des16 = uri16->Uri().UriDes();
	gFullUri16.Copy(KUri0);
	result = des16.Compare(gFullUri16);
	test(!result);
	CleanupStack::PopAndDestroy(uri16);
	}

void ValidateTest(const TDesC8& aUri, TInt aErrorToAssert)
	{
	TUriParser8 parser8;
	TUriParser16 parser16;
	parser8.Parse(aUri);
	TInt result = parser8.Validate();
	test(result == aErrorToAssert);
	gFullUri16.Copy(aUri);
	parser16.Parse(gFullUri16);
	result = parser16.Validate();
	test(result == aErrorToAssert);
	}

void ValidateSip8Test(const TDesC8& aUri, TInt aErrorToAssert)
	{
	TUriParser8 parser8;
	parser8.Parse(aUri);
	TInt result = parser8.Validate();
	test(result == aErrorToAssert);
	}
	
void ValidateUriTests()
	{
	test.Next(_L("@SYMTestCaseID IWS-APPPROTOCOLS-INETURILIST-TESTUTILS-0003 Validate URIs 8 and 16 bit"));
	 
	test.Next(_L("[Validate] Check full correct URI"));
	ValidateTest(KVldUri0, KErrNone);
	
	test.Next(_L("[Validate] Check unsupported scheme"));
	ValidateTest(KVldNotSupported, KErrNotSupported);
	
	test.Next(_L("[Validate] Check invalid hosts"));
	ValidateTest(KVldInvalidHost0, KUriUtilsErrInvalidHost);
	ValidateTest(KVldInvalidHost1, KUriUtilsErrInvalidHost);
	ValidateTest(KVldInvalidHost2, KUriUtilsErrInvalidHost);
	ValidateTest(KVldInvalidHost3, KUriUtilsErrInvalidHost);
	ValidateTest(KVldInvalidHost4, KUriUtilsErrInvalidHost);
	ValidateSip8Test(KVldInvalidHost5, KUriUtilsErrInvalidHost );
	
	test.Next(_L("[Validate] Check valid hosts"));
	ValidateTest(KVldValidHost0, KErrNone);
	ValidateTest(KVldValidHost1, KErrNone);
	ValidateTest(KVldValidHost2, KErrNone);
	
	test.Next(_L("[Validate] Check invalid ports"));
	ValidateTest(KVldInvalidPort0, KUriUtilsErrInvalidPort);
	ValidateTest(KVldInvalidNoPort, KUriUtilsErrInvalidPort);
	test.Next(_L("[Validate] Check valid ports"));
	ValidateTest(KVldValidPort0, KErrNone);
	
	test.Next(_L("[Validate] Check invalid usernames and passwords"));
	ValidateTest(KVldInvalidUserInfo0, KUriUtilsErrInvalidUserInfo);
	ValidateTest(KVldInvalidUserInfo1, KUriUtilsErrInvalidUserInfo);
	ValidateTest(KVldInvalidUserInfo2, KUriUtilsErrInvalidUserInfo);
	ValidateTest(KVldInvalidUserInfo3, KUriUtilsErrInvalidUserInfo);
	test.Next(_L("[Validate] Check valid usernames and passwords"));
	ValidateTest(KVldValidUserInfo0, KErrNone);
	ValidateTest(KVldValidUserInfo1, KErrNone);
	ValidateTest(KVldValidUserInfo2, KErrNone);
	ValidateTest(KVldValidUserInfo3, KErrNone);
	
	test.Next(_L("[Validate] Check invalid parameters"));
	ValidateTest(KVldInvalidParams0, KUriUtilsErrInvalidParam);
	ValidateTest(KVldInvalidParams1, KUriUtilsErrInvalidParam);
	ValidateTest(KVldInvalidParams2, KUriUtilsErrInvalidParam);
	ValidateTest(KVldInvalidParams3, KUriUtilsErrInvalidParam);
	ValidateTest(KVldInvalidParams4, KUriUtilsErrInvalidParam);
	ValidateTest(KVldInvalidParams5, KUriUtilsErrInvalidParam);
	test.Next(_L("[Validate] Check valid parameters"));
	ValidateTest(KVldValidParams0, KErrNone);
	ValidateTest(KVldValidParams1, KErrNone);
	ValidateTest(KVldValidParams2, KErrNone);
	
	test.Next(_L("[Validate] Check invalid headers"));
	ValidateTest(KVldInvalidHeaders0, KUriUtilsErrInvalidHeaders);
	ValidateTest(KVldInvalidHeaders1, KUriUtilsErrInvalidHeaders);
	ValidateTest(KVldInvalidHeaders2, KUriUtilsErrInvalidHeaders);
	test.Next(_L("[Validate] Check valid headers"));
	ValidateTest(KVldValidHeaders0, KErrNone);
	ValidateTest(KVldValidHeaders1, KErrNone);
	ValidateTest(KVldValidHeaders2, KErrNone);
	ValidateTest(KVldValidHeaders3, KErrNone);
	
	test.Next(_L("[Validate] General tests"));
	ValidateTest(KVldInvalidGeneral1, KUriUtilsErrInvalidPort);
	ValidateTest(KVldInvalidGeneral2, KUriUtilsErrInvalidHost);
	ValidateTest(KVldValidGeneral1, KErrNone);
	}
	
void EquivalenceTest(const TDesC8& aLhs, const TDesC8& aRhs, TInt aExpected)
	{
	TUriParser8 lhs8;
	lhs8.Parse(aLhs);
	TUriParser8 rhs8;
	rhs8.Parse(aRhs);
	TInt result = lhs8.Equivalent(rhs8);
	test(result == aExpected);
	
	TUriParser16 lhs16;
	gFullUri16.Copy(aLhs);
	lhs16.Parse(gFullUri16);
	TUriParser16 rhs16;
	gFullUriRhs16.Copy(aRhs);
	rhs16.Parse(gFullUriRhs16);
	
	result = lhs16.Equivalent(rhs16);
	test(result == aExpected);
	}
	
void EquivalenceUriTests()
	{
	test.Next(_L("@SYMTestCaseID IWS-APPPROTOCOLS-INETURILIST-TESTUTILS-0004 Equivalent URIs 8 and 16 bit"));
	EquivalenceTest(KEquivUri0, KEquivUri0, KErrNone);
	EquivalenceTest(KEquivUri0, KEquivScheme0, KErrNone);
	
	test.Next(_L("[Equivalent] Compare user names and passwords"));
	EquivalenceTest(KEquivUri0, KEquivUserInfo0, KUriUtilsErrDifferentUserInfo);
	EquivalenceTest(KEquivUri0, KEquivUserInfo1, KUriUtilsErrDifferentUserInfo);

	test.Next(_L("[Equivalent] Compare hosts"));
	EquivalenceTest(KEquivUri0, KEquivHost0, KErrNone);
	EquivalenceTest(KEquivUri1, KEquivHost0, KUriUtilsErrDifferentHost);
	EquivalenceTest(KEquivUri1, KEquivHost1, KErrNone);
	
	test.Next(_L("[Equivalent] Compare hosts"));
	EquivalenceTest(KEquivUri0, KEquivPort0, KUriUtilsErrDifferentPort);
	
	test.Next(_L("[Equivalent] Compare parameters"));
	EquivalenceTest(KEquivUri0, KEquivParam0, KErrNone);
	EquivalenceTest(KEquivUri0, KEquivParam1, KErrNone);
	EquivalenceTest(KEquivUri0, KEquivParam2, KUriUtilsErrDifferentPath);
	EquivalenceTest(KEquivUri0, KEquivParam3, KUriUtilsErrDifferentPath);
	EquivalenceTest(KEquivParam4, KEquivParam5, KErrNone);
	EquivalenceTest(KEquivParam4, KEquivParam6, KErrNone);
	EquivalenceTest(KEquivParam4, KEquivParam7, KUriUtilsErrDifferentPath);
	EquivalenceTest(KEquivParam4, KEquivParam8, KUriUtilsErrDifferentPath);
	EquivalenceTest(KEquivParam4, KEquivParam9, KUriUtilsErrDifferentPath);
	EquivalenceTest(KEquivParam7, KEquivParam4, KUriUtilsErrDifferentPath);
	EquivalenceTest(KEquivParam4, KEquivParam10, KErrNone);
	EquivalenceTest(KEquivParam4, KEquivParam11, KUriUtilsErrDifferentPath);
	EquivalenceTest(KEquivParam12, KEquivParam13, KUriUtilsErrDifferentPath);
	EquivalenceTest(KEquivParam4, KEquivParam14, KUriUtilsErrDifferentPath);
	EquivalenceTest(KEquivParam14, KEquivParam4, KUriUtilsErrDifferentPath);
	
	test.Next(_L("[Equivalent] Compare headers"));
	EquivalenceTest(KEquivHeader0, KEquivHeader1, KErrNone);
	EquivalenceTest(KEquivHeader0, KEquivHeader2, KUriUtilsErrDifferentQuery);
	EquivalenceTest(KEquivHeader2, KEquivHeader0, KUriUtilsErrDifferentQuery);
	EquivalenceTest(KEquivHeader2, KEquivHeader3, KErrNone);
	EquivalenceTest(KEquivHeader3, KEquivHeader2, KErrNone);
	EquivalenceTest(KEquivHeader4, KEquivHeader5, KErrNone);
	EquivalenceTest(KEquivHeader4, KEquivHeader6, KErrNone);
	EquivalenceTest(KEquivHeader4, KEquivHeader7, KUriUtilsErrDifferentQuery);
	
	test.Next(_L("[Equivalent] Expanded URIs"));
	EquivalenceTest(KEquivUri0, KEquivExpand0, KErrNone);
	EquivalenceTest(KEquivExpand0, KEquivUri0, KErrNone);
	EquivalenceTest(KEquivUri0, KEquivExpand1, KErrNone);
	EquivalenceTest(KEquivUri0, KEquivExpand2, KUriUtilsErrDifferentUserInfo);
	EquivalenceTest(KEquivExpand3, KEquivExpand4, KErrNone);
	EquivalenceTest(KEquivExpand4, KEquivExpand3, KErrNone);
	
	test.Next(_L("[Equivalent] General tests"));
	EquivalenceTest(KEquivGeneral1, KEquivGeneral2, KUriUtilsErrDifferentPort);
	}
	
void doMainL()
	{
	test.Start(_L("Starting unit tests"));
	
	ParseUriTests();
	GenarateUriTestsL();
	ValidateUriTests();
	EquivalenceUriTests();
	
	test.End();
	test.Close();
	}

GLDEF_C TInt E32Main()
//
// Main function
	{
	__UHEAP_MARK;

	CTrapCleanup* theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	delete theCleanup;

	__UHEAP_MARKEND;
	
	return KErrNone;
	}

