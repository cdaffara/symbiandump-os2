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

#include "MemSpyCommandLine.h"

// System includes
#include <bacline.h>
#include <bautils.h>
#include <memspyengineclientinterface.h>
#include <memspysession.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>

// User includes
#include "MemSpyCommands.h"

/*
 CMemSpyCommandLine::CMemSpyCommandLine()
 {	
 }
 */

CMemSpyCommandLine::CMemSpyCommandLine( CConsoleBase& aConsole )
	 :CActive(EPriorityHigh), iConsole(aConsole)
	{
	CActiveScheduler::Add(this);
	}

CMemSpyCommandLine::~CMemSpyCommandLine()
	{
	Cancel();

	if (iMemSpySession)
		{
		iMemSpySession->Close();
		}
	delete iMemSpySession;
	iFsSession.Close();
	}

void CMemSpyCommandLine::ConstructL()
	{
	User::LeaveIfError(iFsSession.Connect());
	iMemSpySession = new (ELeave) RMemSpySession();
	ConnectToMemSpyL();
	}

CMemSpyCommandLine* CMemSpyCommandLine::NewLC(CConsoleBase& aConsole)
	{
	CMemSpyCommandLine* self = new (ELeave) CMemSpyCommandLine(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CMemSpyCommandLine::PerformOpL(const CCommandLineArguments& aCommandLine)
	{
	const TInt count = aCommandLine.Count();
	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::PerformOpL() - START - arg count: %d, this: 0x%08x", count, this ) );

	if (count > 1)
		{
		// Get main command
		TBuf<KMemSpyCommandLineMaxLength> command;
		command.Copy(aCommandLine.Arg(1));
		command.UpperCase();

		// Extract arguments into array
		CDesCArrayFlat* args = new (ELeave) CDesCArrayFlat(4);
		CleanupStack::PushL(args);
		for (TInt i = 2; i < count; i++)
			{
			args->AppendL(aCommandLine.Arg(i));
			}

		// Perform op
		PerformSingleOpL(command, *args);

		// Tidy up
		CleanupStack::PopAndDestroy(args);
		}
	else if ( count == 1 )
		{
		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::PerformOpL() - nno commands and parameters set" ) );
		}
	else
		{
		User::Leave(KErrUnderflow);
		}
	}

void CMemSpyCommandLine::PerformSingleOpL(const TDesC& aCommand, const CDesCArray& aParameters)
	{
	// Record duration
	TTime timeStart;
	timeStart.HomeTime();
	const TInt paramCount = aParameters.Count();

#ifdef _DEBUG
	TRACE( RDebug::Print( _L("[MemSpyCmdLine] CMemSpyCommandLine::PerformSingleOpL() - START - command: %S, paramCount: %d, this: 0x%08x"), &aCommand, paramCount, this ) );
	for (TInt i = 0; i < paramCount; i++)
		{
		const TPtrC pParam(aParameters[i]);
		TRACE( RDebug::Print( _L("[MemSpyCmdLine] CMemSpyCommandLine::PerformSingleOpL() - param[%02d] = [%S]"), i, &pParam ) );
		}
#else
	RDebug::Print( _L("[MemSpyCmdLine] COMMAND: [%S] {%02d}..."), &aCommand, paramCount );
#endif

	TFileName batchFile;
	batchFile.Append(aCommand);

	TInt err = KErrNotSupported;
	TInt error = KErrNotSupported;

	// --- HELP
	if ( aCommand.CompareF( KMemSpyCmdHelp1) == 0 || 
		aCommand.CompareF(KMemSpyCmdHelp2) == 0 || 
		aCommand.CompareF(KMemSpyCmdHelp3) == 0 || 
		aCommand.CompareF(KMemSpyCmdHelp4) == 0)
		{
		iConsole.Write(KHelpMessage);
		iConsole.Write(KMemSpyCLINewLine);
		iConsole.Write(KHelpOutputCommand);
		iConsole.Write(KHelpOutputToFileCommand);
		iConsole.Write(KHelpHeapDumpCommand);
		iConsole.Write(KHelpSwmtCommand);
		iConsole.Write(KHelpKillServerCommand);
		iConsole.Write(KMemSpyCLINewLine);
		iConsole.Write(KHelpCommand);

		// Show input prompt.
		iCommandPromptPos = iConsole.CursorPos();
		RedrawInputPrompt();
		WaitForInput();

		CActiveScheduler::Start();
		}
	// --- OUTPUT
	//TODO: directory option to be added
	else if (aCommand.CompareF(KMemSpyCmdOutput) == 0) //change output mode   
		{
		if (paramCount >= 1)
			{
			if (aParameters[0].CompareF(KMemSpyCmdOutputParameterFile) == 0)
				{
				if (paramCount == 2)
					{
					TBuf<KMaxFileName> directory;
					directory.Copy(aParameters[1]);
					iMemSpySession->SwitchOutputToFileL(directory);
					}
				else
					{
					iMemSpySession->SwitchOutputToFileL(KNullDesC);
					}
				}
			else if( aParameters[0].CompareF( KMemSpyCmdOutputParameterTrace ) == 0)
				{
				TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::PerformSingleOpL() - this: 0x%08x - Output Trace", this ) );
				iMemSpySession->SwitchOutputToTraceL();
				}
			}
		}
	// --- HEAP DUMP    
	else if (aCommand.CompareF(KMemSpyCmdHeapDump) == 0)
		{
		RedrawStatusMessage(KHeapDumpMessage);

		if (paramCount == 0) // no parameter - dump all heap data + kernel heap at the end
			{

			TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::PerformSingleOpL() - this: 0x%08x - Heap_Dump (all threads)", this ) );
			// Dump heap data for all threads - Thread agnostic operation					
			iMemSpySession->OutputHeapData();
			// Dump kernel heap data
			iMemSpySession->OutputThreadHeapDataL( KMemSpyClientServerThreadIdKernel );					
			}
		else if (paramCount >= 1)
			{
			if (aParameters[0].CompareF(KMemSpyCmdHeapDumpParameterAll) == 0)
				{
				iMemSpySession->OutputHeapData();
				iMemSpySession->OutputThreadHeapDataL( KMemSpyClientServerThreadIdKernel );				
				}
			else if( aParameters[0].CompareF( KMemSpyCmdHeapDumpParameterKernel ) == 0 )
				{
				TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::PerformSingleOpL() - this: 0x%08x - Heap_DumpKernel", this ) );
				iMemSpySession->OutputThreadHeapDataL( KMemSpyClientServerThreadIdKernel );				
				}
			else
				{
				// Dump heap data for named thread - filter
				const TPtrC pThreadName(aParameters[0]);
				TRACE( RDebug::Print( _L("[MemSpyCmdLine] CMemSpyCommandLine::PerformSingleOpL() - this: 0x%08x - Heap_Dump (%S)"), this, &pThreadName ) );
				iMemSpySession->OutputThreadHeapDataL(pThreadName);
				}
			}
		}

	// --- SYSTEM WIDE MEMORY TRACKING    
	else if (aCommand.CompareF(KMemSpyCmdSwmt) == 0)
		{
		RedrawStatusMessage(KSWMTMessage);

		TInt categories(0);
		TName threadNameFilter;

		if (paramCount == 0) //default state -> "dumpnow" command with "all" categories
			{
			TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::PerformSingleOpL() - this: 0x%08x - dumpnow command", this ) );
			TInt category = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryAll;
			iMemSpySession->SetSwmtCategoriesL(category);
			iMemSpySession->ForceSwmtUpdateL();
			}
		else if (paramCount >= 1)
			{
			const TPtrC pParam(aParameters[0]);
			if (pParam.CompareF(KMemSpyCmdSwmtParameterStarttimer) == 0) // "starttimer" - start tracking
				{
				TInt result(0);
				categories = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryAll;
				iMemSpySession->SetSwmtTimerIntervalL(KMemSpySysMemTrackerConfigMinTimerPeriod);

				if (paramCount >= 2) // user gave some optional parameters - <categories> or <value in seconds>
					{
					TLex lex(aParameters[1]);
					if (lex.Val(result) == KErrNone) //if 2nd parameter is not number, then parse parameters
						{
						if( result >= KMemSpySysMemTrackerConfigMinTimerPeriod && result <= KMemSpySysMemTrackerConfigMaxTimerPeriod )
							{
							iMemSpySession->SetSwmtTimerIntervalL( result );							
							}
						}
					TRAP( err, ParseSWMTParametersL( aParameters, categories, threadNameFilter) );
					}

				//if( !err )
				//	{
				/*
				 _LIT( KPressS, "Press 's' to stop the timer " );
				 iConsole.Write( KPressS );
				 
				 iCommandPromptPos = iConsole.CursorPos();					
				 RedrawInputPrompt();					
				 WaitForInput();
				 */

				iMemSpySession->StartSwmtTimerL();

				//CActiveScheduler::Start();									
				//	}	
				}
			else if (pParam.CompareF(KMemSpyCmdSwmtParameterStoptimer) == 0) // "stoptime" - stop tracking
				{
				iMemSpySession->StopSwmtTimerL();
				}
			else if (pParam.CompareF(KMemSpyCmdSwmtParameterDumpnow) == 0) // "dumpnow" - runs one tracking cycle (CmdSWMT_ForceUpdate before)
				{
				categories = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryAll;
				if (paramCount >= 2) // user gave some optional parameters - <categories>
					{
					TRAP( err, ParseSWMTParametersL( aParameters, categories, threadNameFilter) );
					}

				if (!err)
					{
					iMemSpySession->SetSwmtCategoriesL(categories);
					iMemSpySession->ForceSwmtUpdateL();
					}
				}
			else //no parameters ("starttimer / stoptimer / dumpnow"), just categories / thread filter
			//so dumpnow is used as default with category / thread specified
				{
				TRAP( err, ParseSWMTParametersL( aParameters, categories, threadNameFilter) );
				if (!err)
					{
					iMemSpySession->SetSwmtCategoriesL(categories);
					if (threadNameFilter.Length() > 0)
						{
						iMemSpySession->SetSwmtFilter(threadNameFilter);
						}
					}
				iMemSpySession->ForceSwmtUpdateL();
				}
			}
		}
	// --- KILL SERVER
	else if (aCommand.CompareF(KMemSpyCmdKillServer) == 0)
		{
		}

	// RedrawStatusMessage();   

	TRACE( RDebug::Print( _L("[MemSpyCmdLine] CMemSpyCommandLine::PerformSingleOpL() - END - err: %d, this: 0x%08x, cmd: %S" ), err, this, &aCommand ) );

	// Calculate duration
	TTime timeEnd;
	timeEnd.HomeTime();
	TTimeIntervalSeconds interval(0);
	timeEnd.SecondsFrom(timeStart, interval);

	// Print some info
	if (err != KErrNone)
		{
		RDebug::Print( _L("[MemSpyCmdLine] COMMAND: [%S] {%02d} {%08d sec} => ERROR: %d"), &aCommand, paramCount, interval.Int(), err );
		}
	else
		{
		RDebug::Print( _L("[MemSpyCmdLine] COMMAND: [%S] {%02d} {%08d sec} => OK"), &aCommand, paramCount, interval.Int() );
		}

	// Spacer
	RDebug::Printf(" ");
	}

void CMemSpyCommandLine::ConnectToMemSpyL()
	{
	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ConnectToMemSpyL() - START - this: 0x%08x", this ) );

	TInt err = iMemSpySession->Connect();
	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ConnectToMemSpyL() - connect #1 err: %d, this: 0x%08x", err, this ) );

	if (err == KErrNotFound)
		{
		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ConnectToMemSpyL() - launching MemSpy... - this: 0x%08x", this ) );
		LaunchMemSpyL();

		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ConnectToMemSpyL() - launched MemSpy - this: 0x%08x", this ) );
		err = iMemSpy->Connect();

		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ConnectToMemSpyL() - connect #2 err: %d, this: 0x%08x", err, this ) );
		}

	User::LeaveIfError(err);

	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ConnectToMemSpyL() - END - ok - this: 0x%08x", this ) );
	}

void CMemSpyCommandLine::LaunchMemSpyL()
	{
	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::LaunchMemSpyL() - START - this: 0x%08x", this ) );

	TInt err = KErrGeneral;
	RProcess proc;

	// Try to run server first
	err = proc.Create(KMemSpyProcessName0, KNullDesC);
	if (err == KErrNone)
		{
		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::LaunchMemSpyL() - Create server process successfully... - this: 0x%08x", this ) );

		TRequestStatus status;
		proc.Rendezvous(status);
		proc.Resume();

		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::LaunchMemSpyL() - MemSpy resumed, waiting for Rendezvous... - this: 0x%08x", this ) );

		User::WaitForRequest(status);
		err = status.Int();
		proc.Close();

		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::LaunchMemSpyL() - Rendezvous complete: %d, this: 0x%08x", err, this ) );
		}

	// If server is not available, try with s60 UI
	if (err != KErrNone)
		{
		err = proc.Create(KMemSpyProcessName1, KNullDesC);
		if (err == KErrNone)
			{
			TFullName fullName;
			proc.FullName(fullName);
			TRACE( RDebug::Print( _L("[MemSpyCmdLine] CMemSpyCommandLine::LaunchMemSpyL() - Create S60 UI process successfully... - this: 0x%08x, name: %S"), this, &fullName ) );

			TRequestStatus status;
			proc.Rendezvous(status);
			proc.Resume();

			TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::LaunchMemSpyL() - MemSpy resumed, waiting for Rendezvous... - this: 0x%08x", this ) );
			User::WaitForRequest(status);
			err = status.Int();
			proc.Close();

			TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::LaunchMemSpyL() - Rendezvous complete: %d, this: 0x%08x", err, this ) );
			}
		}

	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::LaunchMemSpyL() - final error: %d, this: 0x%08x", err, this ) );
	User::LeaveIfError(err);
	User::After(10 * 1000000);

	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::LaunchMemSpyL() - END - ok - this: 0x%08x", this ) );
	}

CDesCArray* CMemSpyCommandLine::ReadLinesL(RFile& aFile)
	{
	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ReadLinesL() - START - this: 0x%08x", this ) );
	CDesCArrayFlat* lines = new (ELeave) CDesCArrayFlat(10);
	CleanupStack::PushL(lines);

	TInt size = 0;
	User::LeaveIfError(aFile.Size(size));
	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ReadLinesL() - this: 0x%08x, file size: %d", this, size ) );

	// Read file
	HBufC8* narrowBuf = HBufC8::NewLC(size);
	TPtr8 pBufNarrow(narrowBuf->Des());
	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ReadLinesL() - this: 0x%08x, reading file..." ) );
	User::LeaveIfError(aFile.Read(pBufNarrow));
	if (pBufNarrow.Length() <= 2)
		{
		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ReadLinesL() - this: 0x%08x - BAD FILE LENGTH", this ) );
		User::Leave(KErrCorrupt);
		}

	// Look for BOM and convert to unicode
	HBufC* unicodeText = HBufC::NewL(size);
	TPtr pUnicodeText(unicodeText->Des());
	if (pBufNarrow[0] == 0xFF && pBufNarrow[1] == 0xFE)
		{
		// It's unicode... and we don't want the BOM, hence -2
		const TInt textLength = size - 2;
		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ReadLinesL() - this: 0x%08x - UNICODE TEXT - textLength: %d", this, textLength / 2 ) );
		Mem::Copy( (TUint8 *)pUnicodeText.Ptr(), pBufNarrow.Ptr() + 2, textLength );
		pUnicodeText.SetLength(textLength / 2);
		}
	else
		{
		// It's ASCII, convert it to unicode...
		TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ReadLinesL() - this: 0x%08x - ASCII TEXT - textLength: %d", this, size ) );
		for (TInt i = 0; i < size; i++)
			{
			pUnicodeText.Append(pBufNarrow[i]);
			}
		}

	// Discard old narrow text
	CleanupStack::PopAndDestroy(narrowBuf);
	CleanupStack::PushL(unicodeText);

	// Try to extract lines of text...
	HBufC* line = HBufC::NewLC(1024);
	TPtr pLine(line->Des());
	const TInt length = unicodeText->Length();
	for (TInt i = 0; i < length; i++)
		{
		const TChar c(pUnicodeText[i]);
		//
		if (c == '\r' || c == '\n')
			{
			pLine.Trim();
			if (pLine.Length())
				{
				TRACE( RDebug::Print( _L("[MemSpyCmdLine] CMemSpyCommandLine::ReadLinesL() - this: 0x%08x - LINE[%03d] %S"), this, lines->Count(), line ) );
				lines->AppendL(pLine);
				}

			pLine.Zero();
			}
		else
			{
			pLine.Append(c);
			}
		}

	// Save last line, just in cae it didn't end with a CR/LF
	pLine.Trim();
	if (pLine.Length())
		{
		TRACE( RDebug::Print( _L("[MemSpyCmdLine] CMemSpyCommandLine::ReadLinesL() - this: 0x%08x - LINE[%03d] %S"), this, lines->Count(), line ) );
		lines->AppendL(pLine);
		}

	CleanupStack::PopAndDestroy(2, unicodeText); // line & unicodeText
	CleanupStack::Pop(lines);
	TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine::ReadLinesL() - END - this: 0x%08x", this ) );

	return lines;
	}

void CMemSpyCommandLine::ParseSWMTParametersL(const CDesCArray& aParameters,TInt& aCategories, TDes& aFilter)
	{
	TInt result(0);
	// Check if the first parameter is a number.
	// In that case other parameters are ignored.
	TLex lex(aParameters[0]);
	if (lex.Val(result) != KErrNone)
		{
		// Parameters were given in text form:
		const TInt count(aParameters.Count());
		for (TInt i = 0; i < count; i++)
			{
			lex = aParameters[i]; //check if num.
			if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeHeap) == 0)
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap;
			//| TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHeap;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeChunk) == 0)
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryLocalChunks
								| TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalChunks;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeCode) == 0)
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMLoadedCode;
			//else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeStack) == 0)
			//	result
			//			|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserStacks;
			//else if ( aParameters[i].CompareF( KMemSpyCmdSWMTTypeGlobalData ) == 0 )
			//    result |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalData;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeRamDrive) == 0)
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMDrive;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeOpenFile) == 0)
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryOpenFiles;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeDiskSpace) == 0)
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryDiskusage;
			//else if ( aParameters[i].CompareF( KMemSpyCmdSWMTTypeFbserv ) == 0 ) // enables both FABS and BITM 
			//    result |= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryBitmapHandles;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeHandleGeneric)
					== 0) // enables both HGEN and HPAS
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHandles;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeFileServerCache)
					== 0)
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryFileServerCache;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeSystemMemory)
					== 0)
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategorySystemMemory;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeWindowGroup)
					== 0)
				result
						|= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryWindowGroups;
			else if (aParameters[i].CompareF(KMemSpyCmdSWMTTypeAll) == 0) //"all" category added
				result
						= TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryAll;
			else if (aParameters[i].CompareF(KMemSpyCmdSwmtParameterDumpnow)
					== 0 || aParameters[i].CompareF(
					KMemSpyCmdSwmtParameterStarttimer) == 0
					|| aParameters[i].CompareF(KMemSpyCmdSwmtParameterStoptimer)
							== 0)
				{
				TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine:: command parameter") );
				}
			else if (lex.Val(result) == KErrNone)
				{
				TRACE( RDebug::Printf( "[MemSpyCmdLine] CMemSpyCommandLine:: number - timer period") );
				}
			else// if ( aParameters[i].Find( KMemSpyCmdSWMTTypeHeapFilter ) == 0 )
				{
				aFilter.Copy(aParameters[i].Right(aParameters[i].Length() - 11));
				}
			/*  else
			 {
			 //User::Leave( KErrNotSupported );            	            
			 }*/
			}
		}
	else if (aParameters.Count() > 1)//&& aParameters[1].Find( KMemSpyCmdSWMTTypeHeapFilter ) == 0 )
		{
		aFilter.Copy(aParameters[1].Right(aParameters[1].Length() - 11));
		}
	aCategories = result;
	}

