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

#ifndef __URIANDAUTHORITYTESTS_H__
#define __URIANDAUTHORITYTESTS_H__

#include <e32base.h>

#include "CUriAndAuthorityTest.h"

//
//
//	Uri Resolving test data
//
//

_LIT(KBaseFile, "file://c|/data.wml");
_LIT(KRelativeFile1, "#card2");	_LIT(KResolvedFile1, "file://c|/data.wml#card2");

_LIT(KBase, "http://a/b/c/d;p?q");
_LIT(KRelative1, "g:h");		_LIT(KResolved1, "g:h");
_LIT(KRelative2, "g");			_LIT(KResolved2, "http://a/b/c/g");
_LIT(KRelative3, "./g");		_LIT(KResolved3, "http://a/b/c/g");
_LIT(KRelative4, "g/");			_LIT(KResolved4, "http://a/b/c/g/");
_LIT(KRelative5, "/g");			_LIT(KResolved5, "http://a/g");
_LIT(KRelative6, "//g");		_LIT(KResolved6, "http://g");
_LIT(KRelative7, "?y");			_LIT(KResolved7, "http://a/b/c/d;p?y");
_LIT(KRelative8, "g?y");		_LIT(KResolved8, "http://a/b/c/g?y");
_LIT(KRelative9, "#s");			_LIT(KResolved9, "http://a/b/c/d;p?q#s");
_LIT(KRelative10, "g#s");		_LIT(KResolved10, "http://a/b/c/g#s");
_LIT(KRelative11, "g?y#s");		_LIT(KResolved11, "http://a/b/c/g?y#s");
_LIT(KRelative12, ";x");		_LIT(KResolved12, "http://a/b/c/;x");
_LIT(KRelative13, "g;x");		_LIT(KResolved13, "http://a/b/c/g;x");
_LIT(KRelative14, "g;x?y#s");	_LIT(KResolved14, "http://a/b/c/g;x?y#s");
_LIT(KRelative15, ".");			_LIT(KResolved15, "http://a/b/c/");
_LIT(KRelative16, "./");		_LIT(KResolved16, "http://a/b/c/");
_LIT(KRelative17, "..");		_LIT(KResolved17, "http://a/b/");
_LIT(KRelative18, "../");		_LIT(KResolved18, "http://a/b/");
_LIT(KRelative19, "../g");		_LIT(KResolved19, "http://a/b/g");
_LIT(KRelative20, "../..");		_LIT(KResolved20, "http://a/");
_LIT(KRelative21, "../../");	_LIT(KResolved21, "http://a/");
_LIT(KRelative22, "../../g");	_LIT(KResolved22, "http://a/g");

// abnormal examples

_LIT(KRelative23, "../../../g");	_LIT(KResolved23, "http://a/../g");		//The Resolved Uri as per RFC3986 is http://a/g
_LIT(KRelative24, "../../../../g");	_LIT(KResolved24, "http://a/../../g");	//The Resolved Uri as per RFC3986 is http://a/g
_LIT(KRelative25, "/./g");	_LIT(KResolved25, "http://a/./g");				//The Resolved Uri as per RFC3986 is http://a/g
_LIT(KRelative26, "/../g");	_LIT(KResolved26, "http://a/../g");				//The Resolved Uri as per RFC3986 is http://a/g
_LIT(KRelative27, "g.");	_LIT(KResolved27, "http://a/b/c/g.");
_LIT(KRelative28, ".g");	_LIT(KResolved28, "http://a/b/c/.g");
_LIT(KRelative29, "g..");	_LIT(KResolved29, "http://a/b/c/g..");
_LIT(KRelative30, "..g");	_LIT(KResolved30, "http://a/b/c/..g");

// non sensical uses of '.' and '..'
_LIT(KRelative31, "./../g");	_LIT(KResolved31, "http://a/b/g");
_LIT(KRelative32, "./g/.");	_LIT(KResolved32, "http://a/b/c/g/");
_LIT(KRelative33, "g/./h");	_LIT(KResolved33, "http://a/b/c/g/h");
_LIT(KRelative34, "g/../h");	_LIT(KResolved34, "http://a/b/c/h");
_LIT(KRelative35, "g;x=1/./y");	_LIT(KResolved35, "http://a/b/c/g;x=1/y");
_LIT(KRelative36, "g;x=1/../y");	_LIT(KResolved36, "http://a/b/c/y");

// use of query and fragment
_LIT(KRelative37, "g?y/./x");	_LIT(KResolved37, "http://a/b/c/g?y/./x");
_LIT(KRelative38, "g?y/../x");	_LIT(KResolved38, "http://a/b/c/g?y/../x");
_LIT(KRelative39, "g#s/./x");	_LIT(KResolved39, "http://a/b/c/g#s/./x");
_LIT(KRelative40, "g#s/../x");	_LIT(KResolved40, "http://a/b/c/g#s/../x");

// backwards compatiable , do relative parsing if schemes match
_LIT(KRelative41, "http:g");	_LIT(KResolved41, "http://a/b/c/g");

