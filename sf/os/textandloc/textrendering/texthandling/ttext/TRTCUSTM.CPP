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
#include <txtstyle.h>
#include "TXTMRTSR.H"
#include <gdi.h>
#include <conpics.h>											   						 
#include <s32mem.h>
#include <s32file.h>
#include <flddef.h>
#include <fldbltin.h>
#include <fldset.h>
#include "TRTCUSTM.h"

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

LOCAL_D RFs theFs;
LOCAL_D CFileStore* TheStore;
LOCAL_D CParaFormatLayer* GlobalParaFormatLayer;
LOCAL_D CCharFormatLayer* GlobalCharFormatLayer;
LOCAL_D CTrapCleanup* TheTrapCleanup;
const TInt KTestCleanupStack=0x500;


class TStoreResolver : public MRichTextStoreResolver
	{
public:
	virtual const CStreamStore& StreamStoreL(TInt /*aPos*/)const {return *iStore;}
public:
	CStreamStore* iStore;
	};


////////////////////////////////////////////////////////////////////////////////////////////
class TTestFieldFactoryTRTCUSTM : public MTextFieldFactory
	{
public:
	// from MTextFieldFactory
	virtual CTextField* NewFieldL(TUid aFieldType); 
	// Creates a field of the type specified
	// Returns NULL if it does not recognise/support the field type
	};

CTextField* TTestFieldFactoryTRTCUSTM::NewFieldL(TUid aFieldType)
// Creates a field (in aHeader) of the type specified in aHeader
// 
	{
	CTextField* field=NULL;
	if (aFieldType==KDateTimeFieldUid)
		field = (CTextField*)new(ELeave) CDateTimeField();
	return field;
	}
/////////////////////////////////////////////////////////////////////////////////////////////

void CT_TRTCUSTM::WriteInlineL(RWriteStream& aStream,CRichText* aRichText)
	{
	aRichText->CancelInsertCharFormat();
	aRichText->ExternalizeFieldDataL(aStream);
	aRichText->ExternalizeStyleDataL(aStream);
	TBool hasMarkupData=aRichText->HasMarkupData();
	aStream.WriteUint8L((TUint8)hasMarkupData!=EFalse);
	if (hasMarkupData)
		aRichText->ExternalizeMarkupDataL(aStream);	
	aRichText->ExternalizePlainTextL(aStream);
	}

void CT_TRTCUSTM::ReadInlineL(RReadStream& aStream,CRichText* aRichText)
	{
	aRichText->InternalizeFieldDataL(aStream);
	aRichText->InternalizeStyleDataL(aStream);
	TBool hasMarkupData=(TBool)aStream.ReadUint8L();
	if (hasMarkupData)
		aRichText->InternalizeMarkupDataL(aStream);	
	aRichText->InternalizePlainTextL(aStream);
	}


TStreamId CT_TRTCUSTM::PerformSaveL(CRichText* aRichText)
//
	{
	RStoreWriteStream out;
	TStreamId id1=out.CreateLC(*TheStore);
	WriteInlineL(out,aRichText);
	delete aRichText;
	out.CommitL();
	CleanupStack::PopAndDestroy();  // out
	TheStore->CommitL();
	return id1;
	}


CRichText* CT_TRTCUSTM::PerformLoadL(TStreamId aId)
//
	{
	CRichText* text=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	RStoreReadStream in;
	in.OpenLC(*TheStore,aId);
	TRAPD(ret,
	ReadInlineL(in,text));
	test(ret==KErrNone);
	CleanupStack::PopAndDestroy();  // in
	return text;
	}


CStyleList* CT_TRTCUSTM::CreatePopulatedStyleList()
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

_LIT(KTRtCustOutputFile,"c:\\etext\\TRTCUSTM.DAT");

