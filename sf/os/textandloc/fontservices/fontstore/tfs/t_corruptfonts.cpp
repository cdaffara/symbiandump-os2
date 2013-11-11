/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#include "t_corruptfonts.h"



class CTCorruptFontRejection : public CTGraphicsBase
	{
public:
	CTCorruptFontRejection(CTestStep* aStep);
	~CTCorruptFontRejection();
protected:
	// from CTGraphicsBase
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL(void);
private:
	void TestCorruptFonts(TInt aExpectedError, TInt aFilesWithExpectedError, TInt aExpectedNotSupported=0);
	void CopyCorruptFontsL(const TDesC& aFontSourceDir, const TDesC& aFontExt);
private:
	CFontStore*	iFontStore;
	RHeap*		iHeap;
	RFs			iFs;
	CFileMan* 	iFileMan;
	RPointerArray<HBufC> iFontFileList;
	};


_LIT(KCorruptGdrDirROM, "z:\\PlatTest\\Graphics\\TestData\\corrupt_gdr_fonts\\");
_LIT(KCorruptTtfDirROM, "z:\\PlatTest\\Graphics\\TestData\\corrupt_ttf_fonts\\");

_LIT(KCorruptFontDirRAM, "c:\\PlatTest\\Graphics\\TestData\\");

_LIT(KBitmapFontExt, ".gdr");
_LIT(KTrueTypeFontExt, ".ttf");
_LIT(KOpenTypeFontExt, ".otf");
_LIT(KCccFontExt, ".ccc");

const TInt KNumGdrTestFiles = 1;
const TInt KNumTtfTestFiles = 26;
// number of fonts that should be found corrupt when given the .ccc extension
const TInt KNumCccCorruptFiles = 24;



CTCorruptFontRejection::CTCorruptFontRejection(CTestStep* aStep) : CTGraphicsBase(aStep), iHeap(NULL)
	{
	}


CTCorruptFontRejection::~CTCorruptFontRejection()
	{
	while (iFontFileList.Count())
		{
		// delete corrupt file from C drive
		(void) iFileMan->Delete(*iFontFileList[0]);
		delete iFontFileList[0];
		iFontFileList.Remove(0);
		}
	delete iFontStore;
	iFontFileList.Close();
	delete iFileMan;
	iFs.Close();
	iHeap->__DbgMarkEnd(0);
	iHeap->Close();
	User::Heap().Check();
	REComSession::FinalClose();
	}


void CTCorruptFontRejection::ConstructL()
	{
	iHeap = UserHeap::ChunkHeap(NULL,0x10000,0x10000);
	if (NULL == iHeap)
		User::Leave(KErrNoMemory);
	iHeap->__DbgMarkStart();

	iFontStore = CFontStore::NewL(iHeap);

	User::LeaveIfError(iFs.Connect());
	TInt err = iFs.MkDirAll(KCorruptFontDirRAM);
	if ( (err != KErrNone) && (err != KErrAlreadyExists) )
		{
		User::Leave(err);
		}
	iFileMan = CFileMan::NewL(iFs);
	}


