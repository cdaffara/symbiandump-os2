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
// Unit test data for PREQ748 - Adding support for the SIP scheme in URIs.
// Plus adding two new methods validate and equivalent
// 
//

_LIT8(KUri0,		"sip:user:pass@location.com:1666;transport=tcp;x=123;df223;2342;2323?asd=b");
_LIT8(KScheme0,		"sip");
_LIT8(KUserInfo0,	"user:pass");
_LIT8(KHost0,		"location.com");
_LIT8(KPort0,		"1666");
_LIT8(KParams0,		";transport=tcp;x=123;df223;2342;2323");
_LIT8(KHeaders0,	"asd=b");

_LIT8(KVldUri0,	"sip:user:pass@location.com:1666;transport=tcp;x=123;df=223?asd=b");
_LIT8(KVldNotSupported,	"http://user:pass@location.com");
_LIT8(KVldInvalidScheme,	"s1p");
_LIT8(KVldInvalidHost0,		"sip:user@loc..com");
_LIT8(KVldInvalidHost1,		"sip:user@loc[.com");
_LIT8(KVldInvalidHost2,		"sip:user@loc~ation.com");
_LIT8(KVldInvalidHost3,		"sip:hos#t");
_LIT8(KVldInvalidHost4,		"sip:host#");
_LIT8(KVldInvalidHost5,     "SIP:@:[]"); 
_LIT8(KVldValidHost0,		"sip:user@location.com:1666");
_LIT8(KVldValidHost1,		"sip:user@loc-ation.com:1666");
_LIT8(KVldValidHost2,		"sip:user@loc123-at-ion.com:1666");
_LIT8(KVldInvalidPort0,		"sip:user@location.com:port");
_LIT8(KVldInvalidNoPort,	"sip:user@location.com:;transport=tcp");
_LIT8(KVldValidPort0,		"sip:user@location.com:1666");
_LIT8(KVldInvalidUserInfo0,	"sip:use]r@location.com:1666");
_LIT8(KVldInvalidUserInfo1,	"sip:user:p|wd@location.com:1666");
_LIT8(KVldInvalidUserInfo2,	"sip:@location.com:1666");
_LIT8(KVldInvalidUserInfo3,	"sip:user:@location.com:1666");
_LIT8(KVldValidUserInfo0,	"sip:user@location.com:1666");
_LIT8(KVldValidUserInfo1,	"sip:user:p.a's*s-_w~d!@location.com:1666");
_LIT8(KVldValidUserInfo2,	"sip:%61ser@location.com:1666");
_LIT8(KVldValidUserInfo3,	"sip:user:%61pwd@location.com:1666");
_LIT8(KVldInvalidParams0,	"sip:user@location.com:1666;transport=");
_LIT8(KVldInvalidParams1,	"sip:user@location.com:1666;=1234");
_LIT8(KVldInvalidParams2,	"sip:user@location.com:1666;c\transport=tcp");
_LIT8(KVldInvalidParams3,	"sip:host;#=value");
_LIT8(KVldInvalidParams4,	"sip:host;name=#");
_LIT8(KVldInvalidParams5,	"sip:host;name=tom;name=dick");
_LIT8(KVldValidParams0,		"sip:user@location.com:1666;transport=tcp");
_LIT8(KVldValidParams1,		"sip:user@location.com:1666;transport=tcp;thru-_.!~*'()%61[]/:&+$=42");
_LIT8(KVldValidParams2,		"sip:user@location.com:1666;parameter");
_LIT8(KVldInvalidHeaders0,	"sip:user@location.com:1666?header=c\b");
_LIT8(KVldInvalidHeaders1,	"sip:host?name=tom&name=dick");
_LIT8(KVldInvalidHeaders2,	"sip:host?name=tom&date");
_LIT8(KVldValidHeaders0,	"sip:user@location.com:1666?header=123");
_LIT8(KVldValidHeaders1,	"sip:user@location.com:1666?header=123&head-_.!~*'()%61[]/:?+$=42");
_LIT8(KVldValidHeaders2,	"sip:host?date=");
_LIT8(KVldValidHeaders3,	"SIPS:bob@Symbian.com?to=");

_LIT8(KVldInvalidGeneral1,	"sIps:atalanta.com:8830/04-1.html");
_LIT8(KVldInvalidGeneral2,	"sIps:atalanta..com:8830");
_LIT8(KVldValidGeneral1,	"sips:user;info@atalanta.com:8830;04-1.html");

_LIT8(KParseUri,           "http://@:[]"); 

