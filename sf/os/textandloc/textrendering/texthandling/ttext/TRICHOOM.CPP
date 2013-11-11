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


#include <txtrich.h>
#include "TSTCLIPB.H"
#include <txtstyle.h>
#include <gdi.h>
#include <conpics.h>											   						 
#include <flddef.h>
#include <fldbltin.h>
#include <s32mem.h>
#include <s32file.h>
#include "../incp/T_PMLPAR.H"
#include "T_RICHOOM.h"

#define test(cond)											\
	{														\
	TBool __bb = (cond);									\
	TEST(__bb);												\
	if (!__bb)												\
		{													\
		ERR_PRINTF1(_L("ERROR: Test Failed"));				\
		User::Leave(1);										\
		}													\
	}

const TInt KTestCleanupStack=0x500;

LOCAL_D RFs theFs;
LOCAL_D CParaFormatLayer* GlobalParaFormatLayer;
LOCAL_D CCharFormatLayer* GlobalCharFormatLayer;
LOCAL_D MPictureFactory* GlobalPictureFactory;
LOCAL_D CTrapCleanup* TheTrapCleanup;
LOCAL_D CParser* TheParser;
LOCAL_D RFs TheSession;


CRichText* CT_RICHOOM::LoadIntoTextL(TFileName& aFileName)
//
	{
	TRAPD(ret,
	TheParser=CParser::NewL());
	CRichText* text=NULL;
	TRAP(ret,
	text=TheParser->ParseL(aFileName));
	delete GlobalParaFormatLayer;
	delete GlobalCharFormatLayer;
	GlobalParaFormatLayer=(CParaFormatLayer*)text->GlobalParaFormatLayer();
	GlobalCharFormatLayer=(CCharFormatLayer*)text->GlobalCharFormatLayer();
	delete TheParser;
	TheParser=NULL;
	return text;
	}


void CT_RICHOOM::ConstructOOM()
// Test CRichText construction, (and index), forcing a leave error at each
// possible stage of the process.
//
	{
	INFO_PRINTF1(_L("Construction under low memory conditions"));
	
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	TInt failRate=0;
	CRichText* doc=NULL;
	for (failRate=1;;failRate++)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		TRAPD(ret,doc=CRichText::NewL(paraLayer,charLayer));
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			test(doc==NULL);
			}
		else
			{
			test(doc!=NULL);
//			test(!doc->HasMarkupData());
			delete doc;
			__UHEAP_MARKEND;
			break;
			}
		}
	__UHEAP_RESET;
	delete paraLayer;
	delete charLayer;
	TBuf<36> answer;
	answer.Format(_L("        #allocs for full c'tion: %d\n"),failRate-1);
	INFO_PRINTF1(answer);
	}


