// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CUriAndAuthorityTest.h"

#include <e32base.h>

#include "UriAndAuthorityTestCommon.h"
#include <uriutilscommon.h>
#include <uriutils.h>
#include "UriAndAuthorityTests.h"
#include "SipUriAndAuthorityTests.h"

CUriAndAuthorityTest* CUriAndAuthorityTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CUriAndAuthorityTest* self = new (ELeave) CUriAndAuthorityTest(aTestHarness);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUriAndAuthorityTest* CUriAndAuthorityTest::NewL(CIpuTestHarness* aTestHarness)
	{
	CUriAndAuthorityTest* self = CUriAndAuthorityTest::NewLC(aTestHarness);
	CleanupStack::Pop(self);
	return self;
	}

CUriAndAuthorityTest::CUriAndAuthorityTest(CIpuTestHarness* aTestHarness)
: iTestHarness(aTestHarness)
	{
	}

void CUriAndAuthorityTest::ConstructL()
	{
	}

CUriAndAuthorityTest::~CUriAndAuthorityTest()
	{
	}

void CUriAndAuthorityTest::DoTestsL()
	{
	// Test TUriParser Component Extraction
	TestUriComponentExtractionL(KComps0, KScheme0, KUserInfo0, KHost0, KPort0, KPath0, KQuery0, KFragment0);
	TestUriComponentExtractionL(KComps1, KScheme1, KUserInfo1, KHost1, KPort1, KPath1, KQuery1, KFragment1);
	TestUriComponentExtractionL(KComps2, KScheme2, KUserInfo2, KHost2, KPort2, KPath2, KQuery2, KFragment2);
	TestUriComponentExtractionL(KComps3, KScheme3, KUserInfo3, KHost3, KPort3, KPath3, KQuery3, KFragment3);
	TestUriComponentExtractionL(KComps4, KScheme4, KUserInfo4, KHost4, KPort4, KPath4, KQuery4, KFragment4);
	TestUriComponentExtractionL(KComps5, KScheme5, KUserInfo5, KHost5, KPort5, KPath5, KQuery5, KFragment5);
	TestUriComponentExtractionL(KComps6, KScheme6, KUserInfo6, KHost6, KPort6, KPath6, KQuery6, KFragment6);
	TestUriComponentExtractionL(KComps10, KScheme10, KUserInfo10, KHost10, KPort10, KPath10, KQuery10, KFragment10);
	TestUriComponentExtractionL(KComps11, KScheme11, KUserInfo11, KHost11, KPort11, KPath11, KQuery11, KFragment11);
	TestUriComponentExtractionL(KComps12, KScheme12, KUserInfo12, KHost12, KPort12, KPath12, KQuery12, KFragment12);
	TestUriComponentExtractionL(KComps13, KScheme13, KUserInfo13, KHost13, KPort13, KPath13, KQuery13, KFragment13);
	TestUriComponentExtractionL(KComps14, KScheme14, KUserInfo14, KHost14, KPort14, KPath14, KQuery14, KFragment14);
	TestUriComponentExtractionL(KComps15, KScheme15, KUserInfo15, KHost15, KPort15, KPath15, KQuery15, KFragment15);
	TestUriComponentExtractionL(KComps16, KScheme16, KUserInfo16, KHost16, KPort16, KPath16, KQuery16, KFragment16);
	TestUriComponentExtractionL(KComps17, KScheme17, KUserInfo17, KHost17, KPort17, KPath17, KQuery17, KFragment17);
	TestUriComponentExtractionL(KComps18, KScheme18, KUserInfo18, KHost18, KPort18, KPath18, KQuery18, KFragment18);
	TestUriComponentExtractionL(KComps19, KScheme19, KUserInfo19, KHost19, KPort19, KPath19, KQuery19, KFragment19);
	TestUriComponentExtractionL(KComps20, KScheme20, KUserInfo20, KHost20, KPort20, KPath20, KQuery20, KFragment20);
	TestUriComponentExtractionL(KComps21, KScheme21, KUserInfo21, KHost21, KPort21, KPath21, KQuery21, KFragment21);
	TestUriComponentExtractionL(KComps22, KScheme22, KUserInfo22, KHost22, KPort22, KPath22, KQuery22, KFragment22);
	TestUriComponentExtractionL(KComps23, KScheme23, KUserInfo23, KHost23, KPort23, KPath23, KQuery23, KFragment23);
	TestUriComponentExtractionL(KComps24, KScheme24, KUserInfo24, KHost24, KPort24, KPath24, KQuery24, KFragment24);
	TestUriComponentExtractionL(KComps25, KScheme25, KUserInfo25, KHost25, KPort25, KPath25, KQuery25, KFragment25);
	TestUriComponentExtractionL(KComps26, KScheme26, KUserInfo26, KHost26, KPort26, KPath26, KQuery26, KFragment26);
	TestUriComponentExtractionL(KComps27, KScheme27, KUserInfo27, KHost27, KPort27, KPath27, KQuery27, KFragment27);
	TestUriComponentExtractionL(KComps28, KScheme28, KUserInfo28, KHost28, KPort28, KPath28, KQuery28, KFragment28);
	TestUriComponentExtractionL(KComps29, KScheme29, KUserInfo29, KHost29, KPort29, KPath29, KQuery29, KFragment29);

	//
	// IPv6 Test cases Section 2.9.1
	TestUriComponentExtractionL(KComps30, KScheme30, KUserInfo30, KHost30, KPort30, KPath30, KQuery30, KFragment30);
	TestUriComponentExtractionL(KComps31, KScheme31, KUserInfo31, KHost31, KPort31, KPath31, KQuery31, KFragment31);
	TestUriComponentExtractionL(KComps32, KScheme32, KUserInfo32, KHost32, KPort32, KPath32, KQuery32, KFragment32);
	TestUriComponentExtractionL(KComps33, KScheme33, KUserInfo33, KHost33, KPort33, KPath33, KQuery33, KFragment33);
	TestUriComponentExtractionL(KComps34, KScheme34, KUserInfo34, KHost34, KPort34, KPath34, KQuery34, KFragment34);
	TestUriComponentExtractionL(KComps35, KScheme35, KUserInfo35, KHost35, KPort35, KPath35, KQuery35, KFragment35);
	TestUriComponentExtractionL(KComps36, KScheme36, KUserInfo36, KHost36, KPort36, KPath36, KQuery36, KFragment36);

	TestAuthorityComponentExtractionL(KAuthority0, KUserInfo0, KHost0, KPort0);
	TestAuthorityComponentExtractionL(KAuthority1, KUserInfo1, KHost1, KPort1);
	TestAuthorityComponentExtractionL(KAuthority2, KUserInfo2, KHost2, KPort2);
	TestAuthorityComponentExtractionL(KAuthority3, KUserInfo3, KHost3, KPort3);
	TestAuthorityComponentExtractionL(KAuthority4, KUserInfo4, KHost4, KPort4);
	TestAuthorityComponentExtractionL(KAuthority5, KUserInfo5, KHost5, KPort5);
	TestAuthorityComponentExtractionL(KAuthority6, KUserInfo6, KHost6, KPort6);
	TestAuthorityComponentExtractionL(KAuthority10, KUserInfo10, KHost10, KPort10);
	TestAuthorityComponentExtractionL(KAuthority11, KUserInfo11, KHost11, KPort11);
	TestAuthorityComponentExtractionL(KAuthority12, KUserInfo12, KHost12, KPort12);
	TestAuthorityComponentExtractionL(KAuthority13, KUserInfo13, KHost13, KPort13);
	TestAuthorityComponentExtractionL(KAuthority14, KUserInfo14, KHost14, KPort14);
	TestAuthorityComponentExtractionL(KAuthority15, KUserInfo15, KHost15, KPort15);
	TestAuthorityComponentExtractionL(KAuthority16, KUserInfo16, KHost16, KPort16);
	TestAuthorityComponentExtractionL(KAuthority17, KUserInfo17, KHost17, KPort17);
	TestAuthorityComponentExtractionL(KAuthority18, KUserInfo18, KHost18, KPort18);
	TestAuthorityComponentExtractionL(KAuthority19, KUserInfo19, KHost19, KPort19);
	TestAuthorityComponentExtractionL(KAuthority20, KUserInfo20, KHost20, KPort20);
	TestAuthorityComponentExtractionL(KAuthority21, KUserInfo21, KHost21, KPort21);
	TestAuthorityComponentExtractionL(KAuthority22, KUserInfo22, KHost22, KPort22);
	TestAuthorityComponentExtractionL(KAuthority23, KUserInfo23, KHost23, KPort23);
	TestAuthorityComponentExtractionL(KAuthority24, KUserInfo24, KHost24, KPort24);
	TestAuthorityComponentExtractionL(KAuthority25, KUserInfo25, KHost25, KPort25);
	TestAuthorityComponentExtractionL(KAuthority26, KUserInfo26, KHost26, KPort26);
	TestAuthorityComponentExtractionL(KAuthority27, KUserInfo27, KHost27, KPort27);
	TestAuthorityComponentExtractionL(KAuthority28, KUserInfo28, KHost28, KPort28);
	TestAuthorityComponentExtractionL(KAuthority29, KUserInfo29, KHost29, KPort29);

	// IPv6 Test cases
	TestAuthorityComponentExtractionL(KAuthority30, KUserInfo30, KHost30, KPort30);
	TestAuthorityComponentExtractionL(KAuthority31, KUserInfo31, KHost31, KPort31);
	TestAuthorityComponentExtractionL(KAuthority32, KUserInfo32, KHost32, KPort32);
	TestAuthorityComponentExtractionL(KAuthority33, KUserInfo33, KHost33, KPort33);
	TestAuthorityComponentExtractionL(KAuthority34, KUserInfo34, KHost34, KPort34);
	TestAuthorityComponentExtractionL(KAuthority35, KUserInfo35, KHost35, KPort35);
	TestAuthorityComponentExtractionL(KAuthority36, KUserInfo36, KHost36, KPort36);

	TestResolveL(KBase,KRelative1,KResolved1);
	TestResolveL(KBase,KRelative2,KResolved2);
	TestResolveL(KBase,KRelative3,KResolved3);
	TestResolveL(KBase,KRelative4,KResolved4);
	TestResolveL(KBase,KRelative5,KResolved5);
	TestResolveL(KBase,KRelative6,KResolved6);
	TestResolveL(KBase,KRelative7,KResolved7);
	TestResolveL(KBase,KRelative8,KResolved8);
	TestResolveL(KBase,KRelative9,KResolved9);
	TestResolveL(KBase,KRelative10,KResolved10);
	TestResolveL(KBase,KRelative11,KResolved11);
	TestResolveL(KBase,KRelative12,KResolved12);
	TestResolveL(KBase,KRelative13,KResolved13);
	TestResolveL(KBase,KRelative14,KResolved14);
	TestResolveL(KBase,KRelative15,KResolved15);
	TestResolveL(KBase,KRelative16,KResolved16);
	TestResolveL(KBase,KRelative17,KResolved17);
	TestResolveL(KBase,KRelative18,KResolved18);
	TestResolveL(KBase,KRelative19,KResolved19);
	TestResolveL(KBase,KRelative20,KResolved20);
	TestResolveL(KBase,KRelative21,KResolved21);
	TestResolveL(KBase,KRelative22,KResolved22);

	TestResolveL(KBase,KRelative23,KResolved23);
	TestResolveL(KBase,KRelative24,KResolved24);
	TestResolveL(KBase,KRelative25,KResolved25);
	TestResolveL(KBase,KRelative26,KResolved26);
	TestResolveL(KBase,KRelative27,KResolved27);
	TestResolveL(KBase,KRelative28,KResolved28);
	TestResolveL(KBase,KRelative29,KResolved29);
	TestResolveL(KBase,KRelative30,KResolved30);

	TestResolveL(KBase,KRelative31,KResolved31);
	TestResolveL(KBase,KRelative32,KResolved32);
	TestResolveL(KBase,KRelative33,KResolved33);
	TestResolveL(KBase,KRelative34,KResolved34);
	TestResolveL(KBase,KRelative35,KResolved35);
	TestResolveL(KBase,KRelative36,KResolved36);

	TestResolveL(KBase,KRelative37,KResolved37);
	TestResolveL(KBase,KRelative38,KResolved38);
	TestResolveL(KBase,KRelative39,KResolved39);
	TestResolveL(KBase,KRelative40,KResolved40);

	TestResolveL(KBase,KRelative41,KResolved41);

	TestResolveL(KBase,KRelative42,KResolved42);

	iTestHarness->LogIt(_L("Next Test - Defect EDNMTAN-4H9K6A, CUrlWrap::ResolveL does not parse file Urls properly"));
	TestResolveL(KBaseFile, KRelativeFile1, KResolvedFile1);

	iTestHarness->LogIt(_L("Next Test - Defect EDNSTAR-4HTNB4, CUrlWrap::ResolveL panics with zero length aUrlBase"));
	TestResolveL(KNullDesC, KRelativeFile1, KRelativeFile1);

	iTestHarness->LogIt(_L("Next Test - Defect EDNPHAD-4J6EUY, CUrlWrap::ResolveL fails when the base and relative URLs both have fragments"));
	TestResolveL(KBaseBug1, KRelativeBug1, KResolvedBug1);
	TestResolveL(KBaseBug2, KRelativeBug2, KResolvedBug2);
	TestResolveL(KBaseBug2, KRelativeBug3, KResolvedBug3);

	iTestHarness->LogIt(_L("Next Test - Defect EDNFUDN-4JYFH7, CUrl overflow causes WINS crash during sub-doc fetch"));
	TestResolveL(KBaseBug3, KRelativeBug4, KResolvedBug4);
	
	TestUriCompareL(KUri_Compare0_a, KUri_Compare0_b, KUriMatchFlags0);
	TestUriCompareL(KUri_Compare1_a, KUri_Compare1_b, KUriMatchFlags1);

	// IPv6 Test cases for section 2.9.1.1.1
	TestUriCompareL(KUri_Compare2_a, KUri_Compare2_b, KUriMatchFlags2);
	// and section 2.9.1.1.2
	TestUriCompareL(KUri_Compare3_a, KUri_Compare3_b, KUriMatchFlags3);

	TestAuthorityCompareL(KAuth_Compare0_a, KAuth_Compare0_b, KAuthMatchFlags0);
	TestAuthorityCompareL(KAuth_Compare1_a, KAuth_Compare1_b, KAuthMatchFlags1);

	TestUriComponentPresenceL(KUri_Presence0, KUriPresenceFlags0);
	TestUriComponentPresenceL(KUri_Presence1, KUriPresenceFlags1);

	TestAuthorityComponentPresenceL(KAuthority_Presence0, KAuthorityPresenceFlags0);
	TestAuthorityComponentPresenceL(KAuthority_Presence1, KAuthorityPresenceFlags1);

	TestUriDesL(KUri_Des0);

	TestAuthorityDesL(KAuthority_Des0);

	TestUriSchemeValidiyL(KUri_SchemeValidation0, KUri_SchemeValidity0);
	TestUriSchemeValidiyL(KUri_SchemeValidation1, KUri_SchemeValidity1);

	TestUriWithoutFragmentL(KUri_WithoutFragment0, KUri_WithoutFragment_Result0);
	TestUriWithoutFragmentL(KUri_WithoutFragment1, KUri_WithoutFragment_Result1);

	TestUriSetComponentL(KUri_Scheme0, KUri_Host0, KUri_Userinfo0, KUri_Port0, KUri_Path0, KUri_Query0, KUri_Fragment0,
		KUri_SchemeComp0, KUri_HostComp0, KUri_UserinfoComp0, KUri_PortComp0, KUri_PathComp0, KUri_QueryComp0, KUri_FragmentComp0);

	TestUriRemoveComponentL(KUri_NoScheme0, KUri_NoHost0, KUri_NoUserinfo0, KUri_NoPort0, KUri_NoPath0, 
		KUri_NoQuery0, KUri_NoFragment0, KUri_Whole0);	
	
	// Same test as above, but with an IPv6 format host, section 2.9.1.4
	// Apart from the IPv6 address, the component data from the previous test can be reused
	TestUriSetComponentL(KUri_Scheme0, KUri_IPv6Host0, KUri_IPv6UserInfo0, KUri_IPv6Port0, KUri_IPv6Path0, KUri_IPv6Query0, KUri_IPv6Fragment0,
		KUri_SchemeComp0, KUri_IPv6HostComp0, KUri_UserinfoComp0, KUri_PortComp0, KUri_PathComp0, KUri_QueryComp0, KUri_FragmentComp0);

	TestUriRemoveComponentL(KUri_IPv6NoScheme0, KUri_IPv6NoHost0, KUri_IPv6NoUserinfo0, KUri_IPv6NoPort0, KUri_IPv6NoPath0, 
		KUri_IPv6NoQuery0, KUri_IPv6NoFragment0, KUri_IPv6Whole0);	
	
	TestAuthoritySetComponentL(KAuthority_Host0, KAuthority_Userinfo0, KAuthority_Port0, 
		KAuthority_HostComp0, KAuthority_UserinfoComp0, KAuthority_PortComp0);

	TestAuthoritySetAndEscapeComponentL(KAuthorityEscaped_Host0, KAuthorityEscaped_Userinfo0, KAuthorityEscaped_Port0, 
		KAuthorityEscaped_HostComp0, KAuthorityEscaped_UserinfoComp0, KAuthorityEscaped_PortComp0);

	TestAuthorityRemoveComponentL(KAuthority_NoHost0, KAuthority_NoUserinfo0, KAuthority_NoPort0, KAuthority_Whole0);

	// Same tests as above but for IPv6, as per section 2.9.1.4
	TestAuthoritySetComponentL(KAuthority_IPv6Host0, KAuthority_IPv6Userinfo0, KAuthority_IPv6Port0, 
	KAuthority_IPv6HostComp0, KAuthority_IPv6UserinfoComp0, KAuthority_IPv6PortComp0);

	TestAuthorityRemoveComponentL(KAuthority_IPv6NoHost0, KAuthority_IPv6NoUserinfo0, KAuthority_IPv6NoPort0, KAuthority_IPv6Whole0);
	}

