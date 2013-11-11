// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Note : Testing for file uri component extraction for external media drive will be done only 
// if an external media drive does exist
// 
//

#include "CFileUriTest.h"

#include <e32base.h>
#include <escapeutils.h>
#include "FileUriTestCommon.h"

//File URI Constents
_LIT8(KFileUriScheme8, "file");
_LIT16(KFileUriScheme16, "file");

//
//
//	File Uri Tests
//
//

CFileUriTest* CFileUriTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CFileUriTest* self = new (ELeave) CFileUriTest(aTestHarness);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CFileUriTest* CFileUriTest::NewL(CIpuTestHarness* aTestHarness)
	{
	CFileUriTest* self = CFileUriTest::NewLC(aTestHarness);
	CleanupStack::Pop(self);
	return self;
	}

CFileUriTest::CFileUriTest(CIpuTestHarness* aTestHarness)
: iTestHarness(aTestHarness)
	{
	}

void CFileUriTest::ConstructL()
	{
	}

CFileUriTest::~CFileUriTest()
	{
	}

void CFileUriTest::DoTestsL()
	{
	TBuf<3> drive(_L("$:\\"));
	//File URI Creation Test - for public files on fixed and removable media 
	//----------------------------------------------------------------------------------
	_LIT(KFullFileName, "c:\\public\\bar\\foo\\some\\randomness.txt");
	
	//Testing 8-bit & 16-bit File URI creation for public file on a fixed drive
	_LIT(KExpectedUri, "file://c/public/bar/foo/some/randomness.txt");
	TestFileUriCreationL(KFullFileName, KExpectedUri, 0);
	
	_LIT(KFullFileName2, "c:\\dummy\\file.txt");
 	_LIT(KExpectedUri3, "file://c/dummy/file.txt");
	TestFileUriCreationL(KFullFileName2, KExpectedUri3, 0);

	_LIT(KFullFileName1, "c:\\randomness.txt");
	
	//Testing 8-bit & 16-bit File URI creation for public file on a fixed drive
	_LIT(KExpectedUri2, "file://c/randomness.txt");
	TestFileUriCreationL(KFullFileName1, KExpectedUri2, 0);

	//Testing 8-bit & 16-bit File URI creation for public file on a removable media drive
	_LIT(KExpectedUri1, "file://ext-media/public/bar/foo/some/randomness.txt");
	TestFileUriCreationL(KFullFileName, KExpectedUri1, EExtMedia);
	
	_LIT(KExtMediaFullFileName, "X:\\mydir\\myfile.ext");
	_LIT(KExtMediaExpectedUri1, "file://ext-media/mydir/myfile.ext");
	TestFileUriCreationL(KExtMediaFullFileName, KExtMediaExpectedUri1, EExtMedia);
	//----------------------------------------------------------------------------------

	//File URI component extraction Test - Schema, Path and Filename - 
	// - for public files on fixed and removable media 
	//----------------------------------------------------------------------------------
	_LIT(KFileName, "c:\\public\\bar\\foo\\some\\randomness.txt");
	
	//Testing 8-bit & 16-bit File URI creation for public file on a fixed drive
	_LIT(KExpectedPath, "/c/public/bar/foo/some/randomness.txt");
	TestFileUriComponentExtractionL(KFileName, KExpectedPath, KFileName, drive, 0);
	
	//Testing 8-bit & 16-bit File URI creation for public file on a fixed drive
	_LIT(KFileName1, "c:\\system\\·····.ini");
	_LIT(KExpectedPath1, "/c/system/·····.ini");
	TestFileUriComponentExtractionL(KFileName1, KExpectedPath1, KFileName1, drive, 0);

	//Creating files on removable media drive (X:) 
	// Need to change the removable Drive letter if X doesn't exist or its not removable media drive  	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TDriveInfo driveInfo;
	TInt err = KErrNotFound;
	TInt driveNum = EDriveA;
	
	//Checking for extenal media drive does exist
	for (; driveNum <= EDriveZ && err!=KErrNone; driveNum++)   
		{
		if (fs.Drive(driveInfo, driveNum ) == KErrNone
			&& (driveInfo.iDriveAtt & KDriveAttRemovable))       
			{
			drive[0]= TInt16('A' + driveNum);
			err = KErrNone;
			}
		}
	
	// Test only if external media drive exist
  	if (err == KErrNone)
  		{
  	 	driveNum--;
  			
		User::LeaveIfError(fs.SetSessionPath(drive));
		TInt ret = fs.MkDir(_L("\\mydir\\"));
		if (ret == KErrNone || ret == KErrAlreadyExists)
			{		
			RFile file;
			ret = file.Create(fs,_L("\\mydir\\myfile.ext"),EFileWrite|EFileShareAny);
			file.Close();
			if (ret == KErrNone || ret == KErrAlreadyExists)
				{		
				//Testing 8-bit & 16-bit File URI creation for public file on a removable media drive
				_LIT(KExpectedExtMediaPath1, "/ext-media/mydir/myfile.ext");
				TFileName extMediaFileName(KExtMediaFullFileName);
				extMediaFileName[0] = drive[0];
				TestFileUriComponentExtractionL(extMediaFileName, KExpectedExtMediaPath1, extMediaFileName, drive, EExtMedia);
				CFileMan* fm = CFileMan::NewL(fs);
				ret = fm->RmDir(_L("\\mydir\\"));
				delete fm;	
				}
			}
		}	
	
	CleanupStack::PopAndDestroy(&fs);
	//----------------------------------------------------------------------------------
  	  		
	//File URI component extraction, obj created using CUri::NewLC Test 
	// - Schema, Path and Filename
	//----------------------------------------------------------------------------------
	
	TestFileUriCreationWithOldAPIAndComponentExtractionL(KFileName, KExpectedPath, KFileName );
		
	//----------------------------------------------------------------------------------
  		
	// resolve %-codes
	TestFileUriNameExtractionL(_L("file://c/dir/test/file%20name.html"), _L("file name.html"), _L("\\dir\\test\\file name.html") );
	// params in path
	TestFileUriNameExtractionL(_L("http://www.text.org/cgi-bin/apps/index.html;one;two;three"), _L("index.html"), _L("\\cgi-bin\\apps\\index.html") );

	// resolve %-codes
	TestFileUriNameExtractionL(_L8("file://c/dir/test/file%20name.html"), _L("file name.html"), _L("\\dir\\test\\file name.html") );
	
	// params in path
	TestFileUriNameExtractionL(_L8("http://www.text.org/cgi-bin/apps/index.html;one;two;three"), _L("index.html"), _L("\\cgi-bin\\apps\\index.html") );
	
	//----------------------------------------------------------------------------------


				
	//File URI Creation Test - for private files on fixed and removable media 
	//----------------------------------------------------------------------------------
	
	//Testing 8-bit & 16-bit File URI creation for private file on a fixed drive
	_LIT(KExpectedprivateUri, "file://private/Z/bar/foo/some/randomness.txt");
	
	_LIT(KRelativeFileName, "c:\\bar\\foo\\some\\randomness.txt");
	TestPrivateFileUriCreationL(KRelativeFileName, EDriveZ, KExpectedprivateUri, 0);
	
	_LIT(KRelativeFileName1, "\\bar\\foo\\some\\randomness.txt");
	TestPrivateFileUriCreationL(KRelativeFileName1, EDriveZ, KExpectedprivateUri, 0);
	
	_LIT(KRelativeFileName2, "bar\\foo\\some\\randomness.txt");
	TestPrivateFileUriCreationL(KRelativeFileName2, EDriveZ, KExpectedprivateUri, 0);
	
	//Testing 8-bit & 16-bit File URI creation for private file on a removable media drive
	_LIT(KExtMediaRelativeFileName, "\\mydir\\myfile.ext");
	_LIT(KExpectedPrivateExtMediaUri, "file://private/ext-media/mydir/myfile.ext");
	TestPrivateFileUriCreationL(KExtMediaRelativeFileName, EDriveZ, KExpectedPrivateExtMediaUri, EExtMedia);
	//----------------------------------------------------------------------------------
			
	//File URI component extraction Test - Schema, Path and Filename - 
	// - for private files on fixed and removable media 
	//----------------------------------------------------------------------------------
	
	//Retrieveing application private path
	
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);		
	TPath applPrivatePath;
	User::LeaveIfError(fs.PrivatePath(applPrivatePath));
	CleanupStack::PopAndDestroy(&fs);
		
	//Testing 8-bit & 16-bit File URI creation for private file on a fixed drive
	_LIT(KExpectedPrivateFilePath, "/private/Z/bar/foo/some/randomness.txt");
	_LIT(KExpectedPrivateFileName, "bar\\foo\\some\\randomness.txt");
	TFileName expectedPrivateFileName(_L("Z:"));
	expectedPrivateFileName.Append(applPrivatePath);
	expectedPrivateFileName.Append(KExpectedPrivateFileName);
	
	TestPrivateFileUriComponentExtractionL(KRelativeFileName1, EDriveZ, KExpectedPrivateFilePath, expectedPrivateFileName, 0);
		
	//Testing 8-bit & 16-bit File URI creation for private file on a removable media drive
	_LIT(KExpectedPrivateExtMediaFilePath, "/private/ext-media/mydir/myfile.ext");
	expectedPrivateFileName[0] = drive[0];
	
	//Since we can't create folder "private" on removable media drive for testing purpose, the file extraction testing is not possible.
	// So need to pass KNullDesC to avoid file extraction
	TestPrivateFileUriComponentExtractionL(KExtMediaRelativeFileName, EDriveZ, KExpectedPrivateExtMediaFilePath, KNullDesC, EExtMedia);
	//----------------------------------------------------------------------------------
	}
	