_LIT8(KEquivUri0,			"sip:user:pass@location.com:1666;transport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivUri1,			"sip:user:pass@192.168.0.1:1666;transport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivScheme0,		"sIp:user:pass@location.com:1666;transport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivUserInfo0,		"sip:uSer:pass@location.com:1666;transport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivUserInfo1,		"sip:user@location.com:1666;transport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivHost0,			"sip:user:pass@locaTion.com:1666;transport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivHost1,			"sip:user:pass@192.168.00.01:1666;transport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivPort0,			"sip:user:pass@location.com:1888;transport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivParam0,			"sip:user:pass@location.com:1666;transport=tcp?asd=b");
_LIT8(KEquivParam1,			"sip:user:pass@location.com:1666;df=223;transport=tcp;x=123?asd=b");
_LIT8(KEquivParam2,			"sip:user:pass@location.com:1666;transport=udp?asd=b");
_LIT8(KEquivParam3,			"sip:user:pass@location.com:1666;transport=tcp;x=0;df=223");
_LIT8(KEquivParam4,			"sip:location.com;user=user;ttl=ttl;method=method;param;maddr=192.168.0.1");
_LIT8(KEquivParam5,			"sip:location.com;maddr=192.168.0.1;ttl=ttl;method=method;user=user");
_LIT8(KEquivParam6,			"sip:location.com;user=user;ttl=ttl;transport=tcp;method=method;maddr=192.168.0.1");
_LIT8(KEquivParam7,			"sip:location.com;user=user;ttl=ttl;method=method");
_LIT8(KEquivParam8,			"sip:location.com;user=u$er;ttl=ttl;method=method;maddr=192.168.0.1");
_LIT8(KEquivParam9,			"sip:location.com;user=user;ttl=42;method=method;maddr=192.168.0.1");
_LIT8(KEquivParam10,		"sip:location.com;user=user;ttl=ttl;maddr=192.168.000.001;method=method");
_LIT8(KEquivParam11,		"sip:location.com;user=user;ttl=ttl;maddr=192.168.000.002;method=method");
_LIT8(KEquivParam12,		"sip:location.com;user=user;ttl=ttl;maddr=location1.com;method=method");
_LIT8(KEquivParam13,		"sip:location.com;user=user;ttl=ttl;maddr=location01.com;method=method");
_LIT8(KEquivParam14,		"sip:location.com;user=user;ttl=ttl;method=method;addr=192.168.0.1");

_LIT8(KEquivHeader0,		"sip:location.com?asd=b");
_LIT8(KEquivHeader1,		"sip:location.com?asd=b");
_LIT8(KEquivHeader2,		"sip:location.com?asd=b&h2=2");
_LIT8(KEquivHeader3,		"sip:location.com?h2=2&asd=b");
_LIT8(KEquivHeader4,		"sip:location.com?Call-Id=99&Contact&Content-Encoding=true&Content-Length=256&Content-Type=text&From=user%40location.com&Subject=Test&To=you%40place.com");
_LIT8(KEquivHeader5,		"sip:location.com?i=99&m&e=true&l=256&c=text&f=user%40location.com&s=Test&t=you%40place.com");
_LIT8(KEquivHeader6,		"sip:location.com?m&e=true&i=99&C=text&f=user%40location.com&s=Test&t=you%40place.com&L=256");
_LIT8(KEquivHeader7,		"sip:location.com?m&e=true&i=99&C=text&s=Test&t=you%40place.com&L=256");

_LIT8(KEquivExpand0,		"sip:user:pass@loca%54ion.com:1666;%74rans%50or%74=%74cp;x=123;df%3d223?asd=b");
_LIT8(KEquivExpand1,		"sip:user:%70%61%73%73@location.com:1666;%74ransport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivExpand2,		"sip:user:%50%61%73%73@location.com:1666;%74ransport=tcp;x=123;df=223?asd=b");
_LIT8(KEquivExpand3,		"sipS:Bob+11111;ext=22@[::FfFf:129.144.52.38];%74ransport=//(:t.c*%70);method=SET?c=tests&from=Bob");
_LIT8(KEquivExpand4,		"sIpS:Bo%62+11111;ext=22@[::FfFf:129.144.52.38];method=SET;transport=//(:t.c*%70)?content-type=tests&f=B%6Fb");

_LIT8(KEquivGeneral1,		"sip:alice:SecretWord@192.168.0.5:80;transport=tcp?to=alice&subject=project&content-type=text&call-id=100&contact=bob&from=fred");
_LIT8(KEquivGeneral2,		"SiP:alice:SecretWord@192.168.000.005:90;transport=tcp?t=alice&s=project&c=test&i=200&m=%61llice&f=fred");
