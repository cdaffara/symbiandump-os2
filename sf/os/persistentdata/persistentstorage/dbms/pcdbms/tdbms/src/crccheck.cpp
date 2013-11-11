// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "crccheck.h"

const TInt TDBMS_CRCChecks::SetSessionPath(const TDesC& aPath)
	{
	return ifs.SetSessionPath(aPath);
	}

TInt TDBMS_CRCChecks::FileCrcL(const RFile& aFile, TUint32 &asum)
	{
	const TInt KFileCrcBufSize = 1024;

	TInt err;
	TInt pos = 0;
	// Seek to the beginning of the file.
	if( (err = aFile.Seek(ESeekStart, pos)) != KErrNone)
		return err;

	RBuf8 buffer;
	if((err = buffer.Create(KFileCrcBufSize)) != KErrNone)
		return err;
	CleanupClosePushL(buffer);
	asum=0;
	for(;;)
		{
		err = aFile.Read(buffer, KFileCrcBufSize);
		if(err) break;
		TInt len = buffer.Length();
		if(len == 0) break;
		Mem::Crc32(asum, (TAny*) buffer.Ptr(), len);
		}
	CleanupStack::PopAndDestroy(1, &buffer);
	return err;
	}

const TInt TDBMS_CRCChecks::GenerateCrcL(const TPtrC aFile)
	{
	RFile file;
	TInt err = file.Open(ifs, aFile, EFileRead);
	RDebug::Print(_L("==================== File open=%S, err=%d\n"), &aFile, err);
	if(err != KErrNone) 
		return err;
	CleanupClosePushL(file);

	TestCheckInfo tcinf;
	err = file.Size(tcinf.filesz);
	if(err)
		{
		CleanupStack::PopAndDestroy(1);
		return err;
		}
	err = FileCrcL(file, tcinf.crc);
	if(err)
		{
		CleanupStack::PopAndDestroy(1);
		return err;
		}
    err = iarray.Append(tcinf);
	CleanupStack::PopAndDestroy(1);
	return err;
	}

const TInt TDBMS_CRCChecks::RecordCount()
	{
	return iarray.Count();
	}

const TestCheckInfo TDBMS_CRCChecks::operator[](const TInt aidx)
	{
	return iarray[aidx];
	}

const TInt TDBMS_CRCChecks::DumpCrcRecordsL(const TDesC &alog)
	{
	RFile logfile;
	TInt err = logfile.Replace(ifs, alog, EFileWrite);
	RDebug::Print(_L("==================== File replace=%S, err=%d\n"), &alog, err);
	if(err != KErrNone)
		return err;
	CleanupClosePushL(logfile);
	TBuf8<0x100> output;

	for(TInt i=0 ; i<RecordCount() ; i++)
		{
		output.SetLength(0);
		TestCheckInfo tc = iarray[i];
		output.AppendFormat(_L8("%d %d 0x%08x\n"), i+1, tc.filesz, tc.crc );
		err = logfile.Write(output);
		if(err) break;
		}
	CleanupStack::PopAndDestroy(1);
	return err;
	}

