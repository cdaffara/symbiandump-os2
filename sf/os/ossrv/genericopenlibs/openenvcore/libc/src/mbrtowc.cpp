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
// Name        : mbrtowc.cpp
// Part of     : MRT LIBC
// Contains the source for the mbrtowc API implementation.
// Version     :
//



#include <e32std.h>
#include <utf.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdio.h>
#include "wcharcnv.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif 

#ifdef EMULATOR

GET_STATIC_VAR_FROM_TLS(mbrtowc_mbs, mbstate_t)
#define mbs (*GET_WSD_VAR_NAME(mbrtowc_mbs, s)())
#endif //EMULATOR

extern "C" {

//-----------------------------------------------------------------------------
//Function Name : size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, 
//				mbstate_t *ps) 
//Description   :inspects at most  n  bytes  of the  multibyte  string starting
//at s, extracts the next complete multi-byte character, converts it to a wide
//character and stores it at  *pwc.It  updates the shift state *ps.
//Return Value  : returns the number of bytes parsed from the multi-byte 
//sequence starting at s, if a non-L'\0' wide character  was  recognized. It 
//returns 0, if a Lâ\0â wide character was recognized. It returns (size_t)(-1)
//and sets errno to EILSEQ, if an invalid  multibyte sequence  was encountered.
//It returns (size_t)(-2) if it couldn't parse a complete multibyte character, 
//meaning that n should be increased.
//-----------------------------------------------------------------------------
EXPORT_C size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps)
{
	int rval = 0;
#ifndef EMULATOR	
	static mbstate_t mbs;
#endif //EMULATOR	

	if (ps == NULL)
		ps = &mbs;
	
	if (s)
	{
		wchar_t wide;

		//number of chars to convert has a max of MB_CUR_MAX
		TInt maxlen = (n > MB_CUR_MAX ? MB_CUR_MAX : n);
		
		TPtrC8 src((const TUint8*)s, maxlen);
		//length of 1 as we only want 1 wide character
		TPtr16 awc((TUint16*)&wide, 1);		

		TInt ret = ConvertToUnicodeFromUtf8(awc, src, ps);
	
		//return the number of chars converted which is the max number 
		//- the number not converted unless the character converted 
		//was the wide null character
		if (ret >= 0)
		{
			//rval = (L'\0' != wide) ? maxlen - ret : 0;
			if(L'\0' == wide)
			{
				rval = 0;
			}
			else
			{
				rval = ret;
			}

			if (pwc)
		    	{
			    	//only assign the return if we have a target
				*pwc = wide;	
			}
		}
		else
		{
			rval = ret;
		}
	}
	return (size_t)(rval);

} //end of function

} //extern "C"
