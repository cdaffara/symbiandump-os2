// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// FUNCTION
// <<atof>>, <<atoff>>---string to double or float
// INDEX
// atof
// INDEX
// atoff
// ANSI_SYNOPSIS
//

// #include <stdlib.h>
// double atof(const char *<[s]>);
// float atoff(const char *<[s]>);
// TRAD_SYNOPSIS
// #include <stdlib.h>
// double atof(<[s]>)
// char *<[s]>;
// float atoff(<[s]>)
// char *<[s]>;
// <<atof>> converts the initial portion of a string to a <<double>>.
// <<atoff>> converts the initial portion of a string to a <<float>>.
// The functions parse the character string <[s]>,
// locating a substring which can be converted to a floating point
// value. The substring must match the format:
// . [+|-]<[digits]>[.][<[digits]>][(e|E)[+|-]<[digits]>]
// The substring converted is the longest initial
// fragment of <[s]> that has the expected format, beginning with
// the first non-whitespace character.  The substring
// is empty if <<str>> is empty, consists entirely
// of whitespace, or if the first non-whitespace character is
// something other than <<+>>, <<->>, <<.>>, or a digit.
// <<atof(<[s]>)>> is implemented as <<strtod(<[s]>, NULL)>>.
// <<atoff(<[s]>)>> is implemented as <<strtodf(<[s]>, NULL)>>.
// RETURNS
// <<atof>> returns the converted substring value, if any, as a
// <<double>>; or <<0.0>>,  if no conversion could be performed.
// If the correct value is out of the range of representable values, plus
// or minus <<HUGE_VAL>> is returned, and <<ERANGE>> is stored in
// <<errno>>.
// If the correct value would cause underflow, <<0.0>> is returned
// and <<ERANGE>> is stored in <<errno>>.
// <<atoff>> obeys the same rules as <<atof>>, except that it
// returns a <<float>>.
// PORTABILITY
// <<atof>> is ANSI C. <<atof>>, <<atoi>>, and <<atol>> are subsumed by <<strtod>>
// and <<strtol>>, but are used extensively in existing code. These functions are
// less reliable, but may be faster if the argument is verified to be in a valid
// range.
// Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
// <<lseek>>, <<read>>, <<sbrk>>, <<write>>.
//

#include <e32std.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "../INC/ESTLIB.H"
extern "C" {
	
/**
Convert string to double-precision floating-point value.
Parses string interpreting its content as a floating-point value
until a character that can not be interpreted is found, 
and returns a double precision value.
@return   The converted double value from the input string.
If an error occurs 0 is returned.
@param s String representing a floating point number. 
@param tail Address of a pointer. 
This is filled by the function with the address where scan has ended. 
Serves to determine where there is the first non-numerical character in the string.
*/
EXPORT_C double strtod (const char *s, char** tail) __SOFTFP
	{
	TLex8 data=(unsigned char *)s;
	TReal64 ret=0.0;
	data.SkipSpace();
	TBool minus = data.Peek() == '-';
	TInt r=data.Val(ret);
	if (r==KErrOverflow)
		{
		errno=ERANGE;
		if (minus)
			ret = -HUGE_VAL;
		else
			ret = HUGE_VAL;
		}
	else if (r==KErrUnderflow)
		errno=ERANGE;
	if (tail)
		*tail=(char*)(s+data.Offset());
	return ret;
	}

/**
Convert string to double.
Parses string interpreting its content as a floating point number 
and returns a value of type double.
@return   The converted floating point value of the input string.
  On overflow the result is undefined.
  If an error occurs 0.0 is returned.
@param s String representing a floating point number. 
*/
EXPORT_C double atof (const char *s) __SOFTFP
	{
	return strtod(s,NULL);
	}

} // extern "C"
