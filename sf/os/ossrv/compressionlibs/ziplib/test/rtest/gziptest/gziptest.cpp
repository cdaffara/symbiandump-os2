// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <ezgzip.h>

#if !defined(__WINS__)
_LIT(KPath, "Z:\\test\\gzip\\");
#else
_LIT(KPath, "C:\\test\\gzip\\");
#endif

_LIT(KOutputFilePath, "C:\\test\\gzip\\");

_LIT(KInputFile, "Hello.txt");
_LIT(KCompressFile,"Hello.gz");
_LIT(KDecompressFile,"Hello_Decompress.txt");


static RTest				test(_L("gziptest.exe"));
static RFs					TheFs;
static CTrapCleanup* 		TheTrapCleanup 		= NULL;
static TInt 				TheBufferSize		= 500;
static TFileName 			TheInputFile(KInputFile);
static TFileName 			TheCompressFile(KCompressFile);
static TFileName 			TheDecompressFile(KDecompressFile);

static void CompressFileL(TFileName& aInputFile, TFileName& aCompressFile, TInt aBufferSize = TheBufferSize);
static void DecompressFileL(TFileName& aInputFile, TFileName& aDecompressFile, TInt aBufferSize = TheBufferSize);
static void CompareFileDataL(TFileName& aSourceFile, TFileName& aDestFile, TInt& aCompare);
static void ThreadForPanicTest(TThreadFunction aPanicThreadFunc);

inline void DeleteFile(TFileName& aFileName, TBool aPathSpecified = EFalse)
	{
	TFileName deleteFilePath;
	if(!aPathSpecified)
		{
		deleteFilePath.Append(KOutputFilePath);
		}
	deleteFilePath.Append(aFileName);

	TRAPD(error, TheFs.Delete(deleteFilePath));
	if(error != KErrNone)
		test.Printf(_L("File not deleted: %S"), &deleteFilePath);
	}
	