//
//
//	Uri Tests
//
//

void CUriAndAuthorityTest::TestUriComponentExtractionL(const TDesC16& aUri, const TDesC16& aScheme,
										   const TDesC16& aUserinfo, const TDesC16& aHost, 
										   const TDesC16& aPort, const TDesC16& aPath, 
										   const TDesC16& aQuery, const TDesC16& aFragment) const
	{
	iTestHarness->StartTestL(_L("Test TUriParser Component Extraction (deprecated 16-bit )"));
	iTestHarness->LogIt(_L("Uri - %S"), &aUri);

	TInt error = DoUriComponentExtraction<TUriParser16>(aUri, aScheme, aUserinfo, aHost, aPort, aPath, aQuery, aFragment);
	
	//check for failed Sip test
	if (KErrNotFound!=aPath.Find(_L(",")) && KErrNotFound!=aScheme.Find(_L("sIp")))
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess6);	
		error=KErrNone;
		}
	iTestHarness->EndTest(error);

	iTestHarness->StartTestL(_L("Test TUriParser Component Extraction"));
	iTestHarness->LogIt(_L("Uri - %S"), &aUri);


	TRAP( error , DoUriComponentExtractionL(aUri, aScheme, aUserinfo, aHost, aPort, aPath, aQuery, aFragment) )
	
	//check for failed Sip test
	if (aPath.Find(_L(","))!=KErrNotFound  && aScheme.Find(_L("sIp")) != KErrNotFound)
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess6);	
		error=KErrNone;
		}
	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* uriBuf = HBufC8::NewLC(aUri.Length());
	TPtr8 uri8Bit = uriBuf->Des();
	uri8Bit.Copy(aUri);

	HBufC8* schemeBuf = HBufC8::NewLC(aScheme.Length());
	TPtr8 scheme8Bit = schemeBuf->Des();
	scheme8Bit.Copy(aScheme);

	HBufC8* userinfoBuf = HBufC8::NewLC(aUserinfo.Length());
	TPtr8 userinfo8Bit = userinfoBuf->Des();
	userinfo8Bit.Copy(aUserinfo);

	HBufC8* hostBuf = HBufC8::NewLC(aHost.Length());
	TPtr8 host8Bit = hostBuf->Des();
	host8Bit.Copy(aHost);

	HBufC8* portBuf = HBufC8::NewLC(aPort.Length());
	TPtr8 port8Bit = portBuf->Des();
	port8Bit.Copy(aPort);

	HBufC8* pathBuf = HBufC8::NewLC(aPath.Length());
	TPtr8 path8Bit = pathBuf->Des();
	path8Bit.Copy(aPath);

	HBufC8* queryBuf = HBufC8::NewLC(aQuery.Length());
	TPtr8 query8Bit = queryBuf->Des();
	query8Bit.Copy(aQuery);

	HBufC8* fragmentBuf = HBufC8::NewLC(aFragment.Length());
	TPtr8 fragment8Bit = fragmentBuf->Des();
	fragment8Bit.Copy(aFragment);

	iTestHarness->StartTestL(_L("Test TUriParser Component Extraction (8-bit)"));
	iTestHarness->LogIt(_L("Uri - %S"), &aUri);

	error = DoUriComponentExtraction<TUriParser8>(uri8Bit, scheme8Bit, userinfo8Bit, host8Bit, port8Bit, path8Bit, query8Bit, fragment8Bit);

	//check for failed Sip test
	if (aPath.Find(_L(","))!=KErrNotFound && aScheme.Find(_L("sIp"))!=KErrNotFound)
		{
		iTestHarness->LogIt(KCompsSipUriInvalidMess6);	
		error=KErrNone;
		}
	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(8, uriBuf);	// uriBuf, schemeBuf, userinfoBuf, hostBuf, portBuf, pathBuf, queryBuf, fragmentBuf
	}