void CFileUriTest::TestFileUriCreationL(const TDesC16& aFullFileName, const TDesC16& aExpectedUri, TUint aFlags) const
	{
	//Testing 8-bit File URI creation for public file on a fixed drive	
	iTestHarness->StartTestL(_L("Test CUri - Public File URI creation (8bit)"));
	iTestHarness->LogIt(_L("FileName: %S; \n\t\tFileStorageType:%D; \n\t\tExpected URI:%S"), &aFullFileName, aFlags, &aExpectedUri);
	TInt error = KErrNone;
	CUri8* uri8 = CUri8::CreateFileUriL(aFullFileName,aFlags);
	CleanupStack::PushL(uri8);
	HBufC8* expectedUri8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aExpectedUri);
	CleanupStack::PushL(expectedUri8);
	error = DoFileUriExtraction<CUri8>(*uri8, *expectedUri8);
	CleanupStack::PopAndDestroy(expectedUri8);
	CleanupStack::PopAndDestroy(uri8);
	iTestHarness->EndTest(error);
	
	//Testing 16-bit File URI creation for public file on a fixed drive
	error = KErrNone;
	iTestHarness->StartTestL(_L("Test CUri - File URI creation (16bit)"));
	iTestHarness->LogIt(_L("FileName: %S; \n\t\tFileType:%D; \n\t\tExpected URI:%S"), &aFullFileName, aFlags, &aExpectedUri);
	CUri16* uri16 = CUri16::CreateFileUriL(aFullFileName,aFlags);
	CleanupStack::PushL(uri16);
	error = DoFileUriExtraction<CUri16>(*uri16, aExpectedUri);
	CleanupStack::PopAndDestroy(uri16);
	iTestHarness->EndTest(error);
	}

