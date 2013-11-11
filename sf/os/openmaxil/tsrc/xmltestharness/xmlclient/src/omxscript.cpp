/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>			// Console
#include <e32debug.h>
#include <multimedia/omx_xml_script.h>

//  Constants

_LIT(KTextConsoleTitle, "Console");
_LIT(KTextFailed, " failed, leave code = %d");
_LIT(KTextPressAnyKey, " [press any key]\n");

//  Global Variables

LOCAL_D CConsoleBase* console; // write all messages to this


//  Local Functions

/**
 * Logs script output to console and RDebug.
 */
class TConsoleLogger : public MOmxScriptTestLogger
	{
	void Log(const TText8* aFile, TInt aLine, TOmxScriptSeverity /*aSeverity*/, const TDes& aMessage)
		{
		TPtrC8 fileDes8(aFile);
		TBuf<255> fileDes;
		fileDes.Copy(fileDes8);
		console->Printf(_L("%S:%d %S\n"), &fileDes, aLine, &aMessage);
		RDebug::Print(_L("%S:%d %S"), &fileDes, aLine, &aMessage);
		}
	};

LOCAL_C void ShowUsage()
	{
	console->Write(_L("Usage: omxscript <filename> <section>\n"));
	}

LOCAL_C TInt ParseSize(const TDesC& aDes)
	{
	TLex lex(aDes);
	TInt val;
	if(lex.Val(val) != KErrNone || val < 0)
		{
		return KErrArgument;
		}
	switch(lex.Get())
		{
	case 0:	// no modifier
		break;
	case 'K':
		val <<= 10;
		break;
	case 'M':
		val <<= 20;
		break;
	default:	// bad modifier
		return KErrArgument;
		}
	if(lex.Get() != 0)
		{
		// trailing text
		return KErrArgument;
		}
	return val;
	}

/**
 * Extracts parameters from the command line.
 * This method must not use the cleanup stack; there may not be one allocated since we may be switching heaps.
 */
LOCAL_C TInt ParseCommandLineArgs(TDes& commandLine, TPtrC& aFilename, TPtrC& aSection, TInt &aHeapSize)
	{
	// copy the command line
	if(User::CommandLineLength() > commandLine.MaxLength())
		{
		return KErrTooBig;
		}
	User::CommandLine(commandLine);
	
	// parse filename, section and other args from the command line
	TInt heapSize = KErrNotFound;
	TPtrC filename(KNullDesC);
	TPtrC section(KNullDesC);
	TLex lex(commandLine);
	lex.SkipSpaceAndMark();
	while(!lex.Eos())
		{
		lex.SkipCharacters();
		TPtrC arg = lex.MarkedToken();
		lex.SkipSpaceAndMark();
		if(arg == _L("-heap"))
			{
			if(lex.Eos())
				{
				// no param following
				return KErrArgument;
				}
			lex.SkipCharacters();
			TPtrC heapArg = lex.MarkedToken();
			lex.SkipSpaceAndMark();
			heapSize = ParseSize(heapArg);
			if(heapSize == KErrArgument)
				{
				return KErrArgument;
				}
			}
		else if(filename.Length() == 0)
			{
			filename.Set(arg);
			}
		else if(section.Length() == 0)
			{
			section.Set(arg);
			}
		else
			{
			// to many unnamed params
			return KErrArgument;
			}
		}
	if(section.Length() == 0)
		{
		return KErrArgument;
		}
	aHeapSize = heapSize;
	aFilename.Set(filename);
	aSection.Set(section);
	return KErrNone;
	}

LOCAL_C void MainL()
	{
	RBuf commandLine;
	commandLine.CreateL(User::CommandLineLength());
	CleanupClosePushL(commandLine);
	TPtrC filename(KNullDesC);
	TPtrC section(KNullDesC);
	TInt heapSize = KErrNotFound;
	TInt error = ParseCommandLineArgs(commandLine, filename, section, heapSize);
	if(error)
		{
		ShowUsage();
		User::Leave(error);
		}
	
	//logs script output to console and RDebug
	TConsoleLogger logger;
	
	COmxXmlScript* script = COmxXmlScript::NewL(logger);
	CleanupStack::PushL(script);
	script->RunScriptL(filename, section);
	CleanupStack::PopAndDestroy(2, &commandLine);
	}

LOCAL_C void DoStartL()
	{
	// Create active scheduler (to run active objects)
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	MainL();

	// Delete active scheduler
	CleanupStack::PopAndDestroy(scheduler);
	}

/**
 * Invokes ParseCommandLineArgs() to retrieve any heap size specified on the command line.
 * The command line descriptor is allocated on the stack (since we are avoiding heap allocations at this point).
 * The descriptors are then thrown away (to avoid consuming too much stack space).
 * Later, the command line will be parsed again but on the heap.
 */
LOCAL_C TInt ParseHeapSize()
	{
	TInt heapSize = KErrNotFound;
	TPtrC filename(KNullDesC);
	TPtrC section(KNullDesC);
	TBuf<255> commandLine;
	// ignore error
	ParseCommandLineArgs(commandLine, filename, section, heapSize);
	return heapSize;
	}

//  Global Functions

GLDEF_C TInt E32Main()
	{
	

	TInt heapSize = ParseHeapSize();


	// switch heap if specified
	RHeap* oldHeap = NULL;
	RHeap* newHeap = NULL;

	if(heapSize != KErrNotFound)
		{
		const TInt KMinHeapGrowBy = 1;
		const TInt KByteAlignment = 0;
		const TBool KSingleThreaded = EFalse;
		newHeap = User::ChunkHeap(NULL, heapSize, heapSize, KMinHeapGrowBy, KByteAlignment, KSingleThreaded);
		if(newHeap == NULL)
			{
			return KErrNoMemory;
			}
		oldHeap = User::SwitchHeap(newHeap);		
		}

	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	// Create output console
	TRAPD(createError, console = Console::NewL(KTextConsoleTitle, TSize(KConsFullScreen,KConsFullScreen)));
	if (createError)
	return createError;
	
	// Run application code inside TRAP harness, wait keypress when terminated
	TRAPD(mainError, DoStartL());
	

	if(mainError)
		{
		console->Printf(KTextFailed, mainError);
		}
	console->Printf(KTextPressAnyKey);
	console->Getch();

	delete console;
	delete cleanup;
	__UHEAP_MARKEND;
	
	if(newHeap != NULL)
		{
		User::SwitchHeap(oldHeap);
		newHeap->Close();
		}

	return KErrNone;
	}

