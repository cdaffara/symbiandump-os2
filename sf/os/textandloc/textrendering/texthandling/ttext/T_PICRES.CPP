/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32base.h>

#include <gdi.h>
#include <conpics.h>
#include <s32file.h>

#include <txtrich.h>
#include <txtfmlyr.h>
#include "TXTMRTSR.H"

#include "../incp/T_PMLPAR.H"
#include "T_PICRES.h"

LOCAL_D CTestStep *pTestStep = NULL;
#define test(cond)											\
	{														\
	TBool __bb = (cond);									\
	pTestStep->TEST(__bb);									\
	if (!__bb)												\
		{													\
		pTestStep->ERR_PRINTF1(_L("ERROR: Test Failed"));	\
		User::Leave(1);										\
		}													\
	}
#undef INFO_PRINTF1
#undef INFO_PRINTF2
// copy from tefexportconst.h
#define INFO_PRINTF1(p1)        pTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1))
#define INFO_PRINTF2(p1, p2)    pTestStep->Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2))


#define UNUSED_VAR(a) a = a

const TInt KTestCleanupStack=0x40;

class CContainer : public CBase, public MRichTextStoreResolver
	{
public:
	static CContainer* NewL();
	~CContainer();
	//
	// Mixin
	//
	virtual const CStreamStore& StreamStoreL(TInt aPos)const;
	// Methods
	TStreamId StoreL(CStreamStore& aStore)const;
	void RestoreL(const CStreamStore& aStore,TStreamId aId,MPictureFactory* aFctry);
protected:
	CContainer();
	void ConstructL();
public:
	CRichText* iText;
	const CParaFormatLayer* iGlobalParaFormatLayer;
	const CCharFormatLayer* iGlobalCharFormatLayer;
	};

LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D RFs TheFs;  // the file server
LOCAL_D RFile TheFile;  // the data file
LOCAL_D CFileStore* TheStore;  // concrete CStreamStore
LOCAL_D CParser* TheParser;
LOCAL_D CContainer* TheContainer;
LOCAL_D CStreamStore* TheDeferredPictureStore;

CContainer* CContainer::NewL()
// Create new container & set its components.
//
	{
	CContainer* self=new(ELeave) CContainer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


CContainer::CContainer()
	{
	}


void CContainer::ConstructL()
	{
	TFileName theFileName = _L("z:\\test\\app-framework\\etext\\t_picres.pml");
	TheParser=CParser::NewL();
	CleanupStack::PushL(TheParser);
	iText=TheParser->ParseL(theFileName);
	CleanupStack::PopAndDestroy();
	iGlobalParaFormatLayer=iText->GlobalParaFormatLayer();
	iGlobalCharFormatLayer=iText->GlobalCharFormatLayer();
	}


CContainer::~CContainer()
	{
	delete iText;
	delete (CParaFormatLayer*)iGlobalParaFormatLayer;
	delete (CCharFormatLayer*)iGlobalCharFormatLayer;
	}


const CStreamStore& CContainer::StreamStoreL(TInt /*aPos*/)const
// Return the deferred picture store.
// In this instance, the deferred picture store does not vary with document position.
//
	{return *TheDeferredPictureStore;}


TStreamId CContainer::StoreL(CStreamStore& aStore)const
// Store this component
//
	{
	CStoreMap* map=CStoreMap::NewLC(aStore);
	iText->StoreComponentsL(aStore,*map);
	//
	RStoreWriteStream stream(*map);
	TStreamId id=stream.CreateLC(aStore);
	iGlobalParaFormatLayer->ExternalizeL(stream);
	iGlobalCharFormatLayer->ExternalizeL(stream);
	stream<< *iText;
	stream.CommitL();
	//
	map->Reset();
	CleanupStack::PopAndDestroy(2);
	return id;
	}


void CContainer::RestoreL(const CStreamStore& aStore,TStreamId aId,MPictureFactory* aFactory)
// Restore this component
//
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aId);
	iGlobalParaFormatLayer=CParaFormatLayer::NewL(stream);
	iGlobalCharFormatLayer=CCharFormatLayer::NewL(stream);
	iText=CRichText::NewL(iGlobalParaFormatLayer,iGlobalCharFormatLayer);
	iText->SetPictureFactory(aFactory,this);
	stream>> *iText;
	//
	CleanupStack::PopAndDestroy();
	//
	iText->RestoreComponentsL(aStore);
	}