TInt CMemSpyCommandLine::FindBatchFile(TDes &aFileName)
	{
	if ( BaflUtils::FileExists( iFsSession, aFileName )) return KErrNone;
	if ( !FindFile( aFileName, _L("\\") ) ) return KErrNone;
	if ( !FindFile( aFileName, _L("\\data\\") ) ) return KErrNone;
	if (!FindFile(aFileName, _L("\\documents\\"))) return KErrNone;
	if (!FindFile(aFileName, _L("\\temp\\"))) return KErrNone;
	if (!FindFile(aFileName, _L("\\system\\temp\\"))) return KErrNone;
	// if ( !FindFile( aFileName, _L("\\private\\1000484b\\") ) ) return KErrNone; // Mail folder KErrPermissionDenied  
	return KErrNotFound;
	}

TInt CMemSpyCommandLine::FindFile(TDes &aFileName, const TDesC &aDirPath)
	{
	TFindFile fileFinder(iFsSession);
	TInt err = fileFinder.FindByDir(aFileName, aDirPath);
	if (!err)
		{
		aFileName.Copy(fileFinder.File());
		}
	return err;
	}

//CLI status messages methods
void CMemSpyCommandLine::RedrawInputPrompt()
	{
	iConsole.SetCursorPosAbs(iCommandPromptPos);
	iConsole.ClearToEndOfLine();
	iConsole.Printf(KMemSpyCLIInputPrompt, &iCommandBuffer);
	}

