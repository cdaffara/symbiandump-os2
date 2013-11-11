// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_backwardscompatibilitytests.h"

CBackwardsCompatibilityTests::CBackwardsCompatibilityTests(const TDesC &aStepName) : CEZlibEZipTests(aStepName)
	{
	}

TVerdict CBackwardsCompatibilityTests::doTestStepL()
	{
 	if(TestStepName() == KTestDefOldInfNewCompatibilityL)
		{
   		INFO_PRINTF1(KTestDefOldInfNewCompatibilityL);
   		SetTestStepResult(TestDeflateOldInflateNewCompatibitlityL());
   		}
	else if(TestStepName() == KTestDefNewInfOldCompatibilityL)
		{
   		INFO_PRINTF1(KTestDefNewInfOldCompatibilityL);
   		SetTestStepResult(TestDeflateNewInflateOldCompatibitlityL());
   		}
	else if(TestStepName() == KTestGZipDefOldInfNewCompatibilityL)
		{
   		INFO_PRINTF1(KTestGZipDefOldInfNewCompatibilityL);
   		SetTestStepResult(TestGZipDeflateOldInflateNewCompatibitlityL());
   		}
	else if(TestStepName() == KTestGZipDefNewInfOldCompatibilityL)
		{
   		INFO_PRINTF1(KTestGZipDefNewInfOldCompatibilityL);
   		SetTestStepResult(TestGZipDeflateNewInflateOldCompatibitlityL());
   		}
	else if(TestStepName() == KTestZipArchiveDecompressCompatibilityL)
		{
   		INFO_PRINTF1(KTestZipArchiveDecompressCompatibilityL);
   		SetTestStepResult(TestZipArchiveDecompressCompatibitlityL());
   		}
	else if(TestStepName() == KTestOSCompatibilityL)
		{
		INFO_PRINTF1(KTestOSCompatibilityL);
		SetTestStepResult(TestOSCompatibilityL());
		}
	else if(TestStepName() == KTestOSGZipCompatibilityL)
			{
			INFO_PRINTF1(KTestOSGZipCompatibilityL);
			SetTestStepResult(TestOSGZipCompatibilityL());
			}
	return TestStepResult();
	}

/*
 * This function gets the CDir object for the given path & folder name & 
 * also generates the actual path of the CDir to be used later...
 */
void CBackwardsCompatibilityTests::GetCDirLC(const TFileName &aFolderPath, const TFileName &aFolderName, TFileName &aCDirFolderPath, CDir* &aCDir)
	{
	aCDirFolderPath.Append(aFolderPath);
	aCDirFolderPath.Append(aFolderName);
	aCDirFolderPath.Append(KBackslash);
	
	TInt err = iRfs.GetDir(aCDirFolderPath, KEntryAttNormal|KEntryAttDir, ESortByName | EDirsFirst, aCDir);
	CleanupStack::PushL(aCDir);
	}

