/*
 * 

 * Copyright (c) 1987, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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
 * Portions Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)setenv.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/lib/libc/stdlib/setenv.c,v 1.9 2002/03/22 21:53:10 obrien Exp $");

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __SYMBIAN32__
#include <errno.h> //errno
#include <unistd.h> //environ
#include "sysreent.h"
#endif //__SYMBIAN32__

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#ifdef EMULATOR
GET_STATIC_VAR_FROM_TLS(alloced, char**)
#define alloced  (*GET_WSD_VAR_NAME(alloced, s)())
#endif //EMULATOR

char *__findenv(const char *, int *);

/*
 * setenv --
 *	Set the value of the environmental variable "name" to be
 *	"value".  If rewrite is set, replace any current value.
 */
EXPORT_C
int
setenv(name, value, rewrite)
	const char *name;
	const char *value;
	int rewrite;
{
#ifndef __SYMBIAN32__
	extern char **environ;
	#else
	char *temp1, *temp2;
	int found = 0;
#endif //__SYMBIAN32__
#ifndef EMULATOR
	static char **alloced;			/* if allocated space before */
#endif //EMULATOR

	char *c;
	int l_value, offset;
	
#ifdef __SYMBIAN32__
	if( name == NULL || 			//name is NULL
	    (strcmp(name, "") == 0) ||  //name points to an empty string
	    (strchr(name, '=') != NULL) //name contains the char '='
	  )
	{
		errno = EINVAL;	//set the errno
		return -1;		//return error
	}
#endif //__SYMBIAN32__
	
	if (*value == '=')			/* no `=' in value */
		++value;
	l_value = strlen(value);
	if ((c = __findenv(name, &offset))) {	/* find if already exists */
		if (!rewrite)
			return (0);
		if (strlen(c) >= l_value) {	/* old larger; copy over */
			while ( (*c++ = *value++) ) { }
			return (0);
		}
	} else {					/* create new slot */
		int cnt;
		char **p;

			for (p = environ, cnt = 0; *p; ++p, ++cnt) { }
		if (alloced == environ) {			/* just increase size */
			p = (char **)BackendReAlloc((char *)environ,
			    (size_t)(sizeof(char *) * (cnt + 2)));
			if (!p)
			{
				#ifdef __SYMBIAN32__
				errno = ENOMEM;
				#endif //__SYMBIAN32__
				
				return (-1);
			}
			alloced = environ = p;
		}
		else {				/* get new space */
						/* copy old entries into it */
			p = BackendAlloc((size_t)(sizeof(char *) * (cnt + 2)));
			if (!p)
			{
#ifdef __SYMBIAN32__
				errno = ENOMEM;
#endif //__SYMBIAN32__
				
				return (-1);
			}
			bcopy(environ, p, cnt * sizeof(char *));
#ifdef __SYMBIAN32__
			BackendFree(environ);
#endif //__SYMBIAN32__
			alloced = environ = p;
		}
		environ[cnt + 1] = NULL;
		offset = cnt;
	}
#ifdef __SYMBIAN32__
	if (c && (environ[offset] != NULL))
	{
		found = 1;
		temp1 = environ[offset];
	}
#endif //__SYMBIAN32__
			for (c = (char *)name; *c && *c != '='; ++c) {	}/* no `=' in name */
	if (!(temp2 =			/* name + `=' + value */
	    BackendAlloc((size_t)((int)(c - name) + l_value + 2))))
	{
#ifdef __SYMBIAN32__
		errno = ENOMEM;
#endif //__SYMBIAN32__
		
		return (-1);
	}
#ifdef __SYMBIAN32__
	if(found == 1)
		BackendFree(temp1);
		
	environ[offset] = temp2;
#endif //__SYMBIAN32__
			for (c = environ[offset]; (*c = *name++) && *c != '='; ++c)	{ }
			for (*c++ = '='; (*c++ = *value++); ) {	}
	return (0);
}

/*
 * unsetenv(name) --
 *	Delete environmental variable "name".
 */
EXPORT_C
void
unsetenv(name)
	const char *name;
{
#ifndef __SYMBIAN32__
	extern char **environ;
#endif
	
	char **p;
	int offset;

	while (__findenv(name, &offset))	/* if set multiple times */
	{
#ifdef __SYMBIAN32__
		char *temp = environ[offset];
		BackendFree(temp);
#endif //__SYMBIAN32__
		for (p = &environ[offset];; ++p)
			if (!(*p = *(p + 1)))
				break;
	}
}
