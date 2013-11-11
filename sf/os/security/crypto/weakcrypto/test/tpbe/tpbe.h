/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef TPBE_H
#define TPBE_H

#include "pkcs5kdf.h"
#include "pkcs12kdf.h"

_LIT8(KInputStart, "<input>");
_LIT8(KInputEnd, "</input>");
_LIT8(KPasswdStart, "<passwd>");
_LIT8(KPasswdEnd, "</passwd>");
_LIT8(KKdfStart, "<kdf>");
_LIT8(KKdfEnd, "</kdf>");
_LIT8(KSaltLenBytesStart, "<salt_len_bytes>");
_LIT8(KSaltLenBytesEnd, "</salt_len_bytes>");
_LIT8(KCipherStart, "<cipher>");
_LIT8(KCipherEnd, "</cipher>");
_LIT8(KIterCountStart, "<iter_count>");
_LIT8(KIterCountEnd, "</iter_count>");
_LIT8(KECipherAES_CBC_128, "ECipherAES_CBC_128");
_LIT8(KECipherAES_CBC_192, "ECipherAES_CBC_192");
_LIT8(KECipherAES_CBC_256, "ECipherAES_CBC_256");
_LIT8(KECipherDES_CBC, "ECipherDES_CBC");
_LIT8(KECipher3DES_CBC, "ECipher3DES_CBC");
_LIT8(KECipherRC2_CBC_40, "ECipherRC2_CBC_40");
_LIT8(KECipherRC2_CBC_128, "ECipherRC2_CBC_128");
_LIT8(KECipherRC2_CBC_40_16, "ECipherRC2_CBC_40_16");
_LIT8(KECipherRC2_CBC_128_16, "ECipherRC2_CBC_128_16");

_LIT8(KECipherARC4_128, "ECipherARC4_128");
_LIT8(KECipherARC4_40, "ECipherARC4_40");
_LIT8(KECipher2Key3DES_CBC, "ECipher2Key3DES_CBC");
_LIT8(KECipherRC2_CBC_40_5, "ECipherRC2_CBC_40_5");

#endif