void CFileUriTest::TestPrivateFileUriCreationL(const TDesC16& aRelativeFileName, const TDriveNumber aDrive, const TDesC16& aExpectedUri, TUint aFlags) const
	{
	//Testing 8-bit File URI creation for public file on a fixed drive
	iTestHarness->StartTestL(_L("Test CUri - Private File URI creation (8bit)"));
	iTestHarness->LogIt(_L("FileName: %S; \n\t\tFileStorageType:%D; \n\t\tDrive:%c; \n\t\tExpectedURI:%S"), &aRelativeFileName, aFlags, ('A' + aDrive), &aExpectedUri);
	TInt error = KErrNone;
	CUri8* uri8 = CUri8::CreatePrivateFileUriL(aRelativeFileName,aDrive,aFlags);
	CleanupStack::PushL(uri8);
	HBufC8* expectedUri8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aExpectedUri);
	error = DoFileUriExtraction<CUri8>(*uri8, *expectedUri8);
	delete expectedUri8;
	CleanupStack::PopAndDestroy(uri8);
	iTestHarness->EndTest(error);

	//Testing 16-bit File URI creation for public file on a fixed drive
	error = KErrNone;
	iTestHarness->StartTestL(_L("Test CUri - Private File URI creation (16bit)"));
	iTestHarness->LogIt(_L("FileName: %S; \n\t\tFileStorageType:%D; \n\t\tDrive:%c; \n\t\tExpectedURI:%S"), &aRelativeFileName, aFlags, ('A' + aDrive), &aExpectedUri);
	CUri16* uri16 = CUri16::CreatePrivateFileUriL(aRelativeFileName,aDrive,aFlags);
	CleanupStack::PushL(uri16);
	error = DoFileUriExtraction<CUri16>(*uri16, aExpectedUri);
	CleanupStack::PopAndDestroy(uri16);
	iTestHarness->EndTest(error);	
	}