void CT_RICHOOM::CopyPasteOOM()
	{
// Set up the framework
	INFO_PRINTF1(_L("Copy/Paste OOM"));
	CRichText* richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	TFileName file=_L("z:\\test\\app-framework\\etext\\climb.txt");
	/*TInt count=*/richText1->ImportTextFileL(0,file,CPlainText::EOrganiseByParagraph);
	//
	// Insert some picture components
	CXzePicture* pic1=CXzePicture::NewL('p');
	CXzePicture* pic2=CXzePicture::NewL('l');
	CXzePicture* pic3=CXzePicture::NewL('l');
	TPictureHeader hdr1;
	TPictureHeader hdr2;
	TPictureHeader hdr3;
	hdr1.iPictureType = KUidXzePictureType;
	hdr2.iPictureType = KUidXzePictureType;
	hdr3.iPictureType = KUidXzePictureType;
	hdr1.iPicture = pic1;
	hdr2.iPicture = pic2;
	hdr3.iPicture = pic3;
	richText1->InsertL(richText1->DocumentLength(),hdr3);
	richText1->InsertL(richText1->DocumentLength(),hdr2);
	richText1->InsertL(richText1->DocumentLength(),hdr1);
	//
	// Apply some random formatting
	TCharFormat charFormat; TCharFormatMask charMask;
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	charMask.SetAttrib(EAttFontStrokeWeight);
	richText1->ApplyCharFormatL(charFormat,charMask,10,37);
//
// Copy to clipboard normally.
	INFO_PRINTF1(_L("Copy Normally"));
	User::LeaveIfError(TheSession.Connect());
	CClipboard* writeBoard=CClipboard::NewForWritingLC(TheSession);
	richText1->CopyToStoreL(writeBoard->Store(),writeBoard->StreamDictionary(),0,richText1->DocumentLength()-1);  // forces virtual trailing phrase
	writeBoard->CommitL();
	CleanupStack::PopAndDestroy();  // writeboard
	writeBoard=NULL;
	delete richText1;
	richText1=NULL;
//
// Paste from clipboard normally.
	INFO_PRINTF1(_L("Paste Normally"));
	__UHEAP_MARK;
	CRichText* empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	MDemPictureFactory pictureFactory;
	empty->SetPictureFactory(&pictureFactory,NULL);
	CClipboard* readBoard=CClipboard::NewForReadingLC(TheSession);
	TRAPD(ret,
	empty->PasteFromStoreL(readBoard->Store(),readBoard->StreamDictionary(),0));
	test(ret==KErrNone);
	delete empty;
	empty=NULL;
	CleanupStack::PopAndDestroy();  // readBoard
	readBoard=NULL;
	__UHEAP_MARKEND;
//
// Paste from clipboard with OOM
	INFO_PRINTF1(_L("Paste with OOM"));
	INFO_PRINTF1(_L("...please wait"));
	__UHEAP_MARK;
	empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	for (TInt ii=1; ;ii++)
		{
		readBoard=CClipboard::NewForReadingLC(TheSession);
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		TRAP(ret,empty->PasteFromStoreL(readBoard->Store(),readBoard->StreamDictionary(),0));
		__UHEAP_RESET;
		empty->Reset();
		CleanupStack::PopAndDestroy();
		if (ret==KErrNone)
			break;
		}
	readBoard=NULL;

	//
	delete empty;
	empty=NULL;
	TheSession.Close();
	__UHEAP_MARKEND;
	}


void CT_RICHOOM::CopyPasteOOM2()
// Test case for defect HA-282 - simple case
//
	{
	__UHEAP_MARK;
// Set up the framework
	INFO_PRINTF1(_L("Copy/Paste OOM 2"));
	CRichText* richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	richText1->InsertL(0,_L("HelloHello"));
	richText1->InsertL(5,CEditableText::EParagraphDelimiter);
	//
	// Format the first paragraph
	TCharFormat charFormat; TCharFormatMask charMask;
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	charMask.SetAttrib(EAttFontStrokeWeight);
	richText1->ApplyCharFormatL(charFormat,charMask,0,6);
//
// Copy to clipboard normally.
	INFO_PRINTF1(_L("Copy Normally"));
	User::LeaveIfError(TheSession.Connect());
	CClipboard* writeBoard=CClipboard::NewForWritingLC(TheSession);
	richText1->CopyToStoreL(writeBoard->Store(),writeBoard->StreamDictionary(),2,6);  // forces virtual trailing phrase
	writeBoard->CommitL();
	CleanupStack::PopAndDestroy();  // writeboard
	writeBoard=NULL;
//
// Paste from clipboard normally.
	INFO_PRINTF1(_L("Paste Normally"));
//	__UHEAP_MARK;
	CRichText* empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	empty->AppendTakingSolePictureOwnershipL(*richText1);
	CClipboard* readBoard=CClipboard::NewForReadingLC(TheSession);
	TRAPD(ret,
	empty->PasteFromStoreL(readBoard->Store(),readBoard->StreamDictionary(),empty->DocumentLength()));
	test(ret==KErrNone);
	delete empty;
	empty=NULL;
	CleanupStack::PopAndDestroy();  // readBoard
	readBoard=NULL;
//	__UHEAP_MARKEND;
//
// Paste from clipboard with OOM
	INFO_PRINTF1(_L("Paste with OOM"));
	INFO_PRINTF1(_L("...please wait"));
	__UHEAP_MARK;
	empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	empty->AppendTakingSolePictureOwnershipL(*richText1);
	delete richText1;
	richText1=NULL;
	for (TInt ii=1; ;ii++)
		{
		readBoard=CClipboard::NewForReadingLC(TheSession);
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		TRAP(ret,empty->PasteFromStoreL(readBoard->Store(),readBoard->StreamDictionary(),empty->DocumentLength()));
		__UHEAP_RESET;
//		empty->Reset();
		CleanupStack::PopAndDestroy();
		if (ret==KErrNone)
			break;
		}
	readBoard=NULL;
	//
	delete empty;
	empty=NULL;
	__UHEAP_MARKEND;
	TheSession.Close();
	__UHEAP_MARKEND;
	}