void CUriAndAuthorityTest::TestResolveL(const TDesC& aBase, const TDesC& aReference, const TDesC& aExpected) const
	{
	iTestHarness->StartTestL(_L("Testing Resolving Functionality (16-bit)"));
	iTestHarness->LogIt(_L("Test Resolving %S , %S -> %S"), &aBase, &aReference, &aExpected);

	TInt error = DoTestResolve<TUriParser16, CUri16>(aBase, aReference, aExpected);

	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* baseBuf = HBufC8::NewLC(aBase.Length());
	TPtr8 base8Bit = baseBuf->Des();
	base8Bit.Copy(aBase);

	HBufC8* refBuf = HBufC8::NewLC(aReference.Length());
	TPtr8 ref8Bit = refBuf->Des();
	ref8Bit.Copy(aReference);

	HBufC8* expectedBuf = HBufC8::NewLC(aExpected.Length());
	TPtr8 expected8Bit = expectedBuf->Des();
	expected8Bit.Copy(aExpected);

	iTestHarness->StartTestL(_L("Testing Resolving Functionality (8-bit)"));
		
	//Abnormal examples below differs from RFC 2396 to the current implementation of 
	//RFC3986. So, the expected output for ResolveL for Uri8 is modified to support
	//section 5.4.2 of RFC 3986.
	 
	if( aReference.Compare(KRelative23) == 0 ||
		aReference.Compare(KRelative24) == 0 ||
		aReference.Compare(KRelative25) == 0 ||
		aReference.Compare(KRelative26) == 0 )
		{
		_LIT8(KRfc3986,"http://a/g");
		expected8Bit.Copy(KRfc3986);
		_LIT(KRfc3986Bit16,"http://a/g");
		iTestHarness->LogIt(_L("Test Resolving as per RFC3986 %S , %S -> %S"), &aBase, &aReference, &KRfc3986Bit16);
		}
	else
		{
	iTestHarness->LogIt(_L("Test Resolving %S , %S -> %S"), &aBase, &aReference, &aExpected);
		}
	error = DoTestResolve<TUriParser8, CUri8>(base8Bit, ref8Bit, expected8Bit);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(3, baseBuf);	// baseBuf, refBuf, expectedBuf
	}

void CUriAndAuthorityTest::TestUriCompareL(const TDesC& aUri1, const TDesC& aUri2, TInt aMatchFlags) const
	{
	iTestHarness->StartTestL(_L("Test TUriC Component Compare (16-bit)"));

	TInt error = DoUriCompare<TUriParser16>(aUri1, aUri2, aMatchFlags);

	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* uriBuf1 = HBufC8::NewLC(aUri1.Length());
	TPtr8 uri8Bit1 = uriBuf1->Des();
	uri8Bit1.Copy(aUri1);

	HBufC8* uriBuf2 = HBufC8::NewLC(aUri2.Length());
	TPtr8 uri8Bit2 = uriBuf2->Des();
	uri8Bit2.Copy(aUri2);

	iTestHarness->StartTestL(_L("Test TUriC Component Compare (8-bit)"));

	error = DoUriCompare<TUriParser8>(uri8Bit1, uri8Bit2, aMatchFlags);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(2, uriBuf1);	// uriBuf1, uriBuf2
	}

void CUriAndAuthorityTest::TestUriComponentPresenceL(const TDesC& aUri, TInt aPresenceFlags) const
	{
	iTestHarness->StartTestL(_L("Test TUriC Component Presence (16-bit)"));

	TInt error = DoUriComponentPresence<TUriParser16>(aUri, aPresenceFlags);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* uriBuf = HBufC8::NewLC(aUri.Length());
	TPtr8 uri8Bit = uriBuf->Des();
	uri8Bit.Copy(aUri);

	iTestHarness->StartTestL(_L("Test TUriC Component Presence (8-bit)"));

	error = DoUriComponentPresence<TUriParser8>(uri8Bit, aPresenceFlags);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(uriBuf);	// uriBuf
	}

void CUriAndAuthorityTest::TestUriDesL(const TDesC& aUri) const
	{
	iTestHarness->StartTestL(_L("Test TUriC UriDes() (16-bit deprecated)"));

	TInt error = DoUriDes<TUriParser16>(aUri);

	iTestHarness->EndTest(error);
	
	iTestHarness->StartTestL(_L("Test TUriC UriDes() (16-bit)"));

	TRAP( error , DoUriDesL(aUri) );

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* uriBuf = HBufC8::NewLC(aUri.Length());
	TPtr8 uri8Bit = uriBuf->Des();
	uri8Bit.Copy(aUri);

	iTestHarness->StartTestL(_L("Test TUriC UriDes() (8-bit)"));

	error = DoUriDes<TUriParser8>(uri8Bit);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(uriBuf);	// uriBuf
	}

void CUriAndAuthorityTest::TestUriSchemeValidiyL(const TDesC& aUri, TBool aValidScheme) const
	{
	iTestHarness->StartTestL(_L("Test TUriC Scheme Validation (16-bit)"));

	TInt error = DoUriSchemeValidation<TUriParser16>(aUri, aValidScheme);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* uriBuf = HBufC8::NewLC(aUri.Length());
	TPtr8 uri8Bit = uriBuf->Des();
	uri8Bit.Copy(aUri);

	iTestHarness->StartTestL(_L("Test TUriC Scheme Validation (8-bit)"));

	error = DoUriSchemeValidation<TUriParser8>(uri8Bit, aValidScheme);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(uriBuf);	// uriBuf
	}