void CT_TRTCUSTM::CustomLoadSave()
	{
// Set up the framework
	theFs.Delete(KTRtCustOutputFile);
	theFs.MkDirAll(KTRtCustOutputFile);
	TheStore=CPermanentFileStore::CreateLC(theFs,KTRtCustOutputFile,EFileRead|EFileWrite);
	TheStore->SetTypeL(TheStore->Layout());

//
// Case (1) Rich text with owned style list. - no markup data
//
	INFO_PRINTF1(_L("RT + no markup + style list owned"));
	//
	// Setup the rich text
	CStyleList* list=CreatePopulatedStyleList();
	CRichText* richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,*list);
	TBool hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));
	//
	// Save the rich text
	TStreamId id1=PerformSaveL(richText1);
	//
	// Load the rich text
	CRichText* empty=NULL;
	TRAPD(ret,
	empty=PerformLoadL(id1));
	test(ret==KErrNone);
	hasMarkupData=empty->HasMarkupData();
	test(hasMarkupData);
	delete empty;
	empty=NULL;
//
// Case (2) Rich text with referenced style list. - no markup data
//
	INFO_PRINTF1(_L("RT + no markup + style list externally owned"));
	//
	// Setup the rich text
	list=CreatePopulatedStyleList();
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,*list,CEditableText::ESegmentedStorage,2);
	richText1->SetStyleListExternallyOwned(ETrue);
	hasMarkupData=richText1->HasMarkupData();
	test(!hasMarkupData);
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	hasMarkupData=richText1->HasMarkupData();
	test(!hasMarkupData);
	//
	// Save the rich text
	id1=PerformSaveL(richText1);
	//
	// Load the rich text
	empty=PerformLoadL(id1);
	hasMarkupData=empty->HasMarkupData();
	test(!hasMarkupData);
	TInt paragraphCount=empty->ParagraphCount();
	test(paragraphCount==3);
	test(list->Count()==3);  // the style list is now externally owned.
	delete empty;
	test(list->Count()==3);  // the style list should not have been destroyed by the rich text
	delete list;
	empty=NULL;
//
// Case (3) Rich text with referenced style list. - with markup
//
	INFO_PRINTF1(_L("RT + markup + style list externally owned"));
	// Setup the rich text
	list=CreatePopulatedStyleList();
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,*list);
	richText1->SetStyleListExternallyOwned(ETrue);
	hasMarkupData=richText1->HasMarkupData();
	test(!hasMarkupData);
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	CParaFormat* paraFormat=CParaFormat::NewLC();
	TParaFormatMask paraMask;
	paraFormat->iHorizontalAlignment=CParaFormat::ERightAlign;
	paraMask.SetAttrib(EAttAlignment);
	richText1->ApplyParaFormatL(paraFormat,paraMask,8,0);
	CleanupStack::PopAndDestroy();  // paraFormat
	hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);
	//
	// Save the rich text
	id1=PerformSaveL(richText1);
	//
	// Load the rich text
	empty=PerformLoadL(id1);
	hasMarkupData=empty->HasMarkupData();
	test(hasMarkupData);
	paragraphCount=empty->ParagraphCount();
	test(paragraphCount==3);
	test(list->Count()==3);  // the style list is now externally owned.
	delete empty;
	test(list->Count()==3);  // the style list should not have been destroyed by the rich text
	delete list;
	empty=NULL;
//
// Case (4) Rich text with no style list. - no effective markup
//
	INFO_PRINTF1(_L("RT + delete picture + no style list"));
	// Setup the rich text
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	hasMarkupData=richText1->HasMarkupData();
	test(!hasMarkupData);
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	//
	// Create & insert a picture
	CXzePicture* pic1=CXzePicture::NewL('o');
	TPictureHeader header1;
	TSize size;
	pic1->GetSizeInTwips(size);
	header1.iPictureType=KUidXzePictureType;
	header1.iPicture=pic1;
	header1.iSize=size;
	richText1->InsertL(3,header1);
	hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);
	richText1->DeleteL(3,1);
	hasMarkupData=richText1->HasMarkupData();
	test(!hasMarkupData);
	//
	// Save the rich text
	id1=PerformSaveL(richText1);
	//
	// Load the rich text
	empty=PerformLoadL(id1);
	hasMarkupData=empty->HasMarkupData();
	test(!hasMarkupData);
	paragraphCount=empty->ParagraphCount();
	test(paragraphCount==3);
	delete empty;
	empty=NULL;
