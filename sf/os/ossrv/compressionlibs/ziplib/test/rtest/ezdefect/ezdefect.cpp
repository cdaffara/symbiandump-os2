// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <zipfile.h>

LOCAL_D RTest				test(_L("ezdefect.exe"));
LOCAL_D RFs					TheFs;
LOCAL_D CTrapCleanup* 		TheTrapCleanup 		= NULL;

#if !defined(__WINS__)
_LIT(KPath, "Z:\\test\\zip\\");
#else
_LIT(KPath, "C:\\test\\zip\\");
#endif



void ExtractFileL(const CZipFileMember* aMember, CZipFile* aZip)
	{
	HBufC* name = aMember->Name()->AllocLC();
	for (TInt i = 0; i<name->Length(); i++)
		{
		if ((*name)[i] == '/')
			{
			name->Des()[i] = '\\';
			}
		}
	
	RZipFileMemberReaderStream* fileStream;
	User::LeaveIfError(aZip->GetInputStreamL(aMember, fileStream));
	CleanupStack::PushL(fileStream);

	TUint32 size = aMember->UncompressedSize();
	HBufC8* bytes = HBufC8::New(size);
	CleanupStack::PushL(bytes);
	TPtr8 ptr = bytes->Des();
	User::LeaveIfError(fileStream->Read(ptr,size));	
	CleanupStack::PopAndDestroy(3,name); // bytes, fileStream and name
	}
		
// Extract all members from the zip.
// These files are non compressed, i.e. EStored.
void ExtractMembersL(CZipFile* aZip)
	{
	test.Next(_L("DEF057916: Getting Corrupt file error when reading wav-file from Zip - PART 2"));

	CZipFileMemberIterator*	members = aZip->GetMembersL();
	CleanupStack::PushL(members);
	
	CZipFileMember* member = members->NextL();
	while (member)
		{
		CleanupStack::PushL(member);
		ExtractFileL(member, aZip);
		CleanupStack::PopAndDestroy(member); 
		member = members->NextL();
		}

	CleanupStack::PopAndDestroy(members); 
	}

// Extract the specified files from the zip.
// These files are non compressed, i.e. EStored.
void DEF057916L()
    {
	test.Next(_L("DEF057916: Getting Corrupt file error when reading wav-file from Zip - PART 1"));

	User::LeaveIfError(TheFs.Connect()); //Connect to file session
	User::LeaveIfError(TheFs.SetSessionPath(KPath)); //Set Session Path to direcrt containing test zip files
	CleanupClosePushL(TheFs);

	const TUint KDataChunk = 2048;
	_LIT(tmpFileName, "\\Test\\Zip\\compression_estored.zip");
	_LIT(testFile1, "zip\\test.wav");
	_LIT(testFile2, "zip\\rfc2459.zip");
	_LIT(testFile3, "zip\\META-INF\\MANIFEST.MF");
	
	CZipFile* iZipFile = CZipFile::NewL(TheFs, tmpFileName);
	CleanupStack::PushL(iZipFile);

	TInt err = KErrNone;

	CZipFileMember* zipMember1 = iZipFile->CaseSensitiveOrCaseInsensitiveMemberL(testFile1);
	if(zipMember1)
		{
		CleanupStack::PushL(zipMember1);

		RZipFileMemberReaderStream* iZipStream;
		iZipFile->GetInputStreamL(zipMember1, iZipStream);
		CleanupStack::PushL(iZipStream);

		HBufC8* data = HBufC8::NewLC(KDataChunk);
		TPtr8 ptr = data->Des();
		err = KErrNone;
		
		err = iZipStream->Read(ptr,KDataChunk);
		test (err == KErrNone);
		
		err = iZipStream->Read(ptr,KDataChunk);
		test (err == KErrEof);
		
		CleanupStack::PopAndDestroy(data);
		CleanupStack::PopAndDestroy(iZipStream);
		CleanupStack::PopAndDestroy(zipMember1);  
		}  
		
		
	CZipFileMember* zipMember2 = iZipFile->CaseSensitiveOrCaseInsensitiveMemberL(testFile2);
	if(zipMember2)
		{
		CleanupStack::PushL(zipMember2);

		RZipFileMemberReaderStream* iZipStream;
		iZipFile->GetInputStreamL(zipMember2, iZipStream);
		CleanupStack::PushL(iZipStream);

		HBufC8* data = HBufC8::NewLC(KDataChunk*65);
		TPtr8 ptr = data->Des();
		err = KErrNone;
		
		err = iZipStream->Read(ptr,KDataChunk*65);
		test (err == KErrNone); // file is very very large
		
		err = iZipStream->Read(ptr,KDataChunk);
		test (err == KErrEof);

		CleanupStack::PopAndDestroy(data);
		CleanupStack::PopAndDestroy(iZipStream);
		CleanupStack::PopAndDestroy(zipMember2);  
		}  
		
	
	CZipFileMember* zipMember3 = iZipFile->CaseSensitiveOrCaseInsensitiveMemberL(testFile3);
	if(zipMember3)
		{
		CleanupStack::PushL(zipMember3);

		RZipFileMemberReaderStream* iZipStream;
		iZipFile->GetInputStreamL(zipMember3, iZipStream);
		CleanupStack::PushL(iZipStream);

		HBufC8* data = HBufC8::NewLC(KDataChunk);
		TPtr8 ptr = data->Des();
		err = KErrNone;
		
		err = iZipStream->Read(ptr,KDataChunk);
		test (err == KErrNone);
		
		err = iZipStream->Read(ptr,KDataChunk);
		test (err == KErrEof);

		CleanupStack::PopAndDestroy(data);
		CleanupStack::PopAndDestroy(iZipStream);
		CleanupStack::PopAndDestroy(zipMember3);  
		}  		
		
	ExtractMembersL (iZipFile);
	
	CleanupStack::PopAndDestroy(iZipFile);
	CleanupStack::PopAndDestroy(&TheFs);
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB-CT-3464
@SYMTestCaseDesc     	RZipFileMemberReaderStream::Read returns incorrect error code under OOM
@SYMTestPriority     	High
@SYMTestActions      	Tries to read zipfile stream with heap allocation set to fail on all following 
						allocations.  Check result returned from Read(TDes16&, TInt) and Read(TDes8&, TInt) 
						is KErrNoMemory.  Note that zip file contents must be compressed, otherwise no 
						allocations are performed and Read() function returns KErrNone.
@SYMTestExpectedResults Read() function should return KErrNoMemory
@SYMDEF                 DEF103961
*/
void DEF103961L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-3464 DEF103961: RZipFileMemberReaderStream::Read returns incorrect error code under OOM "));

	User::LeaveIfError(TheFs.Connect()); //Connect to file session
	User::LeaveIfError(TheFs.SetSessionPath(KPath)); //Set Session Path to directory containing test zip files
	CleanupClosePushL(TheFs);

	const TUint KDataChunk = 512;
	_LIT(testFile1, "zip_archive2_with_comments.zip");
	_LIT(internalFile, "doorslam.wav");
	
	// open zip file
	CZipFile* zipFile = CZipFile::NewL(TheFs, testFile1);
	CleanupStack::PushL(zipFile);

	// get input stream for a file contained in the zip archive
	CZipFileMember* zipMember = zipFile->CaseSensitiveOrCaseInsensitiveMemberL(internalFile);
	test(zipMember!=NULL);
	CleanupStack::PushL(zipMember);
	RZipFileMemberReaderStream* zipStream;
	zipFile->GetInputStreamL(zipMember, zipStream);
	CleanupStack::PushL(zipStream);
	
	// some descriptors to hold stream data...
	HBufC8* data8 = HBufC8::NewLC(KDataChunk);
	TPtr8 ptr8 = data8->Des();
	HBufC16* data16 = HBufC16::NewLC(KDataChunk);
	TPtr16 ptr16 = data16->Des();

	// do memory tests for RZipFileMemberReaderStream::Read() and its overloaded brother
	__UHEAP_SETFAIL(RHeap::EDeterministic,1);
	TInt error = zipStream->Read(ptr8, KDataChunk);
	test (error == KErrNoMemory);
	error = zipStream->Read(ptr16, KDataChunk);
	test (error == KErrNoMemory);
	__UHEAP_SETFAIL(RHeap::ENone,0);
	
	CleanupStack::PopAndDestroy(data16);
	CleanupStack::PopAndDestroy(data8);
	CleanupStack::PopAndDestroy(zipStream);
	CleanupStack::PopAndDestroy(zipMember);
	CleanupStack::PopAndDestroy(zipFile);
	CleanupStack::PopAndDestroy(&TheFs);
	}	

