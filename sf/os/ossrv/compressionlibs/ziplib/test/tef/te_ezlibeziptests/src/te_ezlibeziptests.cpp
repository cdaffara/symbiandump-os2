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

#include "te_ezlibeziptests.h"

CEZlibEZipTests::~CEZlibEZipTests()
	{
	//iInputFile.Close();
	//iOutputFile.Close();
	iRfs.Close();
	}

CEZlibEZipTests::CEZlibEZipTests(const TDesC &aStepName) : iLevel(0), iWindowBits(0), iMemLevel(0), iStrategy((CEZCompressor::TStrategy)0)
	{	
	iRfs.Connect();
	SetTestStepName(aStepName);
	}

void CEZlibEZipTests::DeleteFile(TFileName& aFileName)
	{
	TInt error = iRfs.Delete(aFileName);
	if(error != KErrNone)
		INFO_PRINTF2(_L("File not deleted: %S"), &aFileName);
	}

/*
 * Compresses the file into a zlib file using the EZlib component. 
 * It is assumed that the input and output file names are contained in a 
 * provided ini file.
 */
void CEZlibEZipTests::DoEZlibDeflateL(const TInt aLevel, const TInt aWindowBits, const TInt aMemLevel, TOLDEZLIB::CEZCompressor::TStrategy aStrategy)
	{
	DoEZlibDeflateL(_L(""), _L(""), aLevel, aWindowBits, aMemLevel, aStrategy);
	}

/*
 * Compresses the file into a zlib file using the EZlib component. 
 * If input and output file names are not specified then it is assumed that 
 * the file names are contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlibDeflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName, const TInt aLevel, const TInt aWindowBits, const TInt aMemLevel, const TOLDEZLIB::CEZCompressor::TStrategy aStrategy)
	{
	iTotalTime = 0;
	OpenInputFileLC(aInputFileName);
	OpenOutputFileLC(aOutputFileName);

	TOLDEZLIB::CEZFileBufferManager *bufferManager = TOLDEZLIB::CEZFileBufferManager::NewLC(iInputFile, iOutputFile);	
	TOLDEZLIB::CEZCompressor *compressor = TOLDEZLIB::CEZCompressor::NewLC(*bufferManager, aLevel, aWindowBits, aMemLevel, aStrategy);

	iStartTime = User::FastCounter();
	while(compressor->DeflateL())
		{		
		}	
	iTotalTime = User::FastCounter() - iStartTime;
	
	CleanupStack::PopAndDestroy(4);
	}

/*
 * Compresses the file into a zlib file using the EZlib2 component. 
 * It is assumed that the input and output file names are contained in a 
 * provided ini file.
 */
void CEZlibEZipTests::DoEZlib2DeflateL(const TInt aLevel, const TInt aWindowBits, const TInt aMemLevel, const CEZCompressor::TStrategy aStrategy)
	{
	DoEZlib2DeflateL(_L(""), _L(""), aLevel, aWindowBits, aMemLevel, aStrategy);
	}

/*
 * Compresses the file into a zlib file using the EZlib2 component. 
 * If input and output file names are not specified then it is assumed that 
 * the file names are contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlib2DeflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName, const TInt aLevel, const TInt aWindowBits, const TInt aMemLevel, const CEZCompressor::TStrategy aStrategy)
	{
	iTotalTime = 0;
	OpenInputFileLC(aInputFileName);
	OpenOutputFileLC(aOutputFileName);

	CEZFileBufferManager *bufferManager = CEZFileBufferManager::NewLC(iInputFile, iOutputFile);	
	CEZCompressor *compressor = CEZCompressor::NewLC(*bufferManager, aLevel, aWindowBits, aMemLevel, aStrategy);

	iStartTime = User::FastCounter();
	while(compressor->DeflateL())
		{		
		}	
	iTotalTime = User::FastCounter() - iStartTime;

	CleanupStack::PopAndDestroy(4);
	}

/*
 * Decompresses the file contained in a zlib file using the EZlib component. 
 * It is assumed that the input and output file names are contained in a 
 * provided ini file.
 */
void CEZlibEZipTests::DoEZlibInflateL(const TInt aWindowBits)
	{
	DoEZlibInflateL(_L(""), _L(""), aWindowBits);
	}

/*
 * Decompresses the file contained in a zlib file using the EZlib component. 
 * If input and output file names are not specified then it is assumed that 
 * the file names are contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlibInflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName, const TInt aWindowBits)
	{
	iTotalTime = 0;
	OpenInputFileLC(aInputFileName);
	OpenOutputFileLC(aOutputFileName);

	TOLDEZLIB::CEZFileBufferManager *bufferManager = TOLDEZLIB::CEZFileBufferManager::NewLC(iInputFile, iOutputFile);	
	TOLDEZLIB::CEZDecompressor *decompressor = TOLDEZLIB::CEZDecompressor::NewLC(*bufferManager, aWindowBits);

	iStartTime = User::FastCounter();
	while(decompressor->InflateL())
		{		
		}	
	iTotalTime = User::FastCounter() - iStartTime;
	
	CleanupStack::PopAndDestroy(4);
	}

/*
 * Decompresses the file contained in a zlib file using the EZlib2 component. 
 * It is assumed that the input and output file names are contained in a 
 * provided ini file.
 */
