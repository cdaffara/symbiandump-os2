// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CENTREPNOTIFYHANDLERTESTKEYS_H
#define CENTREPNOTIFYHANDLERTESTKEYS_H

//UID's
const TUid KTestUid = {0x6edcba99};


//Keys

const TUint32 KKey1 = 0x00000001;
const TUint32 KKey2 = 0x00000002;
const TUint32 KKey3 = 0x00000003;
const TUint32 KKey4 = 0x00000004;

const TUint32 KNewTestKey1 = 0x00000005;
const TUint32 KNewTestKey2 = 0x00000006;

const TUint32 KKeyNotExists = 0xfffffff0;

// Key values

const TInt KIntValueInit = 12345;
const TInt KIntValue = 54321;
const TInt KIntValue2 = 98765;
const TInt KIntValue3 = 3333;

const TReal KRealValueInit = 1.5;
const TReal KRealValue = 5.678;
const TReal KRealValue2 = 9.123;

_LIT(KStringValueInit, "this is a test string");
_LIT(KStringValue, "Replaced the original string"); 
_LIT(KStringValue2, "Another string"); 

_LIT8(KBinaryValueInit, "\xab\xc2\x89");
_LIT8(KBinaryValue, "cba982");
_LIT8(KBinaryValue2, "abc567");

_LIT(KGenericString, "Listening whole repository");

_LIT(KResetTestString, "reset");
_LIT(KResetKeyTestString, "resetkey");

_LIT8(KBinaryTestValue, "aaff00");

_LIT(KOriginalKeyString1, "this is a test string");

const TInt KGenericInt = 10;
	

#endif      // CENTREPNOTIFYHANDLERTESTKEYS_H

// End of File
