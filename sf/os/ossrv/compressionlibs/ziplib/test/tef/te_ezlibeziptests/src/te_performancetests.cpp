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

#include "te_performancetests.h"

CPerformanceTests::CPerformanceTests(const TDesC &aStepName) : CEZlibEZipTests(aStepName)
	{
	}

TVerdict CPerformanceTests::doTestStepL()
	{
	if(TestStepName() == KTestDeflatePerformanceL)
		{
   		INFO_PRINTF1(KTestDeflatePerformanceL);
   		SetTestStepResult(TestDeflatePerformanceL());
   		}
	else if(TestStepName() == KTestInflatePerformanceL)
		{
   		INFO_PRINTF1(KTestInflatePerformanceL);
   		SetTestStepResult(TestInflatePerformanceL());
   		}
	else if(TestStepName() == KTestGZipDeflatePerformanceL)
		{
   		INFO_PRINTF1(KTestGZipDeflatePerformanceL);
   		SetTestStepResult(TestGZipDeflatePerformanceL());
   		}
	else if(TestStepName() == KTestGZipInflatePerformanceL)
		{
   		INFO_PRINTF1(KTestGZipInflatePerformanceL);
   		SetTestStepResult(TestGZipInflatePerformanceL());
   		}
	else if(TestStepName() == KTestZipArchiveDecompressPerformanceL)
		{
   		INFO_PRINTF1(KTestZipArchiveDecompressPerformanceL);
   		SetTestStepResult(TestZipArchiveDecompressPerformanceL());
   		}
		
   	return TestStepResult();
	}

TReal CPerformanceTests::FastCountToMilliseconds(const TUint32 aFastCount)
	{
    TInt freqInHz;
    HAL::Get(HAL::EFastCounterFrequency, freqInHz);
    TReal freqInkHz = freqInHz / 1000;
    return (TReal)aFastCount / freqInkHz;
    }

/*
 * Checks if the time it took to complete a task using the EZlib2 component
 * is within a specified time range. If it is true is returned otherwise 
 * false is returned.
 */
TBool CPerformanceTests::IsEZlib2InPerformanceRange(const TUint32 aEZlibTestTime, const TUint32 aEZlib2TestTime, const TInt aPercentPerformanceDiff)
	{
	TReal percent = (aPercentPerformanceDiff * 0.01) + 1;
	if(aEZlib2TestTime > (aEZlibTestTime * percent))
		{
		INFO_PRINTF1(KEZlib2Failed);
		return EFalse;
		}	
	
	return ETrue;
	}

