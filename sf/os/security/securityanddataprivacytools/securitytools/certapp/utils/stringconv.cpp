/*
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


#include <errno.h>
#include <e32base.h>
#include <utf.h>
#include "stringconv.h"
#include "logger.h"

TUint8 *cstrFromUtf16(const TText *aUtf16, TInt aLength, TInt &outputBytes)
{
	TPtrC16 src(aUtf16, aLength);
	
	TPtr8 nulldest(0,0);

	TInt len = CnvUtfConverter::ConvertFromUnicodeToUtf8(nulldest, src);
	if(len<0)
		{
		dbg << Log::Indent() << "ConvertFromUnicodeToUtf8 failed" << Log::Endl();
		FatalError();
		}
	
	TUint8 *buf = new TUint8[len+1];
	TPtr8 dest(buf, len);

	(void) CnvUtfConverter::ConvertFromUnicodeToUtf8(dest, src);
	buf[len] = 0; // Add NUL termination in case it is used with windows APIs
	outputBytes = dest.Length();
	return buf;
}

std::string stringFromUtf16(const TText *aUtf16, TInt aLength)
{
	TInt outputBytes = 0;
	TUint8 *outBuf = cstrFromUtf16(aUtf16, aLength, outputBytes);
	
	std::string str((const char *)outBuf, outputBytes);
	delete [] outBuf;
	return str;
}

std::string stringFromUtf16(const TDesC &aUtf16)
{
	return stringFromUtf16(aUtf16.Ptr(), aUtf16.Length());
}

TText *utf16FromUtf8(const TUint8 *aUtf8, TInt aLength, TInt &outputWords)
{
	// Expand UTF-8 into internal UTF-16LE representation

	TPtrC8 src(aUtf8, aLength);
	
	TPtr16 nulldest(0,0);

	TInt len = CnvUtfConverter::ConvertToUnicodeFromUtf8(nulldest, src);
	if(len<0)
		{
		dbg << Log::Indent() << "ConvertToUnicodeFromUtf8 failed" << Log::Endl();
		FatalError();
		}
	
	TUint16 *buf = new TUint16[len+1];
	TPtr16 dest(buf, len);

	(void) CnvUtfConverter::ConvertToUnicodeFromUtf8(dest, src);

	buf[len] = 0; // Add null termination in case it gets used with windows APIs
	outputWords = dest.Length();
	return buf;
}


// End of file