void CUriAndAuthorityTest::TestUriWithoutFragmentL(const TDesC& aUri, const TDesC& aExpected)
	{
	iTestHarness->StartTestL(_L("Test TUriC Uri Without Fragment (16-bit)"));

	TInt error = DoUriWithoutFragment<TUriParser16, TPtrC16>(aUri, aExpected);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* uriBuf = HBufC8::NewLC(aUri.Length());
	TPtr8 uri8Bit = uriBuf->Des();
	uri8Bit.Copy(aUri);

	HBufC8* expectedBuf = HBufC8::NewLC(aExpected.Length());
	TPtr8 expected8Bit = expectedBuf->Des();
	expected8Bit.Copy(aExpected);

	iTestHarness->StartTestL(_L("Test TUriC Uri Without Fragment (8-bit)"));

	error = DoUriWithoutFragment<TUriParser8, TPtrC8>(uri8Bit, expected8Bit);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(2, uriBuf);	// uriBuf, expectedBuf
	}

void CUriAndAuthorityTest::TestUriSetComponentL(const TDesC& aUri_Scheme, const TDesC& aUri_Host, const TDesC& aUri_Userinfo,
									const TDesC& aUri_Port, const TDesC& aUri_Path, const TDesC& aUri_Query, 
									const TDesC& aUri_Fragment, const TDesC& aScheme, const TDesC& aHost, 
									const TDesC& aUserinfo, const TDesC& aPort, const TDesC& aPath, 
									const TDesC& aQuery, const TDesC& aFragment) const

	{
	iTestHarness->StartTestL(_L("Test CUri SetComponentL (16-bit)"));

	TInt error = DoUriConstruction<CUri16>(aUri_Scheme, aUri_Host, aUri_Userinfo, aUri_Port, aUri_Path,
		aUri_Query, aUri_Fragment, aScheme, aHost, aUserinfo, aPort, aPath, aQuery, aFragment);

	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* uri_SchemeBuf = HBufC8::NewLC(aUri_Scheme.Length());
	TPtr8 uri_Scheme8Bit = uri_SchemeBuf->Des();
	uri_Scheme8Bit.Copy(aUri_Scheme);

	HBufC8* uri_HostBuf = HBufC8::NewLC(aUri_Host.Length());
	TPtr8 uri_Host8Bit = uri_HostBuf->Des();
	uri_Host8Bit.Copy(aUri_Host);

	HBufC8* uri_UserinfoBuf = HBufC8::NewLC(aUri_Userinfo.Length());
	TPtr8 uri_Userinfo8Bit = uri_UserinfoBuf->Des();
	uri_Userinfo8Bit.Copy(aUri_Userinfo);

	HBufC8* uri_PortBuf = HBufC8::NewLC(aUri_Port.Length());
	TPtr8 uri_Port8Bit = uri_PortBuf->Des();
	uri_Port8Bit.Copy(aUri_Port);

	HBufC8* uri_PathBuf = HBufC8::NewLC(aUri_Path.Length());
	TPtr8 uri_Path8Bit = uri_PathBuf->Des();
	uri_Path8Bit.Copy(aUri_Path);

	HBufC8* uri_QueryBuf = HBufC8::NewLC(aUri_Query.Length());
	TPtr8 uri_Query8Bit = uri_QueryBuf->Des();
	uri_Query8Bit.Copy(aUri_Query);

	HBufC8* uri_FragmentBuf = HBufC8::NewLC(aUri_Fragment.Length());
	TPtr8 uri_Fragment8Bit = uri_FragmentBuf->Des();
	uri_Fragment8Bit.Copy(aUri_Fragment);

	// And the individual components
	HBufC8* schemeBuf = HBufC8::NewLC(aScheme.Length());
	TPtr8 scheme8Bit = schemeBuf->Des();
	scheme8Bit.Copy(aScheme);

	HBufC8* userinfoBuf = HBufC8::NewLC(aUserinfo.Length());
	TPtr8 userinfo8Bit = userinfoBuf->Des();
	userinfo8Bit.Copy(aUserinfo);

	HBufC8* hostBuf = HBufC8::NewLC(aHost.Length());
	TPtr8 host8Bit = hostBuf->Des();
	host8Bit.Copy(aHost);

	HBufC8* portBuf = HBufC8::NewLC(aPort.Length());
	TPtr8 port8Bit = portBuf->Des();
	port8Bit.Copy(aPort);

	HBufC8* pathBuf = HBufC8::NewLC(aPath.Length());
	TPtr8 path8Bit = pathBuf->Des();
	path8Bit.Copy(aPath);

	HBufC8* queryBuf = HBufC8::NewLC(aQuery.Length());
	TPtr8 query8Bit = queryBuf->Des();
	query8Bit.Copy(aQuery);

	HBufC8* fragmentBuf = HBufC8::NewLC(aFragment.Length());
	TPtr8 fragment8Bit = fragmentBuf->Des();
	fragment8Bit.Copy(aFragment);

	iTestHarness->StartTestL(_L("Test CUri SetComponentL (8-bit)"));

	error = DoUriConstruction<CUri8>(uri_Scheme8Bit, uri_Host8Bit, uri_Userinfo8Bit, uri_Port8Bit, uri_Path8Bit,
		uri_Query8Bit, uri_Fragment8Bit, scheme8Bit, host8Bit, userinfo8Bit, port8Bit, path8Bit, query8Bit, fragment8Bit);

	iTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(14, uri_SchemeBuf);	// too many to list!
	}
						   
void CUriAndAuthorityTest::TestUriRemoveComponentL(const TDesC& aUri_NoScheme, const TDesC& aUri_NoHost, const TDesC& aUri_NoUserinfo, 
									   const TDesC& aUri_NoPort, const TDesC& aUri_NoPath, const TDesC& aUri_NoQuery, 
									   const TDesC& aUri_NoFragment, const TDesC& aUri_Whole) const
	{
	iTestHarness->StartTestL(_L("Test CUri RemoveComponentL (16-bit)"));

	TInt error = DoUriDestruction<CUri16, TUriParser16>(aUri_NoScheme, aUri_NoHost, aUri_NoUserinfo, aUri_NoPort, aUri_NoPath,
		aUri_NoQuery, aUri_NoFragment, aUri_Whole);

	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* uri_NoSchemeBuf = HBufC8::NewLC(aUri_NoScheme.Length());
	TPtr8 uri_NoScheme8Bit = uri_NoSchemeBuf->Des();
	uri_NoScheme8Bit.Copy(aUri_NoScheme);

	HBufC8* uri_NoHostBuf = HBufC8::NewLC(aUri_NoHost.Length());
	TPtr8 uri_NoHost8Bit = uri_NoHostBuf->Des();
	uri_NoHost8Bit.Copy(aUri_NoHost);

	HBufC8* uri_NoUserinfoBuf = HBufC8::NewLC(aUri_NoUserinfo.Length());
	TPtr8 uri_NoUserinfo8Bit = uri_NoUserinfoBuf->Des();
	uri_NoUserinfo8Bit.Copy(aUri_NoUserinfo);

	HBufC8* uri_NoPortBuf = HBufC8::NewLC(aUri_NoPort.Length());
	TPtr8 uri_NoPort8Bit = uri_NoPortBuf->Des();
	uri_NoPort8Bit.Copy(aUri_NoPort);

	HBufC8* uri_NoPathBuf = HBufC8::NewLC(aUri_NoPath.Length());
	TPtr8 uri_NoPath8Bit = uri_NoPathBuf->Des();
	uri_NoPath8Bit.Copy(aUri_NoPath);

	HBufC8* uri_NoQueryBuf = HBufC8::NewLC(aUri_NoQuery.Length());
	TPtr8 uri_NoQuery8Bit = uri_NoQueryBuf->Des();
	uri_NoQuery8Bit.Copy(aUri_NoQuery);

	HBufC8* uri_NoFragmentBuf = HBufC8::NewLC(aUri_NoFragment.Length());
	TPtr8 uri_NoFragment8Bit = uri_NoFragmentBuf->Des();
	uri_NoFragment8Bit.Copy(aUri_NoFragment);

	HBufC8* uri_WholeBuf = HBufC8::NewLC(aUri_Whole.Length());
	TPtr8 uri_Whole8Bit = uri_WholeBuf->Des();
	uri_Whole8Bit.Copy(aUri_Whole);

	iTestHarness->StartTestL(_L("Test CUri RemoveComponentL (8-bit)"));

	error = DoUriDestruction<CUri8, TUriParser8>(uri_NoScheme8Bit, uri_NoHost8Bit, uri_NoUserinfo8Bit, uri_NoPort8Bit, 
		uri_NoPath8Bit, uri_NoQuery8Bit, uri_NoFragment8Bit, uri_Whole8Bit);

	iTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(8, uri_NoSchemeBuf);	// too many to list!
	}

//
//
//	Authority Tests
//
//

