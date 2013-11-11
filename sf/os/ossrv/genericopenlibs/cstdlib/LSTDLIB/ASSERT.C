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
* FUNCTION
* <<assert>>---Macro for Debugging Diagnostics
* INDEX
* assert
* ANSI_SYNOPSIS
* #include <assert.h>
* void assert(int <[expression]>);
* TRAD_SYNOPSIS
* #include <assert.h>
* assert(<[expression]>)
* int <[expression]>;
* Use this macro to embed debuggging diagnostic statements in
* your programs.  The argument <[expression]> should be an
* expression which evaluates to true (nonzero) when your program
* is working as you intended.
* When <[expression]> evaluates to false (zero), <<assert>>
* calls <<abort>>, after first printing a message showing what
* failed and where:
* . Assertion failed: <[expression]>, file <[filename]>, line <[lineno]>
* The macro is defined to permit you to turn off all uses of
* <<assert>> at compile time by defining <<NDEBUG>> as a
* preprocessor variable.   If you do this, the <<assert>> macro
* expands to
* . (void(0))
* RETURNS
* <<assert>> does not return a value.
* PORTABILITY
* The <<assert>> macro is required by ANSI, as is the behavior
* when <<NDEBUG>> is defined.
* Supporting OS subroutines required (only if enabled): <<close>>, <<fstat>>,
* <<getpid>>, <<isatty>>, <<kill>>, <<lseek>>, <<read>>, <<sbrk>>, <<write>>.
* 
*
*/



#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

EXPORT_C void __assert (const char *file, int line, const char *failedexpr)
{
  (void)fprintf(stderr,
	"assertion \"%s\" failed: file \"%s\", line %d\n",
	failedexpr, file, line);
  abort();
  /* NOTREACHED */
}