void CMemSpyCommandLine::RedrawStatusMessage()
	{
	RedrawStatusMessage(KNullDesC);
	}

void CMemSpyCommandLine::RedrawStatusMessage(const TDesC& aMessage)
	{
	iConsole.SetCursorPosAbs(iStatusMessagePos);
	iConsole.ClearToEndOfLine();
	iConsole.Write(aMessage);
	iConsole.Write(KMemSpyCLINewLine);
	}

void CMemSpyCommandLine::WaitForInput()
	{
	ASSERT( !IsActive() );
	iConsole.Read(iStatus);
	SetActive();
	}

void CMemSpyCommandLine::DoCancel()
	{
	iConsole.ReadCancel();
	}

void CMemSpyCommandLine::RunL()
	{
	TKeyCode key = iConsole.KeyCode();
	//
	if  ( key == EKeyEnter || key == KMemSpyUiS60KeyCodeButtonOk || key == KMemSpyUiS60KeyCodeRockerEnter )
		{
		TRAP_IGNORE( ProcessCommandBufferL() );
		}
	else
		{
		TChar character(key);
		if (character.IsPrint())
			{
			if (iCommandBuffer.Length() < iCommandBuffer.MaxLength())
				{
				iCommandBuffer.Append(TChar(key));
				}

			RedrawInputPrompt();
			}
		}

	WaitForInput();
	}

TInt CMemSpyCommandLine::RunError(TInt aError)
	{
	return KErrNone;
	}

void CMemSpyCommandLine::ProcessCommandBufferL()
	{
	iCommandBuffer.Trim();
	//
#ifdef _DEBUG
	RDebug::Print( _L("[MCon] CMemSpyConsoleMenu::ProcessCommandBufferL() - cmd: [%S]"), &iCommandBuffer );
#endif
	//
	TBool validCommand = EFalse;
	if (iCommandBuffer.Length() == 1)
		{
		// Reset if not recognised...
		validCommand = ETrue;

		const TChar cmd = iCommandBuffer[0];
		switch (cmd)
			{
			case 's':
			case 'S':
				{
				iMemSpy->PerformOperation( EMemSpyClientServerOpSystemWideMemoryTrackingTimerStop );

				CActiveScheduler::Stop();
				return;
				}
			case 'c':
			case 'C':
				CActiveScheduler::Stop();
				return;
			default:
				validCommand = EFalse;
				break;
			}
		}
	if (!validCommand)
		{
		_LIT( KInvalidEntry, "*** ERROR - Invalid Command ***" );
		RedrawStatusMessage(KInvalidEntry);
		RedrawInputPrompt();
		}
	}
