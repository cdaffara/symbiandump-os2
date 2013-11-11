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
// Name        : iswctype.cpp
// Part of     : LIBC
// Contains the source for fchdir
// Version     : 1.0
//


 
#include <e32std.h>
#include <wctype.h>

extern "C" {
//-----------------------------------------------------------------------------
//Function Name : int iswalpha(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "alpha".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "alpha". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswalpha(wint_t i)
{
	TChar::TCharInfo uInfo;

	TChar((TUint)i).GetInfo(uInfo);
	if((uInfo.iCategory == (TChar::ELuCategory))  || 
	 	(uInfo.iCategory == (TChar::ELlCategory)) ||
	 	(uInfo.iCategory == (TChar::ELtCategory)) ||
	 	(uInfo.iCategory == (TChar::ELoCategory)))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswalnum(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "alnum".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "alnum". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswalnum(wint_t i)
{
	TChar::TCharInfo uInfo;
	
	TChar((TUint)i).GetInfo(uInfo);
	if((uInfo.iCategory == (TChar::ENdCategory)) || 
		(uInfo.iCategory == (TChar::ELlCategory)) || 
		(uInfo.iCategory == (TChar::ELuCategory)) ||
	 	(uInfo.iCategory == (TChar::ELtCategory)) ||
	 	(uInfo.iCategory == (TChar::ELoCategory)))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswblank(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "blank".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "blank". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswblank(wint_t i)
{
	const TUint KHorizontalTab = 0x09;
	TChar::TCharInfo uInfo;
	
	TChar((TUint)i).GetInfo(uInfo);
	if((uInfo.iCategory == (TChar::EZsCategory)) || 
		((uInfo.iCategory == (TChar::EControlGroup)) &&
		(uInfo.iBdCategory == (TChar::ESegmentSeparator)) &&
		(uInfo.iLowerCase == KHorizontalTab )))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswcntrl(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "cntrl".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "cntrl". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswcntrl(wint_t i)
{
	TChar::TCharInfo uInfo;
	
	TChar((TUint)i).GetInfo(uInfo);
	if((uInfo.iCategory == (TChar::EZlCategory))  ||
		(uInfo.iCategory == (TChar::EZpCategory)) ||
		(uInfo.iCategory == (TChar::ECcCategory)) ||
		(uInfo.iCategory == (TChar::ECfCategory)))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswdigit(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "digit".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "digit". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswdigit(wint_t i)
{
	TChar::TCharInfo uInfo;
		
	TChar((TUint)i).GetInfo(uInfo);
	if(uInfo.iCategory == (TChar::ENdCategory))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswprint(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "print".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "print". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswprint(wint_t i)
{
	TChar::TCharInfo uInfo;
	
	TChar((TUint)i).GetInfo(uInfo);
	if((uInfo.iCategory == (TChar::ELuCategory))  || 
	 	(uInfo.iCategory == (TChar::ELlCategory)) ||
	 	(uInfo.iCategory == (TChar::ELtCategory)) ||
	 	(uInfo.iCategory == (TChar::ELoCategory)) ||
	 	(uInfo.iCategory == (TChar::ELmCategory)) || 
	 	(uInfo.iCategory == (TChar::EMnCategory)) ||
	 	(uInfo.iCategory == (TChar::EMcCategory)) ||
	 	(uInfo.iCategory == (TChar::EMeCategory)) ||
	 	(uInfo.iCategory == (TChar::ENdCategory)) || 
	 	(uInfo.iCategory == (TChar::ENoCategory)) ||
	 	(uInfo.iCategory == (TChar::EPcCategory)) ||
	 	(uInfo.iCategory == (TChar::EPdCategory)) ||
	 	(uInfo.iCategory == (TChar::EPsCategory)) || 
	 	(uInfo.iCategory == (TChar::EPeCategory)) ||
	 	(uInfo.iCategory == (TChar::EPiCategory)) ||
	 	(uInfo.iCategory == (TChar::EPfCategory)) ||
	 	(uInfo.iCategory == (TChar::EPoCategory)) || 
	 	(uInfo.iCategory == (TChar::ESmCategory)) ||
	 	(uInfo.iCategory == (TChar::EScCategory)) ||
	 	(uInfo.iCategory == (TChar::ESkCategory)) ||
	 	(uInfo.iCategory == (TChar::ESoCategory)) ||
	 	(uInfo.iCategory == (TChar::EZsCategory)))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function
	
//-----------------------------------------------------------------------------
//Function Name : int iswlower(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "lower".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "lower". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswlower(wint_t i)
{
	TChar::TCharInfo uInfo;

	TChar((TUint)i).GetInfo(uInfo);
	if((uInfo.iCategory == (TChar::ELlCategory)))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswgraph(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "graph".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "graph". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswgraph(wint_t i)
{
	TChar::TCharInfo uInfo;
	
	TChar((TUint)i).GetInfo(uInfo);
	if((uInfo.iCategory == (TChar::ELuCategory))  || 
	 	(uInfo.iCategory == (TChar::ELlCategory)) ||
	 	(uInfo.iCategory == (TChar::ELtCategory)) ||
	 	(uInfo.iCategory == (TChar::ELoCategory)) ||
	 	(uInfo.iCategory == (TChar::ELmCategory)) || 
	 	(uInfo.iCategory == (TChar::EMnCategory)) ||
	 	(uInfo.iCategory == (TChar::EMcCategory)) ||
	 	(uInfo.iCategory == (TChar::EMeCategory)) ||
	 	(uInfo.iCategory == (TChar::ENdCategory)) || 
	 	(uInfo.iCategory == (TChar::ENoCategory)) ||
	 	(uInfo.iCategory == (TChar::EPcCategory)) ||
	 	(uInfo.iCategory == (TChar::EPdCategory)) ||
	 	(uInfo.iCategory == (TChar::EPsCategory)) || 
	 	(uInfo.iCategory == (TChar::EPeCategory)) ||
	 	(uInfo.iCategory == (TChar::EPiCategory)) ||
	 	(uInfo.iCategory == (TChar::EPfCategory)) ||
	 	(uInfo.iCategory == (TChar::EPoCategory)) || 
	 	(uInfo.iCategory == (TChar::ESmCategory)) ||
	 	(uInfo.iCategory == (TChar::EScCategory)) ||
	 	(uInfo.iCategory == (TChar::ESkCategory)) ||
	 	(uInfo.iCategory == (TChar::ESoCategory)))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswpunct(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "punct".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "punct". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswpunct(wint_t i)
{
	TChar::TCharInfo uInfo;
	
	TChar((TUint)i).GetInfo(uInfo);
	if((uInfo.iCategory == (TChar::EPcCategory))  ||
		(uInfo.iCategory == (TChar::EPdCategory)) ||
		(uInfo.iCategory == (TChar::EPsCategory)) ||
		(uInfo.iCategory == (TChar::EPeCategory)) ||
		(uInfo.iCategory == (TChar::EPiCategory)) ||
		(uInfo.iCategory == (TChar::EPfCategory)) ||
		(uInfo.iCategory == (TChar::EPoCategory)) ||
		((uInfo.iCategory == (TChar::EScCategory)  &&
	     (uInfo.iBdCategory ==((TChar::EEuropeanNumberTerminator))))) ||
	    ((uInfo.iCategory == (TChar::ESymbolGroup) &&
	     (uInfo.iBdCategory ==((TChar::EEuropeanNumberTerminator))))) ||
	    ((uInfo.iCategory == (TChar::ESymbolGroup) &&
	     (uInfo.iBdCategory ==((TChar::EOtherNeutral))))) ||
	     ((uInfo.iCategory == (TChar::ESkCategory) &&
	     (uInfo.iBdCategory ==((TChar::EOtherNeutral))))))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswspace(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "space".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "space". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswspace(wint_t i)
{
	return (TChar((TUint)i).IsSpace());
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswupper(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "upper".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "upper". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswupper(wint_t i)
{
	TChar::TCharInfo uInfo;
	
	TChar((TUint)i).GetInfo(uInfo);
	if((uInfo.iCategory == (TChar::ELuCategory)))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function

//-----------------------------------------------------------------------------
//Function Name : int iswxdigit(wint_t i)
//Description   : tests whether wc is a wide character belonging to the wide
//character class "xdigit".
//Return Value  : returns  non-zero  if  i is  a  wide  character belonging to
//the  wide  character class "xdigit". Otherwise it returns zero.
//-----------------------------------------------------------------------------
EXPORT_C int iswxdigit(wint_t i)
{
	TChar::TCharInfo uInfo;
	
	TChar((TUint)i).GetInfo(uInfo);
	if((((uInfo.iCategory == (TChar::ELlCategory)) || (uInfo.iCategory == (TChar::ELuCategory))) && 
			((i >= L'a' && i <= L'f') || (i >= L'A' && i <= L'F'))) ||
			(uInfo.iCategory == (TChar::ENdCategory)))
	{
		return (1);
	}
	else
	{
		return (0);
	}
} //end of function
    
//-----------------------------------------------------------------------------
//Function Name : wint_t towlower(wint_t i)
//Description   :If i is a wide character,  it  is  converted  to  lowercase.
//Characters  which  do  not  have case are returned unchanged.
//Return Value  :returns the lowercase equivalent of i, or  WEOF if i is WEOF
//-----------------------------------------------------------------------------
EXPORT_C wint_t towlower(wint_t i)
{
	TChar::TCharInfo uInfo;

	TChar((TUint)i).GetInfo(uInfo);
	return (uInfo.iLowerCase);
} //end of function

//-----------------------------------------------------------------------------
//Function Name : wint_t towupper(wint_t i)
//Description   :If i is a wide character,  it  is  converted  to  uppercase.
//Characters  which  do  not  have case are returned unchanged.
//Return Value  :returns the uppercase equivalent of i, or  WEOF if i is WEOF
//-----------------------------------------------------------------------------
EXPORT_C wint_t towupper(wint_t i)
{
	TChar::TCharInfo uInfo;
	
	TChar((TUint)i).GetInfo(uInfo);
	return (uInfo.iUpperCase);
} //end of function

} // extern "C"
