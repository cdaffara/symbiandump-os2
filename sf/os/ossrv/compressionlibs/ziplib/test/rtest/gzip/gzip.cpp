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
#include <ezgzip.h>

#include <f32file.h>

void ReadAndPrintHeaderL(RFs &rfs, const TDesC &fname);

/**
@SYMTestCaseID          SYSLIB-EZLIB-CT-0829
@SYMTestCaseDesc	    Gzip functionality test
@SYMTestPriority 	    High
@SYMTestActions  	    Decompress and compress a zip file read from the command line. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

LOCAL_C void doExampleL()
	{
	RFs rfs;
	rfs.Connect();
	TBool compress = ETrue;
	TInt bufferSize = 0x8000;
	TBool readHeader = EFalse;
	TBool readTrailer = EFalse;
	TBool noWork = EFalse;

    TInt cmdLineLen = User::CommandLineLength();

	if (cmdLineLen <= 0)
		{
		_LIT(KUsage,"Usage:gzip [-dht] [-u bufferSize] filename\n");
		console->Printf(KUsage);
		User::Leave(1);
		}
    //(cmdLineLen > 0) case
	HBufC *argv = HBufC::NewLC(cmdLineLen);
	TPtr cmd(argv->Des());
    User::CommandLine(cmd);

	TLex arguments(*argv);
	
	TPtrC options(arguments.NextToken());
	TBool expectBufferSize = EFalse;
	_LIT(KBadBufferSize,"Bad buffersize specified\n");
	_LIT(KUnknownOption,"Unknown Options %S\n");

	
	while (options[0]=='-' || expectBufferSize)
		{
		TInt i = 1;

		if (expectBufferSize)
			{
			expectBufferSize = EFalse;
			if (options.Length() == 0)
				{
				console->Printf(KBadBufferSize);
				User::Leave(1);
				}
			else
				{
				TLex bufLex(options);
				if (bufLex.Val(bufferSize) != KErrNone)
					{
					console->Printf(KBadBufferSize);
					User::Leave(1);
					}
				}
			}
		else
			{	
			
			while (i < options.Length())
				{
				if (options[i] == 'd')
					compress = EFalse;
				else if (options[i] == 'b')
					{
					if (i + 1 < options.Length())
						{
						TLex bufLex(options.Right(options.Length() - (i + 1)));
						if (bufLex.Val(bufferSize) != KErrNone)
							{
							console->Printf(KBadBufferSize);
							User::Leave(1);
							}
						}	
					else
						expectBufferSize = ETrue;
					}
				else if (options[i] == 'h')
					readHeader = noWork = ETrue;
				else if (options[i] == 't')
					readTrailer = noWork = ETrue;
				else 
					{
					console->Printf(KUnknownOption,&options);
					i = options.Length();
					}
				i++;
				}
			
			if (i == 1)
				{
				_LIT(KNoOption,"No option specified\n");
				console->Printf(KNoOption);
				User::Leave(1);
				}
			}
		options.Set(arguments.NextToken());
		}

	console->Printf(_L("Buffer Size %d\n"),bufferSize);

	if (readHeader)
		{
		ReadAndPrintHeaderL(rfs,options);
		}

	if (readTrailer)
		{
		TEZGZipTrailer trailer;
		EZGZipFile::LocateAndReadTrailerL(rfs, options, trailer);
		_LIT(KTrailer,"Crc = %d Size = %d\n");
		console->Printf(KTrailer,trailer.iCrc32,trailer.iSize);
		}

	if (!noWork)
		{
		if (!compress)
			{
			
			TPtrC inputFile(options);
			
			HBufC *uncompressedFile = HBufC::NewLC(inputFile.Length()+1);
			_LIT(KUfl,"%S1");
			uncompressedFile->Des().Format(KUfl,&inputFile);
			
			RFile output;
			TInt err;
			
			_LIT(KInfo,"Decompressing file %S\n");
			console->Printf(KInfo,&inputFile);
			
			err = output.Create(rfs, *uncompressedFile,EFileStream | EFileWrite | EFileShareExclusive);
			if (err == KErrAlreadyExists)
				User::LeaveIfError(output.Open(rfs, *uncompressedFile,EFileStream | EFileWrite | EFileShareExclusive));
			else 
				User::LeaveIfError(err);
			CleanupClosePushL(output);
			
			CEZGZipToFile *def = CEZGZipToFile::NewLC(rfs,inputFile,output,bufferSize);
			while (def->InflateL()){/*do nothing*/}
			
			_LIT(KHoorah,"Hoorah");
			console->Printf(KHoorah);
			
			CleanupStack::PopAndDestroy(3);
			}
		else
			{
			TPtrC inputFile(options);
			
			HBufC *compressedFile = HBufC::NewLC(inputFile.Length()+3);
			_LIT(KUfl,"%S.gz");
			compressedFile->Des().Format(KUfl,&inputFile);
			
			RFile input;
			
			_LIT(KInfo,"Compressing file %S to %S\n");
			console->Printf(KInfo,&inputFile,compressedFile);
			
			User::LeaveIfError(input.Open(rfs,inputFile,EFileStream | EFileRead | EFileShareAny));
			CleanupClosePushL(input);

			CEZFileToGZip *com = CEZFileToGZip::NewLC(rfs,*compressedFile,input,bufferSize);
			while (com->DeflateL()){/*do nothing*/}
			
			_LIT(KHoorah,"Hoorah");
			console->Printf(KHoorah);
					
			CleanupStack::PopAndDestroy(3);
			}
		}
	CleanupStack::PopAndDestroy(1);
	rfs.Close();
	}


