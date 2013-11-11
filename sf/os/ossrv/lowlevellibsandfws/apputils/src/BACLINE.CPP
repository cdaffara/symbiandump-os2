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
//

#include <bacline.h>


EXPORT_C CCommandLineArguments* CCommandLineArguments::NewLC()
/** Allocates and constructs a command line arguments parser, putting the returned 
pointer onto the cleanup stack. The function leaves if there is insufficient 
memory.

@return The command line arguments parser. */
	{
	CCommandLineArguments* self=new (ELeave) CCommandLineArguments;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CCommandLineArguments* CCommandLineArguments::NewL()
/** Allocates and constructs a command line arguments parser. The function leaves 
if there is insufficient memory.

@return The command line arguments parser. */
	{
	CCommandLineArguments* self=CCommandLineArguments::NewLC();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CCommandLineArguments::~CCommandLineArguments()
/** Frees resources prior to destruction. */
	{
	delete iArgs;
	delete iCommandLine;
	}

CCommandLineArguments::CCommandLineArguments()
	{
	}

void CCommandLineArguments::ConstructL()
	{
	// allocate args array
	iArgs=new (ELeave) CArrayFixFlat<TPtrC> (10);
	// get raw command line
	RProcess me;
	iCommandLine=HBufC::NewL(User::CommandLineLength());
	TPtr commandLine(iCommandLine->Des());
	User::CommandLine(commandLine);
	iFileName=me.FileName();
	// scan for each argument
	TText* out=CONST_CAST(TText*,iCommandLine->Ptr());
	const TText* scan=out;
	const TText* end=scan+iCommandLine->Length();
	while (scan < end) // scan one argument
		{
		while (scan < end && *scan==' ') // skip leading space
			scan++;
		if (scan == end) // ignore if blank
			break;
		TBool quoted=*scan=='\"'; // note leading quote
		if (quoted)
			scan++;
		TText* start=out; // note start in output
		if (!quoted) // if not quoted, scan for blank
			{
			while (scan < end && *scan!=' ')
				*out++ = *scan++;
			}
		else // quoted, scan for quote
			{
			for (;;) // one quote-or-double sequence
				{
				while (scan < end && *scan!='\"') // all up to quote
					*out++ = *scan++;
				if (scan < end) // skip quote
					scan++;
				if (scan < end && *scan=='\"') // transfer if quote is doubled
					*out++ = *scan++;
				else // finished this arg
					break;
				}
			}
		TPtrC arg(start, out-start);
		iArgs->AppendL(arg);
		}
	}


EXPORT_C TPtrC CCommandLineArguments::Arg(TInt aArg) const
/** Returns a non-modifiable pointer descriptor representing the specified command-line 
argument.

Arg(0) is the file name as specified on the command line. Arg(1), Arg(2) etc. 
are the arguments specified to the command.

The pointer descriptor is valid throughout the lifetime of the CCommandLineArguments 
object. If you wish to retain argument values after the CCommandLineArguments 
object is destroyed, you should copy the argument data into a different object.

@param aArg The index of the desired argument. This number must be less than 
Count(). Specify 0 for the name used to invoke the process. Specify 1, 2 etc. 
for the arguments.
@return Non-modifiable pointer descriptor to the specified argument text. */
	{
	if (aArg > 0 ) // a normal argument
		return iArgs->operator[](aArg-1);
	else // process name
		return TPtrC(iFileName);
	}

EXPORT_C TInt CCommandLineArguments::Count() const
/** Returns the number of command line arguments, including the program name.

@return The number of command line arguments, plus one for the program name. 
Returns 1, if no arguments are specified. */
	{
	return iArgs->Count()+1;
	}
