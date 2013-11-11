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
// Common code useful to all crt0 variants. This is kept in the DLL to allow us to
// change it in future releases.
// 
//

#include <e32std.h>
#include <e32base.h>
#include <unistd.h>	// for chdir, environ
#include <stdlib.h>	// for calloc
#include <string.h>	// for strdup
#include <wchar.h>	// for wcsdup
#include <fcntl.h>	// for O_RDONLY, O_WRONLY and O_CREAT
#include "estlib.h"
#include "sysif.h"
#include "libc_private.h"


extern "C" {
void SetupEnviron();
void GetTmpDirName();
}

const TInt KMaxArgC=20;

static char* wcstombs_alloc (const wchar_t* aString)
	{
	if (aString==NULL)
		return NULL;

	size_t size = wcstombs(0, aString, 0);
	char* buf = (char*)User::Alloc((size+1) * sizeof(char));
	size = wcstombs(buf, aString, size);

	if (size == (size_t)-1)
		{
		User::Free(buf);
		return NULL;
		}
	buf[size] = '\0';
	return buf;
	}

#ifdef EKA2

static wchar_t* allocCommandLine(const TDesC& aPath)
	{
    TInt cmdLength = User::CommandLineLength()+1;	// for zero termination
	TText16* cmdbuf = (TText16*)User::Alloc(cmdLength*sizeof(TText16));
	if (cmdbuf==0)
		return (wchar_t*)cmdbuf;		// we are just doomed, so give up now

	TPtr16 cmdline(cmdbuf, cmdLength);
    User::CommandLine(cmdline);

	// The .EXE recogniser supplies a command line which is the name of the file,
	// followed by a space. This is usually not what's wanted, so remove the
	// filename if it is an exact match for the start of the command line.

	if (cmdline.Find(aPath)==0)
		{
		cmdline.Delete(0, aPath.Length());
		}
	cmdline.ZeroTerminate();
	return (wchar_t*)cmdbuf;
	}

#else//EKA2

static wchar_t* allocCommandLine(const TDesC& aPath)
	{
	RProcess me;
	TInt cmdLength = me.CommandLineLength()+1;	// for zero termination
	TText16* cmdbuf = (TText16*)User::Alloc(cmdLength*sizeof(TText16));
	if (cmdbuf==0)
		return (wchar_t*)cmdbuf;		// we are just doomed, so give up now

	TPtr16 cmdline(cmdbuf, cmdLength);
	me.CommandLine(cmdline);

	// The .EXE recogniser supplies a command line which is the name of the file,
	// followed by a space. This is usually not what's wanted, so remove the
	// filename if it is an exact match for the start of the command line.

	if (cmdline.Find(aPath)==0)
		{
		cmdline.Delete(0, aPath.Length());
		}
	cmdline.ZeroTerminate();
	return (wchar_t*)cmdbuf;
	}

#endif//EKA2