void CUriAndAuthorityTest::TestAuthorityComponentExtractionL(const TDesC16& aAuthority, const TDesC16& aUserinfo, 
												 const TDesC16& aHost, const TDesC16& aPort) const
	{
	iTestHarness->StartTestL(_L("Test TAuthority Component Extraction (deprecated 16-bit)"));
	iTestHarness->LogIt(_L("Uri - %S"), &aAuthority);

	TInt error = DoAuthorityComponentExtraction<TAuthorityParser16>(aAuthority, aUserinfo, aHost, aPort);

	iTestHarness->EndTest(error);
	
	iTestHarness->StartTestL(_L("Test TAuthority Component Extraction (16-bit)"));
	iTestHarness->LogIt(_L("Uri - %S"), &aAuthority);

	TRAP(error, DoAuthorityComponentExtractionL(aAuthority, aUserinfo, aHost, aPort) );

	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* authorityBuf = HBufC8::NewLC(aAuthority.Length());
	TPtr8 authority8Bit = authorityBuf->Des();
	authority8Bit.Copy(aAuthority);

	HBufC8* userinfoBuf = HBufC8::NewLC(aUserinfo.Length());
	TPtr8 userinfo8Bit = userinfoBuf->Des();
	userinfo8Bit.Copy(aUserinfo);

	HBufC8* hostBuf = HBufC8::NewLC(aHost.Length());
	TPtr8 host8Bit = hostBuf->Des();
	host8Bit.Copy(aHost);

	HBufC8* portBuf = HBufC8::NewLC(aPort.Length());
	TPtr8 port8Bit = portBuf->Des();
	port8Bit.Copy(aPort);

	iTestHarness->StartTestL(_L("Test TAuthority Component Extraction (8-bit)"));
	iTestHarness->LogIt(_L("Uri - %S"), &aAuthority);

	error = DoAuthorityComponentExtraction<TAuthorityParser8>(authority8Bit, userinfo8Bit, host8Bit, port8Bit);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(4, authorityBuf);	// authorityBuf, userInfoBuf, hostBuf, portBuf
	}

void CUriAndAuthorityTest::TestAuthorityCompareL(const TDesC& aAuthority1, const TDesC& aAuthority2, TInt aMatchFlags) const
	{
	iTestHarness->StartTestL(_L("Test TAuthorityC Component Compare (16-bit)"));

	TInt error = DoAuthorityCompare<TAuthorityParser16>(aAuthority1, aAuthority2, aMatchFlags);

	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* authorityBuf1 = HBufC8::NewLC(aAuthority1.Length());
	TPtr8 authority8Bit1 = authorityBuf1->Des();
	authority8Bit1.Copy(aAuthority1);

	HBufC8* authorityBuf2 = HBufC8::NewLC(aAuthority2.Length());
	TPtr8 authority8Bit2 = authorityBuf2->Des();
	authority8Bit2.Copy(aAuthority2);

	iTestHarness->StartTestL(_L("Test TAuthorityC Component Compare (8-bit)"));

	error = DoAuthorityCompare<TAuthorityParser8>(authority8Bit1, authority8Bit2, aMatchFlags);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(2, authorityBuf1);	// authorityBuf1, authorityBuf2
	}

void CUriAndAuthorityTest::TestAuthorityComponentPresenceL(const TDesC& aAuthority, TInt aPresenceFlags) const
	{
	iTestHarness->StartTestL(_L("Test TAuthorityC Component Presence (16-bit)"));

	TInt error = DoAuthorityComponentPresence<TAuthorityParser16>(aAuthority, aPresenceFlags);

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* authorityBuf = HBufC8::NewLC(aAuthority.Length());
	TPtr8 authority8Bit = authorityBuf->Des();
	authority8Bit.Copy(aAuthority);

	iTestHarness->StartTestL(_L("Test TAuthorityC Component Presence (8-bit)"));

	error = DoAuthorityComponentPresence<TAuthorityParser8>(authority8Bit, aPresenceFlags);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(authorityBuf);	// authorityBuf
	}

void CUriAndAuthorityTest::TestAuthorityDesL(const TDesC& aAuthority)
	{
	iTestHarness->StartTestL(_L("Test TAuthorityC AuthorityDes() (16-bit deprecated)"));

	TInt error = DoAuthorityDes<TAuthorityParser16>(aAuthority);

	iTestHarness->EndTest(error);
	
	iTestHarness->StartTestL(_L("Test TAuthorityC AuthorityDes() (16-bit)"));

	TRAP( error , DoAuthorityDesL(aAuthority) );

	iTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* authorityBuf = HBufC8::NewLC(aAuthority.Length());
	TPtr8 authority8Bit = authorityBuf->Des();
	authority8Bit.Copy(aAuthority);

	iTestHarness->StartTestL(_L("Test TAuthorityC AuthorityDes() (8-bit)"));

	error = DoAuthorityDes<TAuthorityParser8>(authority8Bit);

	iTestHarness->EndTest(error);

	CleanupStack::PopAndDestroy(authorityBuf);	// authorityBuf
	}

void CUriAndAuthorityTest::TestAuthoritySetComponentL(const TDesC& aAuthority_Host, const TDesC& aAuthority_Userinfo, 
										  const TDesC& aAuthority_Port, const TDesC& aHost, 
										  const TDesC& aUserinfo, const TDesC& aPort)

	{
	iTestHarness->StartTestL(_L("Test CAuthority SetComponentL (16-bit)"));

	TInt error = DoAuthorityConstruction<CAuthority16>(aAuthority_Host, aAuthority_Userinfo, aAuthority_Port, 
		aHost, aUserinfo, aPort);

	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* authority_HostBuf = HBufC8::NewLC(aAuthority_Host.Length());
	TPtr8 authority_Host8Bit = authority_HostBuf->Des();
	authority_Host8Bit.Copy(aAuthority_Host);

	HBufC8* authority_UserinfoBuf = HBufC8::NewLC(aAuthority_Userinfo.Length());
	TPtr8 authority_Userinfo8Bit = authority_UserinfoBuf->Des();
	authority_Userinfo8Bit.Copy(aAuthority_Userinfo);

	HBufC8* authority_PortBuf = HBufC8::NewLC(aAuthority_Port.Length());
	TPtr8 authority_Port8Bit = authority_PortBuf->Des();
	authority_Port8Bit.Copy(aAuthority_Port);

	// And the individual components
	HBufC8* userinfoBuf = HBufC8::NewLC(aUserinfo.Length());
	TPtr8 userinfo8Bit = userinfoBuf->Des();
	userinfo8Bit.Copy(aUserinfo);

	HBufC8* hostBuf = HBufC8::NewLC(aHost.Length());
	TPtr8 host8Bit = hostBuf->Des();
	host8Bit.Copy(aHost);

	HBufC8* portBuf = HBufC8::NewLC(aPort.Length());
	TPtr8 port8Bit = portBuf->Des();
	port8Bit.Copy(aPort);

	iTestHarness->StartTestL(_L("Test CAuthority SetComponentL (8-bit)"));

	error = DoAuthorityConstruction<CAuthority8>(authority_Host8Bit, authority_Userinfo8Bit, authority_Port8Bit,
		host8Bit, userinfo8Bit, port8Bit);

	iTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(6, authority_HostBuf);	// too many to list!
	}

void CUriAndAuthorityTest::TestAuthoritySetAndEscapeComponentL(const TDesC& aAuthority_Host, const TDesC& aAuthority_Userinfo, 
												   const TDesC& aAuthority_Port, const TDesC& aHost, 
												   const TDesC& aUserinfo, const TDesC& aPort) const

	{
	iTestHarness->StartTestL(_L("Test CAuthority SetAndEscapeComponentL (16-bit)"));

	TInt error = DoAuthorityEscapedConstruction<CAuthority16>(aAuthority_Host, aAuthority_Userinfo, aAuthority_Port, 
		aHost, aUserinfo, aPort);

	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* authority_HostBuf = HBufC8::NewLC(aAuthority_Host.Length());
	TPtr8 authority_Host8Bit = authority_HostBuf->Des();
	authority_Host8Bit.Copy(aAuthority_Host);

	HBufC8* authority_UserinfoBuf = HBufC8::NewLC(aAuthority_Userinfo.Length());
	TPtr8 authority_Userinfo8Bit = authority_UserinfoBuf->Des();
	authority_Userinfo8Bit.Copy(aAuthority_Userinfo);

	HBufC8* authority_PortBuf = HBufC8::NewLC(aAuthority_Port.Length());
	TPtr8 authority_Port8Bit = authority_PortBuf->Des();
	authority_Port8Bit.Copy(aAuthority_Port);

	// And the individual components
	HBufC8* userinfoBuf = HBufC8::NewLC(aUserinfo.Length());
	TPtr8 userinfo8Bit = userinfoBuf->Des();
	userinfo8Bit.Copy(aUserinfo);

	HBufC8* hostBuf = HBufC8::NewLC(aHost.Length());
	TPtr8 host8Bit = hostBuf->Des();
	host8Bit.Copy(aHost);

	HBufC8* portBuf = HBufC8::NewLC(aPort.Length());
	TPtr8 port8Bit = portBuf->Des();
	port8Bit.Copy(aPort);

	iTestHarness->StartTestL(_L("Test CAuthority SetAnsEscapeComponentL (8-bit)"));

	error = DoAuthorityEscapedConstruction<CAuthority8>(authority_Host8Bit, authority_Userinfo8Bit, authority_Port8Bit,
		host8Bit, userinfo8Bit, port8Bit);

	iTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(6, authority_HostBuf);	// too many to list!
	}

