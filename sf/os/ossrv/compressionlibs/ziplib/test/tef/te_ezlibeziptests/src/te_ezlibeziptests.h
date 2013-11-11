/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* file te_ezlibeziptests.h
* 
*
*/



#ifndef __TE_EZLIBEZIPTESTS_H__
#define __TE_EZLIBEZIPTESTS_H__

#include <test/testexecutestepbase.h>
#include <f32file.h>
#include <ezcompressor.h>
#include <ezdecompressor.h>
#include <ezfilebuffer.h>
#include <ezgzip.h>
#include <zipfilememberiterator.h>
#include <zipfile.h>
#include "OldEZCompressor.h"
#include "OldEZDecompressor.h"
#include "OldEZFilebuffer.h"
#include "OldEZGzip.h"
#include "oldzipfile.h"

/* File Locations */ 
_LIT(KFilePath, "c:\\test\\ezlibeziptest_data\\");

/* Ini File Keys */
_LIT(KInputFile, "InputFile");
_LIT(KOutputFile, "OutputFile");
_LIT(KCompressionLevel, "CompressionLevel");
_LIT(KWindowBits, "WindowBits");
_LIT(KMemoryLevel, "MemoryLevel");
_LIT(KStrategy, "Strategy");

/* Information Messages */
_LIT(KOpenFileError, "Error opening the input file.");
_LIT(KCreateDirError, "Error creating the output directory.");
_LIT(KCreateFileError, "Error creating the output file.");
_LIT(KInputFilePathKeyError, "Error reading input file path from ini.");
_LIT(KOutputFilePathKeyError, "Error reading output file path from ini.");
_LIT(KCompressionLevelKeyError, "Error reading compression level from ini.");
_LIT(KWindowBitsKeyError, "Error reading window bits from ini.");
_LIT(KMemoryLevelKeyError, "Error reading memory level from ini.");
_LIT(KStrategyKeyError, "Error reading compression strategy from ini.");

/*Zip Archive Folder Name*/
_LIT(KZipArchiveEzlib, "_ezlib");
_LIT(KZipArchiveEzlib2, "_ezlib2");
_LIT(KBackslash, "\\");

/* Temporary Output Filename */
_LIT(KTmpOutputFile, "tmpout.zip");
_LIT(KTmpGzipOutputFile, "tmpgzipout.gz");

class CEZlibEZipTests : public CTestStep
	{	
public:
	virtual ~CEZlibEZipTests();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
protected:
	static const TInt outputBufferSize = 1024;
protected:
	CEZlibEZipTests();
	CEZlibEZipTests(const TDesC& aStepName);
	void DeleteFile(TFileName& aFileName);
	
	void DoEZlibDeflateL(const TInt aLevel = TOLDEZLIB::CEZCompressor::EDefaultCompression, const TInt aWindowBits = TOLDEZLIB::CEZCompressor::EMaxWBits, const TInt aMemLevel = TOLDEZLIB::CEZCompressor::EDefMemLevel, const TOLDEZLIB::CEZCompressor::TStrategy aStrategy = TOLDEZLIB::CEZCompressor::EDefaultStrategy);
	void DoEZlibDeflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName = _L(""), const TInt aLevel = TOLDEZLIB::CEZCompressor::EDefaultCompression, const TInt aWindowBits = TOLDEZLIB::CEZCompressor::EMaxWBits, const TInt aMemLevel = TOLDEZLIB::CEZCompressor::EDefMemLevel, const TOLDEZLIB::CEZCompressor::TStrategy aStrategy = TOLDEZLIB::CEZCompressor::EDefaultStrategy);
	
	void DoEZlib2DeflateL(const TInt aLevel = CEZCompressor::EDefaultCompression, const TInt aWindowBits = CEZCompressor::EMaxWBits, const TInt aMemLevel = CEZCompressor::EDefMemLevel, const CEZCompressor::TStrategy aStrategy = CEZCompressor::EDefaultStrategy);
	void DoEZlib2DeflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName = _L(""), const TInt aLevel = CEZCompressor::EDefaultCompression, const TInt aWindowBits = CEZCompressor::EMaxWBits, const TInt aMemLevel = CEZCompressor::EDefMemLevel, const CEZCompressor::TStrategy aStrategy = CEZCompressor::EDefaultStrategy);
	
	void DoEZlibInflateL(const TInt aWindowBits = TOLDEZLIB::CEZDecompressor::EMaxWBits);
	void DoEZlibInflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName = _L(""), TInt aWindowBits = TOLDEZLIB::CEZDecompressor::EMaxWBits);
		
	void DoEZlib2InflateL(const TInt aWindowBits = CEZDecompressor::EMaxWBits);
	void DoEZlib2InflateL(const TFileName &aInputFileName, const TFileName &aOutputFileName = _L(""), const TInt aWindowBits = CEZDecompressor::EMaxWBits);
		
	void DoEZlibGZipDeflateL(const TFileName &aInputFileName = _L(""), const TFileName &aOutputFileName = _L(""));
	void DoEZlib2GZipDeflateL(const TFileName &aInputFileName = _L(""), const TFileName &aOutputFileName = _L(""));
	
	void DoEZlibGZipInflateL(const TFileName &aInputFileName = _L(""), const TFileName &aOutputFileName = _L(""));
	void DoEZlib2GZipInflateL(const TFileName &aInputFileName = _L(""), const TFileName &aOutputFileName = _L(""));
	
	void DoEZlibZipArchiveDecompressL(const TFileName &aInputFileName = _L(""));
	void DoEZlib2ZipArchiveDecompressL(const TFileName &aInputFileName = _L(""));
	
	void OpenInputFileLC(const TFileName &aInputFileName = _L(""));
	void OpenOutputFileLC(const TFileName &aOutputFileName = _L(""));
	
	void SetCompressDecompressParametersL();
	void SetInputFileLocationL(const TFileName &aInputFileName = _L(""));
	void SetOutputFileLocationL(const TFileName &aOutputFileName = _L(""));
	
	TFileName GenerateZipArchiveFolderNameL(const TFileName &aInputFileName);

protected:
	TUint32 iStartTime;
	TUint32 iTotalTime;
	RFs iRfs;
	RFile iInputFile;
	RFile iOutputFile;
	TFileName iInputFileLocation;
	TFileName iOutputFileLocation;
	TFileName iZipArchiveEzlibFolder;
	TFileName iZipArchiveEzlib2Folder;
	TInt iLevel;
	TInt iWindowBits;
	TInt iMemLevel;
	CEZCompressor::TStrategy iStrategy;
	};

#endif /*__TE_EZLIBEZIPTESTS_H__*/
