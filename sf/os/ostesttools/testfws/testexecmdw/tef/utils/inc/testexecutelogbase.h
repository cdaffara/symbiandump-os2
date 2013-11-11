/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file TestExecuteLogBase.h
*/

#if !(defined __TESTEXECUTE_LOG_BASE_H__)
#define __TESTEXECUTE_LOG_BASE_H__

#include <e32base.h>
#include <test/testexecutelog.h>
#include <test/testexecutepipslog.h>
#include <test/rfilelogger.h>
#include <test/blockitems.h>

// Abstract base class for all logging operations for TEF
class MTestExecuteLogBase
/**
@internalComponent
@test
*/
	{
public:
	virtual ~MTestExecuteLogBase() {};
	IMPORT_C virtual void InitialiseLoggingL(const TDesC& aScriptFilePath, TBool aSeparateLogFileMode, TInt aLogLevel) = 0;
	IMPORT_C virtual void TerminateLoggingL(const TInt aCommentedCommandsCount, const TInt aRemotePanicsCount, const TInt aRunScriptFailCount) = 0;
	IMPORT_C virtual void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,TRefByValue<const TDesC> aFmt,...) = 0;
	IMPORT_C virtual void LogTestCaseResult(const TDesC& aFile, TInt aLine, TInt aSeverity, const TDesC& aCommand, const TDesC& aTestCaseName, TVerdict aResult) = 0;
	IMPORT_C virtual void LogToXml(const TText8* aFile, TInt aLine, TInt aSeverity,const TDesC& aCommandName, const TInt aNumberOfParams, TExtraLogField* aLogFields) = 0;
	IMPORT_C virtual void LogToXml(const TText8* aFile, TInt aLine, TInt aSeverity,const TDesC& aCommandName) = 0;
	IMPORT_C virtual void PrintCurrentScriptLine(TDes& aCurrentScriptLine) = 0;
	IMPORT_C virtual void LogResult(TVerdict aResult, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand,const TDesC& aScriptFile,TInt aSeverity) = 0;
	IMPORT_C virtual TVerdict LogBlock( TTEFItemArray* aItemArray, const TInt aScriptLineNumber ) = 0;
	IMPORT_C virtual void SetLoggerOptions(TInt aLogMode) = 0;
	IMPORT_C virtual TInt Connect() = 0;
	IMPORT_C virtual void Close() = 0;
	IMPORT_C virtual RTestExecuteLogServ& HtmlLogger() = 0;
	IMPORT_C virtual RTestExecutePIPSLogServ& PIPSLogger() = 0;
	IMPORT_C virtual RFileFlogger& XmlLogger() = 0;
	IMPORT_C virtual TInt ShareAuto() = 0;
	IMPORT_C virtual void Write(const TDesC& aLogBuffer) = 0;
	IMPORT_C virtual void WriteFormat(TRefByValue<const TDesC> aFmt,...) = 0;
	IMPORT_C virtual void Write(const TDesC8& aLogBuffer) = 0;
	IMPORT_C virtual void WriteFormat(TRefByValue<const TDesC8> aFmt,...) = 0;
	};

#endif