/**
@SYMTestCaseID		SYSLIB-EZLIB-CT-3475
@SYMTestCaseDesc 	RZipFileMemberReaderStream constructor ignores errors under OOM
@SYMTestPriority    High
@SYMTestActions     Construct RZipFileMemberReaderStream under OOM conditions.  Check the Leave code.
@SYMTestExpectedResults	Should Leave with KErrNoMemory
@SYMDEF              	DEF105995
*/
void DEF105995L()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB-CT-3475 DEF105995: RZipFileMemberReaderStream constructor not checking OOM conditions "));
	
	User::LeaveIfError(TheFs.Connect()); //Connect to file session
	User::LeaveIfError(TheFs.SetSessionPath(KPath)); //Set Session Path to directory containing test zip files
	CleanupClosePushL(TheFs);

	_LIT(testFile1, "zip_archive2_with_comments.zip");
	_LIT(internalFile, "doorslam.wav");
	
	// open zip file
	CZipFile* zipFile = CZipFile::NewL(TheFs, testFile1);
	CleanupStack::PushL(zipFile);

	// get input stream for a file contained in the zip archive
	CZipFileMember* zipMember = zipFile->CaseSensitiveOrCaseInsensitiveMemberL(internalFile);
	test(zipMember!=NULL);
	CleanupStack::PushL(zipMember);
	
	__UHEAP_SETFAIL(RHeap::EDeterministic,2); // so memory allocation in inflateInit2() fails

	RZipFileMemberReaderStream* zipStream;
	TRAPD(err, zipFile->GetInputStreamL(zipMember, zipStream));
	test(err == KErrNoMemory);	

	__UHEAP_SETFAIL(RHeap::ENone,0);
	
	CleanupStack::PopAndDestroy(zipMember);
	CleanupStack::PopAndDestroy(zipFile);
	CleanupStack::PopAndDestroy(&TheFs);
	}
				
void RunTestL()
	{
	DEF057916L(); // Getting Corrupt file error when reading wav-file from Zip
	#if defined(_DEBUG)	// OOM tests are only run on Debug builds
	DEF103961L(); // RZipFileMemberReaderStream::Read returns incorrect error code under OOM
	DEF105995L(); // inflateInit2 is not checked for errors in RZipFileMemberReaderStream constructor
	#endif
	}	

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start( _L("EZlib defect Tests.") );

	TheTrapCleanup=CTrapCleanup::New();

	TRAPD(err,RunTestL());
	test (err==KErrNone);

	test.End();
	test.Close();
	delete TheTrapCleanup;

	__UHEAP_MARKEND;
	return KErrNone;
	}