static void CompressFileL(TFileName& aInputFile, TFileName& aCompressFile, TInt aBufferSize)
	{
	TFileName inputFilePath(KPath);
	inputFilePath.Append(aInputFile);
	
	TFileName compressFilePath(KOutputFilePath);
	compressFilePath.Append(aCompressFile);
	
	RFile input;
	User::LeaveIfError(input.Open(TheFs,inputFilePath, EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(input);
	
	CEZFileToGZip *fileToGzip = NULL;
	TRAPD(error, fileToGzip = CEZFileToGZip::NewL(TheFs, compressFilePath, input, aBufferSize));
	test(error == KErrNone);
	CleanupStack::PushL(fileToGzip);
	
	while(fileToGzip->DeflateL()){/*do nothing*/}
	
	CleanupStack::PopAndDestroy(2);
	}
	
static void DecompressFileL(TFileName& aCompressFile, TFileName& aDecompressFile, TInt aBufferSize)
	{

	TFileName compressFilePath(KPath);
	compressFilePath.Append(aCompressFile);
	
	TFileName decompressFilePath(KOutputFilePath);
	decompressFilePath.Append(aDecompressFile);

	RFile output;
	User::LeaveIfError(output.Replace(TheFs, decompressFilePath, EFileStream | EFileWrite | EFileShareExclusive));
	CleanupClosePushL(output);

	CEZGZipToFile *gzipToFile = NULL;
	TRAPD(error, gzipToFile = CEZGZipToFile::NewL(TheFs, compressFilePath, output, aBufferSize));
	test(error == KErrNone);
	CleanupStack::PushL(gzipToFile);
	
	while (gzipToFile->InflateL()){/*do nothing*/}

	CleanupStack::PopAndDestroy(2);
	
	}
	
static void CompareFileDataL(TFileName& aSourceFile, TFileName& aDestFile, TInt& aCompare)
	{
	
	TFileName sourceFilePath(KPath);
	sourceFilePath.Append(aSourceFile);

	TFileName destFilePath(KOutputFilePath);
	destFilePath.Append(aDestFile);

	RFile source;
	User::LeaveIfError(source.Open(TheFs,sourceFilePath, EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(source);
		
	TInt sourceSize;
	source.Size(sourceSize);
	test(sourceSize > 0);
		
	HBufC8* sourceData = NULL;
	TRAPD(error, sourceData = HBufC8::NewMaxL(sourceSize));
	test(error == KErrNone);
	CleanupStack::PushL(sourceData);
			
	TPtr8 tSourceBufPtr = sourceData->Des();
	User::LeaveIfError(source.Read(0, tSourceBufPtr, sourceSize));
	
	RFile dest;
	User::LeaveIfError(dest.Open(TheFs,destFilePath, EFileStream | EFileWrite | EFileShareExclusive));
	CleanupClosePushL(dest);

	TInt destSize;
	dest.Size(destSize);
	test(destSize > 0);
	
	HBufC8* destData = NULL;
	TRAP(error, destData = HBufC8::NewMaxL(destSize));
	test(error == KErrNone);
	CleanupStack::PushL(destData);
			
	TPtr8 tDestBufPtr = destData->Des();
	User::LeaveIfError(dest.Read(0, tDestBufPtr, destSize));
		
	aCompare = sourceData->Compare(*destData);

	CleanupStack::PopAndDestroy(4);
	
	}

static void ThreadForPanicTest(TThreadFunction aPanicThreadFunc)
	{
   	TRequestStatus threadStatus(KRequestPending);
   	TRequestStatus threadStatus1(KRequestPending); 
   	
   	RThread thread;
	TBool justInTime = User::JustInTime();
	User::SetJustInTime(EFalse);
		
	test.Printf(_L("Starting thread for invalid panic test...\n"));
	TInt err=thread.Create(	_L("Panic thread"),aPanicThreadFunc,KDefaultStackSize,0x1000,0x1000,NULL,EOwnerThread );
	
	test.Printf(_L("Thread Creation returned following error code: %d \n"), err);
	test(err == KErrNone);

	thread.Logon(threadStatus);
	thread.Resume();
	User::WaitForRequest(threadStatus);

#ifdef _DEBUG
	__ASSERT_ALWAYS(thread.ExitType() == EExitPanic,User::Panic(_L("Thread panic mismatch."),KErrGeneral));
	test.Printf(_L("Invalid Panic test completed successfully for udeb build.\n"));
#else
	test.Printf(_L("Invalid Panic test completed successfully for urel build.\n"));
#endif
	
	thread.Logon(threadStatus1);
	thread.Kill(KErrNone);
	User::WaitForRequest(threadStatus1);
	test(threadStatus1 == KErrNone);
	
    CLOSE_AND_WAIT(thread);
    
	User::SetJustInTime(justInTime);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4286
@SYMTestCaseDesc	    To ensure a file can be compressed to a gzip file and then correctly 
						decompressed back to its original state using classes from gzip.cpp.  
						Overwrite output file to verify existing file can be re-used.
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Open existing test file and pass it to CEZFileToGZip::NewL() & 
							compress using CEZFileToGZip::DeflateL() function
						2.	Create or open output decompressed file and pass compressed file to
							CEZGZipToFile::NewL() & decompress using CEZGZipToFile::InflateL() function
						3.	Compare original and new file, they should be identical
						4.	Repeat Steps 1 - 3 overwriting existing compressed output file
@SYMTestExpectedResults Original test file should be the same as the file generated 
						after decompressing it & the test must not fail
@SYMDEF                 REQ8024
*/
void TestCompressDecompressGzip()
	{
	TRAPD(error, CompressFileL(TheInputFile, TheCompressFile));
	test(error == KErrNone);
	TRAP(error, DecompressFileL(TheCompressFile, TheDecompressFile));
	test(error == KErrNone);
	
	TInt compare;
	//compare the files
	TRAP(error, CompareFileDataL(TheInputFile, TheDecompressFile, compare));
	test(error == KErrNone);
	
	if(compare == 0)
		RDebug::Printf("Input file data is same as the Decompressed file data");
	else
		RDebug::Printf("Input file data differs from Decompressed file data");
	
#if !defined(__WINS__)
	DeleteFile(TheCompressFile);
	DeleteFile(TheDecompressFile);
#endif

	//Compress & decompress the same file again to overwrite the existing file & 
	//..to verify that the existing files can be re-used
	TRAP(error, CompressFileL(TheInputFile, TheCompressFile));
	test(error == KErrNone);
	TRAP(error, DecompressFileL(TheCompressFile, TheDecompressFile));
	test(error == KErrNone);
	
#if !defined(__WINS__)
	DeleteFile(TheCompressFile);
	DeleteFile(TheDecompressFile);
#endif
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4287
@SYMTestCaseDesc	    To ensure a file can be compressed to a gzip file and then correctly 
						decompressed back using buffer size as small & big.
						
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Open existing test file and pass it to CEZFileToGZip::NewL() & 
							compress using CEZFileToGZip::DeflateL() with buffer size as "11"
						2.	Create or open output decompressed file and pass compressed file 
							to CEZGZipToFile::NewL() & decompress using CEZGZipToFile::InflateL() 
							with buffer size as "10"
						3.	Repeat Steps 1 & 2 using buffer size as 10 times the file size
@SYMTestExpectedResults Files should be compressed with no errors i.e. KErrNone & the test must not fail 
@SYMDEF                 REQ8024
*/
void TestOutputBuffSize()
	{	
	
	TRAPD(error, CompressFileL(TheInputFile, TheCompressFile, 10));
	test(error == KErrNone);

	TRAP(error, DecompressFileL(TheCompressFile, TheDecompressFile, 10));
	test(error == KErrNone);
	
	TInt bufferSize = 10 * TheBufferSize;
	
#if !defined(__WINS__)
	DeleteFile(TheCompressFile);
	DeleteFile(TheDecompressFile);
#endif

	//Compress & decompress the same file again to overwrite the existing file & 
	//..to verify that the existing files can be re-used & with buffer size as big
	TRAP(error, CompressFileL(TheInputFile, TheCompressFile, bufferSize));
	test(error == KErrNone);
	
	TRAP(error, DecompressFileL(TheCompressFile, TheDecompressFile, bufferSize));
	test(error == KErrNone);
	
#if !defined(__WINS__)
	DeleteFile(TheCompressFile);
	DeleteFile(TheDecompressFile);
#endif

	}

TInt RfileInitForCEZFileToGZip(TAny*)
	{
	RFile rfile;
	TFileName compressFilePath(KOutputFilePath);
	compressFilePath.Append(TheCompressFile);
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, CEZFileToGZip::NewL(TheFs, compressFilePath, rfile, TheBufferSize));
	delete cleanup;
	
	return error;
	}
	
TInt RfileInitForCEZGZipToFile(TAny*)
	{
	RFile rfile;
	TFileName compressFilePath(KOutputFilePath);
	compressFilePath.Append(TheCompressFile);

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error, CEZGZipToFile::NewL(TheFs, compressFilePath, rfile, TheBufferSize));
	delete cleanup;
	
	return error;
	
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4288
@SYMTestCaseDesc	    CEZFileToGZip & CEZGZipToFile class initialisation fails when 
						invalid input RFile object supplied
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Pass NULL RFile input parameter to CEZFileToGZip::NewL()
						2.	Pass NULL RFile input parameter to CEZGZipToFile::NewL()
@SYMTestExpectedResults NewL() method should panic & the test must not fail 
@SYMDEF                 REQ8024
*/
void TestRFileInitialisation()
	{
	test.Printf(_L("Panic Thread: CEZFileToGzip Class initialisation fails when invalid input rfile supplied"));	
	ThreadForPanicTest(RfileInitForCEZFileToGZip);
	test.Printf(_L("Panic Thread: CEZGZipToFile Class initialisation fails when invalid input rfile supplied"));
	ThreadForPanicTest(RfileInitForCEZGZipToFile);
	}
	
/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4289
@SYMTestCaseDesc	    CEZFileToGZip, CEZGZipToFile class initialisation fails 
						when invalid output filename supplied
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Pass invalid filename output parameter to CEZFileToGZip::NewL()
						2.	Pass invalid filename output parameter to CEZGZipToFile::NewL() 
						Note: Filename contains invalid characters
@SYMTestExpectedResults NewL() method should leave with error message & the test must not fail 
@SYMDEF                 REQ8024
*/	
void TestInvalidFilenameL()
	{
	
	_LIT(KInvalidGzip, "*******.gz");
	TFileName invalidGzip(KOutputFilePath);
	invalidGzip.Append(KInvalidGzip);

	_LIT(KInvalidFile, "*******.txt");
	TFileName invalidFile(KOutputFilePath);
	invalidFile.Append(KInvalidFile);
	
	TFileName inputFilePath(KPath);
	inputFilePath.Append(TheInputFile);

	RFile input;
	User::LeaveIfError(input.Open(TheFs,inputFilePath,EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(input);

	TRAPD(error, CEZFileToGZip::NewL(TheFs, invalidGzip, input, TheBufferSize));
	test(error != KErrNone);

	CleanupStack::PopAndDestroy(1);
	
	RFile output;
	TInt err = output.Create(TheFs, invalidFile,EFileStream | EFileWrite | EFileShareExclusive);
	test(error != KErrNone);

	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4290
@SYMTestCaseDesc	    Ensure if output file exists and is read only then it cannot be 
						overwritten with CEZFileToGZip & CEZGZipToFile class
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Pass existing read only output filename parameter to CEZFileToGZip::NewL()
						2.	Pass existing read only output filename parameter to CEZGZipToFile::NewL() 
						& decompress using CEZGZipToFile::InflateL()
@SYMTestExpectedResults CEZFileToGZip::NewL() method should leave with error message, CEZGZipToFile::Inflate()
						should leave with error message & the test must not fail 
@SYMDEF                 REQ8024
*/	
void TestOverwriteOutputFileL()
	{
	_LIT(KReadOnlyGzip, "Hello_Read_Only.gz");	
	TFileName readOnlyNewGZip(KOutputFilePath);
	readOnlyNewGZip.Append(KReadOnlyGzip);
	
	_LIT(KReadOnlyFile, "Hello_Read_Only.txt");
	TFileName readOnlyNewFile(KOutputFilePath);
	readOnlyNewFile.Append(KReadOnlyFile);

	CFileMan* fMan=CFileMan::NewL(TheFs);
	test(fMan!=NULL);
	CleanupStack::PushL(fMan);
	
#if !defined(__WINS__)
	TFileName readOnlyOldGZip(KPath);
	readOnlyOldGZip.Append(KReadOnlyGzip);
	TFileName readOnlyOldFile(KPath);
	readOnlyOldFile.Append(KReadOnlyFile);

	RFile source;
	User::LeaveIfError(source.Open(TheFs,readOnlyOldGZip, EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(source);
	TRAPD(error, fMan->Copy(source, readOnlyNewGZip));
	test(error == KErrNone);
	
	RFile source1;
	User::LeaveIfError(source1.Open(TheFs,readOnlyOldFile, EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(source1);
	TRAP(error, fMan->Copy(source1, readOnlyNewFile));
	test(error == KErrNone);

	CleanupStack::PopAndDestroy(2);
#endif

	TFileName inputFilePath(KPath);
	inputFilePath.Append(TheInputFile);

	RFile input;
	User::LeaveIfError(input.Open(TheFs,inputFilePath,EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(input);

	//setting attributes to read only for readOnlyGZip & readOnlyFile
   	TInt err = fMan->Attribs(readOnlyNewGZip, KEntryAttReadOnly, 0, 0);
	test(err == KErrNone);
	err = fMan->Attribs(readOnlyNewFile, KEntryAttReadOnly, 0, 0);
	test(err == KErrNone);
		
	TRAP(err, CEZFileToGZip::NewL(TheFs, readOnlyNewGZip, input, TheBufferSize));
	test(err != KErrNone);

	RFile output;
	User::LeaveIfError(output.Open(TheFs,readOnlyNewFile,EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(output);

	CEZGZipToFile * gzipToFile = NULL;
	TRAP(err, gzipToFile = CEZGZipToFile::NewL(TheFs, readOnlyNewGZip, output, TheBufferSize));
	test(err == KErrNone);
	CleanupStack::PushL(gzipToFile);
	
	while (err == KErrNone)
	{
		TRAP(err, gzipToFile->InflateL());
	}
	
	test(err != KErrNone); 	

	CleanupStack::PopAndDestroy(4);
	
	//removing the read only attrib before deleting the file
   	fMan=CFileMan::NewL(TheFs);
   	test(fMan!=NULL);
   	CleanupStack::PushL(fMan);
   	err = fMan->Attribs(readOnlyNewGZip, 0, KEntryAttReadOnly, 0);
	test(err == KErrNone);
	err = fMan->Attribs(readOnlyNewFile, 0, KEntryAttReadOnly, 0);
	test(err == KErrNone);
	
	CleanupStack::PopAndDestroy(1);
	
#if !defined(__WINS__)
	DeleteFile(readOnlyNewGZip, ETrue);
	DeleteFile(readOnlyNewFile, ETrue);
#endif
	}

TInt RFSInitForCEZFileToGZipL(TAny*)
	{
	TFileName inputFilePath(KPath);
	inputFilePath.Append(TheInputFile);

	RFile input;
	User::LeaveIfError(input.Open(TheFs,inputFilePath,EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(input);

	TFileName compressFilePath(KOutputFilePath);
	compressFilePath.Append(TheCompressFile);

	CTrapCleanup* cleanup = CTrapCleanup::New();
	RFs rfs; 
	TRAPD(error, CEZFileToGZip::NewL(rfs, compressFilePath, input, TheBufferSize));
	delete cleanup;
	
	CleanupStack::PopAndDestroy(1);
	return error;
	
	}

TInt RFSInitForCEZGZipToFileL(TAny*)
	{
	TFileName compressFilePath(KPath);
	compressFilePath.Append(TheCompressFile);
	
	TFileName decompressFilePath(KOutputFilePath);
	decompressFilePath.Append(TheDecompressFile);

	RFile output;
	User::LeaveIfError(output.Replace(TheFs,decompressFilePath,EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(output);

	CTrapCleanup* cleanup = CTrapCleanup::New();
	RFs rfs; 
	TRAPD(error, CEZGZipToFile::NewL(rfs, compressFilePath, output, TheBufferSize));
	delete cleanup;
	
	CleanupStack::PopAndDestroy(1);
	return error;
	
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4291
@SYMTestCaseDesc	    CEZFileToGZip & CEZGZipToFile class initialisation fails when 
						invalid RFs object is passed
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Pass NULL RFs input parameter to CEZFileToGZip::NewL() 
						2.	Pass NULL RFs input parameter to CEZGZipToFile::NewL()
@SYMTestExpectedResults NewL() method should panic & the test must not fail 
@SYMDEF                 REQ8024
*/	
void TestRFSInitialisation()
	{
	test.Printf(_L("Panic Thread: CEZFileToGZip Class initialisation fails when invalid RFS object is passed"));	
	ThreadForPanicTest(RFSInitForCEZFileToGZipL);
	test.Printf(_L("Panic Thread: CEZGZipToFile Class initialisation fails when invalid RFS object is passed"));
	ThreadForPanicTest(RFSInitForCEZGZipToFileL);
	}

TInt BuffForEZFileToGZipAsZeroL(TAny*)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	CompressFileL(TheInputFile, TheCompressFile, 0);
	delete cleanup;
	return KErrNone;
	}

TInt BuffForEZFileToGZipAsNegativeL(TAny*)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	CompressFileL(TheInputFile, TheCompressFile, -1);
	delete cleanup;
	return KErrNone;
	}

TInt BuffForEZGZipToFileAsZeroL(TAny*)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	DecompressFileL(TheCompressFile, TheDecompressFile, 0);
	delete cleanup;
	return KErrNone;
	}

TInt BuffForEZGZipToFileAsNegativeL(TAny*)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	DecompressFileL(TheCompressFile, TheDecompressFile, -1);
	delete cleanup;
	return KErrNone;	
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4292
@SYMTestCaseDesc	    CEZFileToGZip & CEZGZipToFile class initialisation 
						fails when invalid buffer size parameter passed in
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Pass buffer size parameter value as 0 to CEZFileToGZip::NewL()  
						2.	Pass buffer size parameter as a negative value to CEZFileToGZip::NewL()
						3.	Pass buffer size parameter value as 0 to CEZGZipToFile::NewL()  
						4.	Pass buffer size parameter as a negative value to CEZGZipToFile::NewL()
@SYMTestExpectedResults NewL() method should panic & the test must not fail 
@SYMDEF                 REQ8024
*/	
void TestInvalidBufferSize()
	{
	
	test.Printf(_L("Panic Thread: CEZFileToGZip Class initialisation fails when buffer size is passed as Zero"));	
	ThreadForPanicTest(BuffForEZFileToGZipAsZeroL);
	test.Printf(_L("Panic Thread: CEZFileToGZip Class initialisation fails when buffer size is passed as Negative"));
	ThreadForPanicTest(BuffForEZFileToGZipAsNegativeL);
	test.Printf(_L("Panic Thread: CEZGZipToFile Class initialisation fails when buffer size is passed as Zero"));
	ThreadForPanicTest(BuffForEZGZipToFileAsZeroL);
	test.Printf(_L("Panic Thread: CEZGZipToFile Class initialisation fails when buffer size is passed as Negative"));
	ThreadForPanicTest(BuffForEZGZipToFileAsNegativeL);

	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4293
@SYMTestCaseDesc	    Compress & Decompress another file with CEZFileToGZip & CEZGZipToFile 
						after calling ResetL method
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Open existing test file and pass it to CEZFileToGZip::NewL() & compress using CEZFileToGZip::DeflateL function
						2.	Call CEZFileToGZip::ResetL() with new file parameters
						3.	Compress the new file with CEZFileToGZip::DeflateL()
						4.	Create or open output decompressed file and pass compressed file to CEZGZipToFile::NewL() & decompress using CEZGZipToFile::InflateL() function
						5.	Call CEZGZipToFile::ResetL() with new compressed file parameters
						6.	Decompress the new compressed file with CEZGZipToFile::InflateL()
@SYMTestExpectedResults Files should be compressed with no errors i.e. KErrNone & the test must not fail 
@SYMDEF                 REQ8024
*/	
void TestCompressDecompressResetL()
	{
    
	TFileName inputFilePath(KPath);
	inputFilePath.Append(TheInputFile);

	_LIT(KGzipFile,"Hello_Reset.gz");
	TFileName gzipFileName(KGzipFile);
	TFileName compressfile(KOutputFilePath);
	compressfile.Append(gzipFileName);
	
	RFile input;
	User::LeaveIfError(input.Open(TheFs,inputFilePath,EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(input);
	
	CEZFileToGZip *fileToGzip = CEZFileToGZip::NewLC(TheFs, compressfile, input, TheBufferSize);
	while(fileToGzip->DeflateL()){/*do nothing*/}
	
	fileToGzip->ResetL(TheFs, compressfile, input, TheBufferSize);
	while(fileToGzip->DeflateL()){/*do nothing*/}
	
	CleanupStack::PopAndDestroy(2);
	
#if !defined(__WINS__)
	DeleteFile(gzipFileName);
#endif

	TFileName compressFilePath(KPath);
	compressFilePath.Append(TheCompressFile);

	_LIT(KTxtFile,"Hello_Reset.txt");
	TFileName txtFileName(KTxtFile);
	TFileName decompressfile(KOutputFilePath);
	decompressfile.Append(txtFileName);

	RFile output;
	User::LeaveIfError(output.Replace(TheFs, decompressfile,EFileStream | EFileWrite | EFileShareExclusive));
	CleanupClosePushL(output);

	CEZGZipToFile *gzipToFile = CEZGZipToFile::NewLC(TheFs, compressFilePath, output, TheBufferSize);
	while(gzipToFile->InflateL()){/*do nothing*/}

	gzipToFile->ResetL(TheFs, compressFilePath, output, TheBufferSize);
	while(gzipToFile->InflateL()){/*do nothing*/}
	
	CleanupStack::PopAndDestroy(2);
	
#if !defined(__WINS__)
	DeleteFile(txtFileName);
#endif

	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4294
@SYMTestCaseDesc	    CEZGZipToFile class initialisation fails when source 
						gzip file header is malformed: Header is too small
@SYMTestPriority 	    High
@SYMTestActions  	    Pass corrupted gzip file as an input parameter to CEZGZipToFile::NewL()  
						Note: Change the header of existing gzip file by removing few characters
@SYMTestExpectedResults NewL() method should leave with error message KEZlibErrBadGZipHeader 
						& the test must not fail 
@SYMDEF                 REQ8024
*/	
void TestGzipHeaderSmallL()
	{
	
	_LIT(KGzFile,"Hello_HeaderChanged.gz");	
	TFileName compressfile(KPath);
	compressfile.Append(KGzFile);

	TFileName decompressFilePath(KOutputFilePath);
	decompressFilePath.Append(TheDecompressFile);

	RFile output;
	User::LeaveIfError(output.Replace(TheFs, decompressFilePath,EFileStream | EFileWrite | EFileShareExclusive));
	CleanupClosePushL(output);
	
	TRAPD(error, CEZGZipToFile::NewL(TheFs, compressfile, output, TheBufferSize));
	test(error == KEZlibErrBadGZipHeader);

	CleanupStack::PopAndDestroy(1);

	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4295
@SYMTestCaseDesc	    CEZGZipToFile class initialisation fails when source gzip file 
						header is malformed: Header ID number is incorrect
@SYMTestPriority 	    High
@SYMTestActions  	    Pass corrupted gzip file as an input parameter to CEZGZipToFile::NewL()  
						Note: Change the header ID of existing gzip file
@SYMTestExpectedResults NewL() method should leave with error message KEZlibErrBadGZipHeader 
						& the test must not fail 
@SYMDEF                 REQ8024
*/	
void TestGzipHeaderIdIncorrectL()
	{	

	_LIT(KGzFile,"Hello_HeaderIdChanged.gz");
	TFileName compressfile(KPath);
	compressfile.Append(KGzFile);

	TFileName decompressFilePath(KOutputFilePath);
	decompressFilePath.Append(TheDecompressFile);

	RFile output;
	User::LeaveIfError(output.Replace(TheFs, decompressFilePath,EFileStream | EFileWrite | EFileShareExclusive));
	CleanupClosePushL(output);
	
	TRAPD(error, CEZGZipToFile::NewL(TheFs, compressfile, output, TheBufferSize));
	test(error == KEZlibErrBadGZipHeader);
	
	CleanupStack::PopAndDestroy(1);
	
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4296
@SYMTestCaseDesc	    CEZGZipToFile class fails while decompressing the gzip 
						file with the CRC changed in the trailer
@SYMTestPriority 	    High
@SYMTestActions  	    Pass corrupted gzip file as an input parameter to CEZGZipToFile::NewL()
						Note: Change the CRC within the Trailer of existing gzip file
@SYMTestExpectedResults NewL() method should leave with error message KErrNotFound & the test must not fail
						Note: KErrNotFound error is fired while opening the corrupted gzip file
@SYMDEF                 REQ8024
*/	
void TestGzipTrailerCRCL()
	{

	_LIT(KGzFile,"Hello_TrailerIdChanged.gz");	
	TFileName compressfile(KPath);
	compressfile.Append(KGzFile);

	TFileName decompressFilePath(KOutputFilePath);
	decompressFilePath.Append(TheDecompressFile);

	RFile output;
	User::LeaveIfError(output.Replace(TheFs, decompressFilePath,EFileStream | EFileWrite | EFileShareExclusive));
	CleanupClosePushL(output);
	
	TRAPD(error, CEZGZipToFile::NewL(TheFs, compressfile, output, TheBufferSize));
	test(error == KErrNotFound);
	
	CleanupStack::PopAndDestroy(1);

	}

void TestCompressDecompressL()
	{

	TFileName inputFilePath(KPath);
	inputFilePath.Append(TheInputFile);
	
	TFileName compressFilePath(KOutputFilePath);
	compressFilePath.Append(TheCompressFile);

	RFile input;
	User::LeaveIfError(input.Open(TheFs,inputFilePath,EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(input);
	
	CEZFileToGZip *fileToGzip = CEZFileToGZip::NewLC(TheFs, compressFilePath, input, TheBufferSize);
	while(fileToGzip->DeflateL()){/*do nothing*/}
	
	TFileName decompressFilePath(KOutputFilePath);
	decompressFilePath.Append(TheDecompressFile);

	TFileName compressFilePath1(KPath);
	compressFilePath1.Append(TheCompressFile);

	RFile output;
	User::LeaveIfError(output.Replace(TheFs, decompressFilePath,EFileStream | EFileWrite | EFileShareExclusive));
	CleanupClosePushL(output);

	CEZGZipToFile *gzipToFile = CEZGZipToFile::NewLC(TheFs, compressFilePath1, output, TheBufferSize);
	while(gzipToFile->InflateL()){/*do nothing*/}

	CleanupStack::PopAndDestroy(4);
	
#if !defined(__WINS__)
	DeleteFile(TheCompressFile);
	DeleteFile(TheDecompressFile);
#endif
	}
	
/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4297
@SYMTestCaseDesc	    Out of Memory(OOM) test for CEZFileToGZip &  CEZGZipToFile class
@SYMTestPriority 	    High
@SYMTestActions  	    1.	Set Heap Failure using the macro as:
							__UHEAP_SETFAIL(RHeap::EDeterministic, count)
						2.	Mark the heap using macro: __UHEAP_MARK
						3.	Open existing test file and pass it to CEZFileToGZip::NewL() compress using CEZFileToGZip::DeflateL() function
						4.	Create or open output decompressed file and pass compressed file to CEZGZipToFile::NewL() & decompress using CEZGZipToFile::InflateL() function
						5.	Check there is no memory leak using the macro: __UHEAP_MARKEND
						6.	Repeat the Steps 1-5 increasing the heap value rate i.e. count
@SYMTestExpectedResults The test succeeds with no memory leak on the heap as well as no errors (i.e. KErrNone)
						& the test must not fail 
@SYMDEF                 REQ8024
*/	
void TestOOMGzipL()
	{
   	TInt processHandlesS = 0;
	TInt threadHandlesS = 0;
	TInt processHandlesE = 0;
	TInt threadHandlesE = 0;
	TInt err = KErrNone;
	RThread().HandleCount(processHandlesS, threadHandlesS);
	for(TInt count=1; count<=100; ++count)
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;
		
		TRAP(err,TestCompressDecompressL());
		__UHEAP_MARKEND;
		if(err == KErrNone)
			{
			RDebug::Print(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else 
			{
			test(err == KErrNoMemory);
			}
		__UHEAP_RESET;
		}
	
	test(err == KErrNone);
	__UHEAP_RESET;
	
	RThread().HandleCount(processHandlesE, threadHandlesE);
	
	test(processHandlesS == processHandlesE);
	test(threadHandlesS == threadHandlesE);

	}
/*
 * Helper function for TestDEF117325ToGZipL
 * Deflates a file and then calls ResetL()
 */
void DeflateResetL()
	{
	_LIT(KGzipFile,"Hello_Reset.gz");
	TFileName compressFile(KOutputFilePath);
	compressFile.Append(KGzipFile);

	TFileName inputFilePath(KPath);
	inputFilePath.Append(TheInputFile);
	
	RFile input;	
	User::LeaveIfError(input.Open(TheFs, inputFilePath, EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(input);
		
	CEZFileToGZip *fileToGzip = CEZFileToGZip::NewLC(TheFs, compressFile, input, TheBufferSize);
	while(fileToGzip->DeflateL()) {/*do nothing*/}
	
	fileToGzip->ResetL(TheFs, compressFile, input, TheBufferSize);
	
	CleanupStack::PopAndDestroy(2);
	
	#if !defined(__WINS__)
	DeleteFile(TheInputFile);
	#endif
	}

/*
 * Helper function for TestDEF117325ToFileL
 * Inflates a GZip file and then calls ResetL()
 */
void InflateResetL()
	{
	_LIT(KTxtFile, "Hello_Reset.txt");
	TFileName decompressFile(KOutputFilePath);
	decompressFile.Append(KTxtFile);

	TFileName compressFilePath(KPath);
	compressFilePath.Append(TheCompressFile);
		
	RFile output;
	User::LeaveIfError(output.Replace(TheFs, decompressFile, EFileStream | EFileWrite | EFileShareExclusive));
	CleanupClosePushL(output);
	
	CEZGZipToFile *gzipToFile = CEZGZipToFile::NewLC(TheFs, compressFilePath, output, TheBufferSize);
	while(gzipToFile->InflateL()){/*do nothing*/}
	
	gzipToFile->ResetL(TheFs, compressFilePath, output, TheBufferSize);
	
	CleanupStack::PopAndDestroy(2);
	
	#if !defined(__WINS__)
	DeleteFile(TheCompressFile);
	#endif
	}

/*
 * Helper function for TestDEF117325ToGZipL and TestDEF117325ToFileL.
 * Checks for memory leaks in OOM situations.
 */
void DEF117325L(void (*aResetL)())
	{
	TInt err = KErrNone;
	for(TInt count = 0; count <= 100; count++)
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;	
		
		TRAP(err, aResetL());
		
		__UHEAP_MARKEND;
		__UHEAP_RESET;
		
		if(err == KErrNone)
			{
			RDebug::Print(_L("The test succeeded at heap failure rate = %d.\n"), count);
			break;
			}
		else 
			{
			test(err == KErrNoMemory);
			}
		}
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4319
@SYMTestCaseDesc	    To test that there are no memory leaks when calling ResetL from CEZFileToGZip
@SYMTestPriority 	    Medium
@SYMTestActions  	    1.	Set Heap Failure using the macro __UHEAP_SETFAIL(RHeap::EDeterministic, count)
						2.	Mark the heap using macro __UHEAP_MARK
						3.	Open an existing test file and pass it to CEZFileToGZip::NewL() and compress it using CEZFileToGZip::DeflateL function
						4.	Call CEZFileToGZip::ResetL() with new file parameters
						5.	Check there is no memory leak using the macro __UHEAP_MARKEND
						6.	Repeat steps 1 - 5 increasing the heap value rate i.e. count
@SYMTestExpectedResults The test succeeds with no memory leak on the heap as well as no errors (i.e. KErrNone) 
@SYMDEF                 REQ8024
*/	
void TestDEF117325ToGZipL()
	{		
	DEF117325L(&DeflateResetL);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-UT-4320
@SYMTestCaseDesc	    To test that there are no memory leaks when calling ResetL from CEZGZipToFile
@SYMTestPriority 	    Medium
@SYMTestActions  	    1.	Open an existing test file and pass it to CEZFileToGZip::NewL() and compress it using CEZFileToGZip::DeflateL 
						2.	Set Heap Failure using the macro __UHEAP_SETFAIL(RHeap::EDeterministic, count)
						3.	Mark the heap using macro __UHEAP_MARK
						4.	Open the compressed test file and pass it to CEZGZipToFile::NewL() and decompress it using CEZGZipToFile::InflateL function
						5.	Call CEZGZipToFile::ResetL() with new file parameters
						6.	Check there is no memory leak using the macro __UHEAP_MARKEND
						7.	Repeat steps 2 - 6 increasing the heap value rate i.e. count
@SYMTestExpectedResults The test succeeds with no memory leak on the heap as well as no errors (i.e. KErrNone)
@SYMDEF                 REQ8024
*/	
void TestDEF117325ToFileL()
	{	
	
	// Create a GZip file that can be used by CEZGZipToFile
	CompressFileL(TheInputFile, TheCompressFile);
	
	DEF117325L(&InflateResetL);
	
	#if !defined(__WINS__)
	DeleteFile(TheCompressFile);
	#endif
	}

	

void RunTestL()
	{
	
	User::LeaveIfError(TheFs.Connect()); //Connect to file session
	CleanupClosePushL(TheFs);
	
#if !defined(__WINS__)
	TInt err = TheFs.MkDirAll(KOutputFilePath);  
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		test.Printf(_L("Error while creating dir => Error: %d"), err);
		User::Leave(err);
		}
#endif
	
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4286 Compression - Decompression of Gzip file "));
	TestCompressDecompressGzip();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4287 Compression - Decompression of Gzip file with big & small buffer size "));
	TestOutputBuffSize();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4288 Class initialisation fails when invalid input rfile supplied "));
	TestRFileInitialisation();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4289 Class initialisation fails when invalid output filename supplied "));
	TestInvalidFilenameL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4290 Class initialisation fails while overwriting the read only output file "));
	TestOverwriteOutputFileL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4291 Class initialisation fails when invalid RFS object is passed "));
	TestRFSInitialisation();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4292 Panic Thread: Class initialisation fails when invalid buffer size parameter passed in "));
	TestInvalidBufferSize();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4293 Compress another file with CEZFileToGZip & CEZGZipToFile after calling ResetL method "));
	TestCompressDecompressResetL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4294 CEZGZipToFile class initialisation fails when source gzip file header is malformed: Header is too small "));
	TestGzipHeaderSmallL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4295 CEZGZipToFile class initialisation fails when source gzip file header is malformed: Header ID number is incorrect "));
	TestGzipHeaderIdIncorrectL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4296 CEZGZipToFile class fails while decompressing the gzip file with the CRC changed in the trailer "));
	TestGzipTrailerCRCL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4297 Out of Memory(OOM) test for CEZFileToGZip & CEZGZipToFile class "));
	TestOOMGzipL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4319 Out of Memory(OOM) test for DEF117325: Ezlib: ResetL() in gzip.cpp - a member variable can be deleted twice (CEZFileToGZip) "));
	TestDEF117325ToGZipL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4320 Out of Memory(OOM) test for DEF117325: Ezlib: ResetL() in gzip.cpp - a member variable can be deleted twice (CEZGZipToFile) "));
	TestDEF117325ToFileL();
		

#if !defined(__WINS__)
	CFileMan* fMan=CFileMan::NewL(TheFs);
   	test(fMan!=NULL);
   	CleanupStack::PushL(fMan);
 	err = fMan->RmDir(_L("C:\\test\\gzip"));
 	CleanupStack::PopAndDestroy(1);
#endif
 	
 	CleanupStack::PopAndDestroy(1);
	
	}	

GLDEF_C TInt E32Main()
	{
	
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start( _L("Starting Gzip Tests..") );

	TheTrapCleanup=CTrapCleanup::New();

	TRAPD(err,RunTestL());
	test (err==KErrNone);

	test.End();
	test.Close();
	
	delete TheTrapCleanup;

	__UHEAP_MARKEND;
	return KErrNone;

	}
