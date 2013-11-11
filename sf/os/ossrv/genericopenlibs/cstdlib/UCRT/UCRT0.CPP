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
#include <estlib.h>

#include <unistd.h>	// for chdir
#include <stdlib.h>	// for calloc
#include <string.h>	// for strdup

const TInt KMaxArgC=25;

static char* wcstombs_alloc (const wchar_t* aString)
	{
	if (aString==NULL)
		return NULL;

	size_t size = wcstombs(0, aString, 0);
	char* buf = (char*)malloc(size * sizeof(char));

	size = wcstombs(buf, aString, size);
	if (size == (size_t) -1)
		{
		free(buf);
		return NULL;
		}
	buf = (char*)realloc(buf, (size+1) * sizeof(char));
	return buf;
	}

#ifdef EKA2

static wchar_t* allocCommandLine(const TDesC& aPath)
	{
    TInt cmdLength = User::CommandLineLength()+1;	// for zero termination
	TText16* cmdbuf = (TText16*)malloc(cmdLength*sizeof(TText16));
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
	TText16* cmdbuf = (TText16*)malloc(cmdLength*sizeof(TText16));
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
	// Find out the filename for argv[0]

	TBuf16<KMaxFileName+1> exepath(RProcess().FileName());

	// Sort out argc/argv, creating an array of pointers into a copy of
	// the commandline.

	wchar_t* cmdbuf = allocCommandLine(exepath);
	char* cmd = wcstombs_alloc(cmdbuf);
	free(cmdbuf);
	char* filename = wcstombs_alloc((const wchar_t *)exepath.PtrZ());

	argv = (char**)calloc(KMaxArgC, sizeof(char*));
		
	// Check for memory allocation failures.
	if (argv==0 || cmd==0 || filename== 0)
		{
		// Free any memory that could have been allocated before returning.
		free(cmd);
		free(filename);
		return;		// it's basically doomed at this point anyway	
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
	FOREVER
		{
		char c;
		TInt quoted=0;

		// skip leading whitespace
		do	
			c=*p++;
		while (c && (c==KSpace || c==KTab));

		// update the argv,argc
		if (c=='\0' || argc>=KMaxArgC)
			break;

		argv[argc] = q;
		argc++;

		// copy the argument from p to q
		for (;c!='\0';c=*p++)
			{

			// The UNC filenames format used, e.g. \\host\dir\file
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

	// sort out the environment

	envp=0;
	}

EXPORT_C void __crt0(int& argc, wchar_t**& wargv, wchar_t**& wenvp)
	{     
	// Find out the filename for argv[0]

	TBuf16<KMaxFileName+1> exepath(RProcess().FileName());

	// Sort out argc/argv, creating an array of pointers into a copy of
	// the commandline.

	wchar_t* cmd = allocCommandLine(exepath);
	wchar_t* filename = wcsdup((const wchar_t *)exepath.PtrZ());
	wargv = (wchar_t**)calloc(KMaxArgC, sizeof(wchar_t*));
	
	// Check for memory allocation failures.
	if (wargv==0 || cmd==0 || filename== 0)
		{
		// Free any memory that could have been allocated before returning.
		free(cmd);
		free(filename);
		return;		// it's basically doomed at this point anyway	
		}

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
	FOREVER
		{
		wchar_t c;
		TInt quoted=0;

		// skip leading whitespace
		do	
			c=*p++;
		while (c && (c==KSpace || c==KTab));

		// update the argv,argc
		if (c=='\0' || argc>=KMaxArgC)
			break;

		wargv[argc] = q;
		argc++;

		// copy the argument from p to q
		for (;c!=L'\0';c=*p++)
			{

			// The UNC filenames format used, e.g. \\host\dir\file
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

	// sort out the environment

	wenvp=0;
	}
