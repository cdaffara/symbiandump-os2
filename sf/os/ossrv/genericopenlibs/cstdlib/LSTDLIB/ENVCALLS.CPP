// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// connectors for shared environment variables
// 
//

#include "SYSIF.H"
#include "LPOSIX.H"
#include <errno.h>
#include <string.h>


#include <stdlib_r.h>

extern "C" {

/**
Get string from environment.
@return A null-terminated string with the value of the requested environment 
variable or NULL if that environment variable does not exist.
@param name Null-terminated string containing the name of the requested variable.
*/
EXPORT_C char* getenv (const char* name)
	{
	return _getenv_r (_REENT, name);
	}

/**
A reentrant version of getenv().
*/
EXPORT_C char* _getenv_r (struct _reent *r, const char* name)
	{
//	wchar_t tmpbuf[KMaxFullName+1];		//use the max path length possible
	char *  rval = NULL;
	int err = 0;
	wchar_t * tmpbuf = (wchar_t*)malloc(2*(strlen(name)+1));

	if (tmpbuf)
		{

		if (-1 != mbstowcs(tmpbuf, name, strlen(name)+1))
			{
			MSystemInterface& sysIf=Interface(r);
			wchar_t * wideval = sysIf.getenv(tmpbuf);

			if (wideval)
				{
					
				//get the max size
				int size = 1 + 2* wcslen(wideval);
				
				if (size < _MINNARROWBUFSIZE)
					size = _MINNARROWBUFSIZE;

				if (size > r->_NEBSize)
					{
					//this string could be longer
					char * p = (char*)realloc(r->_pNarrowEnvBuffer, size);
					if (p)
						{
						r->_pNarrowEnvBuffer = p;
						r->_NEBSize = size;
						}
					else
						err = ENOMEM;
					}
				if (wcstombs(r->_pNarrowEnvBuffer, wideval, r->_NEBSize) >= 0)
					rval =  r->_pNarrowEnvBuffer;

				}
			}
		else
			err = EILSEQ;

		}

	MapError(err, r->_errno);
	free(tmpbuf);
	return rval;
	}

EXPORT_C wchar_t* wgetenv (const wchar_t* name)
	{
	return _wgetenv_r (_REENT, name);
	}

/**
A reentrant version of wgetenv().
*/
EXPORT_C wchar_t* _wgetenv_r (struct _reent *r, const wchar_t* name)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.getenv(name);
	}

/**
Removes an environment variable.
*/
EXPORT_C void unsetenv (const char* name)
	{
	_unsetenv_r (_REENT, name);
	}

/**
A reentrant version of unsetenv().
*/
EXPORT_C void _unsetenv_r (struct _reent *r, const char* name)
	{
	int rval = 0;
	wchar_t* tmpbuf = (wchar_t*)malloc(2*(strlen(name)+1));

	if (tmpbuf)
		{
		if (-1 != mbstowcs(tmpbuf, name, strlen(name)+1))
			{
			MSystemInterface& sysIf=Interface(r);
			sysIf.unsetenv(tmpbuf);
			}
		else
			rval = EILSEQ;
		}
	else
		rval = ENOMEM;

	free(tmpbuf);
	MapError(rval, r->_errno);
	}

/**
A wide-character version of a unsetenv()
*/
EXPORT_C void wunsetenv (const wchar_t* name)
	{
	_wunsetenv_r (_REENT, name);
	}

/**
A reentrant version of wunsetenv().
*/
EXPORT_C void _wunsetenv_r (struct _reent *r, const wchar_t* name)
	{
	MSystemInterface& sysIf=Interface(r);
	sysIf.unsetenv(name);
	}

/**
Adds or changes an environment variable.

@return On Success, returns 0.
		On Failure, returns -1, errno may be set and the environment shall be unchanged.
*/
EXPORT_C int setenv (const char *name, const char *value, int rewrite)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _setenv_r(r, name, value, rewrite);
	}

/**
A reentrant version of setenv().
*/
EXPORT_C int _setenv_r (struct _reent *r, const char *name, const char *value, int rewrite)
	{
	int rval = 0;
	wchar_t* wname = (wchar_t*)malloc(2*(strlen(name)+1));
	wchar_t* wvalue = (wchar_t*)malloc(2*(strlen(value)+1));

	if (wname && wvalue)	//the allocs were OK
		{
		if ((-1 != mbstowcs(wname, name, strlen(name)+1)) &&
			(-1 != mbstowcs(wvalue, value, strlen(value)+1)))
			{
			MSystemInterface& sysIf=Interface(r);
			rval = sysIf.setenv(wname, wvalue, rewrite, r->_errno);
			}
		else
			rval =  MapError(EILSEQ, r->_errno);
		}
	else
		rval =  MapError(ENOMEM, r->_errno);

	free (wname);
	free (wvalue);
	return rval;
	}

/**
A wide-character version of a setenv()
*/
EXPORT_C int wsetenv (const wchar_t *name, const wchar_t *value, int rewrite)
	{
	struct _reent *r = _REENT2;
	if (!r)
		return -1; // Memory for library globals is not allocated (errno not set).
	return _wsetenv_r(r, name, value, rewrite);
	}

/**
A reentrant version of wsetenv().
*/
EXPORT_C int _wsetenv_r (struct _reent *r, const wchar_t *name, const wchar_t *value, int rewrite)
	{
	MSystemInterface& sysIf=Interface(r);
	return sysIf.setenv(name,value,rewrite,r->_errno);
	}

} // extern "C"