// possible bug if relative url is a network path with a path aswell!
_LIT(KRelative42, "//g/h/i");	_LIT(KResolved42, "http://g/h/i");

//	ResolveL defect EDNPHAD-4J6EUY
_LIT(KBaseBug1, "http://wap.h2g2.com#welcome");
_LIT(KRelativeBug1, "funcs.wml#setup(\")");	_LIT(KResolvedBug1, "http://wap.h2g2.com/funcs.wml#setup(\")");

_LIT(KBaseBug2, "http://wap.h2g2.com/welcome");
_LIT(KRelativeBug2, "funcs.wml#setup(\")");	_LIT(KResolvedBug2, "http://wap.h2g2.com/funcs.wml#setup(\")");
_LIT(KRelativeBug3, "home/funcs.wml#setup(\")");	_LIT(KResolvedBug3, "http://wap.h2g2.com/home/funcs.wml#setup(\")");

// ResolveL defect EDNFUDN-4JYFH7
_LIT(KBaseBug3, "http://mobile.iii.co.uk");
_LIT(KRelativeBug4, "i/iii_splash.wbm");	_LIT(KResolvedBug4, "http://mobile.iii.co.uk/i/iii_splash.wbm");

// Relative fragment defect EDNEHHO-4M5BU6 - wml/state/variables/substitution/4 -test doesn't work
_LIT(KFragmentBase, "http://trras02.it.cellulardata.com/vswap114/htroot/wap/wml/state/variables/substitution/4/?v=dozig&ts=0003&ti=3");
_LIT(KFragmentRelative, "#card1");
_LIT(KFragmentResolved, "http://trras02.it.cellulardata.com/vswap114/htroot/wap/wml/state/variables/substitution/4/?v=dozig&ts=0003&ti=3#card1");

//
//
//	Uri and Authority component extraction test data
//
//

