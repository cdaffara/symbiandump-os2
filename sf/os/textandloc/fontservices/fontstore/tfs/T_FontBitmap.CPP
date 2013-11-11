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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <s32file.h>
#include <gdi.h>
#include <bautils.h>

#include "GraphicsTestUtils.h"
#include "FNTBODY_OLD.H"
#include "T_FontBitmap.H"


#ifdef __WINS__
_LIT(KEonFontFileName,"z:\\resource\\fonts\\eon14.gdr");
#else
_LIT(KEonFontFileName,"z:\\resource\\fonts\\eon.gdr");
#endif

_LIT(KTSectionFontRAM,"c:\\resource\\fonts\\TSection.gdr");
_LIT(KTSectionFontROM,"z:\\test\\TSection.gdr");


class CTFontBitmap : public CTGraphicsBase
	{
public:
	CTFontBitmap(CTestStep* aStep);
	~CTFontBitmap();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();

	void ConnectFileSysL ();
	void Test();
private:
	void TestAllL();

	CFontBitmap* CreateFontNewLC(CFontStoreFile* aFontStoreFile, RReadStream &aStream);
	CFontBitmapOld* CreateFontOldLC(CFontStoreFile* aFontStoreFile, RReadStream &aStream);

private:
	RHeap* iHeapNew;
	RHeap* iHeapOld;
	RFs iFilesys;
	};

LOCAL_C void DestroyFontNew(TAny* aPtr)
	{
	CFontBitmap* font = reinterpret_cast <CFontBitmap*> (aPtr);
	font->Release();
	font->Release();
	}

LOCAL_C void DestroyFontOld(TAny* aPtr)
	{
	CFontBitmapOld* font = reinterpret_cast <CFontBitmapOld*> (aPtr);
	font->Release();
	font->Release();
	}

TBool operator==(const TBitmapFontCharacterMetrics& l, const TBitmapFontCharacterMetrics& r)
	{
	return (l.iAscentInPixels == r.iAscentInPixels &&
			l.iHeightInPixels == r.iHeightInPixels &&
			l.iLeftAdjustInPixels == r.iLeftAdjustInPixels &&
			l.iMoveInPixels == r.iMoveInPixels &&
			l.iRightAdjustInPixels == r.iRightAdjustInPixels);
	}

