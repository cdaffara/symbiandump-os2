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
//

#ifndef __SIPURIANDAUTHORITYTESTS_H__
#define __SIPURIANDAUTHORITYTESTS_H__

// System includes
//
#include <e32base.h>

template<class TUriParserType, class TDesCType>
TInt DoSipUriValidation(const TDesCType& aSipUri);

template<class TUriParserType, class TDesCType>
TInt DoSipUriEquivalence(const TDesCType& aSipUriA, const TDesCType& aSipUriB, TInt &aSipUriError);

//
//
//	SIP Uri and Authority component extraction test data
//	This data is used for parsing & generating, validating and comparing SIP URIs
//
//

_LIT(KCompsSip0,		"sip:alice@atlanta.com");
_LIT(KSchemeSip0,		"sip");
_LIT(KPathSip0,		"");
_LIT(KQuerySip0,		"");
_LIT(KFragmentSip0,	"");
_LIT(KUserSip0,		"alice");
_LIT(KPassSip0,		"");
_LIT(KHostSip0,		"atlanta.com");
_LIT(KUserInfoSip0,		"alice");
_LIT(KPortSip0,		"");

_LIT(KCompsSip1,		"siP:alice:SecretwoRd@atlaNta.com;transPort=tcp");
_LIT(KSchemeSip1,		"siP");
_LIT(KPathSip1,		";transPort=tcp");
_LIT(KQuerySip1,		"");
_LIT(KFragmentSip1,	"");
_LIT(KUserSip1,		"alice");
_LIT(KPassSip1,		"SecretwoRd");
_LIT(KHostSip1,		"atlaNta.com");
_LIT(KUserInfoSip1,		"alice:SecretwoRd");
_LIT(KPortSip1,		"");

_LIT(KCompsSip2,		"sipS:atlanta.coM;method=REGISTER?to=alice%40atlanta.com");
_LIT(KSchemeSip2,		"sipS");
_LIT(KPathSip2,		";method=REGISTER");
_LIT(KQuerySip2,		"to=alice%40atlanta.com");
_LIT(KFragmentSip2,	"");
_LIT(KUserSip2,		"");
_LIT(KPassSip2,		"");
_LIT(KHostSip2,		"atlanta.coM");
_LIT(KUserInfoSip2,		"");
_LIT(KPortSip2,		"");

_LIT(KCompsSip3,		"sipS:alice:Wor%64@atlanta.coM?subject=project%20x&priority=urgent");
_LIT(KSchemeSip3,		"sipS");
_LIT(KPathSip3,		"");
_LIT(KQuerySip3,		"subject=project%20x&priority=urgent");
_LIT(KFragmentSip3,	"");
_LIT(KUserSip3,		"alice");
_LIT(KPassSip3,		"");
_LIT(KHostSip3,		"atlanta.coM");
_LIT(KUserInfoSip3,		"alice:Wor%64");
_LIT(KPortSip3,		"");

_LIT(KCompsSip4,		"sIp:boB@192.168.2.0:6000;transport=tcp;method=REGISTER?to=sip:bob%40biloxi.com");
_LIT(KSchemeSip4,		"sIp");
_LIT(KPathSip4,		";transport=tcp;method=REGISTER");
_LIT(KQuerySip4,		"to=sip:bob%40biloxi.com");
_LIT(KFragmentSip4,	"");
_LIT(KUserSip4,		"boB");
_LIT(KPassSip4,		"");
_LIT(KHostSip4,		"192.168.2.0");
_LIT(KUserInfoSip4,		"boB");
_LIT(KPortSip4,		"6000");

//The path Parameter begins with , foot note 1 in PREQ748 Analysis.doc
_LIT(KCompsSip5,		"sIp:bobA:pas%73@192.168.2.100:2000;transport=tcp;method=REGISTER?to=sip:bob%40biloxi.com");
_LIT(KSchemeSip5,		"sIp");
_LIT(KPathSip5,		";transport=tcp;method=REGISTER");
_LIT(KQuerySip5,		"to=sip:bob%40biloxi.com");
_LIT(KFragmentSip5,	"");
_LIT(KUserSip5,		"bobA");
_LIT(KPassSip5,		"pas%73");
_LIT(KHostSip5,		"192.168.2.100");
_LIT(KUserInfoSip5,		"bobA:pas%73");
_LIT(KPortSip5,		"2000");