EXPORT_C void __crt0(int& argc, char**& argv, char**& envp)
	{
	wchar_t** wenvp = 0;
	int envcnt = 0;

	// Check to see if I was the product of a popen/popen3/posix_spawn/system op.
	Backend()->CheckOrigins(wenvp, envcnt);

	if (wenvp)
		{
		// Parent has passed in the an environ for the child. Update envp
		envp = (char **)User::Alloc((envcnt+1) * sizeof(char*));
		if (envp)
			{
			for (int i = 0; i < envcnt; ++i)
				{
				int len = wcslen(wenvp[i]) + 1;
				envp[i] = (char *)User::Alloc(len * sizeof(char));
				if (!envp[i] || (wcstombs(envp[i], wenvp[i], len) == (size_t)-1))
					{
					// if envp[i] is NULL, Free does nothing
					User::Free(envp[i]);
					envp[i] = 0;
					break;
					}
				}
			// set the terminal zero
			envp[envcnt] = 0;
			}

		// Cleanup wenvp allocated in CheckOrigins
		for (int i = 0; i < envcnt; ++i)
			{
			User::Free(wenvp[i]);
			}
		User::Free(wenvp);
		}
	else
		{
		// set envp from environ
		char** temp = environ;
		for (envcnt = 0; temp[envcnt]; ++envcnt) { }
		// account for the terminal zero in alloc
		envp = (char**)User::Alloc((envcnt+1) * sizeof(char*));
		if (envp)
			{
			for(int i = 0; i < envcnt; ++i)
				{
				int len = strlen(temp[i]) + 1;
				envp[i] = (char*)User::Alloc(len * sizeof(char));
				if (!envp[i])
					{
					envp[i] = NULL;
					break;
					}
				strncpy(envp[i], temp[i], len);
				}
			// set the terminal zero
			envp[envcnt] = NULL;
			}
		}

	// Find out the filename for argv[0]
	TBuf16<KMaxFileName+1> exepath(RProcess().FileName());

	// Sort out argc/argv, creating an array of pointers into a copy of
	// the commandline.

	wchar_t* cmdbuf = allocCommandLine(exepath);
	char* cmd = wcstombs_alloc(cmdbuf);
	User::Free(cmdbuf);
	char* filename = wcstombs_alloc((const wchar_t *)exepath.PtrZ());

	//Copy program name now so that it can be accessed by others
	__progname = filename;

	argv = (char**)calloc(KMaxArgC, sizeof(char*));
	if (argv==0 || cmd==0 || filename== 0)
		return;		// it's basically doomed at this point anyway

	//Check if there are redirection operators in the arguments
	char* ptr = strpbrk(cmd, "<>");
	if (ptr)
		{
		char* p1 = ptr, *p2;
		int opfd, ipfd;

		switch (*p1)
			{
		case '<':
			p2 = strchr(++p1, '>');
			if (p2)
				{
				*p2++ = 0;
				if (*p2 == '>')
					{
					opfd = open(++p2, O_WRONLY|O_CREAT|O_APPEND);
					}
				else
					{
					opfd = open(p2, O_WRONLY|O_CREAT|O_TRUNC);
					}

				if (opfd > 0)
					{
					dup2(opfd, 1);
					}
				}

			ipfd = open(p1, O_RDONLY);
			if (ipfd > 0)
				{
				dup2(ipfd, 0);
				}
			break;
		case '>':
			p2 = strchr(++p1, '<');
			if (p2)
				{
				*p2++ = 0;
				ipfd = open(p2, O_RDONLY);
				if (ipfd > 0)
					{
					dup2(ipfd, 0);
					}
				}

			if (*p1 == '>')
				{
				opfd = open(++p1, O_WRONLY|O_CREAT|O_APPEND);
				}
			else
				{
				opfd = open(p1, O_WRONLY|O_CREAT|O_TRUNC);
				}

			if (opfd > 0)
				{
				dup2(opfd, 1);
				}
			break;
		default:
			break;
			}

		// Ensure that only the string until the first redirection operator
		// is considered for command args processing
		*ptr = 0;
		}

	// Split the command line into the separate arguments
	// Follow the stdarg.c rules in the Win32 runtime, namely
	// 1. space and tab are whitespace separators, except inside "..." pairs
	// 2. strings of \ are literal unless followed by " (see below)
	// 3. a pair of "" in a quoted string is a literal "

	const char KSpace= ' ';
	const char KTab  = '\t';
	const char KQuote= '"';
	const char KSlash= '\\';

	argv[0]=filename;
	argc = 1;
	char *q = cmd;
	const char* p = cmd;
	int currentMaxArgc = KMaxArgC;
	FOREVER
		{
		char c;
		TInt quoted=0;

		// skip leading whitespace
		do
			c=*p++;
		while (c && (c==KSpace || c==KTab));

		// update the argv,argc
		if (c=='\0')
			break;
		else if(argc >= currentMaxArgc)
			{
			currentMaxArgc += KMaxArgC;
			argv = (char**)realloc(argv, currentMaxArgc * sizeof(char*));
			}

		argv[argc] = q;
		argc++;

		// copy the argument from p to q
		for (;c!='\0';c=*p++)
			{

			// Poor buggers have to cope with UNC filenames, e.g. \\host\dir\file
			// Hence the rather odd rules: for N>=0
			// 2N+1 slash + " => N slash + literal "
			// 2N   slash + " => N slash, start/end quoted substring
			// N    slash + ? => N slash + ?

			int slashcount=0;
			while (c==KSlash)
				{
				*q++=c;		// copy the slashes (might be too many)
				slashcount++;
				c=*p++;
				}
			if (c=='\0')
				break;
			if (c==KQuote)
				{
				q-=(slashcount-(slashcount/2));	// slashes followed by quote - adjust
				if (slashcount&1)
					{
					*q++=c;		// literal quote
					continue;
					}
				if (quoted && *p==KQuote)
					{
					p++;
					*q++=c;		// "" inside quoted section = literal quote
					continue;
					}
				quoted=!quoted;
				continue;
				}
			if (!quoted && (c==KSpace || c==KTab))
				break;
			*q++=c;
			}
		*q++='\0';	// terminate the copy

		if (c=='\0')
			break;	// end of command line
		}

	//If environ is not yet initialized for this Process
	SetupEnviron();
	}