void CFileUriTest::TestFileUriComponentExtractionL(const TDesC16& aFileName, const TDesC16& aPath, const TDesC16& aExpectedFileName, const TDesC16& aExpectedDrive, TUint aFlags) const
	{
	iTestHarness->StartTestL(_L("Test File Uri Component Extraction (8-bit)"));
	iTestHarness->LogIt(_L("FileName: %S; \n\t\tFileStorageType:%D; \n\t\tPath:%S"), &aFileName,aFlags, &aPath);
	HBufC8* fileUriScheme8 = HBufC8::NewLC(4);
	fileUriScheme8->Des().Copy(KFileUriScheme8);
	
	HBufC* fileUriScheme16 = HBufC::NewLC(4);
	fileUriScheme16->Des().Copy(KFileUriScheme16);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CFileMan *fm = CFileMan::NewL(fs);
	CleanupStack::PushL(fm);
	
	TInt len = aFileName.Length();
	TInt ret = 0;

	//Testing 8-bit File URI componet extraction for public file on a fixed drive
	TInt error = KErrNone;
	
	CUri8* uri8 = CUri8::CreateFileUriL(aFileName,aFlags);
	CleanupStack::PushL(uri8);
	
	HBufC8* path8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aPath);
	CleanupStack::PushL(path8);
		
	HBufC8* escapedUriPath8 = EscapeUtils::EscapeEncodeL(*path8, EscapeUtils::EEscapeNormal);
	CleanupStack::PushL(escapedUriPath8);
	
	error = DoFileUriComponentExtraction<TUriParser8, CUri8>(*uri8, *fileUriScheme8, *escapedUriPath8, aExpectedFileName);
	
	iTestHarness->EndTest(error);	
	
	//-ve testing : After renaming file it should return error	
	if (aFlags == EExtMedia && error == KErrNone)
		{
		iTestHarness->StartTestL(_L("EXT-MEDIA::Test File Uri Component Extraction(8-bit)"));
		iTestHarness->LogIt(_L("FileName: %S; \n\t\tFileStorageType:%D; \n\t\tPath:%S; \n\t\tExpectedDrive:%S"), &aFileName,aFlags, &aPath, &aExpectedDrive);
	
		error = KErrNotFound;
		User::LeaveIfError(fs.SetSessionPath(aExpectedDrive));
		ret = fm->Rename(aFileName,(aFileName.Left(len-4)));
		
		if (ret == KErrNone || ret == KErrAlreadyExists)
			{
			TRAP(error, (DoFileUriComponentExtraction<TUriParser8, CUri8>(*uri8, *fileUriScheme8, *path8, aExpectedFileName)));
			error = ((error != KErrNone) ? 0 : -1);		
			ret = fm->Rename(aFileName.Left(aFileName.Length()-4),aFileName,CFileMan::EOverWrite);
			}
	
		iTestHarness->EndTest(error);	
		}
	
	//Testing 16-bit File URI componet extraction for public file on a removable media drive
	iTestHarness->StartTestL(_L("Test File Uri Component Extraction (16-bit)"));
	iTestHarness->LogIt(_L("FileName: \n\t\t%S; \n\t\tFileStorageType:%D; \n\t\tPath:%S"), &aFileName, aFlags, &aPath);
	
	CUri16* uri16 = CUri16::CreateFileUriL(aFileName,aFlags);
	CleanupStack::PushL(uri16);
	
	HBufC* escapedUriPath = HBufC::NewLC(escapedUriPath8->Length());
	escapedUriPath->Des().Copy(*escapedUriPath8);
	
	error = DoFileUriComponentExtraction<TUriParser16, CUri16, TDesC16>(*uri16, *fileUriScheme16, *escapedUriPath, aExpectedFileName);
	
	iTestHarness->EndTest(error);
	
	//-ve testing : After renaming file it should return error
	if (aFlags == EExtMedia && error == KErrNone)
		{
		iTestHarness->StartTestL(_L(" EXT-MEDIA::Test File Uri Component Extraction(16-bit)"));
		iTestHarness->LogIt(_L("FileName: %S; \n\t\tFileStorageType:%D; \n\t\tPath:%S; \n\t\tExpectedDrive:%S"), &aFileName,aFlags, &aPath, &aExpectedDrive);
	
		error = KErrNotFound;
		User::LeaveIfError(fs.SetSessionPath(aExpectedDrive));
		ret = fm->Rename(aFileName,(aFileName.Left(len-4)));
	
		if (ret == KErrNone || ret == KErrAlreadyExists)
			{			
			TRAP(error, (DoFileUriComponentExtraction<TUriParser16, CUri16, TDesC16>(*uri16, *fileUriScheme16, aPath, aExpectedFileName)));
			error = ((error != KErrNone) ? 0 : -1);		
			ret = fm->Rename(aFileName.Left(aFileName.Length()-4),aFileName,CFileMan::EOverWrite);
			}
	
		iTestHarness->EndTest(error);
		}

	CleanupStack::PopAndDestroy(escapedUriPath);
	CleanupStack::PopAndDestroy(uri16);
	CleanupStack::PopAndDestroy(escapedUriPath8);
	CleanupStack::PopAndDestroy(path8);
	CleanupStack::PopAndDestroy(uri8);	
	CleanupStack::PopAndDestroy(4, fileUriScheme8); //fileUriScheme8, fileUriScheme16,fs,fm 		
	}
	
