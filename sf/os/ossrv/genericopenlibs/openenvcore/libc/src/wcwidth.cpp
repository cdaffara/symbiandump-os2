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
// Name        : wcwidth.cpp
// Part of     : MRT LIBC
// Contains the source for wcwidth API implementation.
// Version     :
//


 
#include <e32std.h>
#include <wchar.h>
#include <wctype.h>

#ifdef __cplusplus
extern "C" {
#endif

EXPORT_C int
wcwidth(wchar_t wc)
{
	int rVal,result = 0;
	TChar::TCjkWidth rWid;
		
	//check for end of string
	if(wc == L'\0')
	{
		return (result);
	}
	
	//check if the character is printable
	rVal = iswprint((wint_t)wc);

	//if printable
	if(rVal)
	{
		rWid = (TChar::TCjkWidth)TChar((TUint)wc).GetCjkWidth();
		if(rWid == TChar::ENarrow)
		{
			result = TChar::EHalfWidth;
		}
		if(rWid == TChar::EWide)
		{
			result = TChar::EFullWidth;
		}
	}
	else
	{
		result = -1;//if the character is not printable
	}
	
	return (result);
}

#ifdef __cplusplus
}
#endif