void CEZlibEZipTests::DoEZlib2InflateL(const TInt aWindowBits)
	{
	DoEZlib2InflateL(_L(""), _L(""), aWindowBits);
	}

/*
 * Decompresses the file contained in a zlib file using the EZlib2 component. 
 * If input and output file names are not specified then it is assumed that 
 * the file names are contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlib2InflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName, const TInt aWindowBits)
	{
	iTotalTime = 0;
	OpenInputFileLC(aInputFileName);
	OpenOutputFileLC(aOutputFileName);

	CEZFileBufferManager *bufferManager = CEZFileBufferManager::NewLC(iInputFile, iOutputFile);	
	CEZDecompressor *decompressor = CEZDecompressor::NewLC(*bufferManager, aWindowBits);

	iStartTime = User::FastCounter();
	while(decompressor->InflateL())
		{		
		}	
	iTotalTime = User::FastCounter() - iStartTime;
		
	CleanupStack::PopAndDestroy(4);
	}

/*
 * Compresses a file to a GZip file using the EZlib component. If 
 * input and output file names are not specified then it is assumed that 
 * the file names are contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlibGZipDeflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName)
	{
	iTotalTime = 0;
	OpenInputFileLC(aInputFileName);
	SetOutputFileLocationL(aOutputFileName);
	
	TOLDEZLIB::CEZFileToGZip *compressor = TOLDEZLIB::CEZFileToGZip::NewLC(iRfs, iOutputFileLocation, iInputFile);

	iStartTime = User::FastCounter();
	while(compressor->DeflateL())
		{		
		}	
	iTotalTime = User::FastCounter() - iStartTime;
		
	CleanupStack::PopAndDestroy(2);
	}

/*
 * Compresses a file to a GZip file using the EZlib2 component. If 
 * input and output file names are not specified then it is assumed that 
 * the file names are contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlib2GZipDeflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName)
	{
	iTotalTime = 0;
	OpenInputFileLC(aInputFileName);
	SetOutputFileLocationL(aOutputFileName);

	CEZFileToGZip *compressor = CEZFileToGZip::NewLC(iRfs, iOutputFileLocation, iInputFile);

	iStartTime = User::FastCounter();
	while(compressor->DeflateL())
		{		
		}	
	iTotalTime = User::FastCounter() - iStartTime;
		
	CleanupStack::PopAndDestroy(2);
	}

/*
 * Decompresses the file contained in a GZip file using the EZlib component. If 
 * input and output file names are not specified then it is assumed that 
 * the file names are contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlibGZipInflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName)
	{
	iTotalTime = 0;
	SetInputFileLocationL(aInputFileName);
	OpenOutputFileLC(aOutputFileName);

	TOLDEZLIB::CEZGZipToFile *decompressor = TOLDEZLIB::CEZGZipToFile::NewLC(iRfs, iInputFileLocation, iOutputFile);

	iStartTime = User::FastCounter();
	while(decompressor->InflateL())
		{		
		}	
	iTotalTime = User::FastCounter() - iStartTime;
		
	CleanupStack::PopAndDestroy(2);
	}

/*
 * Decompresses the file contained in a GZip file using the EZlib2 component. If 
 * input and output file names are not specified then it is assumed that 
 * the file names are contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlib2GZipInflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName)
	{
	iTotalTime = 0;
	SetInputFileLocationL(aInputFileName);
	OpenOutputFileLC(aOutputFileName);

	CEZGZipToFile *decompressor = CEZGZipToFile::NewLC(iRfs, iInputFileLocation, iOutputFile);

	iStartTime = User::FastCounter();
	while(decompressor->InflateL())
		{		
		}	
	iTotalTime = User::FastCounter() - iStartTime;	
	
	CleanupStack::PopAndDestroy(2);
	}

/*
 * Decompresses all the files in a Zip Archive using the EZlib 
 * component. If an input filename is not specified it is assumed that the 
 * input file name is contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlibZipArchiveDecompressL(const TFileName &aInputFileName)
	{
	iTotalTime = 0;
	TInt err = KErrNone;
	HBufC8 *outputBuffer = HBufC8::NewLC(outputBufferSize);	
	
	OpenInputFileLC(aInputFileName);
	
	TOLDEZIP::CZipFile *zipFile = TOLDEZIP::CZipFile::NewL(iRfs, iInputFile);
	CleanupStack::PushL(zipFile);	
	TOLDEZIP::CZipFileMemberIterator *fileMemberIter = zipFile->GetMembersL();

	iZipArchiveEzlibFolder = GenerateZipArchiveFolderNameL(aInputFileName);
	iZipArchiveEzlibFolder.Append(KZipArchiveEzlib);
	
	for(TOLDEZIP::CZipFileMember *fileMember = fileMemberIter->NextL(); fileMember != NULL; fileMember = fileMemberIter->NextL())
		{	
		TFileName tmpfilename(iZipArchiveEzlibFolder);
		tmpfilename.Append(KBackslash);
		tmpfilename.Append(*(fileMember->Name()));

		// Check if we have a folder or file
		TPtrC lastChar = tmpfilename.Right(1);
		if(lastChar.Compare(_L("\\")) != 0 && lastChar.Compare(_L("/")) != 0)
			{			
			OpenOutputFileLC(tmpfilename);
			
			TOLDEZIP::RZipFileMemberReaderStream *readerStream;
			zipFile->GetInputStreamL(fileMember, readerStream);
			CleanupStack::PushL(readerStream);
					
			TPtr8 pOutputBuffer = outputBuffer->Des();			
			do
				{
				iStartTime = User::FastCounter();
				err = readerStream->Read(pOutputBuffer, outputBufferSize);
				iTotalTime += User::FastCounter() - iStartTime;
				
				iOutputFile.Write(pOutputBuffer);
				} while(err == KErrNone);

			if(err != KErrEof)
				{
				User::Leave(err);
				}
			
			CleanupStack::PopAndDestroy(2);
			}	
		else
			{
			TFileName fullPath(KFilePath);
			fullPath.Append(tmpfilename);
			iRfs.MkDir(fullPath);
			}
		}

	CleanupStack::PopAndDestroy(3);
	}

/*
 * Decompresses all the files in a Zip Archive using the EZlib2 
 * component. If an input filename is not specified it is assumed that the 
 * input file name is contained in a provided ini file.
 */