void CFileUriTest::TestPrivateFileUriComponentExtractionL(const TDesC16& aFileName, const TDriveNumber aDrive, const TDesC16& aPath, const TDesC16& aExpectedFileName, TUint aFlags) const
	{
	iTestHarness->StartTestL(_L("Test Private File Uri Component Extraction (8-bit)"));
	iTestHarness->LogIt(_L("FileName: %S; \n\t\tDrive:%c, \n\t\tFileStorageType:%D; \n\t\tPath:%S"), &aFileName, ('A' + aDrive), aFlags, &aPath);
	TInt error = KErrNone;
	HBufC8* fileUriScheme8 = HBufC8::NewLC(4);
	TPtr8 fileUriScheme8Ptr = fileUriScheme8->Des();
	fileUriScheme8Ptr.Copy(KFileUriScheme8);
	HBufC* fileUriScheme16 = HBufC::NewLC(4);
	TPtr16 fileUriScheme16Ptr = fileUriScheme16->Des();
	fileUriScheme16Ptr.Copy(KFileUriScheme16);
	
	//Testing 8-bit File URI componet extraction for private file on a fixed drive
	CUri8* prv_uri8 = CUri8::CreatePrivateFileUriL(aFileName,aDrive,aFlags);
	CleanupStack::PushL(prv_uri8);
	HBufC8* path8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aPath);
	error = DoFileUriComponentExtraction<TUriParser8>(*prv_uri8, *fileUriScheme8, *path8, aExpectedFileName);
	delete path8;
	CleanupStack::PopAndDestroy(prv_uri8);	
	iTestHarness->EndTest(error);	
	
	//Testing 16-bit File URI componet extraction for private file on a removable media drive
	iTestHarness->StartTestL(_L("Test Private File Uri Component Extraction (16-bit)"));
	iTestHarness->LogIt(_L("FileName: %S; \n\t\tDrive:%c, \n\t\tFileStorageType:%D; \n\t\tPath:%S"), &aFileName, ('A' + aDrive), aFlags, &aPath);
	CUri16* prv_uri16 = CUri16::CreatePrivateFileUriL(aFileName,aDrive,aFlags);
	CleanupStack::PushL(prv_uri16);
	error = DoFileUriComponentExtraction<TUriParser16, CUri16, TDesC16>(*prv_uri16, *fileUriScheme16, aPath, aExpectedFileName);
	CleanupStack::PopAndDestroy(prv_uri16);
	iTestHarness->EndTest(error);
	
	CleanupStack::PopAndDestroy(2, fileUriScheme8); //fileUriScheme8, fileUriScheme16 	
	}