// IPv6 test cases for SIP URIs
_LIT(KSipUri_IPv6Fragment0,	"sip:userinfo@[::FfFf:129.144.52.38]:port;transport=tcp?query#fragment");	// with fragment
_LIT(KSipUri_IPv6Query0,	"sip:userinfo@[::FfFf:129.144.52.38]:port;transport=tcp?query");			// with query
_LIT(KSipUri_IPv6Path0,		"sip:userinfo@[::FfFf:129.144.52.38]:port;transport=tcp");					// with path
_LIT(KSipUri_IPv6Port0,		"sip:userinfo@[::FfFf:129.144.52.38]:port");							// with port
_LIT(KSipUri_IPv6UserInfo0,	"sip:userinfo@[::FfFf:129.144.52.38]");								// with userinfo
_LIT(KSipUri_IPv6Host0,		"sip:[::FfFf:129.144.52.38]");										// with scheme

// Sip URI construction test
_LIT(KSipUri_Fragment0,		"sip:userinfo@host:port;transport=tcp?query#fragment");	//
_LIT(KSipUri_Query0,		"sip:userinfo@host:port;transport=tcp?query");		
_LIT(KSipUri_Path0,		"sip:userinfo@host:port;transport=tcp");		
_LIT(KSipUri_Port0,			"sip:userinfo@host:port");					
_LIT(KSipUri_Userinfo0,		"sip:userinfo@host");							
_LIT(KSipUri_Host0,		"sip:host");							
_LIT(KSipUri_Scheme0,		"sip:");										

_LIT(KSipUri_SchemeComp0,		"sip");
_LIT(KSipUri_HostComp0,		"host");
_LIT(KSipUri_UserinfoComp0,		"userinfo");
_LIT(KSipUri_PortComp0,		"port");
_LIT(KSipUri_PathComp0,		";transport=tcp");
_LIT(KSipUri_QueryComp0,		"query");
_LIT(KSipUri_FragmentComp0,	"fragment");//

// Sip URI destruction test
_LIT(KSipUri_Whole0,		"sip:userinfo@host:port;transport=tcp?query#fragment");	// Whole uri
_LIT(KSipUri_NoFragment0,	"sip:userinfo@host:port;transport=tcp?query");			// Without fragment
_LIT(KSipUri_NoQuery0,		"sip:userinfo@host:port;transport=tcp");				// Without query
_LIT(KSipUri_NoPath0,		"sip:userinfo@host:port");							// Without path
_LIT(KSipUri_NoPort0,		"sip:userinfo@host");							// Without port
_LIT(KSipUri_NoUserinfo0,	"sip:host");									// Without userinfo
_LIT(KSipUri_NoHost0,		"sip:");										// Without host
_LIT(KSipUri_NoScheme0,	"");											// Without scheme

// IPv6 test cases for SIP URIs
_LIT(KSipUri_IPv6Whole0,		"sip:userinfo@[::FfFf:129.144.52.38]:port;transport=tcp?query#fragment");	// Whole uri
_LIT(KSipUri_IPv6NoFragment0,	"sip:userinfo@[::FfFf:129.144.52.38]:port;transport=tcp?query");			// Without fragment
_LIT(KSipUri_IPv6NoQuery0,		"sip:userinfo@[::FfFf:129.144.52.38]:port;transport=tcp");					// Without query
_LIT(KSipUri_IPv6NoPath0,		"sip:userinfo@[::FfFf:129.144.52.38]:port");							// Without path
_LIT(KSipUri_IPv6NoPort0,		"sip:userinfo@[::FfFf:129.144.52.38]");								// Without port
_LIT(KSipUri_IPv6NoUserinfo0,	"sip:[::FfFf:129.144.52.38]");										// Without userinfo
_LIT(KSipUri_IPv6NoHost0,		"sip:");														// Without host
_LIT(KSipUri_IPv6NoScheme0,	"");															// Without scheme

_LIT(KUri_IPv6HostComp,	"::FfFf:129.144.52.38");

//
//
//	SIP Uri Valiadtion and Equivalence
//
//

const TInt KErrBadSipUriA=(-5000);
const TInt KErrBadSipUriB=(-6000);

