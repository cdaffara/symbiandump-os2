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

#ifndef __ESCAPEUTILSTESTS_H__
#define __ESCAPEUTILSTESTS_H__

// System includes
//
#include <e32base.h>

//
//
// EscapeUtils test data
//
//

_LIT(KEscapeDecoded0,			";/?:@&=+$,");
_LIT(KEscapeEncoded0_auth,		"%3B%2F%3F%3A%40&=+$,");
_LIT(KEscapeEncoded0_path,		"%3B%2F%3F:@&%3D+$,");
_LIT(KEscapeEncoded0_query,		"%3B%2F%3F%3A%40%26%3D%2B%24%2C");
_LIT(KEscapeEncoded0_normal,	";/?:@&=+$,");
_LIT(KEscapeEncoded0_url,		"%3B%2F%3F%3A%40%26%3D%2B%24,");

_LIT(KEscapeDecoded1,			"<>#%\" {}|\\^[]\'");
_LIT(KEscapeEncoded1_auth,		"%3C%3E%23%25%22%20%7B%7D%7C%5C%5E%5B%5D\'"); 
_LIT(KEscapeEncoded1_path,		"%3C%3E%23%25%22%20%7B%7D%7C%5C%5E%5B%5D\'"); 
_LIT(KEscapeEncoded1_query,		"%3C%3E%23%25%22%20%7B%7D%7C%5C%5E%5B%5D\'"); 
_LIT(KEscapeEncoded1_normal,	"%3C%3E%23%25%22%20%7B%7D%7C%5C%5E[]\'");
_LIT(KEscapeEncoded1_url,		"%3C%3E%23%25%22%20%7B%7D%7C%5C%5E%5B%5D%27");

_LIT(KEscapeDecoded2,			"abcde");
_LIT(KEscapeEncoded2_auth,		"abcde");
_LIT(KEscapeEncoded2_path,		"abcde");
_LIT(KEscapeEncoded2_query,		"abcde");
_LIT(KEscapeEncoded2_normal,	"abcde");
_LIT(KEscapeEncoded2_url,		"abcde");

_LIT(KEscapeDecoded3,			";/?:@&=+$[]\"");
_LIT(KEscapeEncoded3_auth,		"%3B%2F%3F%3A%40&=+$%5B%5D%22");
_LIT(KEscapeEncoded3_path,		"%3B%2F%3F:@&%3D+$%5B%5D%22");
_LIT(KEscapeEncoded3_query,		"%3B%2F%3F%3A%40%26%3D%2B%24%5B%5D%22");
_LIT(KEscapeEncoded3_normal,	";/?:@&=+$[]%22");
_LIT(KEscapeEncoded3_url,		"%3B%2F%3F%3A%40%26%3D%2B%24%5B%5D%22");

_LIT(KEscapeDecoded4,			"/;=?[]");
_LIT(KEscapeEncoded4_auth,		"%2F%3B=%3F%5B%5D");
_LIT(KEscapeEncoded4_path,		"%2F%3B%3D%3F%5B%5D");
_LIT(KEscapeEncoded4_query,		"%2F%3B%3D%3F%5B%5D");
_LIT(KEscapeEncoded4_normal,	"/;=?[]");
_LIT(KEscapeEncoded4_url,		"%2F%3B%3D%3F%5B%5D");

_LIT(KEscapeDecoded5,			";/:@?[]\"");
_LIT(KEscapeEncoded5_auth,		"%3B%2F%3A%40%3F%5B%5D%22");
_LIT(KEscapeEncoded5_path,		"%3B%2F:@%3F%5B%5D%22");
_LIT(KEscapeEncoded5_query,		"%3B%2F%3A%40%3F%5B%5D%22");
_LIT(KEscapeEncoded5_normal,	";/:@?[]%22");
_LIT(KEscapeEncoded5_url,		"%3B%2F%3A%40%3F%5B%5D%22");

_LIT(KEscapeDecoded6,			";/?:@&=+$[]!\\\'()~");
_LIT(KEscapeEncoded6_auth,		"%3B%2F%3F%3A%40&=+$%5B%5D!%5C\'()~");
_LIT(KEscapeEncoded6_path,		"%3B%2F%3F:@&%3D+$%5B%5D!%5C\'()~");	
_LIT(KEscapeEncoded6_query,		"%3B%2F%3F%3A%40%26%3D%2B%24%5B%5D!%5C\'()~");
_LIT(KEscapeEncoded6_normal,	";/?:@&=+$[]!%5C\'()~");
_LIT(KEscapeEncoded6_url,		"%3B%2F%3F%3A%40%26%3D%2B%24%5B%5D%21%5C%27%28%29%7E");

_LIT(KEscapeDecoded7,			" <>#%\"{}|\\^`");
_LIT(KEscapeEncoded7_auth,		"%20%3C%3E%23%25%22%7B%7D%7C%5C%5E%60");
_LIT(KEscapeEncoded7_path,		"%20%3C%3E%23%25%22%7B%7D%7C%5C%5E%60");
_LIT(KEscapeEncoded7_query,		"%20%3C%3E%23%25%22%7B%7D%7C%5C%5E%60");
_LIT(KEscapeEncoded7_normal,	"%20%3C%3E%23%25%22%7B%7D%7C%5C%5E%60");
_LIT(KEscapeEncoded7_url,		"%20%3C%3E%23%25%22%7B%7D%7C%5C%5E%60");



_LIT(KUnicode_0, "áóú");
_LIT(KUtf8_0, "%C3%A1%C3%B3%C3%BA");

TChar KExcludedChar0 = ' ';
TBool KExcludedChar0_excluded = ETrue;

TChar KExcludedChar1 = 'a';
TBool KExcludedChar1_excluded = EFalse;

TChar KExcludedChar2 = '%';
TBool KExcludedChar2_excluded = ETrue;

TChar KExcludedChar3 = '@';
TBool KExcludedChar3_excluded = EFalse;

TChar KExcludedChar4 = '6';
TBool KExcludedChar4_excluded = EFalse;

_LIT(KEscapeTriple0, "%5B");
TBool KEscapeTriple0_valid = ETrue;
TInt KEscapeTriple0_value = 0x5b;

_LIT(KEscapeTriple1, "%2a");
TBool KEscapeTriple1_valid = ETrue;
TInt KEscapeTriple1_value = 0x2a;

_LIT(KEscapeTriple2, "bad%2A");
TBool KEscapeTriple2_valid = EFalse;
TInt KEscapeTriple2_value = 0;


#endif // __ESCAPEUTILSTESTS_H__