void ReadAndPrintHeaderL(RFs &rfs, const TDesC &fname)
	{
	TEZGZipHeader header;
	
	if (!EZGZipFile::IsGzipFileL(rfs,fname))
		{
		_LIT(KNotGzipFile,"%S is not a gzip file\n");
		console->Printf(KNotGzipFile,&fname);
		User::Leave(1);
		}
	RFile gzipFile;
	User::LeaveIfError(gzipFile.Open(rfs,fname,EFileStream | EFileRead | EFileShareAny));
	EZGZipFile::ReadHeaderL(gzipFile,header);
	
	_LIT(KFileIds,"ID1 = %d ID2 = %d\n");
	console->Printf(KFileIds,header.iId1,header.iId2);
	_LIT(KCompressionMethod,"Compression Method = %d\n");
	console->Printf(KCompressionMethod,header.iCompressionMethod);
	_LIT(KFlags,"Flags = %d\n");
	console->Printf(KFlags,header.iFlags);
	_LIT(KTime,"Time Stamp = %d\n");
	console->Printf(KTime,header.iTime);
	_LIT(KExtraFlags,"Extra Flags %d\n");
	console->Printf(KExtraFlags,header.iExtraFlags);
	_LIT(KOS,"OS %d\n");
	console->Printf(KOS,header.iOs);
	if (header.iFlags & 4)
		{
		_LIT(KExtraLen,"Extra Length %d\n");
		console->Printf(KExtraLen,header.iXlen);
		HBufC *buf = HBufC::NewMaxLC(header.iExtra->Length());
		buf->Des().Copy(*header.iExtra);
		console->Printf(*buf);
		CleanupStack::PopAndDestroy();
		}
	
	if (header.iFlags & 8)
		{
		_LIT(KName,"Name: %S\n");
		HBufC *buf = HBufC::NewMaxLC(header.iFname->Length());
		buf->Des().Copy(*header.iFname);
		console->Printf(KName,buf);
		CleanupStack::PopAndDestroy();
		}
	
	if (header.iFlags & 16)
		{
		_LIT(KComment,"Comment: %S\n");
		HBufC *buf = HBufC::NewMaxLC(header.iComment->Length());
		buf->Des().Copy(*header.iComment);
		console->Printf(KComment,buf);
		CleanupStack::PopAndDestroy();
		}
	
	if (header.iFlags & 2)
		{
		_LIT(KCrc,"Crc16 = %d\n");
		console->Printf(KCrc,header.iCrc);
		}
	}