_LIT(KOutputFile, "c:\\etext\\t_word.doc");
LOCAL_C void testPictureRestorer(TBool aDeferPictureLoad=ETrue)
// Test Picture persistance.
//
    {
	//
	TheFs.Connect();
	//
	TheFs.Delete(KOutputFile);
	TheFs.MkDirAll(KOutputFile);
	TheStore=CDirectFileStore::CreateL(TheFs,KOutputFile,EFileRead|EFileWrite);
	TheDeferredPictureStore=TheStore;
	CleanupStack::PushL(TheStore);
	TheStore->SetTypeL(KDirectFileStoreLayoutUid);
	//
	// Create concrete picture factory.
	MDemPictureFactory* factory=new(ELeave) MDemPictureFactory;

	TheContainer->iText->Reset();
	TheContainer->iText->InsertL(0,_L("Hello Duncan how"));

	TheContainer->iText->SetPictureFactory(factory,TheContainer);
	// Create some pictures.
	CXzePicture* pic1=CXzePicture::NewL('x');
		CleanupStack::PushL(pic1);
	CXzePicture* pic2=CXzePicture::NewL('z');
		CleanupStack::PushL(pic2);
	CXzePicture* pic3=CXzePicture::NewL('e');
		CleanupStack::PushL(pic3);
	//
	// Create the picture headers
	TPictureHeader hdr1;
	TPictureHeader hdr2;
	TPictureHeader hdr3;
	//
	TSize size;
	pic1->GetSizeInTwips(size);
	hdr1.iPictureType=KUidXzePictureType;
	hdr1.iPicture=pic1;
	hdr2.iPictureType=KUidXzePictureType;
	hdr2.iPicture=pic2;
	hdr3.iPictureType=KUidXzePictureType;
	hdr3.iPicture=pic3;
	//
	// Insert the pictures into the rich text
	TBool hasMarkupData=TheContainer->iText->HasMarkupData();
	test(!hasMarkupData);
	TheContainer->iText->CancelInsertCharFormat();
	TheContainer->iText->InsertL(0,hdr1);
	TheContainer->iText->InsertL(5,hdr2);
	TheContainer->iText->InsertL(7,hdr3);
	TheContainer->iText->InsertL(0,CEditableText::EParagraphDelimiter);
	TheContainer->iText->CancelInsertCharFormat();
	TheContainer->iText->InsertL(2,CEditableText::EParagraphDelimiter);
	CleanupStack::Pop(3);
	hasMarkupData=TheContainer->iText->HasMarkupData();
	test(hasMarkupData);
	//
	// High level Store context
	TStreamId id=TheContainer->StoreL(*TheStore);
//
	delete TheContainer->iText;
	delete (CParaFormatLayer*)TheContainer->iGlobalParaFormatLayer;
	delete (CCharFormatLayer*)TheContainer->iGlobalCharFormatLayer;
//
//
//	Now restore the container with rich text
	TheContainer->RestoreL(*TheStore,id,factory);
	if (!aDeferPictureLoad)
		TheContainer->iText->DetachFromStoreL(CPicture::EDetachFull);
	//
	hasMarkupData=TheContainer->iText->HasMarkupData();
	test(hasMarkupData);
	test(TheContainer->iText->ParagraphCount()==3);
	test(TheContainer->iText->DocumentLength()==21);
	TPtrC view;
	TCharFormat format;
	CPicture* picture;
	//
	// TEST THE PICTURE HEADERS, DEPENDING ON WHETHER DEFERRED LOADING IS SET OR NOT
	TPictureHeader hdrA=TheContainer->iText->PictureHeader(1);
	test(hdrA.iPictureType==KUidXzePictureType);
	if (aDeferPictureLoad)
		{
		test(hdrA.iPicture.IsId());
		}
	else
		{
		test(hdrA.iPicture!=NULL);
		test(hdrA.iPicture.IsPtr());
		test(((CXzePicture*)hdrA.iPicture.AsPtr())->iLabel=='x');
		}
	TPictureHeader hdrB=TheContainer->iText->PictureHeader(7);
	test(hdrB.iPictureType==KUidXzePictureType);
	if (aDeferPictureLoad)
		{
		test(hdrB.iPicture.IsId());
		}
	else
		{
		test(hdrB.iPicture!=NULL);
		test(hdrB.iPicture.IsPtr());
		test(((CXzePicture*)hdrB.iPicture.AsPtr())->iLabel=='z');
		}
	TPictureHeader hdrC=TheContainer->iText->PictureHeader(9);
	test(hdrC.iPictureType==KUidXzePictureType);
	if (aDeferPictureLoad)
		{
		test(hdrC.iPicture.IsId());
		}
	else
		{
		test(hdrC.iPicture!=NULL);
		test(hdrC.iPicture.IsPtr());
		test(((CXzePicture*)hdrC.iPicture.AsPtr())->iLabel=='e');
		}
	TPictureHeader hdrD=TheContainer->iText->PictureHeader(0);  // This is not a picture character
	test(hdrD.iPictureType==KNullUid);
	test(hdrD.iPicture==NULL);
	TSize dummySize;
	test(hdrD.iSize==dummySize);
	//
	TheContainer->iText->GetChars(view,format,1);
	test(view[0]==CEditableText::EPictureCharacter);
	picture=TheContainer->iText->PictureHandleL(1);
	test(((CXzePicture*)picture)->iLabel=='x');
	
	TheContainer->iText->GetChars(view,format,7);
	test(view[0]==CEditableText::EPictureCharacter);
	picture=TheContainer->iText->PictureHandleL(7);
	test(((CXzePicture*)picture)->iLabel=='z');
	
	TheContainer->iText->GetChars(view,format,9);
	test(view[0]==CEditableText::EPictureCharacter);
	picture=TheContainer->iText->PictureHandleL(9);
	test(((CXzePicture*)picture)->iLabel=='e');

	delete factory;
	CleanupStack::PopAndDestroy();  // TheStore
	TheFs.Close();
    }