void CT_RICHOOM::WriteInlineL(RWriteStream& aStream,CRichText* aRichText)
	{
	aRichText->ExternalizeStyleDataL(aStream);
	aRichText->ExternalizeMarkupDataL(aStream);	
	aRichText->ExternalizePlainTextL(aStream);
	}

void CT_RICHOOM::ReadInlineL(RReadStream& aStream,CRichText* aRichText)
	{
	aRichText->InternalizeStyleDataL(aStream);
	aRichText->InternalizeMarkupDataL(aStream);	
	aRichText->InternalizePlainTextL(aStream);
	}


CStyleList* CT_RICHOOM::CreatePopulatedStyleList()
//
	{
	//
	// Create style aswell.
	CStyleList* list=CStyleList::NewL();
	CParagraphStyle* style1=CParagraphStyle::NewL(*GlobalParaFormatLayer,*GlobalCharFormatLayer);
	CParagraphStyle* style2=CParagraphStyle::NewL(*GlobalParaFormatLayer,*GlobalCharFormatLayer);
	CParagraphStyle* style3=CParagraphStyle::NewL(*GlobalParaFormatLayer,*GlobalCharFormatLayer);
	RParagraphStyleInfo info1(style1);
	RParagraphStyleInfo info2(style2);
	RParagraphStyleInfo info3(style3);
	list->AppendL(&info1);
	list->AppendL(&info2);
	list->AppendL(&info3);
	return list;
	}
_LIT(KTRichOutputFile,"c:\\etext\\TRICH.DAT");

void CT_RICHOOM::SaveLoadOOM()
	{
// Set up the framework
	theFs.Delete(KTRichOutputFile);
	theFs.MkDirAll(KTRichOutputFile);
	CFileStore* store = CPermanentFileStore::CreateLC(theFs,KTRichOutputFile,EFileRead|EFileWrite);
	store->SetTypeL(store->Layout());
	CStyleList* list=CreatePopulatedStyleList();
	CRichText* richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,*list);
	TBool hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);  // because of owned style list.
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));

	RStoreWriteStream out;
	TStreamId id1 =  out.CreateLC(*store);
	
	INFO_PRINTF1(_L("Store/Restore OOM"));
	INFO_PRINTF1(_L("Writing Inline noramlly"));
// Write out inline normally
	WriteInlineL(out,richText1);
	delete richText1;
	out.CommitL();
	CleanupStack::PopAndDestroy();  // out
	store->CommitL();
	RStoreReadStream in;
	in.OpenLC(*store,id1);
	CRichText* empty = CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);

	INFO_PRINTF1(_L("Reading Inline noramlly"));
// Read in inline normally:
	TRAPD(ret,ReadInlineL(in,empty));
	test(ret==KErrNone);
	CleanupStack::PopAndDestroy();  // in
	delete empty;
	empty=NULL;

	INFO_PRINTF1(_L("Reading Inline with OOM"));
// Read in inline with OOM:
	empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);

	for (TInt jj = 1; ;++jj)
		{
		RStoreReadStream in2;
		in2.OpenLC(*store,id1);
		__UHEAP_SETFAIL(RHeap::EDeterministic,jj);
		TRAPD(ret,ReadInlineL(in2,empty));
		__UHEAP_RESET;
		CleanupStack::PopAndDestroy();  // in2
		if (ret!=KErrNone)
			empty->Reset();
		else
			{
			test(empty->HasMarkupData());
			break;
			}
		}

	INFO_PRINTF1(_L("Writing Inline with OOM"));