//
// Case (5) Rich text with SetInsertCharFormat()
//
	INFO_PRINTF1(_L("RT + SetInsertCharFormatL()"));
	// Setup the rich text
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	hasMarkupData=richText1->HasMarkupData();
	test(!hasMarkupData);
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	TCharFormat charFormat;
	TCharFormatMask charMask;
	charFormat.iFontPresentation.iStrikethrough=EStrikethroughOn;
	charMask.SetAttrib(EAttFontStrikethrough);
	richText1->SetInsertCharFormatL(charFormat,charMask,3);
	hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);
	//
	// Save the rich text
	id1=PerformSaveL(richText1);
	//
	// Load the rich text
	empty=PerformLoadL(id1);
	hasMarkupData=empty->HasMarkupData();
	test(!hasMarkupData);
	paragraphCount=empty->ParagraphCount();
	test(paragraphCount==3);
	delete empty;
	empty=NULL;
//
// Case (6) Rich text with components - default re/store used.
//

	INFO_PRINTF1(_L("RT + components"));
	// Setup the rich text
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	hasMarkupData=richText1->HasMarkupData();
	test(!hasMarkupData);
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	//
	// Create & insert some fields
	TTestFieldFactoryTRTCUSTM factory;
	richText1->SetFieldFactory(&factory);
	CTextField* field=NULL;
	CTextField* field2=NULL;
	TRAP(ret,
	field=factory.NewFieldL(KDateTimeFieldUid));	test(ret==KErrNone);
	TRAP(ret,
	field2=factory.NewFieldL(KDateTimeFieldUid));    test(ret==KErrNone);
	TRAP(ret,
	richText1->InsertFieldL(0,field,KDateTimeFieldUid));  test(ret==KErrNone);
	TRAP(ret,
	richText1->UpdateFieldL(0));
	test(ret==KErrNone);
	TRAP(ret,
	richText1->InsertFieldL(0,field2,KDateTimeFieldUid));
	test(ret==KErrNone);
	TRAP(ret,
	richText1->UpdateFieldL(0));
	test(ret==KErrNone);
	//
	// Create & insert a picture
	pic1=CXzePicture::NewL('o');
	pic1->GetSizeInTwips(size);
	header1.iPictureType=KUidXzePictureType;
	header1.iPicture=pic1;
	header1.iSize=size;
	richText1->InsertL(0,header1);
	hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);
	//
	// Save the rich text
	TStreamId head=richText1->StoreL(*TheStore);
	delete richText1;
	//
	// Load the rich text
	empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	empty->SetFieldFactory(&factory);
	empty->RestoreL(*TheStore,head);
	//
	// Create the correct factories
	TStoreResolver storeResolver;
	storeResolver.iStore=TheStore;
	MDemPictureFactory* pictureFactory=new(ELeave) MDemPictureFactory;

	empty->SetPictureFactory(pictureFactory,&storeResolver);
	CXzePicture* rPic=(CXzePicture*)empty->PictureHandleL(0);
	test(rPic->iLabel=='o');
	//
	hasMarkupData=empty->HasMarkupData();
	test(hasMarkupData);
	paragraphCount=empty->ParagraphCount();
	test(paragraphCount==3);
	delete empty;
	empty=NULL;
	delete pictureFactory;
