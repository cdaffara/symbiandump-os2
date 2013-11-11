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

#include <e32cmn.h>
#include <f32file.h>
#include <e32test.h>
#include <ezgzip.h>
#include <ezfilebuffer.h>
#include <ezdecompressor.h>

_LIT(KTestTitle, "Decompress Test. ");
_LIT(KFileOutputPath, "c:\\test\\decompresstest\\");
_LIT(KFileInputPath, "c:\\test\\decompresstest\\testfiles\\");

RTest test(_L("decompresstest.exe"));

/* Test macro and function */
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
    if (aValue != aExpected)
    	{
        test.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
        test.operator()(EFalse, aLine);
        }
    }
#define test2(a, b) Check(a, b, __LINE__)


void DecompressZipL(RFile &aInputFile, RFile &aOutputFile)
	{
	CEZFileBufferManager *bufferManager = CEZFileBufferManager::NewLC(aInputFile, aOutputFile);	
	CEZDecompressor *decompressor = CEZDecompressor::NewLC(*bufferManager, MAX_WBITS);
	while(decompressor->InflateL())
		{
		// Do nothing here
		}
	
	CleanupStack::PopAndDestroy(3);
	}

void DecompressGZipL(RFs &aFs, RFile &aOutputFile, const TFileName &aFileInputPath, const TFileName &aFileName)
	{
	aFs.SetSessionPath(aFileInputPath);
	CEZGZipToFile *decompressor = CEZGZipToFile::NewLC(aFs, aFileName, aOutputFile);
	while(decompressor->InflateL())
		{
		// Do nothing here
		}
	
	CleanupStack::PopAndDestroy(1);
	}

void TestDecompressL(RFs &aFs, const TFileName &aFileInputPath, const TFileName &aFileName)
	{
	test.Printf(_L("\nUsing file %S. "), &aFileName);
	
	// Create temporary output file and open it for writing
	TFileName outputFileName;
	RFile outputFile;
	
	aFs.MkDir(KFileOutputPath);
	outputFile.Temp(aFs, KFileOutputPath, outputFileName, EFileWrite);
	CleanupClosePushL(outputFile);
	
	// Decompress file based on whether its a gzip file or zip archive
	aFs.SetSessionPath(aFileInputPath);
	EZGZipFile gzipFile;	
	if(gzipFile.IsGzipFileL(aFs, aFileName))
		{
		// Check for memory leaks
		__UHEAP_MARK;
		TRAPD(err, DecompressGZipL(aFs, outputFile, aFileInputPath, aFileName));
		__UHEAP_MARKEND;
		switch(err)
			{
			case KEZlibErrStream:
			case KEZlibErrData:
			case KEZlibErrBuf:
			case KEZlibErrVersion:
			case KEZlibErrUnexpected:
			case KEZlibErrDeflateTerminated:
			case KEZlibErrInflateTerminated:
			case KEZlibErrInflateDictionary:
			case KEZlibErrNotGZipFile:
			case KEZlibErrInvalidCompression:
			case KEZlibErrBadGZipHeader:
			case KEZlibErrBadGZipTrailer:
			case KEZlibErrBadGZipCrc: 
				break;
			default: 
				if(err > KErrNone || err < KErrNoSecureTime)
					{
					test.Printf(_L("FAILED! error = %d"), err);
					test2(err, KErrNone);	
					}
			}
		}
	else
		{
		// Open the input file for reading
        RFile inputFile;
        
        aFs.SetSessionPath(aFileInputPath);
        User::LeaveIfError(inputFile.Open(aFs, aFileName, EFileRead));
        CleanupClosePushL(inputFile);
		
		__UHEAP_MARK;
		TRAPD(err, DecompressZipL(inputFile, outputFile));
		__UHEAP_MARKEND;
		switch(err)
			{
			case KEZlibErrStream:
			case KEZlibErrData:
			case KEZlibErrBuf:
			case KEZlibErrVersion:
			case KEZlibErrUnexpected:
			case KEZlibErrDeflateTerminated:
			case KEZlibErrInflateTerminated:
			case KEZlibErrInflateDictionary: 
				break;
			default: 
				if(err > KErrNone || err < KErrNoSecureTime)
					{
					test.Printf(_L("FAILED! error = %d"), err);
					test2(err, KErrNone);
					}
			}
			
			CleanupStack::PopAndDestroy(1);
		}
	CleanupStack::PopAndDestroy(1);
	
	// Delete temporary output file
	aFs.SetSessionPath(KFileOutputPath);
	aFs.Delete(outputFileName);
	}