// Write out inline with OOM:
	CStyleList* list2=CreatePopulatedStyleList();
	CRichText* richText2 = CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,*list2);
	richText2->InsertL(0,_L("hello"));
	TStreamId i = store->ExtendL();
	for (TInt ii = 1; ;++ii)
		{
		RStoreWriteStream out2;
		out2.ReplaceLC(*store,i);	
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		TRAP(ret,WriteInlineL(out2,richText2));
		__UHEAP_RESET;
		if (ret==KErrNone)
			{
			out2.CommitL();
			CleanupStack::PopAndDestroy();  // out2
			delete richText2;
			break;		
			}
		else
			CleanupStack::PopAndDestroy();  // out2
		}
	store->CommitL();

	INFO_PRINTF1(_L("Reading Inline after OOM write"));
// Read in inline normally following a write out with OOM
	RStoreReadStream in3;
	in3.OpenLC(*store,i);
	empty->Reset();
	TRAP(ret,ReadInlineL(in3,empty));	// unhandled exception
	test(ret==KErrNone);
	CleanupStack::PopAndDestroy();
	empty->Reset();

//
//
//

	// Set up new component framework
	CXzePicture* pic1=CXzePicture::NewL('p');
	CXzePicture* pic2=CXzePicture::NewL('l');
	CXzePicture* pic3=CXzePicture::NewL('l');
	TPictureHeader hdr1;
	TPictureHeader hdr2;
	TPictureHeader hdr3;
	hdr1.iPictureType = KUidXzePictureType;
	hdr2.iPictureType = KUidXzePictureType;
	hdr3.iPictureType = KUidXzePictureType;
	hdr1.iPicture = pic1;
	hdr2.iPicture = pic2;
	hdr3.iPicture = pic3;
	CRichText* richText3 = CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	richText3->InsertL(0,hdr3);
	richText3->InsertL(0,hdr2);
	richText3->InsertL(0,hdr1);
	richText3->InsertL(2,_L("hello"));

	INFO_PRINTF1(_L("Writing Outline noramlly"));
// Write outline normally:
	TStreamId id2 = richText3->StoreL(*store);
	store->CommitL();
	delete richText3;

	INFO_PRINTF1(_L("Reading Outline noramlly"));
// Read outline normally:
	empty->Reset();
	TRAP(ret,empty->RestoreL(*store,id2));
	test(ret==KErrNone);
	empty->Reset();

	INFO_PRINTF1(_L("Reading Outline with OOM"));
// Read outline with OOM:
	CRichText* inText2=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	for (TInt kk = 1; ;++kk)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,kk);
		//
		TRAP(ret,inText2->RestoreL(*store,id2));
		//
		__UHEAP_RESET;
		if (ret!=KErrNone)
			inText2->Reset();
		else
			{
			delete inText2;
			break;
			}
		}

	CRichText* richText4 = CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	CXzePicture* pic4=CXzePicture::NewL('P');
	CXzePicture* pic5=CXzePicture::NewL('P');
	CXzePicture* pic6=CXzePicture::NewL('P');
	TPictureHeader hdr4;
	TPictureHeader hdr5;
	TPictureHeader hdr6;
	hdr4.iPictureType=KUidXzePictureType;
	hdr5.iPictureType=KUidXzePictureType;
	hdr6.iPictureType=KUidXzePictureType;
	hdr4.iPicture=pic4;
	hdr5.iPicture=pic5;
	hdr6.iPicture=pic6;
	richText4->InsertL(0,hdr4);
	richText4->InsertL(0,hdr5);
	richText4->InsertL(0,hdr6);
	richText4->InsertL(1,_L("hello"));
	
	INFO_PRINTF1(_L("Writing Outline with OOM"));
// Wtite outline with OOM:
	TStreamId id3(0);
	for (TInt mm=1; ;++mm)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,mm);
		TRAPD(ret,id3 = richText4->StoreL(*store));
		if (ret==KErrNone)
			{
			__UHEAP_RESET;
			delete richText4;
			store->CommitL();
			break;		
			}
		}

	INFO_PRINTF1(_L("Reading Outline after OOM write"));