_LIT(KCompsSipUriFileName, "Z:\\T_UriParser\\T_UriParserSipUri.txt");
_LIT(KCompsSipUriComment, "//");
_LIT(KCompsSipUriValidate, "Validate");
_LIT(KCompsSipUriEquivalence, "Equivalence");
_LIT(KCompsSipUriValidateStart, "Starting Validating of SIP URIs");
_LIT(KCompsSipUriFileSystemError, "***** ERROR IN FILE SYSTEM.  T_UriParserSipUri.txt NOT FOUND*****");
_LIT(KCompsSipUriNoValidate, "***** NO VALIDATE SECTION IN FILE *****");
_LIT(KCompsSipUriNoValidateData, "***** NO VALIDATION DATA IN FILE *****");
_LIT(KCompsSipUriNoEquivalence, "***** NO EQUIVALENCE SECTION IN FILE *****");
_LIT(KCompsSipUriNoEquivalenceData, "***** NO EQUIVALENCE DATA IN FILE *****");

//Valid SIP URIs that we use in code to compare the errors with
_LIT(KCompsSipUriValid0, "sip:user@host:5061;lr");
_LIT(KCompsSipUriValid1, "SIPS:bob@Symbian.com?to=");

//Invalid SIP URIs that we use in code to compare the errors with
_LIT(KCompsSipUriInvalid0, "Sim:10.0.1.10:port:%61lice:pass;transport=tcp&subject=wor%6B");
_LIT(KCompsSipUriInvalid1, "sIps:atalanta.com:8830/04-1.html");
_LIT(KCompsSipUriInvalid2, "SIPS:bob@Symbian.com?to=1123&to=678");
_LIT(KCompsSipUriInvalid3, "SiP:%89ob999:-~_*.!pass@:;transport=udp?subject=Proj&subject=Test!**");
_LIT(KCompsSipUriInvalid4, "siP:userinfo@[::FfFf:129.144.52.38]:port/a/b/c?query");
_LIT(KCompsSipUriInvalid5, "sips:userinfo@192.168.2.1;geo.position:=33.51_-119.94_72&subject=Symbi%71n");
_LIT(KCompsSipUriInvalid6, "sip:user@host:5061;lr=");
_LIT(KCompsSipUriInvalid7, "sip:@atlanta.com");

//Should be equal
_LIT(KCompsSipUriEqual0, "sip:alice:SecretWord@192.168.0.1:80;transport=tcp?to=alice&subject=project&content-type=text&call-id=100&contact=bob&from=fred");
_LIT(KCompsSipUriEqual1, "sipS:Bob+11111;ext=22@[::FfFf:129.144.52.38];%74ransport=//(:t.c*%70);method=SET?c=tests&from=Bob");
_LIT(KCompsSipUriEqual2, "sipS:+358-555-1234567;postd=carol:**PASS**@SymBian.com");

//Should NOT be equal
_LIT(KCompsSipUriNotEqual0, "sip:alice:Word@192.168.0.1:80;transport=tcp?to=alice& subject=project&content-type=text&call-id=100&contact=bob&from=fred");
_LIT(KCompsSipUriNotEqual1, "sip:alice@atlanta.com;maddr=239.255.255.1;ttl=15");
_LIT(KCompsSipUriNotEqual2, "sip:alice:SecretWord@192.168.0.5:80;transport=tcp?to=alice&subject=project&content-type=text&call-id=100&contact=bob&from=fred");

_LIT(KCompsSipUriValidMess0, "Correct Error should have been:- KErrNone");

_LIT(KCompsSipUriInvalidMess0, "Correct Error should have been:- Not Supported");
_LIT(KCompsSipUriInvalidMess1, "Correct Error should have been:- Invalid Parameter");
_LIT(KCompsSipUriInvalidMess2, "Correct Error should have been:- Invalid Host");
_LIT(KCompsSipUriInvalidMess3, "Correct Error should have been:- Invalid Port");
_LIT(KCompsSipUriInvalidMess4, "Correct Error should have been:- Not Supported");
_LIT(KCompsSipUriInvalidMess5, "***** INCORRECT  ERROR RETURNED FROM CURI *****");
_LIT(KCompsSipUriInvalidMess6, "Invalid Path");
_LIT(KCompsSipUriInvalidMess7, "Correct Error should have been:- Invalid UserInfo");
_LIT(KCompsSipUriInvalidMess8, "Correct Error should have been:- Invalid Headers");
_LIT(KCompsSipUriInvalidMess9, "Correct Error should have been:- Invalid Path");

_LIT(KCompsSipUriEqualMess0, "The SIP URIs are EQUAL ");
_LIT(KCompsSipUriEqualMess1, "The SIP URIs are NOT EQUAL ");

#endif	// __SIPURIANDAUTHORITYTESTS_H__
