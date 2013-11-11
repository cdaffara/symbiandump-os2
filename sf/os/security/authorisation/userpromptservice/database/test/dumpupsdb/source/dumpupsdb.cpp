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
* Implements a tool to export/import UPS Decision Database
*
*/


 
#include "dumpupsdb.h"

using namespace UserPromptService;

//
//CPrinter
//

CPrinter::CPrinter(CConsoleBase* aConsole):iConsole(aConsole)
/** Constructor */
	{
	
	}
	
CPrinter::~CPrinter()
/** Destructor */
	{
	iReader.Close();
	iFile.Close();
	}
	

CPrinter* CPrinter::NewLC(CConsoleBase* aConsole)
/**
	Creates a new printer object and places the pointer on the cleanup stack.
	@param	aConsole The console object to print text to.
	@return A pointer to the new printer object.
*/
	{
	CPrinter *self = new(ELeave)CPrinter(aConsole);
	CleanupStack::PushL(self);
	return self;
	}
	
CPrinter* CPrinter::NewLC(CConsoleBase* aConsole, RFile& aFile)
/**
	Creates a new printer object and places the pointer on the cleanup stack.
	@param	aConsole	The console object to print text to.
	@param	aFile		A handle to a file to write the text to. The handle is duplicated internally.					
	@return A pointer to the new printer object.
*/
	{
	CPrinter *self = CPrinter::NewLC(aConsole);
	self->ConstructL(aFile);
	return self;
	}


void CPrinter::ConstructL(RFile& aFile)
/** Second phase constructor*/
	{
	User::LeaveIfError(iFile.Duplicate(aFile));
	iLogToFile = ETrue;
	//iReader.Set(iFile);
	iReader.Attach(aFile);
	}

void CPrinter::PrintfL(TRefByValue<const TDesC16> aFormat, ...)
/**
	Formats and writes 16-bit text to the console and/or file
	@param aFormat The 16-bit non-modifiable descriptor containing the format string.
 */
	{
	VA_LIST list;
	VA_START (list, aFormat);
	
	iBuffer.Zero();
	iBuffer.AppendFormatList(aFormat, list);
	
	iConsole->Printf(iBuffer);
	
	if(iLogToFile)
		{
		HBufC8* utf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(iBuffer);
		CleanupStack::PushL(utf8);
		User::LeaveIfError(iFile.Write(*utf8));
		CleanupStack::PopAndDestroy(utf8);
		}
		
	VA_END(list);
	}


void CPrinter::Printf8L(TRefByValue<const TDesC8> aFormat, ...)
/**
	Formats and writes 8-bit text to the console and/or file
	@param aFormat The 8-bit non-modifiable descriptor containing the format string.
 */
	{
	VA_LIST list;
	VA_START (list, aFormat);
	
	iBuffer8.Zero();
	iBuffer8.AppendFormatList(aFormat, list);
	iBuffer.Copy(iBuffer8);
	iConsole->Printf(iBuffer);
	
	if(iLogToFile)
		{
		User::LeaveIfError(iFile.Write(iBuffer8));
		}
	
	VA_END(list);
	}

void CPrinter::PrintOnlyConsoleL(const TDesC& aFormat, ...)
/**
	Formats and writes 16-bit text to the console.
	@param aFormat The 16-bit non-modifiable descriptor containing the format string.
 */
	{
	TBool temp;
	temp = iLogToFile;
	iLogToFile = EFalse;
	
	PrintfL(aFormat);
	
	iLogToFile = temp;
	}
	
	
void CPrinter::Usage(CConsoleBase* aConsole)
/**
	Prints how to use DumpUpsDb Tool.
	@param	aConsole A pointer to the console object
 */
	{
	aConsole->Printf(_L("DUMPUPSDB Version 1, 0\n"));
	aConsole->Printf(_L("A utility for importing and exporting UPS Decision Database.\n"));
	aConsole->Printf(_L("Copyright (c) 2007 Symbian Ltd.  All rights reserved.\n\n"));
	aConsole->Printf(_L("error: wrong number of arguments\n"));
	aConsole->Printf(_L("Usage: DumpUpsDb [-h] [-i] [-e] [-b] -db dbpath [-f filepath]\n\n"));
	aConsole->Printf(_L("Options : -h  Show help page\n"));
	aConsole->Printf(_L("Options : -i  Import an exported database\n"));
	aConsole->Printf(_L("Options : -e  Export the database\n"));
	aConsole->Printf(_L("Options : -db Database file\n"));
	aConsole->Printf(_L("Options : -f  Output/Input file\n"));
	aConsole->Printf(_L("Options : -b  Import/export Client Entity as binary\n\n"));
	aConsole->Printf(_L("Press any key to continue\r\n"));
	aConsole->Getch();
	}

