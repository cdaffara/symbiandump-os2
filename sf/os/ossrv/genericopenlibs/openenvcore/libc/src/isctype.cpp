/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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



/*Use E32 function equivalent of ctype.h macros.
This is hideously expensive, involving EXEC calls to read the array,
but we do get proper localisation.
*/

#include <e32std.h>
#include <_ansi.h>
#include <ctype.h>

extern "C" {


/*
Tests whether the character is alphabetic or a decimal digit. 
*/
EXPORT_C int isalnum(int c)
	{
	return TChar((TUint)c).IsAlphaDigit();
	}

/*
Test for an alphabetic character.
For Unicode, the function returns TRUE for all letters, including those from
syllabaries and ideographic scripts. The function returns FALSE for 
letter-like characters that are in fact diacritics. 
Specifically, the function returns TRUE for categories: 
ELuCategory, ELtCategory, ELlCategory, and ELoCategory; 
it returns FALSE for all other categories including ELmCategory.
*/
EXPORT_C int isalpha(int c)
	{
	return TChar((TUint)c).IsAlpha();
	}

/*
Tests whether the character is a control character.
For Unicode, the function returns TRUE for all characters in the categories:
ECcCategory, ECfCategory, ECsCategory, ECoCategory and ECnCategoryCc.
*/
EXPORT_C int iscntrl(int c)
	{
	return TChar((TUint)c).IsControl();
	}

/*
Tests whether the character is a standard decimal digit.
For Unicode, this function returns TRUE only for the digits 
'0'...'9' (U+0030...U+0039), not for other digits in scripts 
like Arabic, Tamil, etc.
*/
EXPORT_C int isdigit(int c)
	{
	return TChar((TUint)c).IsDigit();
	}

/*
Tests whether the character is a graphic character.
For Unicode, graphic characters include printable characters but not the space
character. Specifically, graphic characters are any character except those in 
categories: EZsCategory,EZlCategory,EZpCategory, ECcCategory,ECfCategory,ECsCategory,
ECoCategory, and ,ECnCategory.
Note that for ISO Latin-1, all alphanumeric and punctuation characters are graphic.
*/
EXPORT_C int isgraph(int c)
	{
	return TChar((TUint)c).IsGraph();
	}

/*
Tests if character is a lowercase letter.
*/
EXPORT_C int islower(int c)
	{
	return TChar((TUint)c).IsLower();
	}

/*
Tests whether the character is a printable character.
For Unicode, printable characters are any character except those in categories:
ECcCategory, ECfCategory, ECsCategory, ECoCategory and ECnCategory.
Note that for ISO Latin-1, all alphanumeric and punctuation characters, 
plus space, are printable.
*/
EXPORT_C int isprint(int c)
	{
	return TChar((TUint)c).IsPrint();
	}

/*
Tests whether the character is a punctuation character.
For Unicode, punctuation characters are any character in the categories:
EPcCategory, EPdCategory, EPsCategory, EPeCategory, EPiCategory, EPfCategory, 
EPoCategory.
*/
EXPORT_C int ispunct(int c)
	{
	 switch(c)
	 	{
	 	case '+' :
	 	case '<' :
	 	case '=' :
	 	case '>' :
	 	case '|' :
	 	case '$' :
	 	case '~' :
	 	case '^' :
	 	case 0x60: return ETrue; // 0x60->hex value for -> ` (GRAVE ACCENT)
	 			  
	 	default:  return TChar((TUint)c).IsPunctuation();
		}
	}

/*
Tests whether the character is a white space character. White space includes
spaces, tabs and separators.
For Unicode, the function returns TRUE for all characters in the categories: 
EZsCategory, EZlCategory and EZpCategory, and also for the characters 
0x0009 (horizontal tab), 0x000A (linefeed), 0x000B (vertical tab), 0x000C (form feed),
 and 0x000D (carriage return).
*/
EXPORT_C int isspace(int c)
	{
	return TChar((TUint)c).IsSpace();
	}

/*
Tests whether the character is uppercase.
*/
EXPORT_C int isupper(int c)
	{
	return TChar((TUint)c).IsUpper();
	}

/*
Tests whether the character is a hexadecimal digit (0-9, a-f, A-F).
*/
EXPORT_C int isxdigit(int c)
	{
	return TChar((TUint)c).IsHexDigit();
	}

/*
Tests whether the character is lowercase.
*/
EXPORT_C int tolower(int c)
	{
	return TChar((TUint)c).GetLowerCase();
	}

/*
Returns the character value after conversion to uppercase or the character's
own value, if no uppercase form exists. The character object itself is not changed.
*/
EXPORT_C int toupper(int c)
	{
	return TChar((TUint)c).GetUpperCase();
	}

} // extern "C"
