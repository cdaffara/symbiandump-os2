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



#ifndef __URIUTILSTESTS_H__
#define __URIUTILSTESTS_H__

// System includes
//
#include <e32base.h>

//
//
// UriUtils test data
//
//

_LIT(KInternetForm0, "http://user@host:port/my%20dir/my%25file.wml?name=dave&surname=cunado#fragment");
_LIT(KDisplayForm0, "http://user@host:port/my dir/my%file.wml?name=dave&surname=cunado#fragment");
_LIT(KValidDisplayForm0, "http://user@host:port/my%20dir/my%25file.wml?name=dave&surname=cunado#fragment");

_LIT(KInternetForm1, "http://user@host:port/my%20dir/my%25file.wml?name=dave&surname=cunado");
_LIT(KDisplayForm1, "http://user@host:port/my dir/my%file.wml?name=dave&surname=cunado");
_LIT(KValidDisplayForm1, "http://user@host:port/my%20dir/my%25file.wml?name=dave&surname=cunado");

_LIT(KInvalidChars0, "this is bad");
_LIT(KInvalidChars1, "#thisisbad");
_LIT(KInvalidChars2, "{thisisbad");
_LIT(KInvalidChars3, "}thisisbad");
_LIT(KInvalidChars4, "|thisisbad");
_LIT(KInvalidChars5, "\\thisisbad");
_LIT(KInvalidChars6, "^thisisbad");
_LIT(KInvalidChars7, "`thisisbad");
_LIT(KInvalidChars8, "<thisisbad");
_LIT(KInvalidChars9, ">thisisbad");
_LIT(KInvalidChars10, "%thisisbad");
_LIT(KInvalidChars11, "\"thisisbad");
TBool KInvalid0 = ETrue;

_LIT(KInvalidChars12, "this%20is%20not");
_LIT(KInvalidChars13, "%23thisisNotbad");
_LIT(KInvalidChars14, "%7BthisisNotbad");
_LIT(KInvalidChars15, "%7DthisisNotbad");
_LIT(KInvalidChars16, "%7CthisisNotbad");
_LIT(KInvalidChars17, "%5CthisisNotbad");
_LIT(KInvalidChars18, "%5EthisisNotbad");
_LIT(KInvalidChars19, "%60thisisNotbad");
_LIT(KInvalidChars20, "%3CthisisNotbad");
_LIT(KInvalidChars21, "%3EthisisNotbad");
_LIT(KInvalidChars22, "%25thisisNotbad");
_LIT(KInvalidChars23, "%22thisisNotbad");
_LIT(KInvalidChars24, "*thisisNotbad");
_LIT(KInvalidChars25, "(thisisNotbad");
_LIT(KInvalidChars26, ")thisisNotbad");
_LIT(KInvalidChars27, "!thisisNotbad");
_LIT(KInvalidChars28, "'thisisNotbad");
TBool KInvalid1 = EFalse;

// Some different forms of IPv6 addresses
_LIT(KHostTypeIPv6Format1, "FEDC:BA98:7654:3210:FEDC:BA98:7654:3210");
_LIT(KHostTypeIPv6Format2, "1080:0:0:0:8:800:200C:4171");
_LIT(KHostTypeIPv6Format3, "3ffe:2a00:100:7031::1");
_LIT(KHostTypeIPv6Format4, "1080::8:800:200C:417A");
_LIT(KHostTypeIPv6Format5, "::192.9.5.5");
_LIT(KHostTypeIPv6Format6, "::FFFF:129.144.52.38");
_LIT(KHostTypeIPv6Format7, "2010:836B:4179::836B:4179");

// IPv4 format addresses
_LIT(KHostTypeIPv4Format, "196.58.110.201");
_LIT(KHostTypeIPv4InvalidFormat1, "196.58");		// this is considered a text address
_LIT(KHostTypeIPv4InvalidFormat2, "196.58.110");	// this is considered a text address

// Text format addresses
_LIT(KHostTypeTextFormat1, "www.symbian.com");
_LIT(KHostTypeTextFormat2, "www.any.number.of.dot.seperated.parts.com");


// Text format addresses


#endif // __URIUTILSTESTS_H__
