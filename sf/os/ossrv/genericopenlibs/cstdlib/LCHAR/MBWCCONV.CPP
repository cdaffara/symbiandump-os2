// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Rrun-time library routines for translating between multibyte and wide characters
// mbstowcs, mbtowc, wcstombs, wctomb and mblen
// 
//

#include <e32std.h>
#include <utf.h>
#include <stdlib.h>
#include <string.h>




extern "C"
{

/**
Converts the multibyte character addressed by s into the corresponding UNICODE
character pwc
@return the length, in bytes, of the multibyte character for which it found 
a UNICODE equivalent
@param pwc Is the address of a wide character, type wchar_t, 
to receive the UNICODE equivalent of s.
@param s Points to the multibyte character to be converted to UNICODE.
@param n Is the maximum width, in bytes, for which to scan s for a valid multibyte
sequence. Regardless of the value of n, no more than MB_CUR_MAX bytes are examined.
*/
EXPORT_C int mbtowc (wchar_t *pwc, const char *s, size_t n)
	{

	int rval = 0;
	if (s)
		{
		wchar_t wide;

		//number of chars to convert has a max of MB_CUR_MAX
		TInt maxlen = (n > MB_CUR_MAX ? MB_CUR_MAX : n);
		
		TPtrC8 src((const TUint8*)s, maxlen);
		TPtr16 awc((TUint16*)&wide, 1);		//length of 1 as we only want 1 wide character

		TInt ret = CnvUtfConverter::ConvertToUnicodeFromUtf8(awc, src);
	
		//return the number of chars converted which is the max number - the number not converted
		//unless the character converted was the wide null character
		if (ret >= 0)
			{
			rval = (L'\0' != wide) ? maxlen - ret : 0;

			if (pwc)
				*pwc = wide;	//only assign the return if we have a target
			}
		else
			return -1;	//the conversion failed.

		}
	return rval;
	}
}


extern "C"
{

EXPORT_C int mbstowcs (wchar_t *wstring, const char * string, size_t size)
	{
	//convert the string "string" to wide characters
	//return number of wide characters

	if (string)
		{
		
		if (wstring)
			{
			TPtrC8 src((const TUint8*)string);
			TPtr16 awc((TUint16*)wstring, size);		//max length of size

			TInt ret = CnvUtfConverter::ConvertToUnicodeFromUtf8(awc, src);
		
			if (ret >= 0)
				{
				TUint len = awc.Length();	//return number of wide characters
				if (len < size)
					awc.ZeroTerminate();
				return len;
				}
			else
				return -1;	//the conversion failed.
			}
		else
			{
			//we have no output string.  
			//ms say return len required
			//gcc says nowt
			return 1+strlen(string);	//max is could be
			}
		}
	return 0;	
	}


	
}



extern "C"
{

/**
Converts a wide character to a multibyte character
@return If s is null, the return value is true (non-zero) if multibyte 
characters have state-dependent encodings, or false (zero) if they do not.
@param mbchar multibyte character
@param wc wide character
*/
EXPORT_C int wctomb (char * mbchar, wchar_t wc)
	{
	if (mbchar)
		{
		//deal with the special null character case
		if (L'\0' == wc)
			{
			*mbchar = '\0';
			return 1;
			}

		//so we have possible character which is not null
		TPtr8 multi((TUint8*)mbchar, 0, MB_CUR_MAX);	//limit max length to MB_CUR_MAX
		TPtrC16 wide ((const TUint16*)&wc);

		TInt ret = CnvUtfConverter::ConvertFromUnicodeToUtf8(multi, wide);

		//ret has the number of wide characters left to convert, or an error
		if (ret >= 0)	//we didn't get an error
			//return the number of characters in the output
			return multi.Length();
		else
			return -1;

		}
		//calling with a null dest string is used to initialise shift state
		//we are only dealing with UTF8 which hasn't got one,
		//therefore we always return 0.
	else
		return 0;
	}
}

extern "C"
{
/**
The wcstombs function converts a wide string to a string of multibyte
characters.
@return 
@param string multibyte string
@param wstring wide string
@param size number of bytes written to.
*/
EXPORT_C int wcstombs (char * string, const wchar_t * wstring, size_t size)
	{
	if (wstring)
		{
		if (string)
			{
			TPtr8 multi((TUint8*)string, size);	//limit max length to size
			TPtrC16 wide((TText16*)wstring);

			TInt ret = CnvUtfConverter::ConvertFromUnicodeToUtf8(multi, wide);

			if (ret >= 0)	//we didn't get an error
				{
				TUint len = multi.Length();
				if (len < size)
					multi.ZeroTerminate();
				return len;			//null terminate
				}
			else
				return -1;
			}
		else
			{
			//we have a null output string
			//ms expects the length back.
			//gcc says nothing about it.
			//quick and dirty!!
			return 1 + (MB_CUR_MAX * wcslen(wstring));		//max it can be
			}
		}
	else
		return 0;
	}
}



extern "C"
{
/**
If string is not a null pointer, the function returns the number of bytes in the 
multibyte string 
that constitute the next multibyte character, 
or it returns -1 if the next n (or the remaining) bytes do not constitute a valid 
multibyte character. mblen does not include the terminating null in the count of bytes. 
@return the number of bytes in the multibyte string
@param string 
@param size 
*/
EXPORT_C int mblen (const char *string, size_t size)
	{
	if (string)
		{
		wchar_t wide;

		
		//deal with an empty string without doing the conversion.
		if ('\0' == *string)
			return 0;

		TInt maxlen = (size > MB_CUR_MAX ? MB_CUR_MAX : size);

		TPtrC8 src((const TUint8*)string, maxlen);
		TPtr16 awc((TUint16*)&wide, 1);		//length of 1 as we only want 1 wide character

		TInt ret = CnvUtfConverter::ConvertToUnicodeFromUtf8(awc, src);
	
		//return the number of chars converted which is the max number - the number not converted
		//unless the character converted was the wide null character
		if (ret >= 0)
			{
			return ((L'\0' != wide) ? maxlen - ret : 0);
			}
		else
			return -1;

		}
		//shift state would be initialised here if were using them
		return 0;
	}
}