void CEZlibEZipTests::DoEZlib2ZipArchiveDecompressL(const TFileName &aInputFileName)
	{
	iTotalTime = 0;
	TInt err = KErrNone;
	HBufC8 *outputBuffer = HBufC8::NewLC(outputBufferSize);	
	
	OpenInputFileLC(aInputFileName);
	
	CZipFile *zipFile = CZipFile::NewL(iRfs, iInputFile);
	CleanupStack::PushL(zipFile);	
	CZipFileMemberIterator *fileMemberIter = zipFile->GetMembersL();

	iZipArchiveEzlib2Folder = GenerateZipArchiveFolderNameL(aInputFileName);
	iZipArchiveEzlib2Folder.Append(KZipArchiveEzlib2);

	for(CZipFileMember *fileMember = fileMemberIter->NextL(); fileMember != NULL; fileMember = fileMemberIter->NextL())
		{
		TFileName tmpfilename(iZipArchiveEzlib2Folder);
		tmpfilename.Append(KBackslash);
		tmpfilename.Append(*(fileMember->Name()));
		
		// Check if we have a folder or file
		TPtrC lastChar = tmpfilename.Right(1);
		if(lastChar.Compare(_L("\\")) != 0 && lastChar.Compare(_L("/")) != 0)
			{
			OpenOutputFileLC(tmpfilename);
			
			RZipFileMemberReaderStream *readerStream;
			zipFile->GetInputStreamL(fileMember, readerStream);
			CleanupStack::PushL(readerStream);
					
			TPtr8 pOutputBuffer = outputBuffer->Des();		
			do
				{
				iStartTime = User::FastCounter();
				err = readerStream->Read(pOutputBuffer, outputBufferSize);
				iTotalTime += User::FastCounter() - iStartTime;
	
				iOutputFile.Write(pOutputBuffer);
				} while(err == KErrNone);
	
			if(err != KErrEof)
				{
				User::Leave(err);
				}
			
			CleanupStack::PopAndDestroy(2);
			}
		else
			{
			TFileName fullPath(KFilePath);
			fullPath.Append(tmpfilename);
			iRfs.MkDir(fullPath);
			}
		}

	CleanupStack::PopAndDestroy(3);
	}

/*
 * Opens an input file for reading. If not input name is specified then it 
 * is assumed the file name is to be obtained from a provided ini file.
 */
void CEZlibEZipTests::OpenInputFileLC(const TFileName &aInputFileName)
	{
	TInt err = KErrNone;	

	SetInputFileLocationL(aInputFileName);

	err = iInputFile.Open(iRfs, iInputFileLocation, EFileStream | EFileRead | EFileShareExclusive);
	if(err != KErrNone)
		{
		INFO_PRINTF1(KOpenFileError);
		User::Leave(err);
		}	
	
	CleanupClosePushL(iInputFile);
	}

