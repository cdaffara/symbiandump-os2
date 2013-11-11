/*
* FUNCTION
* <<strtok>>---get next token from a string
* INDEX
* strtok
* INDEX
* strtok_r
* ANSI_SYNOPSIS
* #include <string.h>
* char *strtok(char *<[source]>, const char *<[delimiters]>)
* char *strtok_r(char *<[source]>, const char *<[delimiters]>,
* char **<[lasts]>)
* TRAD_SYNOPSIS
* #include <string.h>
* char *strtok(<[source]>, <[delimiters]>)
* char *<[source]>;
* char *<[delimiters]>;
* char *strtok_r(<[source]>, <[delimiters]>, <[lasts]>)
* char *<[source]>;
* char *<[delimiters]>;
* char **<[lasts]>;
* A series of calls to <<strtok>> break the string starting at
* <<*<[source]>>> into a sequence of tokens.  The tokens are
* delimited from one another by characters from the string at
* <<*<[delimiters]>>>, at the outset.
* The first call to <<strtok>> normally has a string address as
* the first argument; subsequent calls can use <<NULL>> as the
* first argument, to continue searching the same string.  You
* can continue searching a single string with different
* delimiters by using a different delimiter string on each call.
* <<strtok>> begins by searching for any character not in the
* <[delimiters]> string: the first such character is the
* beginning of a token (and its address will be the result of
* the <<strtok>> call). <<strtok>> then continues searching
* until it finds another delimiter character; it replaces that
* character by <<NULL>> and returns.  (If <<strtok>> comes to
* the end of the <<*<[source]>>> string without finding any more
* delimiters, the entire remainder of the string is treated as
* the next token).
* <<strtok>> starts its search at <<*<[source]>>>, unless you
* pass <<NULL>> as the first argument;  if <[source]> is
* <<NULL>>, <<strtok>> continues searching from the end of the
* last search. Exploiting the <<NULL>> first argument leads to
* non-reentrant code. You can easily circumvent this problem by
* saving the last delimiter address in your application, and
* always using it to pass a non-null <[source]> argument.
* RETURNS
* <<strtok>> returns a pointer to the next token, or <<NULL>> if
* no more tokens can be found.
* PORTABILITY
* <<strtok>> is ANSI C.
* <<strtok>> requires no supporting OS subroutines.
* QUICKREF
* strtok ansi impure
* 
*
*/



#include <sys/reent.h>
#include <string.h>

/**
Sequentially truncate string if delimiter is found.
If string is not NULL, the function scans string for the first occurrence of 
any character included in delimiters. If it is found, the function overwrites 
the delimiter in string by a null-character and returns a pointer to the token,
i.e. the part of the scanned string previous to the delimiter.
After a first call to strtok, the function may be called with NULL as string parameter,
and it will follow by where the last call to strtok found a delimiter.
@return A pointer to the last token found in string.
NULL is returned when there are no more tokens to be found.
@param s Null-terminated string to scan. 
@param delim Null-terminated string containing the separators.
*/
EXPORT_C char *
strtok (register char *s, register const char *delim)
{
    return strtok_r (s, delim, &(_REENT->_scanpoint));
}

/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

EXPORT_C char *
strtok_r (register char *s, register const char *delim, char **lasts)
{
	register char *spanp;
	register int c, sc;
	char *tok;


	if (s == NULL && (s = *lasts) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		*lasts = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*lasts = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}