EXPORT_C void __crt0(int& argc, wchar_t**& wargv, wchar_t**& wenvp)
	{
	int envcnt = 0;

	// Check to see if I was the product of a popen/popen3/posix_spawn/system op.
	Backend()->CheckOrigins(wenvp, envcnt);

	if (!wenvp)
		{
		// set wenvp from environ
		char** temp = environ;
		for (envcnt = 0; temp[envcnt]; ++envcnt) { }
		// account for the terminal zero during alloc
		wenvp = (wchar_t**)User::Alloc((envcnt+1) * sizeof(wchar_t*));
		if (wenvp)
			{
			for(int i = 0; i < envcnt; ++i)
				{
				int len = strlen(temp[i])+1;
				wenvp[i] = (wchar_t*)User::Alloc(len * sizeof(wchar_t));
				if (!wenvp[i] || (mbstowcs(wenvp[i], temp[i], len) == (size_t)-1))
					{
					// if wenvp[i] is NULL, Free does nothing
					User::Free(wenvp[i]);
					wenvp[i] = NULL;
					break;
					}
				}
				// set the terminal 0
			wenvp[envcnt] = NULL;
			}
		}

	// Find out the filename for argv[0]
	TBuf16<KMaxFileName+1> exepath(RProcess().FileName());

	// Sort out argc/argv, creating an array of pointers into a copy of
	// the commandline.

	wchar_t* cmd = allocCommandLine(exepath);
	//coverity[alloc_fn]
	//coverity[assign]
	wchar_t* filename = wcsdup((const wchar_t *)exepath.PtrZ());
	wargv = (wchar_t**)calloc(KMaxArgC, sizeof(wchar_t*));

	if (wargv==0 || cmd==0 || filename==0)
	//coverity[memory_leak]
		return;		// it's basically doomed at this point anyway

	// Split the command line into the separate arguments
	// Follow the stdarg.c rules in the Win32 runtime, namely
	// 1. space and tab are whitespace separators, except inside "..." pairs
	// 2. strings of \ are literal unless followed by " (see below)
	// 3. a pair of "" in a quoted string is a literal "

	const wchar_t KSpace= L' ';
	const wchar_t KTab  = L'\t';
	const wchar_t KQuote= L'"';
	const wchar_t KSlash= L'\\';

	wargv[0]=filename;
	argc = 1;
	wchar_t *q = cmd;
	wchar_t *p = cmd;
	int currentMaxArgc = KMaxArgC;
	FOREVER
		{
		wchar_t c;
		TInt quoted=0;

		// skip leading whitespace
		do
			c=*p++;
		while (c && (c==KSpace || c==KTab));

		// update the argv,argc
		if (c=='\0')
			break;
		else if(argc >= currentMaxArgc)
			{
			currentMaxArgc += KMaxArgC;
			wargv = (wchar_t**)realloc(wargv, currentMaxArgc * sizeof(wchar_t*));
			}

		wargv[argc] = q;
		argc++;

		// copy the argument from p to q
		for (;c!=L'\0';c=*p++)
			{

			// Poor buggers have to cope with UNC filenames, e.g. \\host\dir\file
			// Hence the rather odd rules: for N>=0
			// 2N+1 slash + " => N slash + literal "
			// 2N   slash + " => N slash, start/end quoted substring
			// N    slash + ? => N slash + ?

			int slashcount=0;
			while (c==KSlash)
				{
				*q++=c;		// copy the slashes (might be too many)
				slashcount++;
				c=*p++;
				}
			if (c==L'\0')
				break;
			if (c==KQuote)
				{
				q-=(slashcount-(slashcount/2));	// slashes followed by quote - adjust
				if (slashcount&1)
					{
					*q++=c;		// literal quote
					continue;
					}
				if (quoted && *p==KQuote)
					{
					p++;
					*q++=c;		// "" inside quoted section = literal quote
					continue;
					}
				quoted=!quoted;
				continue;
				}
			if (!quoted && (c==KSpace || c==KTab))
				break;
			*q++=c;
			}
		*q++=L'\0';	// terminate the copy

		if (c==L'\0')
			break;	// end of command line
		}

	//If environ is not yet initialized for this Process
	SetupEnviron();
	GetTmpDirName();
	}