void CPrinter::Wait()
/**
	If no output file is specified then pause after finishing because the console 
	will vanish when it is closed.
 */
	{
	iConsole->Printf(_L("Press any key to continue\r\n"));
	iConsole->Getch();
	}
	

void CPrinter::ReadNextLineL(TDes8& aLine)
	{
	TChar achar = '\n';
	iReader.ReadL(aLine, achar);
	}

TInt CPrinter::FileSizeL()
	{
	return iReader.Source()->SizeL();
	}
//
//CDatabase
//

CDatabase::CDatabase(TBool aImport):iImport(aImport)
/** Constructor */
	{
	
	}
	
CDatabase::~CDatabase()
/** Destructor */
	{
	delete iPrinter;
	delete iUpsDb;
	}
	

CDatabase* CDatabase::NewLC(CConsoleBase* aConsole, RFs& aFs, TBool aImport, const TDesC& aDb, const TDesC& aFile)
/**
	Creates a new database object and places the pointer on the cleanup stack.
	
	@param aConsole Pointer to the console object
	@param aFs Handle to the file server
	@param aImport Whether the operation type is import
	@param aDb The fully qualified path of the decision database
	@param aFile The fully qualified path of the dump file
	@return A pointer to the newly created database object
 */
	{
	CDatabase *self = new(ELeave)CDatabase(aImport);
	CleanupStack::PushL(self);
	self->ConstructL(aConsole, aFs, aDb, aFile);
	return self;
	}
	

void CDatabase::ConstructL(CConsoleBase* aConsole, RFs& aFs, const TDesC& aDb, const TDesC& aFile)
/**
	Second phase constructor for database object
 */
	{
	if(aFile.Length() > 0)
		{
		if(iImport)
			{
			User::LeaveIfError(iFile.Open(aFs, aFile, EFileWrite|EFileShareExclusive));
			}
		else
			{
			User::LeaveIfError(iFile.Replace(aFs, aFile, EFileWrite|EFileShareExclusive));
			}
		
		iPrinter = CPrinter::NewLC(aConsole, iFile);
		}
	else
		{
		iPrinter = CPrinter::NewLC(aConsole);
		}
	
	CleanupStack::Pop(iPrinter);
	
	iUpsDb = CDecisionDbW::NewL(aDb, aFs);
	
	}

void CDatabase::DumpL()
	{
	if(!iImport)
		{//Dump database to the console and files
		//Create an empty filter to get all decisions in the table
		CDecisionFilter *filter = CDecisionFilter::NewLC();
		
		//Create a view object
		CDecisionView *dbView = iUpsDb->CreateViewL(*filter);
		CleanupStack::PushL(dbView);
		
		CActiveWaiter *waiter = new(ELeave)CActiveWaiter();
		CleanupStack::PushL(waiter);
		
		//Fill the decisions into the view object
		dbView->EvaluateView(waiter->iStatus);
		waiter->WaitActiveL(KErrNone);	
		if(iFlag & EAppendTestResults)
			{
			PrintTestHeaderL();
			}
		PrintHeaderL();
		
		CDecisionRecord *record = NULL;
		while((record = dbView->NextDecisionL()) != NULL)
			{
			CleanupStack::PushL(record);
			PrintDecisionL(*record);
			CleanupStack::PopAndDestroy(record);
			}
		
		CleanupStack::PopAndDestroy(3, filter);
		iPrinter->PrintOnlyConsoleL(_L("Exported successfully!\n"));
		if(iFlag & EAppendTestResults)
			{
			PrintTestResultsL();
			}
		}
	else
		{//Import an exported decision file to the decision database
		TInt fileSize;
		fileSize = iPrinter->FileSizeL();
		TInt readSize=0;
		TBuf8<256> buffer;
		TBool skipFirstLine = ETrue;
		CDecisionRecord *record = NULL;
		do
			{
			iPrinter->ReadNextLineL(buffer);
			readSize +=buffer.Length();
			if(skipFirstLine)
				{
				skipFirstLine = EFalse;
				}
			else
				{
				record = ParseAndCreateRecordLC(buffer);
				iUpsDb->CreateDecisionL(*record);
				CleanupStack::PopAndDestroy(record);
				}
		
			buffer.Zero();
			}while(readSize < fileSize);
			
		iPrinter->PrintOnlyConsoleL(_L("Imported successfully!\n"));
		}
	//If both of the flags are not set, wait.
	if(!(iFlag & EAppendTestResults || iFlag & EDoNotStop))
		{
		iPrinter->Wait();
		}
	}