// Read outline normally folliwng a write with OOM:
	empty->Reset();
	TRAP(ret,empty->RestoreL(*store,id3));
	test(ret==KErrNone);
		
	CleanupStack::PopAndDestroy();  // store
	delete empty;
	}


void CT_RICHOOM::CheckTextIntegrity(const CRichText* aText)
// Simply invokes a method to run the text (& component's) onvariants.
//
	{
	__UHEAP_RESET;
	CParaFormat* paraFormat=CParaFormat::NewLC();
	aText->GetParagraphFormatL(paraFormat,0);  // forces CRichTextIndex::__DbgTextInvariant() to run.
	CleanupStack::PopAndDestroy();  // paraFormat
	}
	
	
void CT_RICHOOM::BasicEditOOM()
// Tests basic editing functions under OOM conditions.
//
	{
	INFO_PRINTF1(_L("Basic Editing methods under OOM"));
	INFO_PRINTF1(_L("InsertL(TChar)"));
//
//	Insert a single character
	const TInt insertCharLoop=10;
	const TInt KSmallTextBufferSize=1;
	CRichText* richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,CEditableText::ESegmentedStorage,KSmallTextBufferSize);
	richText1->SetPictureFactory(NULL,NULL);  // forces index generation
	for (TInt mm=1;mm<=insertCharLoop;mm++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,mm);
		for (TInt nn=1;nn<=insertCharLoop;nn++)
			{
			TRAPD(ret,richText1->InsertL(richText1->DocumentLength(),'x'));
			if (ret!=KErrNone)
				{
				test(richText1->DocumentLength()+1==nn);
				CheckTextIntegrity(richText1);
				break;
				}
			}
		__UHEAP_RESET;
		richText1->Reset();
		}
	__UHEAP_RESET;
	delete richText1;
//
//
	INFO_PRINTF1(_L("InsertL(TDesC&)"));
//
//	Insert a descriptor
	const TInt insertDescriptorLoop=10;
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,CEditableText::ESegmentedStorage,KSmallTextBufferSize);
	richText1->SetPictureFactory(NULL,NULL);  // forces index generation
	TPtrC buf(_L("1234567890"));

	for (TInt nn=1;nn<=insertDescriptorLoop+1;nn++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,nn);
		TRAPD(ret,richText1->InsertL(richText1->DocumentLength(),buf));
		if (ret!=KErrNone)
			{
			test(richText1->DocumentLength()==0);
			}
		else
			{
			test(richText1->DocumentLength()==buf.Length());
			}
		CheckTextIntegrity(richText1);
		richText1->Reset();
		}
	__UHEAP_RESET;
	delete richText1;

//
//
	INFO_PRINTF1(_L("InsertL(TPictureHeader&)"));
//
//	Insert a picture header
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,CEditableText::ESegmentedStorage,KSmallTextBufferSize);
	//
	for (TInt ii=18; ;ii++)
		{
		// create the picture header
		__UHEAP_RESET;
		CXzePicture* pic1=CXzePicture::NewL('o');
		TPictureHeader hdr1;
		hdr1.iPictureType=KUidXzePictureType;
		hdr1.iPicture=pic1;
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		TRAPD(ret,richText1->InsertL(richText1->DocumentLength(),hdr1));
		if (ret!=KErrNone)
			{
			test(richText1->DocumentLength()==0);
//			if (ii<19)
//				delete pic1;
/*
	THE ABOVE 2 LINES ARE WRONG NOW, SINCE THE PICTURE IS DELETED AUTOMATICALLY, IF THE INSERTION FAILS.

*/
			richText1->Reset();
			}
		else
			{
			test(richText1->DocumentLength()==1);
			break;
			}
		}
	__UHEAP_RESET;
	delete richText1;

	__UHEAP_RESET;
//	delete richText1;
	}