/*
 * Creates a new output file and opens it for writing. If not output name
 * is specified then it is assumed the file name is to be obtained from
 * a provided ini file.
 */
void CEZlibEZipTests::OpenOutputFileLC(const TFileName &aOutputFileName)
	{
	TInt err = KErrNone;	
	
	SetOutputFileLocationL(aOutputFileName);	
	
	err = iRfs.MkDirAll(iOutputFileLocation);  
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		INFO_PRINTF1(KCreateDirError);
		User::Leave(err);
		}
	
	err = iOutputFile.Replace(iRfs, iOutputFileLocation, EFileStream | EFileWrite | EFileShareExclusive);
	if(err != KErrNone)
		{
		INFO_PRINTF1(KCreateFileError);
		User::Leave(err);
		}	
	
	CleanupClosePushL(iOutputFile);
	}

/*
 * Sets the compression/decompression parameters using the values in the provided ini file.
 */
void CEZlibEZipTests::SetCompressDecompressParametersL()
	{
	TBool ret = ETrue;
	
	ret = GetIntFromConfig(ConfigSection(), KCompressionLevel, iLevel);
	if(!ret)
		{
		INFO_PRINTF1(KCompressionLevelKeyError);
		User::Leave(KErrUnknown);
		}
	
	ret = GetIntFromConfig(ConfigSection(), KWindowBits, iWindowBits);
	if(!ret)
		{
		INFO_PRINTF1(KWindowBitsKeyError);
		User::Leave(KErrUnknown);
		}
	
	ret = GetIntFromConfig(ConfigSection(), KMemoryLevel, iMemLevel);
	if(!ret)
		{
		INFO_PRINTF1(KMemoryLevelKeyError);
		User::Leave(KErrUnknown);
		}
	
	TInt strategy;
	ret = GetIntFromConfig(ConfigSection(), KStrategy, strategy);
	if(!ret)
		{
		INFO_PRINTF1(KStrategyKeyError);
		User::Leave(KErrUnknown);
		}
	
	switch(strategy)
		{
		case 1: iStrategy = CEZCompressor::EDefaultStrategy;
				break;
		case 2: iStrategy = CEZCompressor::EFiltered;
				break;
		case 3: iStrategy = CEZCompressor::EHuffmanOnly;
				break;
		default: INFO_PRINTF1(_L("Invalid strategy in ini file."));
				 User::Leave(KErrUnknown);
		}	
	}

/*
 * Sets the input file name and path. If no filename is specified then it is assumed
 * that the name is contained within a provided ini file.
 */
void CEZlibEZipTests::SetInputFileLocationL(const TFileName &aInputFileName)
	{
	TBool ret = ETrue;
	TPtrC16 inputFileName = aInputFileName;	
	iInputFileLocation = KFilePath;

	if(aInputFileName == _L(""))
		{		
		ret = GetStringFromConfig(ConfigSection(), KInputFile, inputFileName);
		if(!ret)
			{
			INFO_PRINTF1(KInputFilePathKeyError);
			User::Leave(KErrUnknown);
			}		
		}
	iInputFileLocation.Append(inputFileName);
	}

/*
 * Sets the output file name and path. If no filename is specified then it is assumed
 * that the name is contained within the ini file.
 */
void CEZlibEZipTests::SetOutputFileLocationL(const TFileName &aOutputFileName)
	{
	TBool ret = ETrue;
	TPtrC16 outputFileName = aOutputFileName;
	iOutputFileLocation = KFilePath;
	
	if(aOutputFileName == _L(""))
		{		
		ret = GetStringFromConfig(ConfigSection(), KOutputFile, outputFileName);
		if(!ret)
			{
			INFO_PRINTF1(KOutputFilePathKeyError);
			User::Leave(KErrUnknown);
			}		
		}
	iOutputFileLocation.Append(outputFileName);	
	}

TFileName CEZlibEZipTests::GenerateZipArchiveFolderNameL(const TFileName &aInputFileName)
	{
	TPtrC16 inputFileName;
	TBool ret = ETrue;
	
	if(aInputFileName == _L(""))
		{		
		ret = GetStringFromConfig(ConfigSection(), KInputFile, inputFileName);
		if(!ret)
			{
			INFO_PRINTF1(KInputFilePathKeyError);
			User::Leave(KErrUnknown);
			}	
		}
	else
		inputFileName.Set(aInputFileName);

	TInt pos = inputFileName.Locate('.');
	TFileName folderName = inputFileName.Left(pos);
	
	return folderName;
	}

TVerdict CEZlibEZipTests::doTestStepPreambleL()
	{	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CEZlibEZipTests::doTestStepPostambleL()
	{	
	return TestStepResult();
	}

