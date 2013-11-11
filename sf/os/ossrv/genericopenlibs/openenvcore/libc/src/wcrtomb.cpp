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
// Name        : wcrtomb.cpp
// Part of     : MRT LIBC
// Contains the source for the wcrtomb API implementation.
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

GET_STATIC_VAR_FROM_TLS(wcrtomb_mbs, mbstate_t)
#define mbs (*GET_WSD_VAR_NAME(wcrtomb_mbs, s)())
#endif //EMULATOR

//-----------------------------------------------------------------------------
//Function Name : size_t wcrtomb (char * mbchar, wchar_t wc, mbstate_t* ps) 
//Description   : Converts a wide character to a multibyte character
//Return Value  : return If mbchar is null, the return value is true (non-zero)
//if multibyte characters have state-dependent encodings, or false (zero) 
//if they do not.
//-----------------------------------------------------------------------------
extern "C" {

EXPORT_C size_t wcrtomb (char * mbchar, wchar_t wc, mbstate_t* ps)
{
#ifndef EMULATOR	
	static mbstate_t mbs;
#endif //EMULATOR	

	if (ps == NULL)
		ps = &mbs;

	mbstate_t *state = NULL;
	if(ps)
	{
		state = ps;
	}
	else
	{
		state =	&mbs;
	}
	if (mbchar)
	{
		//deal with the special null character case
		if (L'\0' == wc)
		{
			*mbchar = '\0';
			if ( state )
    	   	{
    	   		memset(state,0,sizeof(state));	
    	    	state->__count = _EUTF16InitialState;
    	    }
			return (1);
		}
		
		//return value of 0 is possible if the first half of a surrogate pair seen
		//crash is possible if mbchar is not at least MB_CUR_MAX bytes long
		return _Utf16ToUtf8(mbchar,wc, state,MB_CUR_MAX);
	}
	//calling with a null dest string is used to initialise shift state
	//effect is exactly as if internal dst buffer was used.
	//therefore we always return 1.
	else
	{
	    	if ( state )
	        {
	        	memset(state,0,sizeof(state));	
	        	state->__count = _EUTF16InitialState;
	        }
			return (1);
	}
} //end of function

} //extern "C"
