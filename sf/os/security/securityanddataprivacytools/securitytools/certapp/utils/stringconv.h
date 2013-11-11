#ifndef __STRINGCONV_H__
#define __STRINGCONV_H__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <string>
#include <e32base.h>

/**
 * @file
 * @internalComponent
 */

// Converts the utf16 input into a new buffer and returns that buffer
TUint8 *cstrFromUtf16(const TText *aUtf16, TInt aLength, TInt &outLen); // length is in chars not bytes
std::string stringFromUtf16(const TText *aUtf16, TInt aLength); // length is in chars not bytes
std::string stringFromUtf16(const TDesC &aUtf16);

// Converts the utf8 input into a new buffer and returns that buffer
// The new buffer will be NUL terminated, but the NUL is not included in the outputWords length
TText *utf16FromUtf8(const TUint8 *aUtf8, TInt aLength, TInt &outputWords);


#endif