const TInt TDBMS_CRCChecks::ValidateCrcRecordsL(const TDesC &alog)
	{
	RFile logfile;
	TInt err = logfile.Open(ifs, alog, EFileRead);
	RDebug::Print(_L("==================== File open=%S, err=%d\n"), &alog, err);
	if(err != KErrNone)
		return err;
	CleanupClosePushL(logfile);
   	
	TInt logfilesz;
	if((err = logfile.Size(logfilesz)) != KErrNone)
		{
		CleanupStack::PopAndDestroy(1);
		return err;
		}

	RBuf8 input;
	if((err = input.Create(logfilesz)) != KErrNone)
		{
		CleanupStack::PopAndDestroy(1);
		return err;
		}
	CleanupClosePushL(input);
	// Read the entire file.
	err = logfile.Read(input);
	if(err != KErrNone)
		{
		CleanupStack::PopAndDestroy(2);
		return err;
		}
	TInt nread = input.Length();
	if(nread != logfilesz)
		{
		CleanupStack::PopAndDestroy(2);
		User::Leave(KErrCorrupt);  // wrong error.
		}

	TPtrC8 slice;
	slice.Set(input);
	TInt offset=0;
	TBuf8<0x100> expected;
	TInt i; // we check this after the loop...
	for(i=0; ; i++)
		{
		// Find the next carriage return in the file. 'slice' represents
		// the next bit of the file before the next carriage return.
		// Will this break on Symbian? That has '\r\n'...
		TInt nextcr = slice.Locate(TChar('\n'));
		// If no carriage return is found we must have reached the end of
		// the file.
		if(nextcr == KErrNotFound)
		  	break;
		// 'got' is the current line from the file, including the carriage
		// return.
		TPtrC8 got = slice.Left(nextcr+1);

		// Before we construct the string this object expects to see,
		// check we're not out of array..
		// The number of lines in the file we checking should match what
		// we have in our internal array.
		if(i >= iarray.Count())
			{
			err = ECrcCheckMoreRecords;
			break;
			}

		// Construct a string from our internal data that is expected to be
		// the same as the data read in from the crc file.
		expected.SetLength(0);
		TestCheckInfo tc = iarray[i];
		expected.AppendFormat(_L8("%d %d 0x%08x\n"), i+1, tc.filesz, tc.crc );
        // Compare what we've got to what we expect.
		if(got.Compare(expected) != 0)
			{
			err = ECrcCheckMismatch;
			break;
			}

		offset += nextcr + 1;
		slice.Set(input.Mid(offset));
		}
	// The number of lines in the file we checking should match what
	// we have in our internal array. Here this indicates that we seem to
	// have more in our array than appear in the file.
	if(!err && (i != iarray.Count()))
		err = ECrcCheckFewerRecords;
	CleanupStack::PopAndDestroy(2);
	return err;
	}

const void TDBMS_CRCChecks::ErrorReportL(const TInt aerr, TPtrC& aerrmsg)
	{
	switch(aerr)
		{
		case KErrNotFound:
				aerrmsg.Set(_L("Failed to open CRC log file.\n"));
				break;
		case KErrNoMemory:
				aerrmsg.Set(_L("Out of memory.\n"));
				break;
		case KErrNone:
		case ECrcCheckOk:
				aerrmsg.Set(_L("CRC check ok.\n"));
				break;
		case ECrcCheckMismatch:
				aerrmsg.Set(_L("CRC mismatch.\n"));
				break;
		case ECrcCheckFewerRecords:
				aerrmsg.Set(_L("Fewer CRCs than in the file!\n"));
				break;
		case ECrcCheckMoreRecords:
				aerrmsg.Set(_L("More CRCs in the file than I have!\n"));
				break;
		default:
				aerrmsg.Set(_L("Broken!\n")); // PANIC?
				break;
		}
	return;
	}

#ifdef CRC_TEST
int E32Main(void)
	{
	CTrapCleanup *cleanup = CTrapCleanup::New();
	__ASSERT_ALWAYS(cleanup != NULL, User::Invariant());

	TDBMS_CRCChecks mycrc;
	TInt err;
   	TRAPD(lc, err = mycrc.GenerateCrcL(_L("crcchecks.mmp")));
   	TRAP(lc, err = mycrc.GenerateCrcL(_L("bld.inf")));

	TRAP(lc, err = mycrc.DumpCrcRecordsL(_L("wibble")));
   	TRAP(lc, err = mycrc.GenerateCrcL(_L("t_alter.mmp")));
	TRAP(lc, err = mycrc.ValidateCrcRecordsL(_L("wibble")));
	switch(err)
		{
		case TDBMS_CRCChecks::ECrcCheckMismatch:
				printf("Got CRC mismatch\n");
				break;
		case TDBMS_CRCChecks::ECrcCheckFewerRecords:
				printf("I have more CRCs than in the file!\n");
				break;
		case TDBMS_CRCChecks::ECrcCheckMoreRecords:
				printf("More CRCs in the file than I have!\n");
				break;
		default:
				printf("Broken!\n");
				break;
		}
	printf("Leavecode = %d, err = %d\n", lc, err );

	delete cleanup;
	}
#endif