void CT_RICHOOM::AppendTest1L()
	{
//	Insert a single character
	const TInt insertDescriptorLoop=10;
	const TInt KSmallTextBufferSize=1;
	CRichText* source=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,CEditableText::ESegmentedStorage,KSmallTextBufferSize);
	CRichText* target=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,CEditableText::ESegmentedStorage,KSmallTextBufferSize);
	source->InsertL(source->DocumentLength(),_L("1234567890"));	
	target->InsertL(target->DocumentLength(),_L("abcdef"));
	TInt targetStartLength=target->DocumentLength();
	for (TInt nn=1;nn<=insertDescriptorLoop+2;nn++) // +1 for the last para delim, +1 to force a suceed
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,nn);
		TRAPD(ret,target->AppendTakingSolePictureOwnershipL(*source));
		if (ret!=KErrNone)
			{
			test(target->DocumentLength()==targetStartLength);
			}
		else
			{
			TInt targetLength=target->DocumentLength();
			test(targetLength==targetStartLength+source->DocumentLength()+1);
			break;
			}
		CheckTextIntegrity(target);
		}
	__UHEAP_RESET;
	delete target;
	delete source;
	}


void CT_RICHOOM::AppendTest2L(CRichText* aTarget)
	{
	__UHEAP_RESET;
	const TInt KSmallTextBufferSize=1;
	TFileName filename=_L("z:\\test\\app-framework\\etext\\richoom1.pml");
	CRichText* source=LoadIntoTextL(filename);
	if (!aTarget)
		aTarget=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,CEditableText::ESegmentedStorage,KSmallTextBufferSize);
	TInt targetStartLength=aTarget->DocumentLength();
	for (TInt ii = 1; ;++ii)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		TRAPD(ret,
			aTarget->AppendTakingSolePictureOwnershipL(*source));
		__UHEAP_RESET;
		if (ret!=KErrNone)
			{
			test(aTarget->DocumentLength()==targetStartLength);
			}
		else
			{// We have succeeded in appending the document.
			TInt paraDelimiter=(targetStartLength>0) ? 1 : 0;
			test(aTarget->DocumentLength()==targetStartLength+paraDelimiter+source->DocumentLength());
			break;
			}
		}
	delete source;
	delete aTarget;
	}


void CT_RICHOOM::AppendTestNewL(CRichText* aTarget)
	{
	const TInt KSmallTextBufferSize=4;
	TFileName filename=_L("z:\\test\\app-framework\\etext\\richoom2.pml");
	CRichText* source=LoadIntoTextL(filename);
	if (!aTarget)
		{
		aTarget=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,CEditableText::ESegmentedStorage,KSmallTextBufferSize);
		aTarget->InsertL(0,_L("This is paragraph number one"));
		aTarget->InsertL(1,CEditableText::EParagraphDelimiter);
		}
	TInt targetStartLength=aTarget->DocumentLength();
	for (TInt ii = 1; ;++ii)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		TRAPD(ret,
			aTarget->AppendTakingSolePictureOwnershipL(*source));
		__UHEAP_RESET;
		if (ret!=KErrNone)
			{
			test(aTarget->DocumentLength()==targetStartLength);
			}
		else
			{// We have succeeded in appending the document.
			TInt paraDelimiter=(targetStartLength>0) ? 1 : 0;
			test(aTarget->DocumentLength()==targetStartLength+paraDelimiter+source->DocumentLength());
			break;
			}
		}
	delete source;
	delete aTarget;
	}


void CT_RICHOOM::AppendTest3L(CRichText* aTarget)
	{
	const TInt KSmallTextBufferSize=31;
	TFileName filename=_L("z:\\test\\app-framework\\etext\\richoom2.pml");
	CRichText* source=LoadIntoTextL(filename);
	if (!aTarget)
		aTarget=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,CEditableText::ESegmentedStorage,KSmallTextBufferSize);
	TInt targetStartLength=aTarget->DocumentLength();
	for (TInt ii = 1; ;++ii)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		TRAPD(ret,
			aTarget->AppendTakingSolePictureOwnershipL(*source));
		__UHEAP_RESET;
		if (ret!=KErrNone)
			{
			test(aTarget->DocumentLength()==targetStartLength);
			}
		else
			{// We have succeeded in appending the document.
			TInt paraDelimiter=(targetStartLength>0) ? 1 : 0;
			test(aTarget->DocumentLength()==targetStartLength+paraDelimiter+source->DocumentLength());
			break;
			}
		}
	delete source;
	delete aTarget;
	}


