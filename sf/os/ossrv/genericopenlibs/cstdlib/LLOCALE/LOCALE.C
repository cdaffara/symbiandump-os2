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
* <<setlocale>>, <<localeconv>>---select or query locale
* INDEX
* setlocale
* INDEX
* localeconv
* INDEX
* _setlocale_r
* INDEX
* _localeconv_r
* ANSI_SYNOPSIS
* #include <locale.h>
* char *setlocale(int <[category]>, const char *<[locale]>);
* lconv *localeconv(void);
* char *_setlocale_r(void *<[reent]>,
* int <[category]>, const char *<[locale]>);
* lconv *_localeconv_r(void *<[reent]>);
* TRAD_SYNOPSIS
* #include <locale.h>
* char *setlocale(<[category]>, <[locale]>)
* int <[category]>;
* char *<[locale]>;
* lconv *localeconv();
* char *_setlocale_r(<[reent]>, <[category]>, <[locale]>)
* char *<[reent]>;
* int <[category]>;
* char *<[locale]>;
* lconv *_localeconv_r(<[reent]>);
* char *<[reent]>;
* <<setlocale>> is the facility defined by ANSI C to condition the
* execution environment for international collating and formatting
* information; <<localeconv>> reports on the settings of the current
* locale.
* This is a minimal implementation, supporting only the required <<``C''>>
* value for <[locale]>; strings representing other locales are not
* honored.  (<<``''>> is also accepted; it represents the default locale
* for an implementation, here equivalent to <<``C''>>.)
* If you use <<NULL>> as the <[locale]> argument, <<setlocale>> returns
* a pointer to the string representing the current locale (always
* <<``C''>> in this implementation).  The acceptable values for
* <[category]> are defined in `<<locale.h>>' as macros beginning with
* <<"LC_">>, but this implementation does not check the values you pass
* in the <[category]> argument.
* <<localeconv>> returns a pointer to a structure (also defined in
* `<<locale.h>>') describing the locale-specific conventions currently
* in effect.  
* <<_localeconv_r>> and <<_setlocale_r>> are reentrant versions of
* <<localeconv>> and <<setlocale>> respectively.  The extra argument
* <[reent]> is a pointer to a reentrancy structure.
* RETURNS
* <<setlocale>> returns either a pointer to a string naming the locale
* currently in effect (always <<``C''>> for this implementation), or, if
* the locale request cannot be honored, <<NULL>>.
* <<localeconv>> returns a pointer to a structure of type <<lconv>>,
* which describes the formatting and collating conventions in effect (in
* this implementation, always those of the C locale).
* PORTABILITY
* ANSI C requires <<setlocale>>, but the only locale required across all
* implementations is the C locale.
* No supporting OS subroutines are required.
* setlocale, localeconv : internationalize your locale.
* (Only "C" or null supported).
* 
*
*/



#include <locale.h>
#include <string.h>
#include <limits.h>
#include <reent.h>

static const struct lconv lconv = 
{
  ".", "", "", "", "", "", "", "", "", "",
  CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
  CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
};

/**
The facility defined to condition the execution environment 
for international collating and formatting information.
@return either a pointer to a string naming the locale
currently in effect or, if the locale request cannot be honored NULL.
@param category Category
@param locale Locale
*/
EXPORT_C char* setlocale (int category, const char *locale)
{
  if (locale && strcmp (locale, "C") && strcmp (locale, ""))
   return 0;
  return "C";
}

/**
Reports on the settings of the current locale.
@return a pointer to a structure of type lconv,
which describes the formatting and collating conventions in effect
*/
EXPORT_C struct lconv* localeconv (void)
{
  return (struct lconv *) &lconv;
}
