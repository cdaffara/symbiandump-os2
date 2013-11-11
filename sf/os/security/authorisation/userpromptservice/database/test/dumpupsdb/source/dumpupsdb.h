/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Defines a tool to export/import UPS Decision Database
*
*/


/**
 @file
 @test
*/

#ifndef DUMPUPSDB_H
#define DUMPUPSDB_H
 
#include "upsdbw.h"
#include "activewaiter.h"

#include <e32std.h>
#include <f32file.h>
#include <e32cons.h>
#include <bacline.h>
#include <utf.h>

using namespace UserPromptService;


_LIT(KAppName, "UPS Decision Database Dump Tool");

//Set flag values
static const TUint16 KLocClientSid			= 0x0001;
static const TUint16 KLocEvaluatorId		= 0x0002;
static const TUint16 KLocServiceId			= 0x0004;
static const TUint16 KLocServerSid			= 0x0008;
static const TUint16 KLocFingerprint		= 0x0010;
static const TUint16 KLocClientEntity		= 0x0020;
static const TUint16 KLocDescription		= 0x0040;
static const TUint16 KLocResult				= 0x0080;
static const TUint16 KLocEvaluatorInfo		= 0x0100;
static const TUint16 KLocMajorPolicyVersion	= 0x0200;
static const TUint16 KLocRecordId			= 0x0400;

//
//CPrinter
//

class CPrinter : public CBase
/**
 Class that prints text to the console and optionally writes to a log file.
 */
	{
public:
	static CPrinter* NewLC(CConsoleBase* aConsole);
	static CPrinter* NewLC(CConsoleBase* aConsole, RFile& aFile);
	static void Usage(CConsoleBase* aConsole);
	
	~CPrinter();
	
	void PrintfL(TRefByValue<const TDesC16> aFormat, ...);
	void Printf8L(TRefByValue<const TDesC8> aFormat, ...);
	void PrintOnlyConsoleL(const TDesC16& aFormat, ...);
	void Wait();
	void ReadNextLineL(TDes8& aLine);
	TInt FileSizeL();

private	:
	CPrinter(CConsoleBase* aConsole);
	void ConstructL(RFile& aFile);

private:
	/** Console object to print text to the console*/
	CConsoleBase* iConsole;
	
	/** Optional file handle to write text to the file*/
	RFile iFile;
	
	/** Whether to log to the output to the file */
	TBool iLogToFile;
	
	/** Temporary 16-bit buffer*/
	TBuf<256> iBuffer;
	
	/** Temporary 8-bit buffer*/
	TBuf8<256> iBuffer8;
	
	/** File stream handle*/
	RFileReadStream iReader;
	};

//
//CDatabase
//

class CDatabase : public CBase
/**
	This class connects to the database and import/exports it to the console/file
 */
	{
public:
	static CDatabase* NewLC(CConsoleBase* aConsole, RFs& aFs, TBool aImport, const TDesC& aDb, const TDesC& aFile);
	
	~CDatabase();
	
	void DumpL();

private:
	CDatabase(TBool aImport);
	void ConstructL(CConsoleBase* aConsole, RFs& aFs, const TDesC& aDb, const TDesC& aFile);
	
	void PrintDecisionL(CDecisionRecord& aRecord);
	void PrintHeaderL();
	CDecisionRecord* ParseAndCreateRecordLC(TDesC8& aLine);
	TUint8 HexToIntL(TUint8* aPtr);
	void HexToStrL(TDes8& aSource);
	void PrintTestResultsL();
	void PrintTestHeaderL();
	
private:
	/** Indicates the operation type: Import Db is ETrue and Export Db is EFalse*/	
	TBool iImport;
	
	/** Handle to the decision database*/
	CDecisionDbW* iUpsDb;
	
	/** Handle to the printer object*/
	CPrinter* iPrinter;
	
	/** Optional file handle to write text to the file*/
	RFile iFile;

public:	
	/** Enumerated values to arrange run options. */
	enum TDbOptions
		{
		/** Indicates whether the dummy test result lines are written into the output file. */
		EAppendTestResults = 0x01,
		/** Indicates whether the command line waits for the user input at the end of the operation.*/
		EDoNotStop = 0x02,
		/** Indicates that fingerprint and client entity fields are printed as binary instead hexadecimal. */
		EPrintBinary = 0x04
		};
	
	TUint8 iFlag;	///< A flag for running options.
	};