void CDatabase::PrintDecisionL(CDecisionRecord& aRecord)
	{
	_LIT(KDelimiter,	";");
	_LIT(KWriteId,		"\"%08x\"");
	_LIT(KWriteString,	"\"%S\"");
	_LIT8(KWriteString8,"\"%S\"");
	_LIT8(KWriteHex8,	"%02x");

	
	iPrinter->PrintfL(KWriteId,aRecord.iClientSid.iId);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteId,aRecord.iEvaluatorId.iUid);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteId,aRecord.iServiceId.iUid);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteId,aRecord.iServerSid.iId);
	iPrinter->PrintfL(KDelimiter);
	
	TBuf8<KUpsMaxFingerprintLength*2> hexdump;
	TUint8 *ptr = (TUint8 *)aRecord.iFingerprint.Ptr();
	TInt i;
	TInt len = aRecord.iFingerprint.Length();

	for(i=0; i<len; ++i)
		{
		hexdump.AppendFormat(KWriteHex8,ptr[i]);
		}
	iPrinter->Printf8L(KWriteString8,&hexdump);
	iPrinter->PrintfL(KDelimiter);
		
	if(iFlag & EPrintBinary)
		{
		iPrinter->Printf8L(KWriteString8,&aRecord.iClientEntity);
		iPrinter->PrintfL(KDelimiter);
		}
	else
		{
		hexdump.Zero();
		ptr = (TUint8 *)aRecord.iClientEntity.Ptr();
		len = aRecord.iClientEntity.Length();
		for(i=0; i<len; ++i)
			{
			hexdump.AppendFormat(KWriteHex8,ptr[i]);
			}
		iPrinter->Printf8L(KWriteString8,&hexdump);
		iPrinter->PrintfL(KDelimiter);
		}	
		
	iPrinter->PrintfL(KWriteString,&aRecord.iDescription);
	iPrinter->PrintfL(KDelimiter);
	
	iPrinter->PrintfL(KWriteId,aRecord.iResult);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteId,aRecord.iEvaluatorInfo);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteId,aRecord.iMajorPolicyVersion);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteId,aRecord.iRecordId);
	
	iPrinter->Printf8L(_L8("\n"));
	}


void CDatabase::PrintHeaderL()
	{
	_LIT(KDelimiter,	";");
	_LIT(KWriteString,	"\"%S\"");
	
	iPrinter->PrintfL(KWriteString,&KColClientSid);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColEvaluatorId);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColServiceId);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColServerSid);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColFingerprint);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColClientEntity);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColDescription);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColResult);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColEvaluatorInfo);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColMajorPolicyVersion);
	iPrinter->PrintfL(KDelimiter);
	iPrinter->PrintfL(KWriteString,&KColRecordId);
	iPrinter->Printf8L(_L8("\n"));
	}
	
CDecisionRecord* CDatabase::ParseAndCreateRecordLC(TDesC8& aLine)
/**
	Parse a line and create a decision record from the parsed values
	@param aLine A line containing a decision record values quoted with double quotes and separated by semi-colon
	@return A newly created decision record
 */
	{
	CDecisionRecord* record = NULL;
	
	TLex8 parser(aLine);
	TChar achar;
	TBool start = ETrue;
	
	TUint16 flag = 0x078F;
	TUint16 pos	 = 0x0001;
	
	TInt32 value=0; 
	TUint32 hexVal = 0;
	
	TSecureId clientSid(0);
	TSecureId serverSid(0);
	TUid serviceId = TUid::Null();
	TUid evaluatorId = TUid::Null();
	TBuf8<KUpsMaxFingerprintLength*2> fingerprint;
	TBuf8<KUpsMaxClientEntityLength*2> clientEntity;
	RBuf description;
	TUint8 result=0;
	TUint32 evaluatorInfo=0;
	TUint16 policyVer=0;
	TUint32 recordId=0;
	
	while(!parser.Eos())
		{
		//Get cyrrent char
		achar = parser.Get();
		//Skip delimiter
		if(achar == ';')
			{
			start = ETrue;
			}
		//if double quote start or stop token reading	
		if('\"' == achar)
			{
			if(start)
				{
				parser.Mark();
				start = EFalse;
				}
			else
				{
				parser.UnGet();
				
				if(flag & pos)
					{
					TLex8 intToken(parser.MarkedToken());
					switch(pos)
						{
						case KLocClientSid:
							intToken.Val(hexVal,EHex);
							clientSid.iId = hexVal;
							break;
						case KLocEvaluatorId:
							intToken.Val(hexVal,EHex);
							evaluatorId.iUid = hexVal;
							break;
						case KLocServiceId:
							intToken.Val(hexVal,EHex);
							serviceId.iUid = hexVal;
							break;
						case KLocServerSid:
							intToken.Val(hexVal,EHex);
							serverSid.iId = hexVal;
							break;
						case KLocResult:
							intToken.Val(value);
							result = (TUint8)value;
							break;
						case KLocEvaluatorInfo:
							intToken.Val(value);
							evaluatorInfo = value;
							break;
						case KLocMajorPolicyVersion:
							intToken.Val(value);
							policyVer = (TUint16)value;
							break;
						case KLocRecordId:
							intToken.Val(value);
							recordId = (TUint32)value;							
							break;
						default:
							User::Leave(KErrGeneral);
						}
					}
				else
					{
					switch(pos)
						{
						case KLocDescription:
							{	
							TPtrC8 tmpDescription = parser.MarkedToken();
							description.Create(tmpDescription.Length());
							description.CleanupClosePushL();
							description.Copy(tmpDescription);
							break;
							}
	
						case KLocFingerprint:
							{
							fingerprint = parser.MarkedToken();
							HexToStrL(fingerprint);	
							break;						
							}
						case KLocClientEntity:
							{
							clientEntity = parser.MarkedToken();
							if(!(iFlag & EPrintBinary))							
								{
								HexToStrL(clientEntity);
								}
							break;							
							}
							default:
							User::Leave(KErrGeneral);
						}
					}
				
					
				start = ETrue;
				pos = pos<<1;
				
				}			
			}
		}
	record = CDecisionRecord::NewL(clientSid,evaluatorId,serviceId,serverSid,fingerprint,clientEntity,description,result,policyVer,evaluatorInfo,recordId);
	CleanupStack::PopAndDestroy(&description);
	CleanupStack::PushL(record);
	return record;
	}

	