void CUriAndAuthorityTest::TestAuthorityRemoveComponentL(const TDesC& aAuthority_NoHost, const TDesC& aAuthority_NoUserinfo, 
									   const TDesC& aAuthority_NoPort, const TDesC& aAuthority_Whole) const
	{
	iTestHarness->StartTestL(_L("Test CAuthority RemoveComponentL (16-bit)"));

	TInt error = DoAuthorityDestruction<CAuthority16, TAuthorityParser16>(aAuthority_NoHost, aAuthority_NoUserinfo, 
		aAuthority_NoPort, aAuthority_Whole);

	iTestHarness->EndTest(error);

	// Make 8-bit copies
	HBufC8* authority_NoHostBuf = HBufC8::NewLC(aAuthority_NoHost.Length());
	TPtr8 authority_NoHost8Bit = authority_NoHostBuf->Des();
	authority_NoHost8Bit.Copy(aAuthority_NoHost);

	HBufC8* authority_NoUserinfoBuf = HBufC8::NewLC(aAuthority_NoUserinfo.Length());
	TPtr8 authority_NoUserinfo8Bit = authority_NoUserinfoBuf->Des();
	authority_NoUserinfo8Bit.Copy(aAuthority_NoUserinfo);

	HBufC8* authority_NoPortBuf = HBufC8::NewLC(aAuthority_NoPort.Length());
	TPtr8 authority_NoPort8Bit = authority_NoPortBuf->Des();
	authority_NoPort8Bit.Copy(aAuthority_NoPort);

	HBufC8* authority_WholeBuf = HBufC8::NewLC(aAuthority_Whole.Length());
	TPtr8 authority_Whole8Bit = authority_WholeBuf->Des();
	authority_Whole8Bit.Copy(aAuthority_Whole);

	iTestHarness->StartTestL(_L("Test CAuthority RemoveComponentL (8-bit)"));

	error = DoAuthorityDestruction<CAuthority8, TAuthorityParser8>(authority_NoHost8Bit, authority_NoUserinfo8Bit, 
		authority_NoPort8Bit, authority_Whole8Bit);

	iTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(4, authority_NoHostBuf);	// too many to list!
	}

//
//
//	Implementation of LOCAL functions
//
//

template<class TUriParserType, class TDesCType>
TInt DoUriComponentExtraction(const TDesCType& aUri, const TDesCType& aScheme,
										 const TDesCType& aUserinfo, const TDesCType& aHost,
										 const TDesCType& aPort, const TDesCType& aPath,
										 const TDesCType& aQuery, const TDesCType& aFragment)
	{
	TUriParserType uriParser;
	TInt error = uriParser.Parse(aUri);
	// Is this a valid Uri?
	if( error == KUriUtilsErrInvalidUri )
		{
		return error;
		}

	// Check scheme...
	if( TestComponent(uriParser, aScheme, EUriScheme) != 0 )
		{
		return KUriUtilsErrDifferentScheme;
		}
	// Check userinfo...
	if( TestComponent(uriParser, aUserinfo, EUriUserinfo) != 0 )
		{
		return KUriUtilsErrDifferentUserInfo;
		}
	// Check host...
	if( TestComponent(uriParser, aHost, EUriHost) != 0 )
		{
		return KUriUtilsErrDifferentHost;
		}
	// Check port...
	if( TestComponent(uriParser, aPort, EUriPort) != 0 )
		{
		return KUriUtilsErrDifferentPort;
		}
	// Check path...
	if( TestComponent(uriParser, aPath, EUriPath) != 0)
		{
		return KUriUtilsErrDifferentPath;
		}
	// Check query...
	if( TestComponent(uriParser, aQuery, EUriQuery) != 0 )
		{
		return KUriUtilsErrDifferentQuery;
		}
	// Check fragment...
	if( TestComponent(uriParser, aFragment, EUriFragment) != 0 )
		{
		return KUriUtilsErrDifferentFragment;
		}
	return KErrNone;
	}


/** create 8-bit URI from descriptor, compare components*/
void DoUriComponentExtractionL(const TDesC& aUri, const TDesC& aScheme,
										 const TDesC& aUserinfo, const TDesC& aHost,
										 const TDesC& aPort, const TDesC& aPath,
										 const TDesC& aQuery, const TDesC& aFragment)
	{
	CUri8* uri = UriUtils::CreateUriL(aUri);
	CleanupStack::PushL(uri);

	// Check scheme...
	HBufC* text = uri->Uri().DisplayFormL(EUriScheme);
	TInt error = ( text->Compare(aScheme) == 0 ) ? KErrNone : KUriUtilsErrDifferentScheme;
	delete text;
	User::LeaveIfError(error);
	
	// Check userinfo...
	text = uri->Uri().DisplayFormL(EUriUserinfo);
	error = ( text->Compare(aUserinfo) == 0 ) ? KErrNone : KUriUtilsErrDifferentUserInfo;
	delete text;
	User::LeaveIfError(error);
	
	// Check host...
	text = uri->Uri().DisplayFormL(EUriHost);
	error = ( text->Compare(aHost) == 0 ) ? KErrNone : KUriUtilsErrDifferentHost;
	delete text;
	User::LeaveIfError(error);
	
	// Check port...
	text = uri->Uri().DisplayFormL(EUriPort);
	error = ( text->Compare(aPort) == 0 ) ? KErrNone : KUriUtilsErrDifferentPort;
	delete text;
	User::LeaveIfError(error);
	
	// Check path...
	text = uri->Uri().DisplayFormL(EUriPath);
	error = ( text->Compare(aPath) == 0 ) ? KErrNone : KUriUtilsErrDifferentPath;
	delete text;
	User::LeaveIfError(error);

	// Check query...
	text = uri->Uri().DisplayFormL(EUriQuery);
	error = ( text->Compare(aQuery) == 0 ) ? KErrNone : KUriUtilsErrDifferentQuery;
	delete text;
	User::LeaveIfError(error);

	// Check fragment...
	text = uri->Uri().DisplayFormL(EUriFragment);
	error = ( text->Compare(aFragment) == 0 ) ? KErrNone : KUriUtilsErrDifferentFragment;
	delete text;
	User::LeaveIfError(error);
	CleanupStack::PopAndDestroy(uri);
	}
	
	
template<class TAuthorityParserType, class TDesCType>
TInt DoAuthorityComponentExtraction(const TDesCType& aAuthority, const TDesCType& aUserInfo, 
											 const TDesCType& aHost, const TDesCType& aPort)
	{
	TAuthorityParserType authorityParser;
	TInt error = authorityParser.Parse(aAuthority);

	// Check userinfo...
	if( TestComponent(authorityParser, aUserInfo, EAuthorityUserinfo) != 0 )
		{
		return KUriUtilsErrDifferentUserInfo;
		}
	// Check host...
	if( TestComponent(authorityParser, aHost, EAuthorityHost) != 0 )
		{
		return KUriUtilsErrDifferentHost;
		}
	// Check port...
	if( TestComponent(authorityParser, aPort, EAuthorityPort) != 0 )
		{
		return KUriUtilsErrDifferentPort;
		}
	return error;
	}

/** create 8-bit Authority from descriptor, compare components*/
void DoAuthorityComponentExtractionL(const TDesC& aAuthority, const TDesC& aUserInfo, 
											 const TDesC& aHost, const TDesC& aPort)
	{
	CAuthority8* authority = UriUtils::CreateAuthorityL(aAuthority);
	CleanupStack::PushL(authority);

	// Check userinfo...
	HBufC* text = authority->Authority().DisplayFormL(EAuthorityUserinfo);
	TInt error = ( text->Compare(aUserInfo) == 0 ) ? KErrNone : KUriUtilsErrDifferentUserInfo;
	delete text;
	User::LeaveIfError(error);

	// Check host...
	text = authority->Authority().DisplayFormL(EAuthorityHost);
	error = ( text->Compare(aHost) == 0 ) ? KErrNone : KUriUtilsErrDifferentHost;
	delete text;

	// Check port...
	text = authority->Authority().DisplayFormL(EAuthorityPort);
	error = ( text->Compare(aPort) == 0 ) ? KErrNone : KUriUtilsErrDifferentPort;
	delete text;
	
	CleanupStack::PopAndDestroy(authority);
	}

template<class TParserType, class TDesCType, class TComponentType>
TInt TestComponent(const TParserType& aParser, const TDesCType& aExpected, TComponentType aComponent)
	{
	return aParser.Extract(aComponent).Compare(aExpected);
	}

template<class TUriParserType, class CUriType, class TDesCType>
TInt DoTestResolve(const TDesCType& aBase, const TDesCType& aReference, const TDesCType& aExpected)
	{
	TUriParserType baseUri;
	TInt error = baseUri.Parse(aBase);
	if ( error != KErrNone )
		{
		return error;
		}
	TUriParserType refUri;
	error = refUri.Parse(aReference);
	if ( error != KErrNone )
		{
		return error;
		}
	CUriType* resolvedUri = NULL;
	TRAP(error, resolvedUri = CUriType::ResolveL(baseUri, refUri));
	if( error != KErrNone )
		{
		return error;
		}
	error = resolvedUri->Uri().UriDes().Compare(aExpected);
	
	delete resolvedUri;
	return error;
	}