//
// Case (7) Rich text with fields with referenced style list with markup
//
	INFO_PRINTF1(_L("RT + fields + markup + style list"));
	// Setup the rich text
	list=CreatePopulatedStyleList();
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,*list);
	hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	paraFormat=CParaFormat::NewLC();
	paraMask.ClearAll();
	paraFormat->iHorizontalAlignment=CParaFormat::ERightAlign;
	paraMask.SetAttrib(EAttAlignment);
	richText1->ApplyParaFormatL(paraFormat,paraMask,8,0);
	CleanupStack::PopAndDestroy();  // paraFormat
	hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);
	//
	// Now add a text field to this.
	richText1->SetFieldFactory(&factory);
	field=NULL;
	TRAP(ret,
	field=factory.NewFieldL(KDateTimeFieldUid));
	test(ret==KErrNone);
	TRAP(ret,
	richText1->InsertFieldL(0,field,KDateTimeFieldUid));
	test(ret==KErrNone);
	TRAP(ret,
	richText1->UpdateFieldL(0));
	test(ret==KErrNone);
	//
	// Save the rich text
	//
	// 1st the components.
	CStoreMap* map=CStoreMap::NewLC(*TheStore);
	richText1->StoreFieldComponentsL(*TheStore,*map);
	//
	RStoreWriteStream out(*map);
	id1=out.CreateLC(*TheStore);
	//
	richText1->CancelInsertCharFormat();
	richText1->ExternalizeFieldDataL(out);
	richText1->ExternalizeStyleDataL(out);
	hasMarkupData=richText1->HasMarkupData();
	out.WriteUint8L((TUint8)hasMarkupData!=EFalse);
	if (hasMarkupData)
		richText1->ExternalizeMarkupDataL(out);	
	richText1->ExternalizePlainTextL(out);
	//
	delete richText1;
	out.CommitL();
	//
	map->Reset();
	CleanupStack::PopAndDestroy(2);  // map,out
	TheStore->CommitL();
	//
	// Load the rich text
	empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	empty->SetFieldFactory(&factory);
	RStoreReadStream in;
	in.OpenLC(*TheStore,id1);
	empty->InternalizeFieldDataL(in);
	empty->InternalizeStyleDataL(in);
	hasMarkupData=(TBool)in.ReadUint8L();
	if (hasMarkupData)
		empty->InternalizeMarkupDataL(in);	
	empty->InternalizePlainTextL(in);
	CleanupStack::PopAndDestroy();  // in
	//	
	empty->RestoreFieldComponentsL(*TheStore);
	hasMarkupData=empty->HasMarkupData();
	test(hasMarkupData);
	paragraphCount=empty->ParagraphCount();
	test(paragraphCount==3);
	test(empty->StyleList()->Count()==3);  // the style list is now externally owned.
	delete empty;
	empty=NULL;
//
// Case (8) Rich text with fields with referenced style list with markup - default store
//
	INFO_PRINTF1(_L("RT + fields + markup + style list - default store"));
	// Setup the rich text
	list=CreatePopulatedStyleList();
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,*list);
	hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	paraFormat=CParaFormat::NewLC();
	paraMask.ClearAll();
	paraFormat->iHorizontalAlignment=CParaFormat::ERightAlign;
	paraMask.SetAttrib(EAttAlignment);
	richText1->ApplyParaFormatL(paraFormat,paraMask,8,0);
	CleanupStack::PopAndDestroy();  // paraFormat
	hasMarkupData=richText1->HasMarkupData();
	test(hasMarkupData);
	//
	// Now add a text field to this.
	richText1->SetFieldFactory(&factory);
	field=NULL;
	TRAP(ret,
	field=factory.NewFieldL(KDateTimeFieldUid));
	test(ret==KErrNone);
	TRAP(ret,
	richText1->InsertFieldL(0,field,KDateTimeFieldUid));
	test(ret==KErrNone);
	TRAP(ret,
	richText1->UpdateFieldL(0));
	test(ret==KErrNone);
	//
	// Save the rich text
	id1=richText1->StoreL(*TheStore);
	delete richText1;
	//
	// Load the rich text
	empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	empty->SetFieldFactory(&factory);
	empty->RestoreL(*TheStore,id1);
	//
	hasMarkupData=empty->HasMarkupData();
	test(hasMarkupData);
	paragraphCount=empty->ParagraphCount();
	test(paragraphCount==3);
	TEtextComponentInfo ci=empty->ComponentInfo();	
	test(ci.iFieldCount==1);
	test(ci.iStyleCount==3);
	delete empty;
	empty=NULL;	//