/**
@SYMTestCaseID          SYSLIB-EZLIB2-CT-4315
@SYMTestCaseDesc	    To test for invalid return codes, panics and memory leaks when 
                        decompressing invalid Zip archive and GZip files.
@SYMTestPriority 	    Medium
@SYMTestActions  	    1.	Create an instance of CDir and get the number of files and 
                            directories in the test file directory.
                        2.	Loop through the list of files and directories, held by CDir. 
                            a.	If the current entry is a directory navigate to it and go 
                                back to step 1.
                            b.	If the current entry is a file, open a temporary output 
                                file for writing. Then check if we have a GZip or Zip archive.
                            c.	If we have a GZip file:
                                i.	Call __UHEAP_MARK
                                ii.	Create a CEZGZipToFile passing it the open output file 
                                    and the name of the input file.
                                iii.Call InflateL as many times as needed to decompress 
                                    the input file.
                                iv.	Call __UHEAP_MARKEND
                                v.	Check any leave errors against a list of expected 
                                    errors. If an unexpected leave value is returned, panic.
                            d.	If we have a Zip archive
                                i.	Open the input file for reading.
                                ii.	Call __UHEAP_MARK.
                                iii.Create a CEZFileBufferManager passing it the open 
                                    input and output files.
                                iv.	Create a CEZDecompressor passing it the 
                                    CEZFileBufferManager and windowBits of 15.
                                v.	Call InflateL as many times as needed to decompress 
                                    the input file.
                                vi.	Call __UHEAP_MARKEND
                                vii.Check any leave errors against a list of expected 
                                    errors. If an unexpected leave value is returned, panic.
                            e.	Delete the temporary output file.
@SYMTestExpectedResults There will be no memory leaks or panics and all return codes will 
                        be the expected ones.
@SYMDEF                 REQ8024
*/
void TestL(RFs &aFs, TFileName aFileInputPath)
	{
	test.Printf(_L("\nIn directory %S. "), &aFileInputPath);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-CT-4315 "));
	
	CDir *inputFiles;
	aFs.GetDir(aFileInputPath, KEntryAttNormal|KEntryAttDir, ESortByName | EDirsFirst, inputFiles);
	CleanupStack::PushL(inputFiles);
	
	TInt numInputFiles = inputFiles->Count();
	for(TInt i = 0; i < numInputFiles; i++)
		{
		TEntry entry = (*inputFiles)[i];
		
		// If we have a directory we need to try decompressing the files contained in it
		if(entry.IsDir())
			{
			TFileName currentDir = aFileInputPath;
			currentDir.Append(entry.iName);
			currentDir.Append(_L("\\"));
			
			TestL(aFs, currentDir);
			}
		else
			{
			TRAPD(err, TestDecompressL(aFs, aFileInputPath, entry.iName));
			test2(err, KErrNone);
			}
		}
	CleanupStack::PopAndDestroy(1);
	}

void RunTestL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TFileName filePath(KFileInputPath);
	TestL(fs, filePath);

	CleanupStack::PopAndDestroy(1);
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(KTestTitle);

	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(err, RunTestL());
	test2(err, KErrNone);
	
	test.End();
	test.Close();
	delete cleanup;

	__UHEAP_MARKEND;
	return KErrNone;
	}