template<class TUriParserType, class TDesCType>
TInt DoUriCompare(const TDesCType& aUri1, const TDesCType& aUri2, TInt aMatchFlags)
	{
	TUriParserType uri1;
	TInt error = uri1.Parse(aUri1);
	if( error == KUriUtilsErrInvalidUri )
		{
		return error;
		}
	TUriParserType uri2;
	error = uri2.Parse(aUri2);
	if( error == KUriUtilsErrInvalidUri )
		{
		return error;
		}
	// Test compare for fragment
	if( (error = TestCompare(uri1, uri2, EUriFragment, (aMatchFlags & EFragmentFlag))) != 0 )
		{
		return KErrNotFound;
		}
	// Test compare for query
	if( (error = TestCompare(uri1, uri2, EUriQuery, (aMatchFlags & EQueryFlag))) != 0 )
		{
		return KErrNotFound;
		}
	// Test compare for path
	if( (error = TestCompare(uri1, uri2, EUriPath, (aMatchFlags & EPathFlag))) != 0 )
		{
		return KErrNotFound;
		}
	// Test compare for port
	if( (error = TestCompare(uri1, uri2, EUriPort, (aMatchFlags & EPortFlag))) != 0 )
		{
		return KErrNotFound;
		}
	// Test compare for userinfo
	if( (error = TestCompare(uri1, uri2, EUriUserinfo, (aMatchFlags & EUserinfoFlag))) != 0 )
		{
		return KErrNotFound;
		}
	// Test compare for host
	if( (error = TestCompare(uri1, uri2, EUriHost, (aMatchFlags & EHostFlag))) != 0 )
		{
		return KErrNotFound;
		}
	// Test compare for scheme
	if( (error = TestCompare(uri1, uri2, EUriScheme, (aMatchFlags & ESchemeFlag))) != 0 )
		{
		return KErrNotFound;
		}
	return error;
	}

template<class TAuthorityParserType, class TDesCType>
TInt DoAuthorityCompare(const TDesCType& aAuthority1, const TDesCType& aAuthority2, TInt aMatchFlags)
	{
	TAuthorityParserType auth1;
	TInt error = auth1.Parse(aAuthority1);
	TAuthorityParserType auth2;
	error = auth2.Parse(aAuthority2);
	// Test compare for userinfo
	if( (error = TestCompare(auth1, auth2, EAuthorityUserinfo, (aMatchFlags & EUserinfoFlag))) != 0 )
		{
		return KErrNotFound;
		}
	// Test compare for port
	if( (error = TestCompare(auth1, auth2, EAuthorityPort, (aMatchFlags & EPortFlag))) != 0 )
		{
		return KErrNotFound;
		}
	// Test compare for host
	if( (error = TestCompare(auth1, auth2, EAuthorityHost, (aMatchFlags & EHostFlag))) != 0 )
		{
		return KErrNotFound;
		}
	return error;
	}