_LIT(KOutputFile1, "c:\\etext\\t_word1.doc");
LOCAL_C void testPictureRestorer2(TBool aAlwaysFailToLoad=EFalse)
// Test Picture persistance.
//
    {
	//
	TheFs.Connect();
	//
	TheFs.Delete(KOutputFile1);
	TheFs.MkDirAll(KOutputFile1);
	TheStore=CDirectFileStore::CreateL(TheFs,KOutputFile1,EFileRead|EFileWrite);
	TheDeferredPictureStore=TheStore;
	CleanupStack::PushL(TheStore);
	TheStore->SetTypeL(KDirectFileStoreLayoutUid);
	//
	// Create concrete picture factory.
	MDemPictureFactory* factory=new(ELeave) MDemPictureFactory;

	TheContainer->iText->Reset();
	TheContainer->iText->InsertL(0,_L("Hello Duncan how"));

	TheContainer->iText->SetPictureFactory(factory,TheContainer);
	// Create some pictures.
	CXzeDoor* pic1=CXzeDoor::NewL('x',aAlwaysFailToLoad);
		CleanupStack::PushL(pic1);
	CXzeDoor* pic2=CXzeDoor::NewL('z',aAlwaysFailToLoad);
		CleanupStack::PushL(pic2);
	CXzePicture* pic3=CXzePicture::NewL('e');  // Control: will always load.
		CleanupStack::PushL(pic3);
	//
	// Create the picture headers
	TPictureHeader hdr1;
	TPictureHeader hdr2;
	TPictureHeader hdr3;
	//
	TSize size;
	pic1->GetSizeInTwips(size);
	hdr1.iPictureType=KUidXzeDoorType;
	hdr1.iPicture=pic1;
	hdr2.iPictureType=KUidXzeDoorType;
	hdr2.iPicture=pic2;
	hdr3.iPictureType=KUidXzePictureType;
	hdr3.iPicture=pic3;
	//
	// Insert the pictures into the rich text
	TBool hasMarkupData=TheContainer->iText->HasMarkupData();
	test(!hasMarkupData);
	TheContainer->iText->CancelInsertCharFormat();
	TheContainer->iText->InsertL(0,hdr1);
	TheContainer->iText->InsertL(5,hdr2);
	TheContainer->iText->InsertL(7,hdr3);
	TheContainer->iText->InsertL(0,CEditableText::EParagraphDelimiter);
	TheContainer->iText->CancelInsertCharFormat();
	TheContainer->iText->InsertL(2,CEditableText::EParagraphDelimiter);
	CleanupStack::Pop(3);  // pic1,2,3 - ownership transferred to rich text
	hasMarkupData=TheContainer->iText->HasMarkupData();
	test(hasMarkupData);
	//
	// High level Store context - all pictures currently in memory
	TStreamId id=TheContainer->StoreL(*TheStore);
//
	delete TheContainer->iText;
	delete (CParaFormatLayer*)TheContainer->iGlobalParaFormatLayer;
	delete (CCharFormatLayer*)TheContainer->iGlobalCharFormatLayer;
//
//
//	Now restore the container with rich text
	TheContainer->RestoreL(*TheStore,id,factory);
//
//
//  Now store the stuff again
	TRAPD(ret,
	TheContainer->iText->DetachFromStoreL(CPicture::EDetachFull));
	if (ret==KErrNotSupported)
	    INFO_PRINTF1(_L("   SIMULATION: Some picture data is not supported by the current factory."));
//	if (aAlwaysFailToLoad)
//		test(error==KErrNotFound);
//	else
//		test(error==KErrNone);
	id=KNullStreamId;
	TRAP(ret,
	id=TheContainer->StoreL(*TheStore));
	test(ret==KErrNone);
//
// ...and restore it to check what we have got.
	delete TheContainer->iText;
	delete (CParaFormatLayer*)TheContainer->iGlobalParaFormatLayer;
	delete (CCharFormatLayer*)TheContainer->iGlobalCharFormatLayer;
    TheContainer->RestoreL(*TheStore,id,factory);
	TInt pictureCount=TheContainer->iText->PictureCount();
	if (aAlwaysFailToLoad)
	    {
		test(pictureCount==1);
	    }
	else
	    {
		test(pictureCount==3);
	    }
//
	delete factory;
	CleanupStack::PopAndDestroy();  // TheStore
	TheFs.Close();
    }