static void MainL(void)
/**
Takes a User Prompt Service decision database file and dumps it as human readable text to the 
console. The user may also specify the name of an output file on the command line. If so, text
is also written to this file. Also recreates a UPS decision database from a dumped file. Where
the database is created is specified by the user.
*/
	{
	
	RFs fs;
	
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	CConsoleBase *console = Console::NewL(KAppName,TSize(KDefaultConsWidth, KDefaultConsHeight));
	CleanupStack::PushL(console);
	
	CCommandLineArguments *cmdLineArgs = CCommandLineArguments::NewLC();
	
	TInt argc;
	argc = cmdLineArgs->Count();
	
	//At least operation type and database file path must be given
	if(argc < 4)
		{
		CPrinter::Usage(console);
		User::Leave(KErrArgument);
		}
	
	TInt	loopCount(1);
	TBool	import(EFalse);
	TUint16 *dbPath(0);
	TInt	dbPathLen(0);
	TUint16 *filePath(0);
	TInt	filePathLen(0);
	TUint8	flag(0);
	
	while(loopCount+1 <= argc)
		{
		TPtrC argv = cmdLineArgs->Arg(loopCount);
		
		if(argv.Locate('-'))
			{
			break;
			}
			
		switch(argv.Ptr()[1])
			{
			case 'i':
			case 'I':
				{//the operation type is import
				import = ETrue;
				break;
				}
			case 'e':
			case 'E':
				{//the operation type is export
				import = EFalse;
				break;
				}
			case 'd':
			case 'D':
				{//the decision database file path
				TPtrC path = cmdLineArgs->Arg(++loopCount);
				dbPath = (TUint16 *)path.Ptr();
				dbPathLen = path.Length();
				break;
				}
			case 'f': 
			case 'F':
				{//the file which the database will be dumped to or populated from
				TPtrC path = cmdLineArgs->Arg(++loopCount);
				filePath = (TUint16 *)path.Ptr();
				filePathLen = path.Length();
				break;
				}
			case 's':
			case 'S':
				{//This parameter is not published in the help page
				 //It's been added to get rid of waiting at the end of the program.
				flag |= CDatabase::EDoNotStop;
				break;	
				}
			case 't':
			case 'T':
				{//This parameter is not published in the help page
				 //It's been added to get rid of parsing error in the test environment.
				flag |= CDatabase::EAppendTestResults;
				break;	
				}
			case 'b':
			case 'B':
				{//This parameter is not published in the help page
				 //It's been added to print fields as binary instead hexadecimal.
				flag |= CDatabase::EPrintBinary;
				break;	
				}
			default:
				break;
			}//switch
		
		++loopCount;
			
		}//while loop

	if(loopCount != argc || !dbPath || (import && !filePath))
		{
		CPrinter::Usage(console);
		User::Leave(KErrArgument);
		}
	
	TPtr db(dbPath,dbPathLen);
	db.SetLength(dbPathLen);
	
	CDatabase* database = NULL;	
	
	if(filePath)
		{
		TPtr file(filePath,filePathLen);
		file.SetLength(filePathLen);
		database = CDatabase::NewLC(console, fs, import, db, file);
		}
	else
		{
		database = CDatabase::NewLC(console, fs, import, db, KNullDesC);
		}
	
	database->iFlag = flag;
		
	//Do actual import or export database operation	
	database->DumpL();
	
	CleanupStack::PopAndDestroy(4, &fs);
	}


GLDEF_C TInt E32Main()
	{
	CTrapCleanup *dumpupsdbCleanup = CTrapCleanup::New();
	if(dumpupsdbCleanup == NULL)
		{
		return KErrNoMemory;
		}
	
	CActiveScheduler* dumpupsdbScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(dumpupsdbScheduler);
		
	__UHEAP_MARK;
	
	TRAPD(retval,MainL());
	
	__UHEAP_MARKEND;
	
	delete dumpupsdbCleanup;
	delete dumpupsdbScheduler;
	
	if(retval == KErrArgument)
		{
		return KErrNone;
		}
	return retval;
	}
	
#endif