template<class TParserType, class TComponentType>
TInt TestCompare(const TParserType& aData1, const TParserType& aData2, TComponentType aComponent, TBool aExpectedMatch)
	{
	TInt error = aData1.Compare(aData2, aComponent);
	TBool match = error == 0;
	if( (!match && aExpectedMatch) | (match && !aExpectedMatch) ) 
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class TUriParserType, class TDesCType>
TInt DoUriComponentPresence(const TDesCType& aUri, TInt aPresenceFlags)
	{
	TUriParserType uri;
	TInt error = uri.Parse(aUri);
	if( error == KUriUtilsErrInvalidUri )
		{
		return error;
		}
	// Test compare for scheme
	if( (error = TestPresence(uri, EUriScheme, (aPresenceFlags & ESchemeFlag))) != 0)
		{
		return KErrNotFound;
		}
	// Test compare for host
	if( (error = TestPresence(uri, EUriHost, (aPresenceFlags & EHostFlag))) != 0)
		{
		return KErrNotFound;
		}
	// Test compare for userinfo
	if( (error = TestPresence(uri, EUriUserinfo, (aPresenceFlags & EUserinfoFlag))) != 0)
		{
		return KErrNotFound;
		}
	// Test compare for port
	if( (error = TestPresence(uri, EUriPort, (aPresenceFlags & EPortFlag))) != 0)
		{
		return KErrNotFound;
		}
	// Test compare for path
	if( (error = TestPresence(uri, EUriPath, (aPresenceFlags & EPathFlag))) != 0)
		{
		return KErrNotFound;
		}
	// Test compare for query
	if( (error = TestPresence(uri, EUriQuery, (aPresenceFlags & EQueryFlag))) != 0)
		{
		return KErrNotFound;
		}
	// Test compare for fragment
	if( (error = TestPresence(uri, EUriFragment, (aPresenceFlags & EFragmentFlag))) != 0)
		{
		return KErrNotFound;
		}
	return error;
	}

template<class TAuthorityParserType, class TDesCType>
TInt DoAuthorityComponentPresence(const TDesCType& aAuthority, TInt aPresenceFlags)
	{
	TAuthorityParserType authority;
	TInt error = authority.Parse(aAuthority);
	// Test compare for host
	if( (error = TestPresence(authority, EAuthorityHost, (aPresenceFlags & EHostFlag))) != 0)
		{
		return KErrNotFound;
		}
	// Test compare for userinfo
	if( (error = TestPresence(authority, EAuthorityUserinfo, (aPresenceFlags & EUserinfoFlag))) != 0)
		{
		return KErrNotFound;
		}
	// Test compare for port
	if( (error = TestPresence(authority, EAuthorityPort, (aPresenceFlags & EPortFlag))) != 0)
		{
		return KErrNotFound;
		}
	return error;
	}

template<class TParserType, class TComponentType>
TInt TestPresence(const TParserType& aData, TComponentType aComponent, TBool aExpectedPresence)
	{
	TBool present = aData.IsPresent(aComponent);
	if( (!present && aExpectedPresence) | (present && !aExpectedPresence) ) 
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class TUriParserType, class TDesCType>
TInt DoUriDes(const TDesCType& aUri)
	{
	TUriParserType uri;
	TInt error = uri.Parse(aUri);
	if( error == KUriUtilsErrInvalidUri )
		{
		return error;
		}
	if( uri.UriDes().Compare(aUri) != 0)
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

void DoUriDesL(const TDesC& aUri)
	{
	CUri8* uri = UriUtils::CreateUriL(aUri);
	CleanupStack::PushL(uri);
	
	HBufC* text = uri->Uri().DisplayFormL();
	TInt error = ( text->Compare(aUri) == 0 ) ? KErrNone : KErrNotFound;
	delete text;
	User::LeaveIfError(error);		
	CleanupStack::PopAndDestroy(uri);
	}
	
template<class TAuthorityParserType, class TDesCType>
TInt DoAuthorityDes(const TDesCType& aAuthority)
	{
	TAuthorityParserType authority;
	TInt error = authority.Parse(aAuthority);
	if( (error = authority.AuthorityDes().Compare(aAuthority)) != 0)
		{
		return KErrNotFound;
		}
	return error;
	}

void DoAuthorityDesL(const TDesC& aAuthority)
	{
	CAuthority8* authority = UriUtils::CreateAuthorityL(aAuthority);
	CleanupStack::PushL(authority);
	
	HBufC* text = authority->Authority().DisplayFormL();
	TInt error = ( text->Compare(aAuthority) == 0 ) ? KErrNone : KErrNotFound;
	delete text;
	User::LeaveIfError(error);

	CleanupStack::PopAndDestroy(authority);
	}

template<class TUriParserType, class TDesCType>
TInt DoUriSchemeValidation(const TDesCType& aUri, TBool aValidScheme)
	{
	TUriParserType uri;
	TInt error = uri.Parse(aUri);
	if( error == KUriUtilsErrInvalidUri )
		{
		return error;
		}
	TBool valid = uri.IsSchemeValid();
	if( (!valid && aValidScheme) | (valid && !aValidScheme) ) 
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class TUriParserType, class TPtrCType, class TDesCType>
TInt DoUriWithoutFragment(const TDesCType& aUri, const TDesCType& aExpected)
	{
	TUriParserType uri;
	TInt error = uri.Parse(aUri);
	if( error == KUriUtilsErrInvalidUri )
		{
		return error;
		}
	TPtrCType uriWithoutFragment;
	uri.UriWithoutFragment(uriWithoutFragment);
	if( (error = uriWithoutFragment.Compare(aExpected)) != 0)
		{
		return KErrNotFound;
		}
	return error;
	}

template<class CUriType, class TDesCType>
TInt DoUriConstruction(const TDesCType& aUri_Scheme, const TDesCType& aUri_Host, const TDesCType& aUri_Userinfo, 
							   const TDesCType& aUri_Port, const TDesCType& aUri_Path, const TDesCType& aUri_Query, 
							   const TDesCType& aUri_Fragment, const TDesCType& aScheme, const TDesCType& aHost, 
							   const TDesCType& aUserinfo, const TDesCType& aPort, const TDesCType& aPath, 
							   const TDesCType& aQuery, const TDesCType& aFragment)
	{
	// Create the CUriType object
	CUriType* uri = NULL;
	TRAPD(error, uri = CUriType::NewL());
	if( error != KErrNone )
		{
		return error;
		}
	CleanupStack::PushL(uri);
	// Add scheme...
	if( DoUriSetAndCompareComponent<CUriType>(uri, EUriScheme, aScheme, aUri_Scheme) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}

	// Add host...
	if( DoUriSetAndCompareComponent<CUriType>(uri, EUriHost, aHost, aUri_Host) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Add userinfo...
	if( DoUriSetAndCompareComponent<CUriType>(uri, EUriUserinfo, aUserinfo, aUri_Userinfo) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Add port...
	if( DoUriSetAndCompareComponent<CUriType>(uri, EUriPort, aPort, aUri_Port) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Add path...
	if( DoUriSetAndCompareComponent<CUriType>(uri, EUriPath, aPath, aUri_Path) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Add query...
	if( DoUriSetAndCompareComponent<CUriType>(uri, EUriQuery, aQuery, aUri_Query) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Add fragment...
	if( DoUriSetAndCompareComponent<CUriType>(uri, EUriFragment, aFragment, aUri_Fragment) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Cleanup and return
	CleanupStack::PopAndDestroy(uri);	// uri
	return KErrNone;
	}

template<class CUriType, class TDesCType>
TInt DoUriSetAndCompareComponent(CUriType* aUri, TUriComponent aComponent, const TDesCType& aData, const TDesCType& aExpected)
	{
	TRAPD(error, aUri->SetComponentL(aData, aComponent));
	if( error != KErrNone )
		{
		return error;
		}
	// Compare with expected
	if( aUri->Uri().UriDes().Compare(aExpected) != 0)
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class CUriType, class TUriParserType, class TDesCType>
TInt DoUriDestruction(const TDesCType& aUri_NoScheme, const TDesCType& aUri_NoHost, const TDesCType& aUri_NoUserinfo, 
							  const TDesCType& aUri_NoPort, const TDesCType& aUri_NoPath, const TDesCType& aUri_NoQuery, 
							  const TDesCType& aUri_NoFragment, const TDesCType& aUri_Whole)
	{
	// Create a TUriC-derived objet from whole uri
	TUriParserType parser;
	TInt error = parser.Parse(aUri_Whole);
	if( error == KUriUtilsErrInvalidUri )
		{
		return error;
		}
	// Create the CUriType object
	CUriType* uri = NULL;
	TRAP(error, uri = CUriType::NewL(parser));
	if( error != KErrNone )
		{
		return error;
		}
	CleanupStack::PushL(uri);
	// Remove fragment...
	if( DoUriRemoveAndCompareComponent<CUriType>(uri, EUriFragment, aUri_NoFragment) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Remove query...
	if( DoUriRemoveAndCompareComponent<CUriType>(uri, EUriQuery, aUri_NoQuery) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Remove path...
	if( DoUriRemoveAndCompareComponent<CUriType>(uri, EUriPath, aUri_NoPath) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Remove port...
	if( DoUriRemoveAndCompareComponent<CUriType>(uri, EUriPort, aUri_NoPort) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Remove userinfo...
	if( DoUriRemoveAndCompareComponent<CUriType>(uri, EUriUserinfo, aUri_NoUserinfo) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Remove host...
	if( DoUriRemoveAndCompareComponent<CUriType>(uri, EUriHost, aUri_NoHost) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Remove scheme...
	if( DoUriRemoveAndCompareComponent<CUriType>(uri, EUriScheme, aUri_NoScheme) != KErrNone )
		{
		CleanupStack::PopAndDestroy(uri);	// uri
		return KErrNotFound;
		}
	// Cleanup and return
	CleanupStack::PopAndDestroy(uri);	// uri
	return KErrNone;
	}

template<class CUriType, class TDesCType>
TInt DoUriRemoveAndCompareComponent(CUriType* aUri, TUriComponent aComponent, const TDesCType& aExpected)
	{
	TRAPD(error, aUri->RemoveComponentL(aComponent));
	if( error != KErrNone )
		{
		return error;
		}
	// Compare with expected
	if( aUri->Uri().UriDes().Compare(aExpected) != 0)
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class CAuthorityType, class TDesCType>
TInt DoAuthorityConstruction(const TDesCType& aAuthority_Host, const TDesCType& aAuthority_Userinfo, const TDesCType& aAuthority_Port, 
							   const TDesCType& aHost, const TDesCType& aUserinfo, const TDesCType& aPort)
	{
	// Create the CAuthorityType object
	CAuthorityType* authority = NULL;
	TRAPD(error, authority = CAuthorityType::NewL());
	if( error != KErrNone )
		{
		return error;
		}
	CleanupStack::PushL(authority);
	// Add host...
	if( DoAuthoritySetAndCompareComponent<CAuthorityType>(authority, EAuthorityHost, aHost, aAuthority_Host) != KErrNone )
		{
		CleanupStack::PopAndDestroy(authority);	// authority
		return KErrNotFound;
		}
	// Add userinfo...
	if( DoAuthoritySetAndCompareComponent<CAuthorityType>(authority, EAuthorityUserinfo, aUserinfo, aAuthority_Userinfo) != KErrNone )
		{
		CleanupStack::PopAndDestroy(authority);	// authority
		return KErrNotFound;
		}
	// Add port...
	if( DoAuthoritySetAndCompareComponent<CAuthorityType>(authority, EAuthorityPort, aPort, aAuthority_Port) != KErrNone )
		{
		CleanupStack::PopAndDestroy(authority);	// authority
		return KErrNotFound;
		}
	// Cleanup and return
	CleanupStack::PopAndDestroy(authority);	// authority
	return KErrNone;
	}

template<class CAuthorityType, class TDesCType>
TInt DoAuthoritySetAndCompareComponent(CAuthorityType* aAuthority, TAuthorityComponent aComponent, const TDesCType& aData, const TDesCType& aExpected)
	{
	TRAPD(error, aAuthority->SetComponentL(aData, aComponent));
	if( error != KErrNone )
		{
		return error;
		}
	// Compare with expected
	if( aAuthority->Authority().AuthorityDes().Compare(aExpected) != 0)
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class CAuthorityType, class TDesCType>
TInt DoAuthorityEscapedConstruction(const TDesCType& aAuthority_Host, const TDesCType& aAuthority_Userinfo, const TDesCType& aAuthority_Port, 
											const TDesCType& aHost, const TDesCType& aUserinfo, const TDesCType& aPort)
	{
	// Create the CAuthorityType object
	CAuthorityType* authority = NULL;
	TRAPD(error, authority = CAuthorityType::NewL());
	if( error != KErrNone )
		{
		return error;
		}
	CleanupStack::PushL(authority);
	// Add host...
	if( DoAuthoritySetAndEscapeAndCompareComponent<CAuthorityType>(authority, EAuthorityHost, aHost, aAuthority_Host) != KErrNone )
		{
		CleanupStack::PopAndDestroy(authority);	// authority
		return KErrNotFound;
		}
	// Add userinfo...
	if( DoAuthoritySetAndEscapeAndCompareComponent<CAuthorityType>(authority, EAuthorityUserinfo, aUserinfo, aAuthority_Userinfo) != KErrNone )
		{
		CleanupStack::PopAndDestroy(authority);	// authority
		return KErrNotFound;
		}
	// Add port...
	if( DoAuthoritySetAndEscapeAndCompareComponent<CAuthorityType>(authority, EAuthorityPort, aPort, aAuthority_Port) != KErrNone )
		{
		CleanupStack::PopAndDestroy(authority);	// authority
		return KErrNotFound;
		}
	// Cleanup and return
	CleanupStack::PopAndDestroy(authority);	// authority
	return KErrNone;
	}

template<class CAuthorityType, class TDesCType>
TInt DoAuthoritySetAndEscapeAndCompareComponent(CAuthorityType* aAuthority, TAuthorityComponent aComponent, const TDesCType& aData, const TDesCType& aExpected)
	{
	TRAPD(error, aAuthority->SetAndEscapeComponentL(aData, aComponent));
	if( error != KErrNone )
		{
		return error;
		}
	// Compare with expected
	if( aAuthority->Authority().AuthorityDes().Compare(aExpected) != 0)
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

template<class CAuthorityType, class TAuthorityParserType, class TDesCType>
TInt DoAuthorityDestruction(const TDesCType& aAuthority_NoHost, const TDesCType& aAuthority_NoUserinfo, 
									const TDesCType& aAuthority_NoPort, const TDesCType& aAuthority_Whole)
	{
	// Create a TAuthorityC-derived objet from whole authority
	TAuthorityParserType parser;
	TInt error = parser.Parse(aAuthority_Whole);
	// Create the CAuthorityType object
	CAuthorityType* authority = NULL;
	TRAP(error, authority = CAuthorityType::NewL(parser));
	if( error != KErrNone )
		{
		return error;
		}
	CleanupStack::PushL(authority);
	// Remove port...
	if( DoAuthorityRemoveAndCompareComponent<CAuthorityType>(authority, EAuthorityPort, aAuthority_NoPort) != KErrNone )
		{
		CleanupStack::PopAndDestroy(authority);	// authority
		return KErrNotFound;
		}
	// Remove userinfo...
	if( DoAuthorityRemoveAndCompareComponent<CAuthorityType>(authority, EAuthorityUserinfo, aAuthority_NoUserinfo) != KErrNone )
		{
		CleanupStack::PopAndDestroy(authority);	// authority
		return KErrNotFound;
		}
	// Remove host...
	if( DoAuthorityRemoveAndCompareComponent<CAuthorityType>(authority, EAuthorityHost, aAuthority_NoHost) != KErrNone )
		{
		CleanupStack::PopAndDestroy(authority);	// authority
		return KErrNotFound;
		}
	// Cleanup and return
	CleanupStack::PopAndDestroy(authority);	// authority
	return KErrNone;
	}

template<class CAuthorityType, class TDesCType>
TInt DoAuthorityRemoveAndCompareComponent(CAuthorityType* aAuthority, TAuthorityComponent aComponent, const TDesCType& aExpected)
	{
	TRAPD(error, aAuthority->RemoveComponentL(aComponent));
	if( error != KErrNone )
		{
		return error;
		}
	// Compare with expected
	if( aAuthority->Authority().AuthorityDes().Compare(aExpected) != 0)
		{
		return KErrNotFound;
		}
	return KErrNone;
	}