void CPerformanceTests::PrintTestTimeInfo(const TUint32 aEZlibTestTime, const TUint32 aEZlib2TestTime)
	{
	TReal EZlibTestTime = FastCountToMilliseconds(aEZlibTestTime);
	TReal EZlib2TestTime = FastCountToMilliseconds(aEZlib2TestTime);
	TReal TestTimeDiff = EZlibTestTime - EZlib2TestTime;

	INFO_PRINTF2(KEZlibTimeMilliseconds, EZlibTestTime);
	INFO_PRINTF2(KEZlib2TimeMilliseconds, EZlib2TestTime);

	if(TestTimeDiff > 0)
		{
		INFO_PRINTF3(KEZlib2FasterMilliseconds, TestTimeDiff, ((TestTimeDiff / EZlibTestTime) * 100));
		}
	else if(TestTimeDiff < 0)
		{
		INFO_PRINTF3(KEZlib2SlowerMilliseconds, -TestTimeDiff, -((TestTimeDiff / EZlibTestTime) * 100));
		}
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4303
@SYMTestCaseDesc     	Testing and comparing the DeflateL() performance of EZlib and EZlib2.
@SYMTestPriority     	High
@SYMTestActions      	1.	Open an input file for reading and create an output file for writing. 
                        2.	Create a CEZFileBufferManager object passing it the input and 
                            output files. 
                        3.	Create a CEZCompressor object passing it the 
                            CEZFileBufferManager object and specified compression parameters.
                        4.	Get the fast count value, by calling FastCounter() in the User 
                            class, and store it in TUint32 called startTime. 
                        5.	Call DeflateL() as many times as needed to compress the data. 
                        6.	Get the fast count value, subtract startTime and store it in a 
                            TUint32 called totalTime. 
                        Perform the above steps using the EZLib and the EZLib2 components. 
                        Check that the time it took EZlib2 is no more than 5% slower.
                        
                        Note: 
                        The test should be repeated for input files of type:
                            •	PNG
                            •	JAR
                            •	TXT
                            •	DOC
                            
                        and with compression parameters:
                            •	EDefaultCompression, ENoCompression, EBestSpeed, 
                                EBestCompression for aLevel
                            •	8 and EMaxWBits for aWindowBits
                            •	1, 9 and EDefMemLevel for aMemLevel
                            •	EDefaultStrategy, EFiltered and EHuffmanOnly for aStrategy
                            •	Varying sizes of each input file should be tested starting 
                                from a couple of kilobytes up to a couple of megabytes.
@SYMTestExpectedResults The time it takes the EZLib2 member functions to complete the steps 
                        should be at worst 5% slower than the time it takes EZLib.
@SYMDEF                 REQ8025
*/

TVerdict CPerformanceTests::TestDeflatePerformanceL()
	{
	SetCompressDecompressParametersL();

	DoEZlibDeflateL(iLevel, iWindowBits, iMemLevel, (TOLDEZLIB::CEZCompressor::TStrategy)iStrategy);
	TUint32 EZlibTestTime = iTotalTime;	

	DoEZlib2DeflateL(iLevel, iWindowBits, iMemLevel, iStrategy);
	TUint32 EZlib2TestTime = iTotalTime;			
	
	PrintTestTimeInfo(EZlibTestTime, EZlib2TestTime);
	
	return (IsEZlib2InPerformanceRange(EZlibTestTime, EZlib2TestTime)) ? EPass : EFail;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4304
@SYMTestCaseDesc     	Testing and comparing the InflateL() performance of EZlib and EZlib2.
@SYMTestPriority     	High
@SYMTestActions      	1.	Open an input file for reading and create an output file for writing. 
                        2.	Create a CEZFileBufferManager object passing it the input and output. 
                        3.	Create a CEZDecompressor object passing it the buffer 
                            CEZFileBufferManager object and a specified decompression parameter.
                        4.	Get the fast count value, by calling FastCounter() in the User 
                            class, and store it in TUint32 called startTime. 
                        5.	Call InflateL() as many times as needed to compress the data. 
                        6.	Get the fast count value, subtract startTime and store it in a 
                            TUint32 called totalTime. 
                        Perform the above steps using the EZLib and the EZLib2 components. 
                        Check that the time it took EZlib2 is no more than 5% slower.
                        
                        Note: 
                        The test should be repeated with decompression parameters:
                            •	8 and EMaxWBits for aWindowBits
                            •	Varying sizes of input files should be tested starting from 
                                a couple of kilobytes up to a couple of megabytes.
@SYMTestExpectedResults The time it takes the EZLib2 member functions to complete the steps 
                        should be at worst 5% slower than the time it takes EZLib.
@SYMDEF                 REQ8025
*/

TVerdict CPerformanceTests::TestInflatePerformanceL()
	{
	TFileName intFile(KTmpOutputFile);
	
	SetCompressDecompressParametersL();

	// Create our compressed file
	DoEZlibDeflateL(_L(""), intFile, iLevel, iWindowBits, iMemLevel, (TOLDEZLIB::CEZCompressor::TStrategy)iStrategy);
	
	TFileName tmpOutputFileLocation = iOutputFileLocation;
	
	DoEZlibInflateL(intFile, _L(""), iWindowBits);
	TUint32 EZlibTestTime = iTotalTime;	

	DeleteFile(tmpOutputFileLocation);
	
	// Create our compressed file
	DoEZlib2DeflateL(_L(""), intFile, iLevel, iWindowBits, iMemLevel, iStrategy);
	
	tmpOutputFileLocation = iOutputFileLocation;
	
	DoEZlib2InflateL(intFile, _L(""), iWindowBits);
	TUint32 EZlib2TestTime = iTotalTime;	
	
	DeleteFile(tmpOutputFileLocation);
	
	PrintTestTimeInfo(EZlibTestTime, EZlib2TestTime);
	
	return (IsEZlib2InPerformanceRange(EZlibTestTime, EZlib2TestTime)) ? EPass : EFail;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4305
@SYMTestCaseDesc     	Testing and comparing the GZip DeflateL() performance of EZlib and EZlib2.
@SYMTestPriority     	High
@SYMTestActions      	1.	Open an input file for reading.
                        2.	Create a CEZFileToGZip object passing it the name of the GZip 
                            file you wish to create and the input file. 
                        3.	Get the fast count value, by calling FastCounter() in the User 
                            class, and store it in TUint32 called startTime. 
                        4.	Call DeflateL() as many times as needed to compress the data. 
                        5.	Get the fast count value, subtract startTime and store it in a 
                            TUint32 called totalTime. 
                        Perform the above steps using the EZLib and the EZLib2 components. 
                        Check that the time it took EZlib2 is no more than 5% slower.
                        
                        Note: 
                        The test should be repeated for input files of type:
                        •	PNG
                        •	JAR
                        •	TXT
                        •	DOC
                        •	JPG
                        Varying sizes of each input file should be tested starting from a 
                        couple of kilobytes up to a couple of megabytes.
@SYMTestExpectedResults The time it takes the EZLib2 member functions to complete the steps 
                        should be at worst 5% slower than the time it takes EZLib.
@SYMDEF                 REQ8025
*/
TVerdict CPerformanceTests::TestGZipDeflatePerformanceL()
	{	
	DoEZlibGZipDeflateL();
	TUint32 EZlibTestTime = iTotalTime;	

	DoEZlib2GZipDeflateL();
	TUint32 EZlib2TestTime = iTotalTime;	
	
	PrintTestTimeInfo(EZlibTestTime, EZlib2TestTime);
	
	return (IsEZlib2InPerformanceRange(EZlibTestTime, EZlib2TestTime)) ? EPass : EFail;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4306
@SYMTestCaseDesc     	Testing and comparing the GZip InflateL() performance of EZlib and EZlib2.
@SYMTestPriority     	High
@SYMTestActions      	1.	Open a GZip output file for writing.
                        2.	Create a CEZGZipToFile object passing it the name of the input 
                            file you wish to decompress and the output file. 
                        3.	Get the fast count value, by calling FastCounter() in the User 
                            class, and store it in TUint32 called startTime. 
                        4.	Call InflateL() as many times as needed to decompress the data. 
                        5.	Get the fast count value, subtract startTime and store it in a 
                            TUint32 called totalTime. 
                        Perform the above steps using the EZLib and the EZLib2 components. 
                        Check that the time it took EZlib2 is no more than 5% slower.
                        
                        Note: Varying sizes of input files should be tested starting from 
                              a couple of kilobytes up to a couple of megabytes.
@SYMTestExpectedResults The time it takes the EZLib2 member functions to complete the steps 
                        should be at worst 5% slower than the time it takes EZLib.
@SYMDEF                 REQ8025
*/
TVerdict CPerformanceTests::TestGZipInflatePerformanceL()
	{
	TFileName intFile(KTmpGzipOutputFile);
	
	// Create the compressed file
	DoEZlibGZipDeflateL(_L(""), intFile);
	
	TFileName tmpOutputFileLocation = iOutputFileLocation;

	DoEZlibGZipInflateL(intFile, _L(""));	
	TUint32 EZlibTestTime = iTotalTime;	
	
	DeleteFile(tmpOutputFileLocation);
	
	// Create the compressed file
	DoEZlib2GZipDeflateL(_L(""), intFile);
	
	tmpOutputFileLocation = iOutputFileLocation;
	
	DoEZlib2GZipInflateL(intFile, _L(""));	
	TUint32 EZlib2TestTime = iTotalTime;	

	DeleteFile(tmpOutputFileLocation);
	
	PrintTestTimeInfo(EZlibTestTime, EZlib2TestTime);
	
	return (IsEZlib2InPerformanceRange(EZlibTestTime, EZlib2TestTime)) ? EPass : EFail;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4307
@SYMTestCaseDesc     	Testing and comparing the performance of EZlib and EZlib2 
                        decompressing a zip archive.
@SYMTestPriority     	High
@SYMTestActions      	1.	Create an output buffer for holding the decompressed data 
                            before it is written to a file.
                        2.	Create a CZipFile object passing it the file name of the zip 
                            archive to be decompressed.
                        3.	Create a CZipFileMemberIterator and use it to loop over each 
                            CZipFileMember in the CZipFile doing the following on each iteration:
                            a.	Create an output file for writing.
                            b.	Create a RZipFileMemberReaderStream object using CZipFile 
                                object to initialise it. 
                            c.	Do the following while the end of the input file has not 
                                been reached:
                                i.	Get the fast count value, by calling FastCounter() in 
                                    the User class, and store it in TUint32 called startTime. 
                                ii.	Read in and decompress part of the file by calling Read() 
                                    on the RZipFileMemberReaderStream, placing the output 
                                    in the output buffer.
                                iii. Get the fast count value, subtract the startTime 
                                     and add the result to totalTime.
                                iv.	Write the output buffer to the output file.
                        Perform the above steps using the EZLib and the EZLib2 components. 
                        Check that the time it took EZlib2 is no more than 5% slower.
                        
                        Note: The test should be repeated for zip archives containing 
                              multiple files and folders within folders.
                              
                              Varying sizes of zip archive files should be tested starting 
                              from a couple of kilobytes up to a few megabytes.
@SYMTestExpectedResults The time it takes the EZLib2 member functions to complete the steps 
                        should be at worst 5% slower than the time it takes EZLib.
@SYMDEF                 REQ8025
*/
TVerdict CPerformanceTests::TestZipArchiveDecompressPerformanceL()
	{	
	
	DoEZlibZipArchiveDecompressL();
	TUint32 EZlibTestTime = iTotalTime;	
	
	DoEZlib2ZipArchiveDecompressL();
	TUint32 EZlib2TestTime = iTotalTime;	
	
	PrintTestTimeInfo(EZlibTestTime, EZlib2TestTime);
	
	return (IsEZlib2InPerformanceRange(EZlibTestTime, EZlib2TestTime)) ? EPass : EFail;
	}