CTFontBitmap::CTFontBitmap(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iHeapNew(NULL),
	iHeapOld(NULL)
	{
	}

CTFontBitmap::~CTFontBitmap()
	{
	iFilesys.Close();
	iHeapOld->Close();
	iHeapNew->Close();
	}

void CTFontBitmap::ConstructL()
	{
	iHeapNew = UserHeap::ChunkHeap(NULL,0x80000, 0x80000);
	iHeapOld = UserHeap::ChunkHeap(NULL,0x80000, 0x80000);

	ConnectFileSysL();
	}

void CTFontBitmap::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFontBitmapStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTFontBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-0028"));
		Test();
		break;	
	case 2:
        	((CTFontBitmapStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFontBitmapStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;	
		}
	((CTFontBitmapStep*)iStep)->RecordTestResultL();
	}
	
void CTFontBitmap::ConnectFileSysL ()
	{
	User::LeaveIfError(iFilesys.Connect());
	}

void CTFontBitmap::Test()
	{
	INFO_PRINTF1(_L("All bitmap fonts"));

	iHeapNew->__DbgMarkStart();
	iHeapOld->__DbgMarkStart();
	TRAPD(err, TestAllL());
	TEST(err == KErrNone);
	iHeapNew->__DbgMarkCheck(ETrue, 0, TPtrC8((TText8*)__FILE__), __LINE__);
	iHeapNew->__DbgMarkEnd(0);
	iHeapOld->__DbgMarkCheck(ETrue, 0, TPtrC8((TText8*)__FILE__), __LINE__);
	iHeapOld->__DbgMarkEnd(0);

	}

/**
	@SYMTestCaseID
	GRAPHICS-FNTSTORE-0028

	@SYMTestCaseDesc
	Tests all bitmap fonts. 

	@SYMTestActions
	1. Creates a CFontStoreFile object from a file placed in ROM.
	2. Copies the file from the Z-drive to the C-drive.
	3. Tries to load the file from the C-drive to create a CFontStoreFile object.
	4. Opens two read streams. One for the old file and one for the new file.
	5. Retrieves the sizes of the files and checks they are the same.
	6. For both files it tries to create a font and allocates memory for them.
	7. Character metrics are compared between the two files.	
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFontBitmap::TestAllL()
	{
	TParse fileNameParse;

	User::LeaveIfError(iFilesys.Parse(KEonFontFileName, fileNameParse));
	CFontStoreFile* fontstorefile_new = CFontStoreFile::NewL(fileNameParse, iFilesys);
	CleanupStack::PushL(fontstorefile_new);
	CFontStoreFile* fontstorefile_old = CFontStoreFile::NewL(fileNameParse, iFilesys);
	CleanupStack::PushL(fontstorefile_old);

	// copy file from z: directory into c:
	TEST( BaflUtils::FileExists(iFilesys, KTSectionFontROM) );	// placed here on WINS by fonts_c.mk or fntstoretest.iby on lubbock

	RSmlTestUtils testUtil;
	CleanupClosePushL(testUtil);
	testUtil.Connect();
	testUtil.CreateDirectoryL(KTSectionFontRAM);
	testUtil.CopyFileL(KTSectionFontROM, KTSectionFontRAM);
	CleanupStack::PopAndDestroy(&testUtil);

	TEST( BaflUtils::FileExists(iFilesys, KTSectionFontRAM) );

	// try to load the from from C: drive
	User::LeaveIfError(iFilesys.Parse(KTSectionFontRAM, fileNameParse));
	CFontStoreFile* fontstorefile_C = CFontStoreFile::NewL(fileNameParse, iFilesys);
	CleanupStack::PushL(fontstorefile_C);
	
	RStoreReadStream stream_new;
	stream_new.OpenLC(*fontstorefile_new->iFileStore, fontstorefile_new->iDataStreamId);
	RStoreReadStream stream_old;
	stream_old.OpenLC(*fontstorefile_old->iFileStore, fontstorefile_old->iDataStreamId);
	TInt i;
	TInt size_new = stream_new.ReadInt32L();
	TInt size_old = stream_old.ReadInt32L();
	TEST(size_new == size_old);
	INFO_PRINTF1(_L(""));
	for (i=0; i<size_new; i++)
		{
		CFontBitmap* font_new = CreateFontNewLC(fontstorefile_new, stream_new);
		CFontBitmapOld* font_old = CreateFontOldLC(fontstorefile_old, stream_old);
		INFO_PRINTF3(_L("Font UID=%X, NumCodeSections=%d\r\n"), font_new->iUid.iUid, font_new->iNumCodeSections);
		TInt alloc_size_new = 0;
		TInt cells_count_new = iHeapNew->AllocSize(alloc_size_new);
		TInt alloc_size_old = 0;
		TInt cells_count_old = iHeapOld->AllocSize(alloc_size_old);
		INFO_PRINTF5(_L("CellsCntNew=%d, AllocSizeNew=%d  CellsCntOld=%d, AllocSizeOld=%d\r\n"), cells_count_new, alloc_size_new, cells_count_old, alloc_size_old);
		INFO_PRINTF2(_L("Profit=%d bytes\r\n"), alloc_size_old - alloc_size_new);
		for(TInt j=0;j<256;j++)
			{
            INFO_PRINTF2(_L("Char code %d"),j);
			const TUint8* bytes_new = NULL;
			const TUint8* bytes_old = NULL;
			TBitmapFontCharacterMetrics metrics_new;
			TBitmapFontCharacterMetrics metrics_old;
			metrics_new = font_new->CharacterMetrics(j, bytes_new);
			metrics_old = font_old->CharacterMetrics(j, bytes_old);
			TEST(bytes_new ? !!bytes_old : !bytes_old);
			TEST(metrics_new == metrics_old);
			if(bytes_new)
				{
				TInt byte_size_new = metrics_new.iMoveInPixels - metrics_new.iLeftAdjustInPixels - metrics_new.iRightAdjustInPixels;
				if(byte_size_new < 8)
					byte_size_new = 8;
				byte_size_new /= 8;
				byte_size_new *= metrics_new.iHeightInPixels;
				TInt byte_size_old = metrics_old.iMoveInPixels - metrics_old.iLeftAdjustInPixels - metrics_old.iRightAdjustInPixels;
				if(byte_size_old < 8)
					byte_size_old = 8;
				byte_size_old /= 8;
				byte_size_old *= metrics_old.iHeightInPixels;
				if(byte_size_new > 0)
					{
					TPtrC8 data_new(bytes_new, byte_size_new);
					TPtrC8 data_old(bytes_old, byte_size_old);
					TInt res = data_new.Compare(data_old);
					TEST(res == 0);
					}
				}
			}
		CleanupStack::PopAndDestroy(2);  //font_new & font_old
		}
	CleanupStack::PopAndDestroy(5);  //fontstorefile & stream - old & new + fontstorefile_C
	}

const TInt KOldFontVer = 41;

CFontBitmap* CTFontBitmap::CreateFontNewLC(CFontStoreFile* aFontStoreFile, RReadStream &aStream)
	{
	CFontBitmap* fontbitmap = (CFontBitmap*)iHeapNew->AllocL(sizeof(CFontBitmap));
	new (fontbitmap) CFontBitmap(iHeapNew, aFontStoreFile);
	TRAPD(ret, fontbitmap->InternalizeL(aStream, KOldFontVer));
	if(ret != KErrNone)
		{
		fontbitmap->Release();
		User::Leave(ret);
		}
	CleanupStack::PushL(TCleanupItem(DestroyFontNew, fontbitmap));
	fontbitmap->UseL();
	return fontbitmap;
	}

CFontBitmapOld* CTFontBitmap::CreateFontOldLC(CFontStoreFile* aFontStoreFile, RReadStream &aStream)
	{
	CFontBitmapOld* fontbitmap = (CFontBitmapOld*)iHeapOld->AllocL(sizeof(CFontBitmapOld));
	new (fontbitmap) CFontBitmapOld(iHeapOld, aFontStoreFile);
	TRAPD(ret, fontbitmap->InternalizeL(aStream));
	if(ret != KErrNone)
		{
		fontbitmap->Release();
		User::Leave(ret);
		}
	CleanupStack::PushL(TCleanupItem(DestroyFontOld, fontbitmap));
	fontbitmap->UseL();
	return fontbitmap;
	}

//--------------
__CONSTRUCT_STEP__(FontBitmap)