void CFileUriTest::TestFileUriCreationWithOldAPIAndComponentExtractionL(const TDesC16& aFileName, const TDesC16& aPath, const TDesC16& aExpectedFileName) const
	{	
	//Testing File URI componet extraction created using CUri::NewLC()
	iTestHarness->StartTestL(_L("Test File Uri Component Extraction created using NewLC (8-bit)"));
	iTestHarness->LogIt(_L("FileName: %S; \n\t\tPath:%S"), &aFileName, &aPath);	
	TInt error = KErrNone;
	HBufC8* fileUriScheme8 = HBufC8::NewLC(4);
	TPtr8 fileUriScheme8Ptr = fileUriScheme8->Des();
	fileUriScheme8Ptr.Copy(KFileUriScheme8);
	HBufC* fileUriScheme16 = HBufC::NewLC(4);
	TPtr16 fileUriScheme16Ptr = fileUriScheme16->Des();
	fileUriScheme16Ptr.Copy(KFileUriScheme16);
	
	_LIT8(KFileUri8, "file://%S");
	HBufC8* path8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aPath);
	CleanupStack::PushL(path8);
	HBufC8 *fileUri8 = HBufC8::NewLC((path8->Length()+10));
	TPtr8 fileUri8Ptr = fileUri8->Des();
	fileUri8Ptr.AppendFormat(KFileUri8,path8);
	TUriParser8 urip8;
	urip8.Parse(*fileUri8);
	CUri8* _uri8 = CUri8::NewLC(urip8); 
	error = DoFileUriComponentExtraction<TUriParser8, CUri8, TDesC8>(*_uri8, *fileUriScheme8, *path8, aExpectedFileName);
	CleanupStack::PopAndDestroy(_uri8);
	iTestHarness->EndTest(error);

	iTestHarness->StartTestL(_L("Test File Uri Component Extraction created using NewLC (16-bit)"));
	iTestHarness->LogIt(_L("FileName: %S; \n\t\tFileStorageType:2; \n\t\tPath:%S"), &aFileName, &aPath);
	HBufC *fileUri16 = EscapeUtils::ConvertToUnicodeFromUtf8L(*fileUri8);
	CleanupStack::PushL(fileUri16);
	TUriParser16 urip16;
	urip16.Parse(*fileUri16);
	CUri16* _uri16 = CUri16::NewLC(urip16);  
	error = DoFileUriComponentExtraction<TUriParser16, CUri16, TDesC16>(*_uri16, *fileUriScheme16, aPath, aExpectedFileName);
	CleanupStack::PopAndDestroy(_uri16);
	iTestHarness->EndTest(error);
	
	CleanupStack::PopAndDestroy(5, fileUriScheme8); //fileUriScheme8, fileUriScheme16, path8, fileUri8, fileUri16  	
	}
	
//
//
//	Implementation of LOCAL functions
//
//
template<class CUriType, class TDesCType>
TInt DoFileUriExtraction(const CUriType& aUri, const TDesCType& aExpectedFileUri)
	{
	TInt error = 0;
	
	// Check file URI...
	error = aUri.Uri().UriDes().Compare(aExpectedFileUri);
	if( error != 0 )
		{
		return error;
		}
	return KErrNone;
	}