TUint8 CDatabase::HexToIntL(TUint8* aPtr)
/**
	Convert a 2-byte hexadecimal representation value to integer (Ex: C2 -> 194).
	@param aPtr Pointer to source data
	@return An integer value against 2-byte hexadecimal value
 */
	{
	//Save and then set third byte NULL
	TUint8 temp = aPtr[2];
	aPtr[2] = '\n';
	//Create a lex string from first two bytes
	TLex8 lex(aPtr);
	//Convert two bytes hex value (ex:9F) to integer (ex:159)
	TUint8 intVal;
	User::LeaveIfError(lex.Val(intVal,EHex));
	//Put the original value back
	aPtr[2] = temp;
	//return integer value of first two hex bytes
	return intVal;
	}

	
void CDatabase::HexToStrL(TDes8& aSource)
/**
	Convert a string containing hexadecimal representation to another string containing binary representation
	@param aSource Source data containing a string
 */
	{
	TUint8 *pSource = (TUint8 *)aSource.Ptr();
	TUint8 *pDest	= (TUint8 *)aSource.Ptr();
	
	TInt len = aSource.Length();
	TInt idxSource;
	TInt idxDest;
	
	for(idxSource=0, idxDest=0; idxSource<len; ++idxDest,idxSource+=2)
		{
		pDest[idxDest] = HexToIntL(pSource+idxSource);
		}
	
	aSource.SetLength(idxDest);
	}


void CDatabase::PrintTestResultsL()
/**
	Prints dummy test results to get rid of log file parsing error in showing test results.
 */
	{
#if 1
	iPrinter->PrintfL(_L("\n\n0 tests failed out of 1\n"));
#else
	iPrinter->PrintfL(_L("\n\nTEST STEP SUMMARY:\n"));
	iPrinter->PrintfL(_L("PASS = 1\n"));
	iPrinter->PrintfL(_L("FAIL = 0\n"));
	iPrinter->PrintfL(_L("ABORT = 0\n"));
	iPrinter->PrintfL(_L("PANIC = 0\n"));
	iPrinter->PrintfL(_L("INCONCLUSIVE = 0\n"));
	iPrinter->PrintfL(_L("UNKNOWN = 0\n"));
	iPrinter->PrintfL(_L("UNEXECUTED = 0\n"));
	iPrinter->PrintfL(_L("COMMENTED COMMAND'S = 0\n"));
	iPrinter->PrintfL(_L("TEST CASE SUMMARY:\n"));
	iPrinter->PrintfL(_L("PASS = 1\n"));
	iPrinter->PrintfL(_L("FAIL = 0\n"));
	iPrinter->PrintfL(_L("INCONCLUSIVE = 0\n"));
#endif
	}
	
void CDatabase::PrintTestHeaderL()
	{
	iPrinter->PrintfL(_L("TEST SYNOPSIS:\n"));
	iPrinter->PrintfL(_L("TEF Version : 2.1.2004\n"));
	iPrinter->PrintfL(_L("START_TESTCASE COUNT : 1\n"));
	iPrinter->PrintfL(_L("RUN_TEST_STEP COUNT : 1\n"));
	iPrinter->PrintfL(_L("RUN_TEST_STEP_RESULT COUNT : 1\n\n"));
	}