//
// Case (9) Rich text with no components whatsoever - default store
//
	INFO_PRINTF1(_L("RT + no markup whatsoever - default store"));
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	hasMarkupData=richText1->HasMarkupData();
	test(!hasMarkupData);
	richText1->InsertL(0,_L("hello"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	richText1->InsertL(richText1->DocumentLength(),_L("there"));
	richText1->InsertL(richText1->DocumentLength(),CEditableText::EParagraphDelimiter);
	//
	// Save the rich text
	id1=richText1->StoreL(*TheStore);
	delete richText1;
	//
	// Load the rich text
	empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	empty->RestoreL(*TheStore,id1);
	//
	hasMarkupData=empty->HasMarkupData();
	test(!hasMarkupData);
	paragraphCount=empty->ParagraphCount();
	test(paragraphCount==3);
	ci=empty->ComponentInfo();
	test(ci.iFieldCount==0);
	test(ci.iStyleCount==0);
	test(ci.iPictureCount==0);
	delete empty;
	empty=NULL;
	//
	//
	CleanupStack::PopAndDestroy();  // TheStore
	}
	
/**
@SYMTestCaseID 			SYSLIB-ETEXT-CT-3380
@SYMTestCaseDesc  		Inserting and deleting rich text and verify behaviours of DeleteParagraph(), Delete() and NotifyDelete(); 
  ie. when encounter paragraph delimiter and hidden character
@SYMTestPriority  		High
@SYMTestActions 		1. Insert and delete a whole paragraph of rich text
						2. Insert a paragraph and delete some text
						3. Insert 2nd paragraph and DeleteParagraph() both
						4. Insert field to rich text object and delete field
						5. Insert field to rich text object after some plain text and delete it 
						6. Insert field to rich text object in between two plain texts then delete the field to merge the texts
						7. Insert plain text in between two fields to rich text object then delete plain text to merge two fields
@SYMTestExpectedResults	CTextFieldSet, CPlainText, CRichText's APIs for performing various types of deletion function properly when 
  dealing with paragraph delimiter and hidden characters in a paragraph
@SYMDEF					PDEF101757
*/
void CT_TRTCUSTM::TestDEF101757()
	{	
	TFindFieldInfo info;
	TTestFieldFactoryTRTCUSTM factory;
	CTextField* field=NULL;
	CTextField* field1=NULL;
	CTextField* field2=NULL;
	CTextField* field3=NULL;
	CTextField* field4=NULL;
	
	_LIT(KSample1, "Hello"); // length:5
	_LIT(KSample2, "How are you"); // length:11
	
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-CT-3380 Insertion and deletion to rich text object "));
	
	CRichText* richText=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	CleanupStack::PushL(richText);
	
	// Case1
	// Insert and delete a whole paragraph of rich text
	richText->InsertL(0, KSample1);
	richText->InsertL(richText->DocumentLength(),CEditableText::EParagraphDelimiter);
	test(richText->DocumentLength()==6);
	
	richText->DeleteParagraph(0, 5); //do not delete para delimiter
	
	richText->InsertL(0, KSample1);
	richText->InsertL(richText->DocumentLength(),CEditableText::EParagraphDelimiter);
	test(richText->DocumentLength()==7);
	
	richText->DeleteParagraph(0, 7); //delete para delimiter
	
	test(richText->DocumentLength()==0);
	test(richText->ParagraphCount()==1); 
	
	// Case2
	// Insert a paragraph and delete some text
	richText->InsertL(0, KSample2);
	richText->InsertL(richText->DocumentLength(),CEditableText::EParagraphDelimiter);
	test(richText->DocumentLength()==12);
	
	richText->DeleteParagraph(10, 1); //delete last char 'u' and not para delimter
	test(richText->DocumentLength()==11);
	
	richText->DeleteParagraph(9, 2); //delete last char 'o' and also para delimter
	test(richText->DocumentLength()==9);
	//should not panic due to fix for PDEF101757
	
	test(richText->ParagraphCount()==1);
	
	// Case3
	// Insert 2nd paragraph and DeleteParagraph() both
	richText->InsertL(richText->DocumentLength(),CEditableText::EParagraphDelimiter);
	test(richText->ParagraphCount()==2);
	richText->InsertL(richText->DocumentLength(),KSample1);
	richText->InsertL(richText->DocumentLength(),CEditableText::EParagraphDelimiter);
	test(richText->DocumentLength()==16);
	test(richText->ParagraphCount()==3); //2 paragraph delimiters including EOD delimiter (always there)

	richText->DeleteParagraph(0, 16);
	test(richText->DocumentLength()==0);
	test(richText->ParagraphCount()==1); //2 paragrsphs deleted
	
	// Case4
	// Insert field to rich text object and delete field
	richText->SetFieldFactory(&factory);
	field=factory.NewFieldL(KDateTimeFieldUid); // TUid KDateTimeFieldUid: length:10
	CleanupStack::PushL(field);
	richText->InsertFieldL(0,field,KDateTimeFieldUid);
	CleanupStack::Pop(field);//richtext has taken ownership successfully
	
	richText->UpdateFieldL(0);
	test(richText->FieldCount()==1);
	richText->DeleteParagraph(0, 10);
	richText->UpdateFieldL(0);
	test(richText->FieldCount()==0);
	
	// Case5
	// Insert field to rich text object after some plain text and delete it 
	richText->InsertL(0, KSample1); 
	field1=factory.NewFieldL(KDateTimeFieldUid);
	CleanupStack::PushL(field1);
	richText->InsertFieldL(5, field1, KDateTimeFieldUid);
	CleanupStack::Pop(field1);
	richText->UpdateFieldL(5);
	test(richText->FieldCount()==1);
	richText->DeleteParagraph(5, 10);
	test(richText->FieldCount()==0);
	
	// Case6
	// Insert field to rich text object in between two plain texts then delete the field to merge the texts
	field2=factory.NewFieldL(KDateTimeFieldUid);
	CleanupStack::PushL(field2);
	richText->InsertFieldL(5, field2, KDateTimeFieldUid);
	CleanupStack::Pop(field2);
	richText->UpdateFieldL(5);
	richText->InsertL(14, KSample1);
	richText->DeleteParagraph(5, 10);
	test(richText->DocumentLength()==10); //two plain texts merged 
	richText->DeleteParagraph(0, 10);
	test(richText->FieldCount()==0);
	test(richText->DocumentLength()==0);
	
	// Case7
	// Insert plain text in between two fields to rich text object then delete plain text to merge two fields
	field3=factory.NewFieldL(KDateTimeFieldUid);
	field4=factory.NewFieldL(KDateTimeFieldUid);
	
	CleanupStack::PushL(field3);
	richText->InsertFieldL(0, field3, KDateTimeFieldUid);
	CleanupStack::Pop(field3);
	richText->UpdateFieldL(0);
	
	richText->InsertL(10, KSample1);
	
	CleanupStack::PushL(field4);
	richText->InsertFieldL(15, field4, KDateTimeFieldUid);
	CleanupStack::Pop(field4);
	richText->UpdateFieldL(15);
	richText->DeleteParagraph(10, 5);
	
	richText->FindFields(info,5);
	test(info.iFirstFieldLen==10);
	test(info.iFirstFieldPos==0);
	richText->FindFields(info,15);
	test(info.iFirstFieldLen==10);
	test(info.iFirstFieldPos==10);
	test(richText->FieldCount()==2);
	
	CleanupStack::PopAndDestroy(richText);
	}
	

//Testcode for INC054540
void CT_TRTCUSTM::TestINC054540()
	{
	theFs.Delete(KTRtCustOutputFile);
	theFs.MkDirAll(KTRtCustOutputFile);
	TheStore=CPermanentFileStore::CreateLC(theFs,KTRtCustOutputFile,EFileRead|EFileWrite);
	TheStore->SetTypeL(TheStore->Layout());
//
//test 1: Test INC054540 fix for 255 fields
//	
	INFO_PRINTF1(_L("test"));
	
	CRichText* richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	//
	// Now add a text field to this.
	TTestFieldFactoryTRTCUSTM factory;
	richText1->SetFieldFactory(&factory);
	CTextField* field=NULL;
	TInt x=0;
	//add 255 fields
	while(x<255)
		{
		TRAPD(ret,field=factory.NewFieldL(KDateTimeFieldUid));
		test(ret==KErrNone);
		TRAP(ret,richText1->InsertFieldL(0,field,KDateTimeFieldUid));
		test(ret==KErrNone);
		TRAP(ret,richText1->UpdateFieldL(0));
		test(ret==KErrNone);
		x++;
		}
	// Save the rich text
	//
	CStoreMap* map=CStoreMap::NewLC(*TheStore);
	richText1->StoreFieldComponentsL(*TheStore,*map);
	//
	RStoreWriteStream out1(*map);
	TStreamId id1=out1.CreateLC(*TheStore);
	//
	richText1->ExternalizeFieldDataL(out1);
	//
	delete richText1;
	out1.CommitL();
	//
	map->Reset();
	CleanupStack::PopAndDestroy(2);  // map,out1
	TheStore->CommitL();
	//
	// Load the rich text
	CRichText* empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	empty->SetFieldFactory(&factory);
	RStoreReadStream in;
	in.OpenLC(*TheStore,id1);
	empty->InternalizeFieldDataL(in);
	CleanupStack::PopAndDestroy();  // in
	//	
	empty->RestoreFieldComponentsL(*TheStore);
	test(empty->FieldCount()==255);
	delete empty;
	empty=NULL;
	
//
//test 2: Test INC054540 fix for more than 255 fields
//	
	INFO_PRINTF1(_L("test for more than 255 fields"));
	richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	// Add the text fields
	richText1->SetFieldFactory(&factory);
	field=NULL;
	x=0;
	//add 256 fields
	while(x<256)
		{
		TRAPD(ret,field=factory.NewFieldL(KDateTimeFieldUid));
		test(ret==KErrNone);
		TRAP(ret,richText1->InsertFieldL(0,field,KDateTimeFieldUid));
		test(ret==KErrNone);
		TRAP(ret,richText1->UpdateFieldL(0));
		test(ret==KErrNone);
		x++;
		}
	// Save the rich text
	//
	map=CStoreMap::NewLC(*TheStore);
	richText1->StoreFieldComponentsL(*TheStore,*map);
	//
	RStoreWriteStream out2(*map);
	id1=out2.CreateLC(*TheStore);
	//
	richText1->ExternalizeFieldDataL(out2);
	//
	delete richText1;
	out2.CommitL();
	//
	map->Reset();	
	CleanupStack::PopAndDestroy(2);  // map,out2
	TheStore->CommitL();
	//
	// Load the rich text
	empty=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer);
	empty->SetFieldFactory(&factory);
	in.OpenLC(*TheStore,id1);
	empty->InternalizeFieldDataL(in);
	CleanupStack::PopAndDestroy();  // in
	//	
	empty->RestoreFieldComponentsL(*TheStore);
	test(empty->FieldCount()==256);
	delete empty;
	empty=NULL;
	//
	//
	CleanupStack::PopAndDestroy();  // TheStore
	}


void CT_TRTCUSTM::doMainL()
	{
	GlobalParaFormatLayer=CParaFormatLayer::NewL();
	GlobalCharFormatLayer=CCharFormatLayer::NewL();
	theFs.Connect();
	//
	CustomLoadSave();
	TestDEF101757();
	TestINC054540();
	//
	delete GlobalParaFormatLayer;
	delete GlobalCharFormatLayer;
	theFs.Close();
	}


void CT_TRTCUSTM::setupCleanup()
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


void CT_TRTCUSTM::DeleteDataFile(const TDesC& aFullName)
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


CT_TRTCUSTM::CT_TRTCUSTM()
    {
    SetTestStepName(KTestStep_T_TRTCUSTM);
    }

TVerdict CT_TRTCUSTM::doTestStepL()
    {
    SetTestStepResult(EFail);

    setupCleanup();
    __UHEAP_MARK;
    
    INFO_PRINTF1(_L("TRTCUSTM"));
    INFO_PRINTF1(_L("Testing custom save/load optimization"));
    TRAPD(error1, doMainL());
    DeleteDataFile(KTRtCustOutputFile);   //deletion of data files must be before call to End() - DEF047652

    __UHEAP_MARKEND;
    delete TheTrapCleanup;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