void CT_RICHOOM::AppendTakingSolePictureOwnershipOOM()
//
	{
	INFO_PRINTF1(_L("AppendTakingSolePictureOwnershipL() under OOM"));
	//
	INFO_PRINTF1(_L("Plain text only"));
	TRAPD(ret,
	AppendTest1L());
	test(ret==KErrNone);
	//
	INFO_PRINTF1(_L("Plain text + phrase markup, with empty target"));
	TRAP(ret,
	AppendTest2L(STATIC_CAST(CRichText*,NULL)));
	test(ret==KErrNone);
	//
	INFO_PRINTF1(_L("Plain text + phrase markup, with non-empty target"));
	TFileName filename=_L("z:\\test\\app-framework\\etext\\richoom1.pml");
	CRichText* target=LoadIntoTextL(filename);
	TRAP(ret,
	AppendTest2L(target));
	test(ret==KErrNone);
	//
//	INFO_PRINTF1(_L("Plain text + shared markup, with empty target"));
//	TRAP(ret,
//	AppendTest3L(STATIC_CAST(CRichText*,NULL)));
//	test(ret==KErrNone);
	//
	INFO_PRINTF1(_L("Plain text + shared markup, non-empty target"));
	TRAP(ret,
	AppendTestNewL(STATIC_CAST(CRichText*,NULL)));
	test(ret==KErrNone);
	//
	INFO_PRINTF1(_L("Plain text + shared markup, with non-empty target"));
	filename=_L("z:\\test\\app-framework\\etext\\richoom2.pml");
	target=LoadIntoTextL(filename);
	TRAP(ret,
	AppendTest3L(target));
	test(ret==KErrNone);
	}


void CT_RICHOOM::DoAppendParagraphOOM(CRichText* aRichText,TBool aUnderOOM,TInt aReplicas)
	{
	if (!aUnderOOM)
		{
		aRichText->AppendParagraphL(aReplicas);
		return;
		}
//
// Append testing for OOM
	TInt charCount=aRichText->DocumentLength();
	TInt paraCount=aRichText->ParagraphCount();
	for (TInt jj = 1; ;++jj)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,jj);
		TRAPD(ret,
		aRichText->AppendParagraphL(aReplicas));
		__UHEAP_RESET;
		TInt newCharCount=aRichText->DocumentLength();
		TInt newParaCount=aRichText->ParagraphCount();
		if (ret!=KErrNone)
			{
			test(newCharCount==charCount);
			test(newParaCount==paraCount);
			}
		else
			{
			test(newCharCount==charCount+aReplicas);
			test(newParaCount==paraCount+aReplicas);
			TBuf<30> buf;
			buf.Format(_L("   allocs=%d\n"),jj);
			INFO_PRINTF1(buf);
			break;
			}
		}
	}


void CT_RICHOOM::AppendParagraphOOM()
//	
	{
	TInt manyParas=5;
	TInt singlePara=1;
	INFO_PRINTF1(_L("AppendParagraphL() under OOM"));
//
	INFO_PRINTF1(_L("appending normally - text has markup - single para"));
	TFileName filename=_L("z:\\test\\app-framework\\etext\\richoom1.pml");
	CRichText* text=LoadIntoTextL(filename);
	DoAppendParagraphOOM(text,EFalse,singlePara);
	delete text;
	text=NULL;

	INFO_PRINTF1(_L("appending with OOM - text has markup - single para"));
	text=LoadIntoTextL(filename);
	DoAppendParagraphOOM(text,ETrue,singlePara);
	delete text;
	text=NULL;

	INFO_PRINTF1(_L("appending normally - text has markup - many paras"));
	text=LoadIntoTextL(filename);
	DoAppendParagraphOOM(text,EFalse,manyParas);
	delete text;
	text=NULL;

	INFO_PRINTF1(_L("appending with OOM - text has markup - many paras"));
	text=LoadIntoTextL(filename);
	DoAppendParagraphOOM(text,ETrue,manyParas);
	delete text;
	text=NULL;
	}


