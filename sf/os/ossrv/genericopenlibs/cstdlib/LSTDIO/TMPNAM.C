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
* <<tmpnam>>, <<tempnam>>---name for a temporary file
* INDEX
* tmpnam
* INDEX
* tempnam
* INDEX
* _tmpnam_r
* INDEX
* _tempnam_r
* ANSI_SYNOPSIS
* #include <stdio.h>
* char *tmpnam(char *<[s]>);
* char *tempnam(char *<[dir]>, char *<[pfx]>);
* char *_tmpnam_r(void *<[reent]>, char *<[s]>);
* char *_tempnam_r(void *<[reent]>, char *<[dir]>, char *<[pfx]>);
* TRAD_SYNOPSIS
* #include <stdio.h>
* char *tmpnam(<[s]>)
* char *<[s]>;
* char *tempnam(<[dir]>, <[pfx]>)
* char *<[dir]>;
* char *<[pfx]>;
* char *_tmpnam_r(<[reent]>, <[s]>)
* char *<[reent]>;
* char *<[s]>;
* char *_tempnam_r(<[reent]>, <[dir]>, <[pfx]>)
* char *<[reent]>;
* char *<[dir]>;
* char *<[pfx]>;
* Use either of these functions to generate a name for a temporary file.
* The generated name is guaranteed to avoid collision with other files
* (for up to <<TMP_MAX>> calls of either function).
* <<tmpnam>> generates file names with the value of <<P_tmpdir>>
* (defined in `<<stdio.h>>') as the leading directory component of the path.
* You can use the <<tmpnam>> argument <[s]> to specify a suitable area
* of memory for the generated filename; otherwise, you can call
* <<tmpnam(NULL)>> to use an internal static buffer.
* <<tempnam>> allows you more control over the generated filename: you
* can use the argument <[dir]> to specify the path to a directory for
* temporary files, and you can use the argument <[pfx]> to specify a
* prefix for the base filename.
* If <[dir]> is <<NULL>>, <<tempnam>> will attempt to use the value of
* environment variable <<TMPDIR>> instead; if there is no such value,
* <<tempnam>> uses the value of <<P_tmpdir>> (defined in `<<stdio.h>>').
* If you don't need any particular prefix to the basename of temporary
* files, you can pass <<NULL>> as the <[pfx]> argument to <<tempnam>>.
* <<_tmpnam_r>> and <<_tempnam_r>> are reentrant versions of <<tmpnam>>
* and <<tempnam>> respectively.  The extra argument <[reent]> is a
* pointer to a reentrancy structure.
* WARNINGS
* The generated filenames are suitable for temporary files, but do not
* in themselves make files temporary.  Files with these names must still
* be explicitly removed when you no longer want them.
* If you supply your own data area <[s]> for <<tmpnam>>, you must ensure
* that it has room for at least <<L_tmpnam>> elements of type <<char>>.
* RETURNS
* Both <<tmpnam>> and <<tempnam>> return a pointer to the newly
* generated filename.
* PORTABILITY
* ANSI C requires <<tmpnam>>, but does not specify the use of
* <<P_tmpdir>>.  The System V Interface Definition (Issue 2) requires
* both <<tmpnam>> and <<tempnam>>.
* Supporting OS subroutines required: <<close>>,  <<fstat>>, <<getpid>>,
* <<isatty>>, <<lseek>>, <<open>>, <<read>>, <<sbrk>>, <<write>>.
* The global pointer <<environ>> is also required.
* 
*
*/



#include <_ansi.h>
#include <stdio_r.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


/* Try to open the file specified, if it can be opened then try
   another one.  */

#define MAXFILENAME 255

static void worker (struct _reent *ptr,char *result,int part3,int *part4)
{
  /*  Generate the filename and make sure that there isn't one called
      it already.  */

  for (;;)
    {
      int t;
      struct stat st;
      _sprintf_r (ptr, result, P_tmpdir "t%x.%x", part3, *part4);
      t = stat(result, &st);
      if (t == -1)
	break;	/* file doesn't exist, so it's a plausible name */
      (*part4)++;
    }
}

/** A reentrant version of tmpnam(). 
*/
EXPORT_C char * _tmpnam_r (struct _reent *p, char *s)
{
  char *result;
  int pid;

  if (s == NULL)
      result = p->_tmpnam;
  else
      result = s;
  pid = getpid();
  worker (p, result, pid, &p->_inc);
  p->_inc++;
  return result;
}

/** A reentrant version of wtmpnam(). 
*/
EXPORT_C wchar_t * _wtmpnam_r (struct _reent *p, wchar_t *s)
	{
	char *result;
	int pid;
	char temp1[MAXFILENAME];
	wchar_t *target;


	if (s == NULL)
		result = p->_tmpnam;
	else
		result = temp1;

	pid = getpid();
	worker (p, result, pid, &p->_inc);
	p->_inc++;

	//store the wide result
	target = s ? s : p->_wtmpnam;
	if (mbstowcs(target, result, 37))
		return target;

	p->_errno=EILSEQ;
	return 0;
	}

#ifndef _REENT_ONLY

/**
Generate a unique temporary filename.
@return A pointer to the string containing the proposed name for a temporary file. 
If NULL was specified as the buffer this points to an internal buffer 
that will be overwritten the next time this function is called, 
otherwise it returns the buffer parameter.
If an error occurs this function returns NULL.
@param s Pointer to an array of bytes, where the proposed tempname will be stored.
*/
EXPORT_C char * tmpnam (char *s)
{
  return _tmpnam_r (_REENT, s);
}

EXPORT_C wchar_t * wtmpnam (wchar_t *s)
{
  return _wtmpnam_r (_REENT, s);
}


#endif