LOCAL_C  void GoL()
// Run the tests
//
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-TTEXT-LEGACY-T_PICRES-0001 RichText Storing - with pictures deferred loading "));
	TheContainer=CContainer::NewL();
	TRAPD(r,
	testPictureRestorer());
	test(r==KErrNone);
	delete TheContainer;  // deletes the rich text, which deletes the contained pictures.
	//
	//
	INFO_PRINTF1(_L("RichText Storing - with pictures auto loading"));
	TheContainer=CContainer::NewL();
	TRAP(r,
	testPictureRestorer(EFalse));  // DO NOT DEFER PICTURE LOADING
	test(r==KErrNone);
	delete TheContainer;  // deletes the rich text, which deletes the contained pictures.
//
//
//
	INFO_PRINTF1(_L("Testing no missing picture app's"));
	TheContainer=CContainer::NewL();
	TRAP(r,
	testPictureRestorer2());
	test(r==KErrNone);
	delete TheContainer;  // deletes the rich text, which deletes the contained pictures.
	//
	//
/*
	TEST NOW REDUNDANT AS OF NEW PICTURE CONTAINMENT MECHANISM AS AT RELEASE 073

	INFO_PRINTF1(_L("Testing missing picture app's"));
	TheContainer=CContainer::NewL();
	TRAP(r,
	testPictureRestorer2(ETrue));  // ALWAYS FAIL TO DETACH FROM STORE
	test(r==KErrNone);
	delete TheContainer;  // deletes the rich text, which deletes the contained pictures.
*/
	}


LOCAL_C void setupCleanup()
//
// Initialise the cleanup stack.
//
    {

	TheTrapCleanup=CTrapCleanup::New();
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)1);\
		test(r==KErrNone);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	}


LOCAL_C void DeleteDataFile(const TDesC& aFullName)
	{
	RFs fsSession;
	TInt err = fsSession.Connect();
	if(err == KErrNone)
		{
		TEntry entry;
		if(fsSession.Entry(aFullName, entry) == KErrNone)
			{
			RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
			err = fsSession.SetAtt(aFullName, 0, KEntryAttReadOnly);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
				}
			err = fsSession.Delete(aFullName);
			if(err != KErrNone) 
				{
				RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
				}
			}
		fsSession.Close();
		}
	else
		{
		RDebug::Print(_L("Error %d connecting file session. File: %S.\n"), err, &aFullName);
		}
	}

CT_PICRES::CT_PICRES()
    {
    SetTestStepName(KTestStep_T_PICRES);
    pTestStep = this;
    }

TVerdict CT_PICRES::doTestStepL()
    {
    SetTestStepResult(EFail);

    INFO_PRINTF1(_L("Testing Picture Restorer mechanism"));
    __UHEAP_MARK;
    setupCleanup();
    TRAPD(r,GoL());

    delete TheTrapCleanup;
    
    __UHEAP_MARKEND;
    
    ::DeleteDataFile(KOutputFile);      //deletion of data files must be before call to End() - DEF047652
    ::DeleteDataFile(KOutputFile1); 

    if (r == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