void CT_RICHOOM::ApplyCharFormatOOM1()
	{
// Set up the framework
	INFO_PRINTF1(_L("ApplyCharFormatL() OOM - no markup"));
	CRichText* richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	richText1->InsertL(0,_L("SOME VERY FINE TEXT"));

// Apply some random formatting
	TCharFormat charFormat; TCharFormatMask charMask;
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	charMask.SetAttrib(EAttFontStrokeWeight);

	for (TInt jj=1; ;jj++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,jj);
		TRAPD(ret,
		richText1->ApplyCharFormatL(charFormat,charMask,5,11));
		__UHEAP_RESET;
		TPtrC buf;
		if (ret==KErrNone)
			{
			TCharFormat newFormat;
			richText1->GetChars(buf,newFormat,5);
			test(newFormat.IsEqual(charFormat,charMask));
			test(buf.Length() < richText1->DocumentLength());
			break;
			}
		else
			{

			}

		}
	delete richText1;
	richText1=NULL;
	}


void CT_RICHOOM::ApplyCharFormatOOM2()
	{
// Set up the framework
	INFO_PRINTF1(_L("ApplyCharFormatL() OOM - with markup"));
	CRichText* richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	richText1->InsertL(0,_L("SOME TEXT"));

// Generate some markup
	TCharFormat cF;
	TCharFormatMask cM;
	cF.iFontSpec.iFontStyle.SetPosture(EPostureItalic);
	cM.SetAttrib(EAttFontPosture);
	richText1->ApplyCharFormatL(cF,cM,3,3);

// Apply some random formatting
	TCharFormat charFormat; TCharFormatMask charMask;
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	charMask.SetAttrib(EAttFontStrokeWeight);

	for (TInt jj=1; ;jj++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic,jj);
		TRAPD(ret,
		richText1->ApplyCharFormatL(charFormat,charMask,3,3));
		__UHEAP_RESET;
		TPtrC buf;
		if (ret==KErrNone)
			{
			TCharFormat newFormat;
			richText1->GetChars(buf,newFormat,3);
			test(newFormat.iFontSpec.iFontStyle.Posture()==EPostureItalic);
			test(newFormat.iFontSpec.iFontStyle.StrokeWeight()==EStrokeWeightBold);
			test(buf.Length()==3);
			break;
			}
		else
			{

			}

		}
	delete richText1;
	richText1=NULL;
	}


void CT_RICHOOM::doMainL()
	{
 
	GlobalPictureFactory=new(ELeave) MDemPictureFactory;
	GlobalParaFormatLayer=CParaFormatLayer::NewL();
	GlobalCharFormatLayer=CCharFormatLayer::NewL();
	theFs.Connect();
	//
	TRAPD(ret,ConstructOOM());
		test(ret==KErrNone);
	TRAP(ret,SaveLoadOOM());
		test(ret==KErrNone);
	TRAP(ret,CopyPasteOOM());
		test(ret==KErrNone);
	TRAP(ret,BasicEditOOM());
		test(ret==KErrNone);
	TRAP(ret,AppendTakingSolePictureOwnershipOOM());
		test(ret==KErrNone);
	TRAP(ret,CopyPasteOOM2());
		test(ret==KErrNone);
	TRAP(ret,AppendParagraphOOM());
		test(ret==KErrNone);
	TRAP(ret,ApplyCharFormatOOM1());
		test(ret==KErrNone);
	TRAP(ret,ApplyCharFormatOOM2());
		test(ret==KErrNone);
	//
	delete GlobalPictureFactory;
	delete GlobalParaFormatLayer;
	delete GlobalCharFormatLayer;
	theFs.Close();
	}


void CT_RICHOOM::setupCleanup()
//
// Initialise the cleanup stack.
//
    {
	TheTrapCleanup=CTrapCleanup::New();
	test(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	test(r==KErrNone);
	}


void CT_RICHOOM::DeleteDataFile(const TDesC& aFullName)
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

CT_RICHOOM::CT_RICHOOM()
    {
    SetTestStepName(KTestStep_T_RICHOOM);
    }

TVerdict CT_RICHOOM::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

	setupCleanup();
    
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-TTEXT-LEGACY-T_TRICHOOM-0001 RichText OOM Testing "));
    TRAPD(error1, doMainL());

	delete TheTrapCleanup;

    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