void CTCorruptFontRejection::RunTestCaseL(TInt aCurTestCase)
	{
	_LIT(KCopyCorruptGdrFonts, "Copy corrupt GDR fonts");
	_LIT(KTestCorruptGdrFonts, "Test corrupt GDR fonts");
	_LIT(KCopyCorruptTrueTypeFonts, "Copy corrupt TrueType fonts");
	_LIT(KTestCorruptTrueTypeFonts, "Test corrupt TrueType fonts");
	TInt copyError = KErrNone;
	((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch(aCurTestCase)
		{
		case 1:
			((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0022"));
			INFO_PRINTF1(KCopyCorruptGdrFonts);
			TRAP(copyError, CopyCorruptFontsL(KCorruptGdrDirROM, KBitmapFontExt));
			TEST(copyError == KErrNone);
			TEST(iFontFileList.Count() == KNumGdrTestFiles);
			break;

		case 2:
			((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0021"));
			INFO_PRINTF1(KTestCorruptGdrFonts);
			// expect KErrNotFound error for missing parts of each font file
			TestCorruptFonts(KErrNotFound, KNumGdrTestFiles);
			TEST(iFontFileList.Count() == 0);
			break;

		case 3: // test .ttf file extension is checked
			((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0022"));
			INFO_PRINTF1(KCopyCorruptTrueTypeFonts);
			TRAP(copyError, CopyCorruptFontsL(KCorruptTtfDirROM, KTrueTypeFontExt));
			TEST(copyError == KErrNone);
			TEST(iFontFileList.Count() == KNumTtfTestFiles);
			break;

		case 4:
			((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0021"));
			INFO_PRINTF1(KTestCorruptTrueTypeFonts);
			// expect KErrCorrupt for each font
			TestCorruptFonts(KErrCorrupt, KNumTtfTestFiles);
			TEST(iFontFileList.Count() == 0);
			break;

		case 5: // should also be checked if extension copied to is .otf
			((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0022"));
			INFO_PRINTF1(KCopyCorruptTrueTypeFonts);
			TRAP(copyError, CopyCorruptFontsL(KCorruptTtfDirROM, KOpenTypeFontExt));
			TEST(copyError == KErrNone);
			TEST(iFontFileList.Count() == KNumTtfTestFiles);
			break;

		case 6:
			((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0021"));
			INFO_PRINTF1(KTestCorruptTrueTypeFonts);
			// expect KErrCorrupt for each font
			TestCorruptFonts(KErrCorrupt, KNumTtfTestFiles);
			TEST(iFontFileList.Count() == 0);
			break;

		case 7: // .ccc files are *almost* in the TrueType format too
			((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0022"));
			INFO_PRINTF1(KCopyCorruptTrueTypeFonts);
			TRAP(copyError, CopyCorruptFontsL(KCorruptTtfDirROM, KCccFontExt));
			TEST(copyError == KErrNone);
			TEST(iFontFileList.Count() == KNumTtfTestFiles);
			break;

		case 8:
			((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0021"));
			INFO_PRINTF1(KTestCorruptTrueTypeFonts);
			// expect KErrCorrupt for <KNumCccCorruptFiles> fonts
			// allow the rest to give KErrNotSupported errors (because there is no rasterizer to load them)
			TestCorruptFonts(KErrCorrupt, KNumCccCorruptFiles, KNumTtfTestFiles - KNumCccCorruptFiles);
			TEST(iFontFileList.Count() == 0);
			break;

		case 9:
            		((CTCorruptFontRejectionStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTCorruptFontRejectionStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTCorruptFontRejectionStep*)iStep)->RecordTestResultL();
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0021

	@SYMTestCaseDesc
	Try loading the corrupt font and check the error returned.
	
	@SYMTestActions
	1. Finds or creates a font file object to support a font file. If an 
	   appropriate font file object exists then no new open font file is created. 
	   In this case the reference count of the font file object is incremented.
	2. Checks the error code returned.
	3. Deletes the file if an error was found.
	4. Checks for not supported files.
	5. Deletes corrupt files from the C-drive.
	6. Checks number of retrieved test file errors.

	@SYMTestExpectedResults
	Corrupt files should give errors.
*/
/**
	@param aExpectedError the expected error code in this test set
	@param aFilesWithExpectedError number of font files expected with this error
	@param aExpectedNotSupported number of font files expected to get KErrNotSupported instead	
*/
void CTCorruptFontRejection::TestCorruptFonts(TInt aExpectedError, TInt aFilesWithExpectedError, TInt aExpectedNotSupported)
	{
	iHeap->__DbgMarkStart();

	TInt addFileError = KErrNone;
	TInt deleteFileError;
	TUid id = KNullUid;
	TInt notSupportedCount = 0;
	TInt errorCount = 0;

	// each font is deleted after it is tested
	while (iFontFileList.Count())
		{
		// try adding the corrupt font file
		TRAP(addFileError, id = iFontStore->AddFileL(*iFontFileList[0]));

		INFO_PRINTF3(_L("Adding corrupt font '%S' gave error %d"), iFontFileList[0], addFileError);

		if (addFileError != KErrNone)
			{
			iFontStore->RemoveFile(id);
			}

		// .ccc file extension test expects some files to fail with KErrNotSupported
		if ( (addFileError == KErrNotSupported) && (notSupportedCount < aExpectedNotSupported) )
			{
			++notSupportedCount;
			}
		else
			{ // should fail
			TEST(addFileError == aExpectedError);
			++errorCount;
			}
		
		// delete corrupt font file from C drive
		deleteFileError = iFileMan->Delete(*iFontFileList[0]);
		if (deleteFileError != KErrNone)
			{
			INFO_PRINTF3(_L("Deleting corrupt font '%S' gave error %d"), iFontFileList[0], deleteFileError);
			}
		
		delete iFontFileList[0];
		iFontFileList.Remove(0);
		}

	INFO_PRINTF4(_L("%d font files had error %d (expected %d)"), errorCount, aExpectedError, aFilesWithExpectedError);
	if ( (notSupportedCount != 0) || (aExpectedNotSupported != 0) )
		{
		INFO_PRINTF3(_L("%d font files had KErrNotSupported (expected %d)"), notSupportedCount, aExpectedNotSupported);
		}

	// correct number of errors found?
	TEST(errorCount == aFilesWithExpectedError);
	TEST(notSupportedCount == aExpectedNotSupported);

	iHeap->__DbgMarkEnd(0);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0022

	@SYMTestCaseDesc
	Tests copying corrupt font files.

	@SYMTestActions
	1. Searches for font files in a directory and puts them in a list.
	2. Copies the files one by one to a directory on the C-drive.
	3. Checks if copying the files succeeded.
	4. Stores the copied files in an array.
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTCorruptFontRejection::CopyCorruptFontsL(const TDesC& aFontSourceDir, const TDesC& aFontExt)
	{
	TFindFile fileFinder(iFs);
	TParse source;
	TParse dest;
	CDir* foundFileList = NULL;

	_LIT(KFBSERVFontFilePattern, "*");
	TInt findFileComplete = fileFinder.FindWildByPath(KFBSERVFontFilePattern, &aFontSourceDir, foundFileList);

	if (findFileComplete)
		{
		INFO_PRINTF3(_L("Found no %S font test files to copy from %S"), &aFontExt, &aFontSourceDir);
		}
	else
		{
		CleanupStack::PushL(foundFileList);

		const TInt foundFileCount = foundFileList->Count();

		INFO_PRINTF4(_L("Found %d test file(s) to copy as %S font(s) from %S"), foundFileCount, &aFontExt, &aFontSourceDir);

		for (TInt i = 0; i < foundFileCount; i++)
			{
			User::LeaveIfError(source.Set((*foundFileList)[i].iName, &fileFinder.File(), NULL));
			User::LeaveIfError(dest.Set((*foundFileList)[i].iName, &KCorruptFontDirRAM(), &aFontExt));

			// copy to C drive
			const TInt copyError = iFileMan->Copy(source.FullName(), dest.FullName());
			if (copyError != KErrNone)
				{
				INFO_PRINTF4(_L("File copy failed (error %d), from '%S' to '%S'"), 
					copyError, &source.FullName(), &dest.FullName());

				User::Leave(copyError);
				}

			// Clear the read only flag so the files can be deleted later
			const TInt attribError = iFileMan->Attribs(dest.FullName(), 0, KEntryAttReadOnly, TTime(0));
			if(attribError != KErrNone)
				{
				INFO_PRINTF3(_L("Making corrupt font '%S' deletable gave error %d"), &dest.FullName(), attribError);
				User::Leave(attribError);
				}	

			// store file name in test list
			TPtrC fontFilename = dest.FullName();
			HBufC* data = fontFilename.AllocL();
			CleanupStack::PushL(data);
			iFontFileList.AppendL(data);
			CleanupStack::Pop(data);
			}

		CleanupStack::PopAndDestroy(foundFileList);
		}
	}



//--------------
__CONSTRUCT_STEP__(CorruptFontRejection)