template<class TUriParserType, class CUriType, class TDesCType, class TDesCType16>
TInt DoFileUriComponentExtraction(const CUriType& aUri, const TDesCType& aScheme, const TDesCType& aPath, const TDesCType16& aExpectedFileName)
	{
	TUriParserType uriParser;
	TInt error = uriParser.Parse(aUri.Uri().UriDes());
	// Is this a valid Uri?
	if( error == KUriUtilsErrInvalidUri )
		{
		return error;
		}

	// Check scheme...
	if( (error = TestComponent(uriParser, aScheme, EUriScheme)) != 0 )
		{
		return error;
		}
	
	// Check path...
	if( (error = TestComponent(uriParser, aPath, EUriPath)) != 0)
		{
		return error;
		}
	
	if (aExpectedFileName.Length() > 0)
		{
		HBufC* tmpFileName = uriParser.GetFileNameL();
		error = tmpFileName->Compare(aExpectedFileName);
		delete tmpFileName;
		if (error != 0)
			{
			return error;
			}
		}	
		
	return KErrNone;
	}
	
template<class TParserType, class TDesCType, class TComponentType>
TInt TestComponent(const TParserType& aParser, const TDesCType& aExpected, TComponentType aComponent)
	{
	return aParser.Extract(aComponent).Compare(aExpected);
	}
	

void CFileUriTest::TestFileUriNameExtractionL(const TDesC& aUri, const TDesC& aFilename, const TDesC& aPath)
	{
	TUriParser uriParser;
	User::LeaveIfError(uriParser.Parse(aUri));
	
	HBufC* name;
	TInt error = KErrNone;
	
	iTestHarness->StartTestL(_L("Test extract file name from URI (16 bit)"));
	iTestHarness->LogIt(_L("URI: %S; \n\t\tExpected name:%S"), &uriParser.UriDes(), &aFilename);

	name = uriParser.GetFileNameL(EUriFileNameTail);
	iTestHarness->LogIt(_L("Computed name:%S"), name);
	error = (aFilename.Compare(*name)==0) ? KErrNone : KErrBadName;
	delete name;
	
	iTestHarness->EndTest(error);

	iTestHarness->StartTestL(_L("Test extract file path from URI (16 bit)"));
	iTestHarness->LogIt(_L("URI: %S; \n\t\tExpected Path:%S"), &uriParser.UriDes(), &aPath);

	name = uriParser.GetFileNameL(EUriFileNamePath);
	iTestHarness->LogIt(_L("Computed Path:%S"), name);
	error = (aPath.Compare(*name)==0) ? KErrNone : KErrBadName;
	delete name;
	
	iTestHarness->EndTest(error);
	}
	
void CFileUriTest::TestFileUriNameExtractionL(const TDesC8& aUri, const TDesC& aFilename, const TDesC& aPath)
	{
	TUriParser8 uriParser;
	User::LeaveIfError(uriParser.Parse(aUri));
	
	HBufC* name;
	TInt error = KErrNone;

	TFileName uri;
	uri.Copy(uriParser.UriDes()); // safe for all valid URIs
	iTestHarness->StartTestL(_L("Test extract file name from URI (8 bit)"));
	iTestHarness->LogIt(_L("URI: %S; \n\t\tExpected name:%S"), &uri, &aFilename);

	name = uriParser.GetFileNameL(EUriFileNameTail);
	iTestHarness->LogIt(_L("Computed name:%S"), name);
	error = (aFilename.Compare(*name)==0) ? KErrNone : KErrBadName;
	delete name;
	
	iTestHarness->EndTest(error);

	iTestHarness->StartTestL(_L("Test extract file path from URI (8 bit)"));
	iTestHarness->LogIt(_L("URI: %S; \n\t\tExpected Path:%S"), &uri, &aPath);

	name = uriParser.GetFileNameL(EUriFileNamePath);
	iTestHarness->LogIt(_L("Computed Path:%S"), name);
	error = (aPath.Compare(*name)==0) ? KErrNone : KErrBadName;
	delete name;
	
	iTestHarness->EndTest(error);
	}
	