/*
 * Compares two file's data to see if both of them are the same & 
 * returns ETrue or EFalse accordingly
 */
 TBool CBackwardsCompatibilityTests::CompareFileDataL(TFileName &aInputFile, TFileName &bOutputFile)
	{
	RFile source;
	User::LeaveIfError(source.Open(iRfs, aInputFile, EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(source);
		
	TInt sourceSize;
	source.Size(sourceSize);
	if(sourceSize <= 0)
		{
		INFO_PRINTF1(_L("Invalid source file size"));
		User::Leave(KErrUnknown);
		}	 
	
	RFile dest;
	User::LeaveIfError(dest.Open(iRfs, bOutputFile, EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(dest);
	
	TInt destSize;
	dest.Size(destSize);
	if(destSize <= 0)
		{
		INFO_PRINTF1(_L("Invalid destination file size"));
		User::Leave(KErrUnknown);
		}	 
	
	if(sourceSize != destSize)
		{
		INFO_PRINTF1(_L("The Size of file deflated using old-ezlib/ezlib2 is not same as the size of file inflated using ezlib2/old-ezlib"));
		CleanupStack::PopAndDestroy(2);
		return EFalse;
		}
	
	HBufC8* sourceData = HBufC8::NewMaxLC(TheBufferSize);
	HBufC8* destData = HBufC8::NewMaxLC(TheBufferSize);
	TPtr8 tSourceBufPtr = sourceData->Des();
	TPtr8 tDestBufPtr = destData->Des();
	
	TInt offset = 0;
	TInt read = 0;
		
	while (offset < sourceSize)
		{
		read = (sourceSize < TheBufferSize) ? sourceSize : TheBufferSize;
		User::LeaveIfError(source.Read(0, tSourceBufPtr, read));
		User::LeaveIfError(dest.Read(0, tDestBufPtr, read));
		
		offset += tSourceBufPtr.Length();
		
		if(sourceData->Compare(*destData) != 0)
			{
			INFO_PRINTF1(_L("The file deflated using old-ezlib/ezlib2 is not same as the file inflated using new ezlib2/old-ezlib"));
			CleanupStack::PopAndDestroy(4);
			return EFalse;
			}
		}
	CleanupStack::PopAndDestroy(4);
	return ETrue;
	}

 /*
  * Compares two folders data to see if both of them are the same & also compares individual file within the folder
  * & returns ETrue or EFalse accordingly
  */
TBool CBackwardsCompatibilityTests::CompareFolderDataL(const CDir& aEzlibDir, const TFileName& aEzlibDirPath, const CDir& aEzlib2Dir, const TFileName& aEzlib2DirPath)
 	{
    TInt countEzlib = aEzlibDir.Count();
    TInt countEzlib2 = aEzlib2Dir.Count();
    
    if(countEzlib != countEzlib2)
    	{
    	INFO_PRINTF1(_L("The count of the files in the folder decompressed using old-ezlib is not the same as the count of the files/folder decompressed using ezlib2"));
		return EFalse;
	   	}
    
    TInt i=0;
    TInt fCount=0, dCount=0;
    TBool bEzlibEntryIsDir;
    TBool bEzlib2EntryIsDir;
        
    while (i<countEzlib)
        {
        const TEntry& ezlibEntry = aEzlibDir[i];
        const TEntry& ezlib2Entry = aEzlib2Dir[i++];
        bEzlibEntryIsDir = ezlibEntry.IsDir();
        bEzlib2EntryIsDir = ezlib2Entry.IsDir();
        
        if (bEzlibEntryIsDir && bEzlib2EntryIsDir)
            {
            dCount++;
            INFO_PRINTF3(_L("%- 16S <DIR NAME> %- 16S \n"), &ezlibEntry.iName, &ezlib2Entry.iName);
            
            TFileName tmpEzlibFolderPath;
            CDir* pDEzlib;
            GetCDirLC(aEzlibDirPath, ezlibEntry.iName, tmpEzlibFolderPath, pDEzlib);
            
            TFileName tmpEzlib2FolderPath;
            CDir* pDEzlib2;
            GetCDirLC(aEzlib2DirPath, ezlib2Entry.iName, tmpEzlib2FolderPath, pDEzlib2);
            
            if(CompareFolderDataL(*pDEzlib, tmpEzlibFolderPath, *pDEzlib2, tmpEzlib2FolderPath) == EFalse)
            	{
               	INFO_PRINTF3(_L("%- 16S folder data differs with %- 16S folder data\n"), &ezlibEntry.iName, &ezlibEntry.iName);
               	return EFalse;
                }
        	CleanupStack::PopAndDestroy(2);
            }
        else if(!bEzlibEntryIsDir && !bEzlib2EntryIsDir)
            {
            fCount++;
            
            if(ezlibEntry.iName != ezlib2Entry.iName)
            	{
            	INFO_PRINTF3(_L("%- 16S filename differs with %- 16S filename\n"), &ezlibEntry.iName, &ezlibEntry.iName);
            	return EFalse;
            	}
            INFO_PRINTF3(_L("EZLIB Filename: %- 16S %+ 8d\n"), &ezlibEntry.iName, ezlibEntry.iSize);
            INFO_PRINTF3(_L("EZLIB2 Filename: %- 16S %+ 8d\n"), &ezlib2Entry.iName, ezlib2Entry.iSize);
            
            TFileName ezlibFile, ezlib2File;
            ezlibFile.Append(aEzlibDirPath);
            ezlibFile.Append(ezlibEntry.iName);
            
            ezlib2File.Append(aEzlib2DirPath);
            ezlib2File.Append(ezlib2Entry.iName);
            
            if(CompareFileDataL(ezlibFile, ezlib2File) == EFalse)
            	{
            	INFO_PRINTF3(_L("%- 16S file data differs with %- 16S file data\n"), &ezlibEntry.iName, &ezlibEntry.iName);
            	return EFalse;
            	}
            }
        else 
        	{
        	INFO_PRINTF3(_L("%- 16S <Dir Or File> %- 16S \n"), &ezlibEntry.iName, &ezlib2Entry.iName);
        	INFO_PRINTF3(_L("%S AND %S Folder data is different"), &iZipArchiveEzlibFolder, &iZipArchiveEzlib2Folder);
        	return EFalse;
        	}
        }
    INFO_PRINTF4(_L("Dirs = %d Files = %d count for folder name: %S \n"),dCount,fCount, &aEzlibDirPath);
 	
	return ETrue;
 	}
/*
 * Decompresses a Zip archive using EZlib and EZlib2 and then compares the files
 */
TVerdict CBackwardsCompatibilityTests::ZipArchiveDecompressCompatibitlityL()
	{
	DoEZlibZipArchiveDecompressL();
	
	DoEZlib2ZipArchiveDecompressL();
	
	TFileName filePath(KFilePath);
    
	//get the CDir object for ezlib & ezlib2 & then compare those folder data.
	TFileName tmpEzlibFolderPath;
    CDir* pDEzlib;
    GetCDirLC(filePath, iZipArchiveEzlibFolder, tmpEzlibFolderPath, pDEzlib);

    TFileName tmpEzlib2FolderPath;
    CDir* pDEzlib2;
    GetCDirLC(filePath, iZipArchiveEzlib2Folder, tmpEzlib2FolderPath, pDEzlib2);

	TVerdict verdict = (CompareFolderDataL(*pDEzlib, tmpEzlibFolderPath, *pDEzlib2, tmpEzlib2FolderPath)) ? EPass : EFail;
	CleanupStack::PopAndDestroy(2);
	
	return verdict;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4301
@SYMTestCaseDesc     	Testing the CEZCompressor::DeflateL() with old EZLib & 
						CEZDecompressor::InflateL() with new EZLib2 & comparing the files 
						for backward compatibility test.
@SYMTestPriority     	High
@SYMTestActions      	Deflate using EZLib:
							1.	Open the input file for reading and create the deflated 
								output file for writing. 
							2.	Create a TOLDEZLIB::CEZFileBufferManager object passing it 
								the input and output files. 
							3.	Create a TOLDEZLIB::CEZCompressor object passing it the 
								TOLDEZLIB::CEZFileBufferManager object and a combination 
								of specified compression parameters as mentioned below in 
								the Note.
							4.	Call TOLDEZLIB::CEZCompressor::DeflateL() as many times as 
								needed to compress the data.. 
							Note: To use old ezlib.dll, TOLDEZLIB namespace has to be used.
						Inflate using EZLib2:
							5.	Open the deflated file for reading and create the output 
								file for writing. 
							6.	Create a CEZFileBufferManager object passing it the input 
								and output. 
							7.	Create a CEZDecompressor object passing it the buffer 
								CEZFileBufferManager object with input parameters as 
								aWindowBits. Note: aWindowBits value should be the same as 
								used while compressing the file in Step 3. 
							8.	Call CEZDecompressor::InflateL() as many times as needed to 
								decompress the data.
							9.	Compare the inflated output file with the original input 
								file used in step 1.
							10.	Cleanup all objects.
							11.	Perform the above steps 1 – 10 with different types of 
								input files & different sizes of input files & varying 
								different compression parameters.
						
						Note: 
						The test should be repeated for different types of input files as 
						well as different size of input file for e.g.:
							•	TXT
							•	IMG
							•	PNG
							•	JAR
							•	Varying sizes of each input file should be tested starting 
								from a couple of kilobytes up to a few hundred megabytes.
						And with compression parameters as:
							•	EDefaultCompression, ENoCompression, EBestSpeed, 
								EBestCompression for aLevel
							•	8 and EMaxWBits for aWindowBits
							•	1, 9 and EDefMemLevel for aMemLevel
							•	EDefaultStrategy, EFiltered and EHuffmanOnly for aStrategy
@SYMTestExpectedResults The input file used for compression by EZLib should be the same as 
						the decompressed output file 
						generated by EZLib2.
@SYMDEF                 REQ7298
*/

TVerdict CBackwardsCompatibilityTests::TestDeflateOldInflateNewCompatibitlityL()
	{
	SetCompressDecompressParametersL();

	TFileName outFile(KTmpOutputFile);
	DoEZlibDeflateL(_L(""), outFile, iLevel, iWindowBits, iMemLevel, (TOLDEZLIB::CEZCompressor::TStrategy)iStrategy);
	TFileName tmpInputFileLocation = iInputFileLocation;
	TFileName tmpOutputFileLocation = iOutputFileLocation;
	
	DoEZlib2InflateL(outFile, _L(""), iWindowBits);
	
	DeleteFile(tmpOutputFileLocation);

	return (CompareFileDataL(tmpInputFileLocation, iOutputFileLocation)) ? EPass : EFail;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4302
@SYMTestCaseDesc     	Testing the CEZCompressor::DeflateL() with new EZLib2 & 
						CEZDecompressor::InflateL() with old EZLib & comparing the files 
						for backward compatibility test.
@SYMTestPriority     	High
@SYMTestActions      	Deflate using EZLib2:
							1.	Open the input file for reading and create the deflated 
								output file for writing. 
							2.	Create a CEZFileBufferManager object passing it the input 
								and output files. 
							3.	Create a CEZCompressor object passing it the 
								CEZFileBufferManager object and a combination of specified 
								compression parameters as mentioned below in the Note.
							4.	Call CEZCompressor::DeflateL() as many times as needed to 
								compress the data.
						Inflate using EZLib:
							5.	Open the deflated file for reading and create the output 
								file for writing. 
							6.	Create a TOLDEZLIB::CEZFileBufferManager object passing it 
								the input and output. 
							7.	Create a TOLDEZLIB::CEZDecompressor object passing it the 
								buffer CEZFileBufferManager object with input parameters 
								as aWindowBits. Note: aWindowBits value should be the same 
								as used while compressing the file in Step 3. 
							8.	Call TOLDEZLIB::CEZDecompressor::InflateL() as many times 
								as needed to decompress the data.
							Note: To use old ezlib.dll, TOLDEZLIB namespace has to be used.
							9.	Compare the inflated output file with the original input 
								file used in step 1.
							10.	Cleanup all objects.
							11.	Perform the above Steps 1 – 10 with different types of 
								input files & different sizes of input files & varying 
								different compression parameters.
						
						Note: 
						The test should be repeated for different types of input files as 
						well as different size of input file for e.g.:
							•	TXT
							•	IMG
							•	PNG
							•	JAR
							•	Varying sizes of each input file should be tested starting 
								from a couple of kilobytes up to a few hundred megabytes.
						And with compression parameters as:
							•	EDefaultCompression, ENoCompression, EBestSpeed, 
								EBestCompression for aLevel
							•	8 and EMaxWBits for aWindowBits
							•	1, 9 and EDefMemLevel for aMemLevel
							•	EDefaultStrategy, EFiltered and EHuffmanOnly for aStrategy
@SYMTestExpectedResults The input file used for compression by EZLib should be the same as 
						the decompressed output file 
						generated by EZLib2.
@SYMDEF                 REQ7298
*/

TVerdict CBackwardsCompatibilityTests::TestDeflateNewInflateOldCompatibitlityL()
	{
	SetCompressDecompressParametersL();

	TFileName outFile(KTmpOutputFile);
	DoEZlib2DeflateL(_L(""), outFile, iLevel, iWindowBits, iMemLevel, (CEZCompressor::TStrategy)iStrategy);
	TFileName tmpInputFileLocation = iInputFileLocation;
	TFileName tmpOutputFileLocation = iOutputFileLocation;
	
	DoEZlibInflateL(outFile, _L(""), iWindowBits);
	
	DeleteFile(tmpOutputFileLocation);

	return (CompareFileDataL(tmpInputFileLocation, iOutputFileLocation)) ? EPass : EFail;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4308
@SYMTestCaseDesc     	Testing the CEZFileToGZip::DeflateL() with old EZLib & 
						CEZGZipToFile::InflateL() with new EZLib2 & comparing the files 
						for backward compatibility test.
@SYMTestPriority     	High
@SYMTestActions      	GZIP Deflate using EZLib:
							1.	Open the input file for reading.
							2.	Create a TOLDEZLIB::CEZFileToGZip object passing it the 
								name of the GZip file you wish to create and the input file. 
							3.	Call TOLDEZLIB::CEZFileToGZip::DeflateL() as many times as 
								needed to compress the data. 
							Note: To use old ezlib.dll, TOLDEZLIB namespace has to be used.
						GZIP Inflate using EZLib2:
							4.	Create or open output decompressed file for writing.
							5.	Pass compressed filename & output filename opened in Step 
								4 to CEZGZipToFile::NewL().
							6.	Call CEZGZipToFile::InflateL() as many times as needed to 
								compress the data.
							7.	Compare the inflated output file with the original input 
								file used in step 1.
							8.	Cleanup all objects.
							9.	Perform the above steps 1 – 8 with different types of 
								input files & different sizes of input files

						Note: The test should be repeated for different types of input 
							  files as well as different size of input file for e.g.:
							•	TXT
							•	IMG
							•	PNG
							•	JAR
							•	Varying sizes of each input file should be tested starting 
								from a couple of kilobytes up to a few hundred megabytes.
@SYMTestExpectedResults The input file used for compression by EZLib should be the same as 
						the decompressed output file 
						generated by EZLib2.
@SYMDEF                 REQ7298
*/

TVerdict CBackwardsCompatibilityTests::TestGZipDeflateOldInflateNewCompatibitlityL()
	{
	TFileName outFile(KTmpGzipOutputFile);
	DoEZlibGZipDeflateL(_L(""), outFile);
	TFileName tmpInputFileLocation = iInputFileLocation;
	TFileName tmpOutputFileLocation = iOutputFileLocation;
	
	DoEZlib2GZipInflateL(outFile, _L(""));
	
	DeleteFile(tmpOutputFileLocation);
	
	return (CompareFileDataL(tmpInputFileLocation, iOutputFileLocation)) ? EPass : EFail;	
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4309
@SYMTestCaseDesc     	Testing the CEZFileToGZip::DeflateL() with new EZLib2 & 
						CEZGZipToFile::InflateL() with old EZLib & comparing the files for 
						backward compatibility test.
@SYMTestPriority     	High
@SYMTestActions      	GZIP Deflate using EZLib2:
							1.	Open the input file for reading.
							2.	Create a CEZFileToGZip object passing it the name of the 
								GZip file you wish to create and the input file. 
							3.	Call CEZFileToGZip::DeflateL() as many times as needed to 
								compress the data. 
						GZIP Inflate using EZLib:
							4.	Create or open output decompressed file for writing
							5.	Pass compressed filename & output filename opened in Step 
								4 to TOLDEZLIB::CEZGZipToFile::NewL().
							6.	Call TOLDEZLIB::CEZGZipToFile::InflateL() as many times as 
								needed to compress the data.
							Note: To use old ezlib.dll, TOLDEZLIB namespace has to be used.
							7.	Compare the inflated output file with the original input 
								file used in step 1.
							8.	Cleanup all objects.
							9.	Perform the above steps 1 – 8 with different types of 
								input files & different sizes of input files
					
						Note: The test should be repeated for different types of input 
							  files as well as different size of input file for e.g.:
							•	TXT
							•	IMG
							•	PNG
							•	JAR
							•	Varying sizes of each input file should be tested starting 
								from a couple of kilobytes up to a few hundred megabytes.
@SYMTestExpectedResults The input file used for compression by EZLib should be the same as 
						the decompressed output file 
						generated by EZLib2.
@SYMDEF                 REQ7298
*/

TVerdict CBackwardsCompatibilityTests::TestGZipDeflateNewInflateOldCompatibitlityL()
	{	
	TFileName outFile(KTmpGzipOutputFile);
	DoEZlib2GZipDeflateL(_L(""), outFile);
	TFileName tmpInputFileLocation = iInputFileLocation;
	TFileName tmpOutputFileLocation = iOutputFileLocation;
	
	DoEZlibGZipInflateL(outFile, _L(""));
		
	DeleteFile(tmpOutputFileLocation);
	
	return (CompareFileDataL(tmpInputFileLocation, iOutputFileLocation)) ? EPass : EFail;	
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4310
@SYMTestCaseDesc     	Testing the backward compatibility of decompressing a zip archive 
						using EZlib2 with EZlib.
@SYMTestPriority     	High
@SYMTestActions      	Decompression of ZIP Archives using EZLib:
							1.	Create an output buffer for holding the decompressed data 
								before it is written to a file.
							2.	Create a TOLDEZIP::CZipFile object passing it the file 
								name of the zip archive to be decompressed.
							3.	Create a TOLDEZIP::CZipFileMemberIterator and use it to 
								loop over each TOLDEZIP::CZipFileMember in the 
								TOLDEZIP::CZipFile doing the following on each iteration:
								a.	Create an output file for writing.
								b.	Create a TOLDEZIP::RZipFileMemberReaderStream object 
									using TOLDEZIP::CZipFile object to initialise it.
								c.	Do the following while the end of the input file has 
									not been reached:
									i.	Read in and decompress part of the file by calling 
										TOLDEZIP::RZipFileMemberReaderStream::Read() on 
										the TOLDEZIP::RZipFileMemberReaderStream, placing 
										the output in the output buffer.
									ii.	Write the output buffer to the output file.
							Note: To use old ezip.dll TOLDEZIP namespace has to be used.
							4.	Cleanup all objects.
						Decompression of ZIP Archives using EZLib2:
							5.	Create an output buffer for holding the decompressed data 
								before it is written to a file
							6.	Create a CZipFile object passing it the file name of the 
								zip archive to be decompressed.
							7.	Create a CZipFileMemberIterator and use it to loop over 
								each CZipFileMember in the CZipFile doing the following on 
								each iteration:
								a.	Create an output file for writing.
								b.	Create a RZipFileMemberReaderStream object using 
									CZipFile object to initialise it.
								c.	Do the following while the end of the input file has 
									not been reached:
								i.	Read in and decompress part of the file by calling 
									RZipFileMemberReaderStream::Read() on the 
									RZipFileMemberReaderStream, placing the output in the 
									output buffer.
								ii.	Write the output buffer to the output file.
							8.	Cleanup all objects.
							9.	Compare the decompressed file generated by EZlib & EZLib2.
							10.	Perform the above steps 1 – 7 with different zip archives 
								having couple of files as well as multiple files & even 
								with different sizes of zip archives.
						
						Note: 
							•	The test should be repeated for zip archives containing 
								only a couple of files and multiple files.
							•	Varying sizes of zip archive files should be tested 
								starting from a couple of kilobytes up to a few hundred 
								megabytes.
@SYMTestExpectedResults The decompressed output files generated by EZLib & EZLib2 should 
						be the same. 
@SYMDEF                 REQ7298
*/

TVerdict CBackwardsCompatibilityTests::TestZipArchiveDecompressCompatibitlityL()
	{
	return ZipArchiveDecompressCompatibitlityL();
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4316
@SYMTestCaseDesc     	To test that Zip Archives created on Linux and Unix Operating 
						Systems can be decompress using EZlib and EZlib2 and that the 
						decompressed files are identical.
@SYMTestPriority     	High
@SYMTestActions      	Decompression of ZIP Archives using EZLib:
							1.	Create an output buffer for holding the decompressed data 
								before it is written to a file.
							2.	Create a TOLDEZIP::CZipFile object passing it the file 
								name of the zip archive to be decompressed.
							3.	Create a TOLDEZIP::CZipFileMemberIterator and use it to 
								loop over each TOLDEZIP::CZipFileMember in the 
								TOLDEZIP::CZipFile doing the following on each iteration:
								a.	Create an output file for writing.
								b.	Create a TOLDEZIP::RZipFileMemberReaderStream object 
									using TOLDEZIP::CZipFile object to initialise it.
								c.	Do the following while the end of the input file has 
									not been reached:
									i.	Read in and decompress part of the file by calling 
										TOLDEZIP::RZipFileMemberReaderStream::Read() on 
										the TOLDEZIP::RZipFileMemberReaderStream, placing 
										the output in the output buffer.
									ii.	Write the output buffer to the output file.
							4.	Cleanup all objects.
						Decompression of ZIP Archives using EZLib2:
							5.	Create an output buffer for holding the decompressed data 
								before it is written to a file
							6.	Create a CZipFile object passing it the file name of the 
								zip archive to be decompressed.
							7.	Create a CZipFileMemberIterator and use it to loop over 
								each CZipFileMember in the CZipFile doing the following 
								on each iteration:
								d.	Create an output file for writing.
								e.	Create a RZipFileMemberReaderStream object using 
									CZipFile object to initialise it.
								f.	Do the following while the end of the input file has 
									not been reached:
									i.	Read in and decompress part of the file by calling 
										RZipFileMemberReaderStream::Read() on the 
										RZipFileMemberReaderStream, placing the output in 
										the output buffer.
									ii.	Write the output buffer to the output file.
							8.	Cleanup all objects.
							9.	Compare the decompressed file generated by EZlib & EZLib2.
							10.	Perform the above steps 1 – 7 with different zip archives 
								having couple of files as well as multiple files & even 
								with different sizes of zip archives.
						
						Note: 
							•	The test should be repeated for Zip Archives created on 
								Linux and Unix.
							•	Varying sizes of Zip Archive files should be tested.
@SYMTestExpectedResults EZlib and EZlib2 will be able to decompress the Zip Archives 
						without any errors and the files will be identical to one another.
@SYMDEF                 REQ8024
*/

TVerdict CBackwardsCompatibilityTests::TestOSCompatibilityL()
	{
	return ZipArchiveDecompressCompatibitlityL();
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4317
@SYMTestCaseDesc     	To test that GZip files created on Linux and Unix Operating 
						Systems can be decompress using EZlib and EZlib2 and that the 
						decompressed files are identical.
@SYMTestPriority     	High
@SYMTestActions      	Decompression of ZIP Archives using EZlib:
							1.	Open an input file for reading and create an output file 
								for writing. 
							2.	Create a TOLDEZLIB::CEZFileBufferManager object passing 
								it the input and output. 
							3.	Create a TOLDEZLIB::CEZDecompressor object passing it the 
								buffer CEZFileBufferManager object and a specified 
								decompression parameter.
							4.	Call InflateL() as many times as needed to compress the data. 
						Decompression of ZIP Archives using EZlib2:
							5.	Open an input file for reading and create an output file 
								for writing. 
							6.	Create a CEZFileBufferManager object passing it the input 
								and output. 
							7.	Create a CEZDecompressor object passing it the buffer 
								CEZFileBufferManager object and a specified decompression 
								parameter.
							8.	Call InflateL() as many times as needed to compress the data. 
							9.	Compare the decompressed files generated by EZlib & EZlib2.
							10.	Delete any output files.
						
						Note: 
							•	The test should be repeated for GZip files created on 
								Linux and Unix.
							•	Varying sizes of GZip file should be tested.
@SYMTestExpectedResults The decompressed output files generated by EZLib & EZLib2 should 
						be the same. 
@SYMDEF                 REQ8024
*/
TVerdict CBackwardsCompatibilityTests::TestOSGZipCompatibilityL()
	{
    TFileName ezlibOutputFile(_L("ezlib.out"));
	TFileName ezlib2OutputFile(_L("ezlib2.out"));
	TFileName ezlibOutputFilePath(KFilePath);
	TFileName ezlib2OutputFilePath(KFilePath);
	
	ezlibOutputFilePath.Append(ezlibOutputFile);
	ezlib2OutputFilePath.Append(ezlib2OutputFile);
		
	DoEZlibGZipInflateL(_L(""), _L("ezlib.out"));	
	DoEZlib2GZipInflateL(_L(""), _L("ezlib2.out"));
	
	TVerdict verdict = (CompareFileDataL(ezlibOutputFilePath, ezlib2OutputFilePath)) ? EPass : EFail;
	
	DeleteFile(ezlibOutputFilePath);
	DeleteFile(ezlib2OutputFilePath);
	
	return verdict;
	}
