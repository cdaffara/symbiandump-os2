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

#include "eustd.h"
#include <ezcompressor.h>
#include <ezdecompressor.h>
#include <ezlib.h>
#include <ezfilebuffer.h>

#include <f32file.h>

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0819
@SYMTestCaseDesc	    Compression and decompression of a file test
@SYMTestPriority 	    High
@SYMTestActions  	    Read the input file given as the parameter from the command line.
                        Compress and decompress the read file.
						Leave on error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

LOCAL_C void doExampleL()
	{
	RFs rfs;
	rfs.Connect();

    TInt cmdLineLen = User::CommandLineLength();
    
	if (cmdLineLen <= 0)
		{
		_LIT(KUsage,"Usage:ezfile filename\n");
		console->Printf(KUsage);
		User::Leave(1);
		}
    //(cmdLineLen > 0) case
	HBufC *argv = HBufC::NewLC(cmdLineLen);
	TPtr16 argPtr=argv->Des();
    User::CommandLine(argPtr);

	TLex arguments(*argv);
	TPtrC inputFile(arguments.NextToken());

	HBufC *outputFile = HBufC::NewLC(inputFile.Length()+2);
	_LIT(KOfl,"%S.z");
	outputFile->Des().Format(KOfl,&inputFile);

	HBufC *uncompressedFile = HBufC::NewLC(inputFile.Length()+1);
	_LIT(KUfl,"%S1");
	uncompressedFile->Des().Format(KUfl,&inputFile);

	RFile input;
	RFile output;
	TInt err;

	_LIT(KInfo,"Compressing file %S\n");
	console->Printf(KInfo,&inputFile);

	User::LeaveIfError(input.Open(rfs, inputFile,EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(input);
	err = output.Create(rfs, *outputFile,EFileStream | EFileWrite | EFileShareExclusive);
	if (err == KErrAlreadyExists)
		User::LeaveIfError(output.Open(rfs, *outputFile,EFileStream | EFileWrite | EFileShareExclusive));
	else 
		User::LeaveIfError(err);
	CleanupClosePushL(output);

	CEZFileBufferManager *fb = CEZFileBufferManager::NewLC(input,output,16384);
	CEZCompressor *def = CEZCompressor::NewLC(*fb);

	while (def->DeflateL()){/*do nothing*/}

	_LIT(KHoorah,"Hoorah");
	console->Printf(KHoorah);

	CleanupStack::PopAndDestroy(4);

	User::LeaveIfError(input.Open(rfs, *outputFile,EFileStream | EFileRead | EFileShareAny));
	CleanupClosePushL(input);
	err = output.Create(rfs, *uncompressedFile,EFileStream | EFileWrite | EFileShareExclusive);
	if (err == KErrAlreadyExists)
		User::LeaveIfError(output.Open(rfs, *uncompressedFile,EFileStream | EFileWrite | EFileShareExclusive));
	else 
		User::LeaveIfError(err);
	CleanupClosePushL(output);
	fb = CEZFileBufferManager::NewLC(input,output,16384);
	CEZDecompressor *inf = CEZDecompressor::NewLC(*fb);

	while (inf->InflateL()){/*do nothing*/}

	console->Printf(KHoorah);

	CleanupStack::PopAndDestroy(7);
	rfs.Close();
	}