_LIT(KComps0,		"http://user:pass@location.com:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KScheme0,		"http");
_LIT(KAuthority0,	"user:pass@location.com:1666");
_LIT(KPath0,		"/abcd/ada/resource.cgi;x=123;df223;2342;2323");
_LIT(KQuery0,		"asd=b");
_LIT(KFragment0,	"part");
_LIT(KUser0,		"user");
_LIT(KPass0,		"pass");
_LIT(KHost0,		"location.com");
_LIT(KUserInfo0,	"user:pass");
_LIT(KPort0,		"1666");

_LIT(KComps1,		"http://user:pass@location.com:/abcd/ada/resource.cgi?asd=b");
_LIT(KScheme1,		"http");
_LIT(KAuthority1,	"user:pass@location.com:");
_LIT(KPath1,		"/abcd/ada/resource.cgi");
_LIT(KQuery1,		"asd=b");
_LIT(KFragment1,	"");
_LIT(KUser1,		"user");
_LIT(KPass1,		"pass");
_LIT(KHost1,		"location.com");
_LIT(KUserInfo1,	"user:pass");
_LIT(KPort1,		"");

_LIT(KComps2,		"http://user:pass@location.com:1666/abcd/ada/resource.cgi");
_LIT(KScheme2,		"http");
_LIT(KAuthority2,	"user:pass@location.com:1666");
_LIT(KPath2,		"/abcd/ada/resource.cgi");
_LIT(KQuery2,		"");
_LIT(KFragment2,	"");
_LIT(KUser2,		"user");
_LIT(KPass2,		"pass");
_LIT(KHost2,		"location.com");
_LIT(KUserInfo2,	"user:pass");
_LIT(KPort2,		"1666");

_LIT(KComps3,		"http://user:pass@location.com/abcd/ada/");
_LIT(KScheme3,		"http");
_LIT(KAuthority3,	"user:pass@location.com");
_LIT(KPath3,		"/abcd/ada/");
_LIT(KQuery3,		"");
_LIT(KFragment3,	"");
_LIT(KUser3,		"user");
_LIT(KPass3,		"pass");
_LIT(KHost3,		"location.com");
_LIT(KUserInfo3,	"user:pass");
_LIT(KPort3,		"");

_LIT(KComps4,		"http://user:pass@location.com");
_LIT(KScheme4,		"http");
_LIT(KAuthority4,	"user:pass@location.com");
_LIT(KPath4,		"");
_LIT(KQuery4,		"");
_LIT(KFragment4,	"");
_LIT(KUser4,		"user");
_LIT(KPass4,		"pass");
_LIT(KHost4,		"location.com");
_LIT(KUserInfo4,	"user:pass");
_LIT(KPort4,		"");

_LIT(KComps5,		"http://");
_LIT(KScheme5,		"http");
_LIT(KAuthority5,	"");
_LIT(KPath5,		"");
_LIT(KQuery5,		"");
_LIT(KFragment5,	"");
_LIT(KUser5,		"");
_LIT(KPass5,		"");
_LIT(KHost5,		"");
_LIT(KUserInfo5,	"");
_LIT(KPort5,		"");

_LIT(KComps6,		"");
_LIT(KScheme6,		"");
_LIT(KAuthority6,	"");
_LIT(KPath6,		"");
_LIT(KQuery6,		"");
_LIT(KFragment6,	"");
_LIT(KUser6,		"");
_LIT(KPass6,		"");
_LIT(KHost6,		"");
_LIT(KUserInfo6,	"");
_LIT(KPort6,		"");

_LIT(KComps10,		"http://user:pass@location.com/abcd/ada/resource.cgi?asd=b#pa://rt");
_LIT(KScheme10,		"http");
_LIT(KAuthority10,	"user:pass@location.com");
_LIT(KPath10,		"/abcd/ada/resource.cgi");
_LIT(KQuery10,		"asd=b");
_LIT(KFragment10,	"pa://rt");
_LIT(KUser10,		"user");
_LIT(KPass10,		"pass");
_LIT(KHost10,		"location.com");
_LIT(KUserInfo10,	"user:pass");
_LIT(KPort10,		"");

_LIT(KComps11,		"http://user:pass@location.com/abcd/ada/resource.cgi?as/d=b#pa://rt");
_LIT(KScheme11,		"http");
_LIT(KAuthority11,	"user:pass@location.com");
_LIT(KPath11,		"/abcd/ada/resource.cgi");
_LIT(KQuery11,		"as/d=b");
_LIT(KFragment11,	"pa://rt");
_LIT(KUser11,		"user");
_LIT(KPass11,		"pass");
_LIT(KHost11,		"location.com");
_LIT(KUserInfo11,	"user:pass");
_LIT(KPort11,		"");

_LIT(KComps12,		"http://user:pass@location.com/abcd/ada/reso://urce.cgi?asd=b#pa://rt");
_LIT(KScheme12,		"http");
_LIT(KAuthority12,	"user:pass@location.com");
_LIT(KPath12,		"/abcd/ada/reso://urce.cgi");
_LIT(KQuery12,		"asd=b");
_LIT(KFragment12,	"pa://rt");
_LIT(KUser12,		"user");
_LIT(KPass12,		"pass");
_LIT(KHost12,		"location.com");
_LIT(KUserInfo12,	"user:pass");
_LIT(KPort12,		"");

_LIT(KComps13,		"http://user:pass@location.com/abc://d/ada/resource.cgi?asd=b#pa://rt");
_LIT(KScheme13,		"http");
_LIT(KAuthority13,	"user:pass@location.com");
_LIT(KPath13,		"/abc://d/ada/resource.cgi");
_LIT(KQuery13,		"asd=b");
_LIT(KFragment13,	"pa://rt");
_LIT(KUser13,		"user");
_LIT(KPass13,		"pass");
_LIT(KHost13,		"location.com");
_LIT(KUserInfo13,	"user:pass");
_LIT(KPort13,		"");

_LIT(KComps14,		"http://user:pass@loc://ation.com/abcd/ada/resource.cgi?asd=b#pa://rt");
_LIT(KScheme14,		"http");
_LIT(KAuthority14,	"user:pass@loc:");
_LIT(KPath14,		"//ation.com/abcd/ada/resource.cgi");
_LIT(KQuery14,		"asd=b");
_LIT(KFragment14,	"pa://rt");
_LIT(KUser14,		"user");
_LIT(KPass14,		"pass");
_LIT(KHost14,		"loc");
_LIT(KUserInfo14,	"user:pass");
_LIT(KPort14,		"");

_LIT(KComps15,		"ht~tp://user:pass@location.com/abcd/ada/resource.cgi?asd=b#pa://rt");
_LIT(KScheme15,		"ht~tp");
_LIT(KAuthority15,	"user:pass@location.com");
_LIT(KPath15,		"/abcd/ada/resource.cgi");
_LIT(KQuery15,		"asd=b");
_LIT(KFragment15,	"pa://rt");
_LIT(KUser15,		"user");
_LIT(KPass15,		"pass");
_LIT(KHost15,		"location.com");
_LIT(KUserInfo15,	"user:pass");
_LIT(KPort15,		"");

_LIT(KComps16,		"user:pass@location.com/abcd/ada/resource.cgi?asd=b#pa://rt");
_LIT(KScheme16,		"user");
_LIT(KAuthority16,	"");
_LIT(KPath16,		"pass@location.com/abcd/ada/resource.cgi");
_LIT(KQuery16,		"asd=b");
_LIT(KFragment16,	"pa://rt");
_LIT(KUser16,		"");
_LIT(KPass16,		"");
_LIT(KHost16,		"");
_LIT(KUserInfo16,	"");
_LIT(KPort16,		"");

_LIT(KComps17,		"/abcd/ada/resource.cgi?asd=b#pa://rt");
_LIT(KScheme17,		"");
_LIT(KAuthority17,	"");
_LIT(KPath17,		"/abcd/ada/resource.cgi");
_LIT(KQuery17,		"asd=b");
_LIT(KFragment17,	"pa://rt");
_LIT(KUser17,		"");
_LIT(KPass17,		"");
_LIT(KHost17,		"");
_LIT(KUserInfo17,	"");
_LIT(KPort17,		"");

_LIT(KComps18,		"resource.cgi?asd=b#pa://rt");
_LIT(KScheme18,		"");
_LIT(KAuthority18,	"");
_LIT(KPath18,		"resource.cgi");
_LIT(KQuery18,		"asd=b");
_LIT(KFragment18,	"pa://rt");
_LIT(KUser18,		"");
_LIT(KPass18,		"");
_LIT(KHost18,		"");
_LIT(KUserInfo18,	"");
_LIT(KPort18,		"");

_LIT(KComps19,		"?asd=b#pa://rt");
_LIT(KScheme19,		"");
_LIT(KAuthority19,	"");
_LIT(KPath19,		"");
_LIT(KQuery19,		"asd=b");
_LIT(KFragment19,	"pa://rt");
_LIT(KUser19,		"");
_LIT(KPass19,		"");
_LIT(KHost19,		"");
_LIT(KUserInfo19,	"");
_LIT(KPort19,		"");

_LIT(KComps20,		"#pa://rt");
_LIT(KScheme20,		"");
_LIT(KAuthority20,	"");
_LIT(KPath20,		"");
_LIT(KQuery20,		"");
_LIT(KFragment20,	"pa://rt");
_LIT(KUser20,		"");
_LIT(KPass20,		"");
_LIT(KHost20,		"");
_LIT(KUserInfo20,	"");
_LIT(KPort20,		"");

_LIT(KComps21,		"resource.cgi");
_LIT(KScheme21,		"");
_LIT(KAuthority21,	"");
_LIT(KPath21,		"resource.cgi");
_LIT(KQuery21,		"");
_LIT(KFragment21,	"");
_LIT(KUser21,		"");
_LIT(KPass21,		"");
_LIT(KHost21,		"");
_LIT(KUserInfo21,	"");
_LIT(KPort21,		"");

_LIT(KComps22,		"?q#f");
_LIT(KScheme22,		"");
_LIT(KAuthority22,	"");
_LIT(KPath22,		"");
_LIT(KQuery22,		"q");
_LIT(KFragment22,	"f");
_LIT(KUser22,		"");
_LIT(KPass22,		"");
_LIT(KHost22,		"");
_LIT(KUserInfo22,	"");
_LIT(KPort22,		"");

_LIT(KComps23,		"?q");
_LIT(KScheme23,		"");
_LIT(KAuthority23,	"");
_LIT(KPath23,		"");
_LIT(KQuery23,		"q");
_LIT(KFragment23,	"");
_LIT(KUser23,		"");
_LIT(KPass23,		"");
_LIT(KHost23,		"");
_LIT(KUserInfo23,	"");
_LIT(KPort23,		"");

_LIT(KComps24,		"#f");
_LIT(KScheme24,		"");
_LIT(KAuthority24,	"");
_LIT(KPath24,		"");
_LIT(KQuery24,		"");
_LIT(KFragment24,	"f");
_LIT(KUser24,		"");
_LIT(KPass24,		"");
_LIT(KHost24,		"");
_LIT(KUserInfo24,	"");
_LIT(KPort24,		"");

_LIT(KComps25,		"http://location.com/abcd/ada/resource.cgi?asd=b#pa://rt");
_LIT(KScheme25,		"http");
_LIT(KAuthority25,	"location.com");
_LIT(KPath25,		"/abcd/ada/resource.cgi");
_LIT(KQuery25,		"asd=b");
_LIT(KFragment25,	"pa://rt");
_LIT(KUser25,		"");
_LIT(KPass25,		"");
_LIT(KHost25,		"location.com");
_LIT(KUserInfo25,	"");
_LIT(KPort25,		"");

_LIT(KComps26,		"http://user@location.com/abcd/ada/resource.cgi?asd=b#pa://rt");
_LIT(KScheme26,		"http");
_LIT(KAuthority26,	"user@location.com");
_LIT(KPath26,		"/abcd/ada/resource.cgi");
_LIT(KQuery26,		"asd=b");
_LIT(KFragment26,	"pa://rt");
_LIT(KUser26,		"user");
_LIT(KPass26,		"");
_LIT(KHost26,		"location.com");
_LIT(KUserInfo26,	"user");
_LIT(KPort26,		"");

_LIT(KComps27,		"http://wap.opengroup.org/");
_LIT(KScheme27,		"http");
_LIT(KAuthority27,	"wap.opengroup.org");
_LIT(KPath27,		"/");
_LIT(KQuery27,		"");
_LIT(KFragment27,	"");
_LIT(KUser27,		"");
_LIT(KPass27,		"");
_LIT(KHost27,		"wap.opengroup.org");
_LIT(KUserInfo27,	"");
_LIT(KPort27,		"");

_LIT(KComps28,		"http://wap.opengroup.org/#index.wml");
_LIT(KScheme28,		"http");
_LIT(KAuthority28,	"wap.opengroup.org");
_LIT(KPath28,		"/");
_LIT(KQuery28,		"");
_LIT(KFragment28,	"index.wml");
_LIT(KUser28,		"");
_LIT(KPass28,		"");
_LIT(KHost28,		"wap.opengroup.org");
_LIT(KUserInfo28,	"");
_LIT(KPort28,		"");

_LIT(KComps29,		"http://wap.opengroup.org/?mystuff");
_LIT(KScheme29,		"http");
_LIT(KAuthority29,	"wap.opengroup.org");
_LIT(KPath29,		"/");
_LIT(KQuery29,		"mystuff");
_LIT(KFragment29,	"");
_LIT(KUser29,		"");
_LIT(KPass29,		"");
_LIT(KHost29,		"wap.opengroup.org");
_LIT(KUserInfo29,	"");
_LIT(KPort29,		"");


// IPv6 test cases

_LIT(KComps30,		"http://user:pass@[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KScheme30,		"http");
_LIT(KAuthority30,	"user:pass@[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:1666");
_LIT(KPath30,		"/abcd/ada/resource.cgi;x=123;df223;2342;2323");
_LIT(KQuery30,		"asd=b");
_LIT(KFragment30,	"part");
_LIT(KUser30,		"user");
_LIT(KPass30,		"pass");
_LIT(KHost30,		"FEDC:BA98:7654:3210:FEDC:BA98:7654:3210");
_LIT(KUserInfo30,	"user:pass");
_LIT(KPort30,		"1666");

_LIT(KComps31,		"http://user:pass@[1080:0:0:0:8:800:200C:417A]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KScheme31,		"http");
_LIT(KAuthority31,	"user:pass@[1080:0:0:0:8:800:200C:417A]:1666");
_LIT(KPath31,		"/abcd/ada/resource.cgi;x=123;df223;2342;2323");
_LIT(KQuery31,		"asd=b");
_LIT(KFragment31,	"part");
_LIT(KUser31,		"user");
_LIT(KPass31,		"pass");
_LIT(KHost31,		"1080:0:0:0:8:800:200C:417A");
_LIT(KUserInfo31,	"user:pass");
_LIT(KPort31,		"1666");

_LIT(KComps32,		"http://user:pass@[3ffe:2a00:100:7031::1]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KScheme32,		"http");
_LIT(KAuthority32,	"user:pass@[3ffe:2a00:100:7031::1]:1666");
_LIT(KPath32,		"/abcd/ada/resource.cgi;x=123;df223;2342;2323");
_LIT(KQuery32,		"asd=b");
_LIT(KFragment32,	"part");
_LIT(KUser32,		"user");
_LIT(KPass32,		"pass");
_LIT(KHost32,		"3ffe:2a00:100:7031::1");
_LIT(KUserInfo32,	"user:pass");
_LIT(KPort32,		"1666");

_LIT(KComps33,		"http://user:pass@[1080::8:800:200C:417A]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KScheme33,		"http");
_LIT(KAuthority33,	"user:pass@[1080::8:800:200C:417A]:1666");
_LIT(KPath33,		"/abcd/ada/resource.cgi;x=123;df223;2342;2323");
_LIT(KQuery33,		"asd=b");
_LIT(KFragment33,	"part");
_LIT(KUser33,		"user");
_LIT(KPass33,		"pass");
_LIT(KHost33,		"1080::8:800:200C:417A");
_LIT(KUserInfo33,	"user:pass");
_LIT(KPort33,		"1666");

_LIT(KComps34,		"http://user:pass@[::192.9.5.5]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KScheme34,		"http");
_LIT(KAuthority34,	"user:pass@[::192.9.5.5]:1666");
_LIT(KPath34,		"/abcd/ada/resource.cgi;x=123;df223;2342;2323");
_LIT(KQuery34,		"asd=b");
_LIT(KFragment34,	"part");
_LIT(KUser34,		"user");
_LIT(KPass34,		"pass");
_LIT(KHost34,		"::192.9.5.5");
_LIT(KUserInfo34,	"user:pass");
_LIT(KPort34,		"1666"); 

_LIT(KComps35,		"http://user:pass@[::FfFf:129.144.52.38]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KScheme35,		"http");
_LIT(KAuthority35,	"user:pass@[::FfFf:129.144.52.38]:1666");
_LIT(KPath35,		"/abcd/ada/resource.cgi;x=123;df223;2342;2323");
_LIT(KQuery35,		"asd=b");
_LIT(KFragment35,	"part");
_LIT(KUser35,		"user");
_LIT(KPass35,		"pass");
_LIT(KHost35,		"::FfFf:129.144.52.38");
_LIT(KUserInfo35,	"user:pass");
_LIT(KPort35,		"1666");

_LIT(KComps36,		"http://user:pass@[2010:836B:4179::836B:4179]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KScheme36,		"http");
_LIT(KAuthority36,	"user:pass@[2010:836B:4179::836B:4179]:1666");
_LIT(KPath36,		"/abcd/ada/resource.cgi;x=123;df223;2342;2323");
_LIT(KQuery36,		"asd=b");
_LIT(KFragment36,	"part");
_LIT(KUser36,		"user");
_LIT(KPass36,		"pass");
_LIT(KHost36,		"2010:836B:4179::836B:4179");
_LIT(KUserInfo36,	"user:pass");
_LIT(KPort36,		"1666");


_LIT(KFileName1,	"http://www.symbian.com");							_LIT(KFileNameExpected1, "");
_LIT(KFileName2,	"http://www.symbian.com/");							_LIT(KFileNameExpected2, "");
_LIT(KFileName3,	"http://www.symbian.com/images/");					_LIT(KFileNameExpected3, "");
_LIT(KFileName4,	"http://www.symbian.com/images");					_LIT(KFileNameExpected4, "images");
_LIT(KFileName5,	"http://www.symbian.com/images/epoc/img1.jpg");		_LIT(KFileNameExpected5, "img1.jpg");
_LIT(KFileName6,	"http://www.symbian.com/file.txt");					_LIT(KFileNameExpected6, "file.txt");

//
//
//	Misc test data
//
//

//	TestCompare input data
//
//_LIT(KUrl0,"http://user:pass@location.com/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part"); // SLPRQG
_LIT(KUrl0,"http://user:pass@location.com/abcd/ada/resource.cgi?asd=b#part"); // SLPRQG
_LIT(KUrl1,"http://user:pass@location.com/abcd/ada/resource.cgi?asd=b"); // SLPRQ
_LIT(KUrl2,"http://user:pass@location.com/abcd/ada/resource.cgi"); // SLPR
_LIT(KUrl3,"http://user:pass@location.com/abcd/ada/"); // SLP
_LIT(KUrl4,"http://user:pass@location.com"); // SL
_LIT(KUrl5,"http://"); // S
_LIT(KUrl6,""); // 

_LIT(KUrl10,"http://user:pass@location.com/abcd/ada/resource.cgi?asd=b#pa://rt"); // SLPRQg
_LIT(KUrl11,"http://user:pass@location.com/abcd/ada/resource.cgi?as/d=b#pa://rt"); // SLPRqG
_LIT(KUrl12,"http://user:pass@location.com/abcd/ada/reso://urce.cgi?asd=b#pa://rt"); // SLPrQG
_LIT(KUrl13,"http://user:pass@location.com/abc://d/ada/resource.cgi?asd=b#pa://rt"); // SLpRQG
_LIT(KUrl14,"http://user:pass@loc://ation.com/abcd/ada/resource.cgi?asd=b#pa://rt"); // SlPRQG
_LIT(KUrl15,"ht~tp://user:pass@location.com/abcd/ada/resource.cgi?asd=b#pa://rt"); // sLPRQG

_LIT(KUrl16,"user:pass@location.com/abcd/ada/resource.cgi?asd=b#pa://rt"); // SLPRQg
_LIT(KUrl17,"/abcd/ada/resource.cgi?asd=b#pa://rt"); // SLPRQg
_LIT(KUrl18,"resource.cgi?asd=b#pa://rt"); // SLPRQg
_LIT(KUrl19,"?asd=b#pa://rt"); // SLPRQg
_LIT(KUrl20,"#pa://rt"); // SLPRQg
_LIT(KUrl21,"resource.cgi"); // SLPRQg

//	Construct with TParseBase test data
//
_LIT(KParse0, "c:\\system\\data.wml");	_LIT(KParseResult0, "file://c|/system/data.wml");
_LIT(KParse1, "c:\\wap files\\test.wml");	_LIT(KParseResult1, "file://c|/wap%20files/test.wml");	// Defect EDNTDAS-4N4K72

//	Unescape url creation test data
//
_LIT(KEscapedString0, "the%20%25%5E%20weather");				_LIT(KUnescapedString0, "the %^ weather");
_LIT(KEscapedString1, "%7Bcheck%3C%3Ethis%5B%5Dout%7D");		_LIT(KUnescapedString1, "{check<>this[]out}");
_LIT(KEscapedString2a, "Reserved;/?:@&=+$,");					_LIT(KUnescapedString2, "Reserved;/?:@&=+$,");
_LIT(KEscapedString2b, "Reserved%3B%2F%3F%3A%40%26%3D%2B%24%2C");


// Construction defect EDNFUDN-4K9MD8
// 
_LIT(KCtorUrl0, ":\\http://www.bbc.co.uk/mobile/mainmenu.wml");

_LIT(KRubbish, "rubbish");
_LIT(KSlashRubbish, "/rubbish");

// Leading whitespace defect EDNEPTA-4PBHSE - WAP:Go-element causes breakpoint if href-attribute has space in its beginning
_LIT(KLeadWhitespace1, " http://wap.iobox.com");	_LIT(KLeadWhitespaceUrl, "http://wap.iobox.com");
_LIT(KLeadWhitespace2, "        http://wap.iobox.com");

//
//
// Uri test data
//
//

_LIT(KUri_IPv6Fragment0,"http://userinfo@[::FfFf:129.144.52.38]:port/a/b/c?query#fragment");	// with fragment
_LIT(KUri_IPv6Query0,	"http://userinfo@[::FfFf:129.144.52.38]:port/a/b/c?query");			// with query
_LIT(KUri_IPv6Path0,	"http://userinfo@[::FfFf:129.144.52.38]:port/a/b/c");					// with path
_LIT(KUri_IPv6Port0,	"http://userinfo@[::FfFf:129.144.52.38]:port");						// with port
_LIT(KUri_IPv6UserInfo0,"http://userinfo@[::FfFf:129.144.52.38]");							// with userinfo
_LIT(KUri_IPv6Host0,	"http://[::FfFf:129.144.52.38]");										// with scheme





_LIT(KUri_IPv6HostComp0,	"::FfFf:129.144.52.38");

_LIT(KUri_Fragment0,	"http://userinfo@host:port/a/b/c?query#fragment");	// With fragment
_LIT(KUri_Query0,		"http://userinfo@host:port/a/b/c?query");			// With query
_LIT(KUri_Path0,		"http://userinfo@host:port/a/b/c");					// With path
_LIT(KUri_Port0,		"http://userinfo@host:port");						// With port
_LIT(KUri_Userinfo0,	"http://userinfo@host");							// With userinfo
_LIT(KUri_Host0,		"http://host");										// With host
_LIT(KUri_Scheme0,		"http:");											// With scheme

_LIT(KUri_SchemeComp0,		"http");
_LIT(KUri_UserinfoComp0,	"userinfo");
_LIT(KUri_HostComp0,		"host");
_LIT(KUri_PortComp0,		"port");
_LIT(KUri_PathComp0,		"/a/b/c");
_LIT(KUri_QueryComp0,		"query");
_LIT(KUri_FragmentComp0,	"fragment");

_LIT(KUri_Compare0_a, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave#card1");
_LIT(KUri_Compare0_b, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave#card1");
const TInt KUriMatchFlags0 = (ESchemeFlag | EUserinfoFlag | EHostFlag | EPortFlag | EPathFlag | EQueryFlag | EFragmentFlag);

_LIT(KUri_Compare1_a, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave#card1");
_LIT(KUri_Compare1_b, "HTTP://USER@Www.symbian.com:1666/wap/dev/different.wml?name=dave#card1");
const TInt KUriMatchFlags1 = (ESchemeFlag | EHostFlag | EPortFlag | EQueryFlag | EFragmentFlag);

_LIT(KUri_Compare2_a, "http://user:pass@[fEdC:Ba98:7654:3210:FeDC:BA98:7654:3210]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KUri_Compare2_b, "http://user:pass@[FEDC:BA98:7654:3210:fedc:BA98:7654:3210]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
const TInt KUriMatchFlags2 = (ESchemeFlag | EUserinfoFlag | EHostFlag | EPortFlag | EPathFlag | EQueryFlag | EFragmentFlag);

_LIT(KUri_Compare3_a, "http://Auser:pass@[::FfFf:129.144.52.38]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
_LIT(KUri_Compare3_b, "http://Buser:pass@[::FfFf:129.144.52.38]:1666/abcd/ada/resource.cgi;x=123;df223;2342;2323?asd=b#part");
const TInt KUriMatchFlags3 = (ESchemeFlag | EHostFlag | EPortFlag | EPathFlag | EQueryFlag | EFragmentFlag);



_LIT(KUri_SchemeValidation0, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave#card1");
TBool KUri_SchemeValidity0 = ETrue;

_LIT(KUri_SchemeValidation1, "ht~tp://user@www.symbian.com:1666/wap/dev/my.wml?name=dave#card1");
TBool KUri_SchemeValidity1 = EFalse;

_LIT(KUri_WithoutFragment0, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave#card1");
_LIT(KUri_WithoutFragment_Result0, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave");

_LIT(KUri_WithoutFragment1, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave");
_LIT(KUri_WithoutFragment_Result1, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave");

_LIT(KUri_Whole0,		"http://userinfo@host:port/a/b/c?query#fragment");	// Whole uri
_LIT(KUri_NoFragment0,	"http://userinfo@host:port/a/b/c?query");			// Without fragment
_LIT(KUri_NoQuery0,		"http://userinfo@host:port/a/b/c");					// Without query
_LIT(KUri_NoPath0,		"http://userinfo@host:port");						// Without path
_LIT(KUri_NoPort0,		"http://userinfo@host");							// Without port
_LIT(KUri_NoUserinfo0,	"http://host");										// Without userinfo
_LIT(KUri_NoHost0,		"http:");											// Without host
_LIT(KUri_NoScheme0,	"");												// Without scheme

_LIT(KUri_IPv6Whole0,		"http://userinfo@[::FfFf:129.144.52.38]:port/a/b/c?query#fragment");	// Whole uri
_LIT(KUri_IPv6NoFragment0,	"http://userinfo@[::FfFf:129.144.52.38]:port/a/b/c?query");				// Without fragment
_LIT(KUri_IPv6NoQuery0,		"http://userinfo@[::FfFf:129.144.52.38]:port/a/b/c");					// Without query
_LIT(KUri_IPv6NoPath0,		"http://userinfo@[::FfFf:129.144.52.38]:port");							// Without path
_LIT(KUri_IPv6NoPort0,		"http://userinfo@[::FfFf:129.144.52.38]");								// Without port
_LIT(KUri_IPv6NoUserinfo0,	"http://[::FfFf:129.144.52.38]");										// Without userinfo
_LIT(KUri_IPv6NoHost0,		"http:");																// Without host
_LIT(KUri_IPv6NoScheme0,	"");	

_LIT(KUri_Presence0, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave#card1");
const TInt KUriPresenceFlags0 = (ESchemeFlag | EUserinfoFlag | EHostFlag | EPortFlag | EPathFlag | EQueryFlag | EFragmentFlag);

_LIT(KUri_Presence1, "http://www.symbian.com:1666?name=dave");
const TInt KUriPresenceFlags1 = (ESchemeFlag | EHostFlag | EPortFlag | EPathFlag | EQueryFlag );

_LIT(KAuthority_Presence0, "user@www.symbian.com:1666");
const TInt KAuthorityPresenceFlags0 = (EUserinfoFlag | EHostFlag | EPortFlag );

_LIT(KAuthority_Presence1, "www.symbian.com");
const TInt KAuthorityPresenceFlags1 = (EHostFlag);

_LIT(KUri_Des0, "http://user@www.symbian.com:1666/wap/dev/my.wml?name=dave#card1");

//
//
// Authority test data
//
//

_LIT(KAuth_Compare0_a, "user@www.symbian.com:1666");
_LIT(KAuth_Compare0_b, "user@www.symbian.com:1666");
const TInt KAuthMatchFlags0 = (EUserinfoFlag | EHostFlag | EPortFlag);

_LIT(KAuth_Compare1_a, "user@www.symbian.com:1666");
_LIT(KAuth_Compare1_b, "USER@Www.symbian.com:1666");
const TInt KAuthMatchFlags1 = (EHostFlag | EPortFlag);

_LIT(KAuthority_Des0, "user@www.symbian.com:1666");

_LIT(KAuthority_Port0,		"userinfo@host:port");	// With port
_LIT(KAuthority_Userinfo0,	"userinfo@host");		// With userinfo
_LIT(KAuthority_Host0,		"host");				// With host

_LIT(KAuthority_UserinfoComp0,	"userinfo");
_LIT(KAuthority_HostComp0,		"host");
_LIT(KAuthority_PortComp0,		"port");

_LIT(KAuthorityEscaped_Port0,		"some%3Astuff@host%2Fname:port%3Fnum");	// With port
_LIT(KAuthorityEscaped_Userinfo0,	"some%3Astuff@host%2Fname");			// With userinfo
_LIT(KAuthorityEscaped_Host0,		"host%2Fname");							// With host

_LIT(KAuthorityEscaped_UserinfoComp0,	"some:stuff");
_LIT(KAuthorityEscaped_HostComp0,		"host/name");
_LIT(KAuthorityEscaped_PortComp0,		"port?num");

_LIT(KAuthority_Whole0,			"userinfo@host:port");	// Whole authority
_LIT(KAuthority_NoPort0,		"userinfo@host");		// Without port
_LIT(KAuthority_NoUserinfo0,	"host");				// Without userinfo
_LIT(KAuthority_NoHost0,		"");					// Without host

_LIT(KAuthority_IPv6Whole0,			"userinfo@[::FfFf:129.144.52.38]:port");	// Whole authority
_LIT(KAuthority_IPv6NoPort0,		"userinfo@[::FfFf:129.144.52.38]");			// Without port
_LIT(KAuthority_IPv6NoUserinfo0,	"[::FfFf:129.144.52.38]");					// Without userinfo
_LIT(KAuthority_IPv6NoHost0,		"");

_LIT(KAuthority_IPv6Port0,		"userinfo@[::FfFf:129.144.52.38]:port");	// With port
_LIT(KAuthority_IPv6Userinfo0,	"userinfo@[::FfFf:129.144.52.38]");			// With userinfo
_LIT(KAuthority_IPv6Host0,		"[::FfFf:129.144.52.38]");					// With host

_LIT(KAuthority_IPv6UserinfoComp0,	"userinfo");
_LIT(KAuthority_IPv6HostComp0,		"::FfFf:129.144.52.38");
_LIT(KAuthority_IPv6PortComp0,		"port");

#endif	// __URIANDAUTHORITYTESTS_H__
