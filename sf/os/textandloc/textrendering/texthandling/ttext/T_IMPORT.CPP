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
#include <e32svr.h>

#include <txtetext.h>
#include <txtfrmat.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <txtglobl.h>
#include <gdi.h>
#include <s32mem.h>
#include <charconv.h>
#include "T_IMPORT.h"

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

/* this fixes a MSVC link warning */
#ifdef __VC32__
#pragma comment (linker, "/opt:noref") 
#endif

#define UNUSED_VAR(a) a = a

const TInt KTestCleanupStack=0x40;

_LIT(KTestFileName1, "c:\\etext\\climb.txg");
_LIT(KTestFileName2, "c:\\etext\\import1.txg");

LOCAL_D void EnsureFileExists(const TDesC& aName)
	{
	RFs fs;
	fs.Connect();
	fs.MkDirAll(aName);
	RFile file;
	file.Create(fs, aName, EFileRead|EFileWrite);
	file.Close();
	fs.Close();
	}

LOCAL_D CTrapCleanup* TheTrapCleanup=NULL;
LOCAL_D CPlainText* TheText=NULL;
LOCAL_D CRichText* richText=NULL;
LOCAL_D CParaFormatLayer* TheGlobalParaLayer=NULL;
LOCAL_D CCharFormatLayer* TheGlobalCharLayer=NULL;



LOCAL_C void CreateTextL()
//
	{
	TheText=CPlainText::NewL();
//
	TCharFormat charFormat;
	TCharFormatMask charMask;
	TheGlobalCharLayer=CCharFormatLayer::NewL(charFormat,charMask);
	TParaFormatMask paraMask;
	TheGlobalParaLayer=CParaFormatLayer::NewL((CParaFormat*)NULL,paraMask);
	richText=CRichText::NewL(TheGlobalParaLayer,TheGlobalCharLayer);
	}


LOCAL_C void DestroyText()
//
	{
	delete TheText;
	delete TheGlobalParaLayer;
	delete TheGlobalCharLayer;
	}


LOCAL_C void ResetTextL()
// Initialise the rich text.
//
	{TheText->Reset();}



LOCAL_C void ImportText1()
// 
	{
	TFileName file=_L("z:\\test\\app-framework\\etext\\import1.txt");
	ResetTextL();
	TInt count=TheText->ImportTextFileL(0,file,CPlainText::EOrganiseByParagraph);
	test(TheText->DocumentLength()==51);
	test(TheText->ParagraphCount()==10);
	test(count==51);
//	Now export
	TFileName exportFile=KTestFileName2();
	EnsureFileExists(exportFile);
	TheText->ExportAsTextL(exportFile,CPlainText::EOrganiseByParagraph,0);
	ResetTextL();
//	and import this export ?!!
	count=TheText->ImportTextFileL(0,exportFile,CPlainText::EOrganiseByLine);
	TInt documentLength=TheText->DocumentLength();
	test(documentLength==49);
	TInt paraCount=TheText->ParagraphCount();
	test(paraCount==3);
	test(count==49);
	ResetTextL();
	}


LOCAL_C void ImportText2()
// A very long text file of a realistic document nature!
//
	{
	TFileName file=_L("z:\\test\\app-framework\\etext\\climb.txt");
	ResetTextL();
	TInt count=TheText->ImportTextFileL(0,file,CPlainText::EOrganiseByParagraph);
	TInt documentLength=TheText->DocumentLength();
	test(documentLength==18396);
	TInt paraCount=TheText->ParagraphCount();
	test(paraCount==158);
	TInt wordCount=TheText->WordCount();
	test(wordCount=3450);
//	Now export
	TFileName exportFile=KTestFileName1();
	//
	//
	EnsureFileExists(exportFile);
	TRAPD(ret,
		TheText->ExportAsTextL(exportFile,CPlainText::EOrganiseByParagraph,0));
	test(ret==KErrNone);
	//
	ResetTextL();
//	and import this export ?!!??!
	count=TheText->ImportTextFileL(0,exportFile,CPlainText::EOrganiseByLine);
	documentLength=TheText->DocumentLength();
	paraCount=TheText->ParagraphCount();
	wordCount=TheText->WordCount();
	test(documentLength==18362);
	test(paraCount==34);
	test(wordCount=3450);
	ResetTextL();
	}

	
/** 
 Test code for INC044582 - [MX0416] Document - Document application crashed when converting file.
 This occurs when more than 256 styles are used. The fix to this defect is to Leave with
 KErrNotSupported if there are more than 256 styles
*/ 
LOCAL_C void TestInc044582()

	{
	__UHEAP_MARK;
	
	//	create an empty style list
	CStyleList* list=CStyleList::NewL();

	// create formats and add to cleanup stack
	CParaFormatLayer* GlobalParaFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(GlobalParaFormatLayer);
	CCharFormatLayer* GlobalCharFormatLayer=CCharFormatLayer::NewL();
	CleanupStack::PushL(GlobalCharFormatLayer);
	
	// create RichText object and supply empty Style list
	CRichText* richText1=CRichText::NewL(GlobalParaFormatLayer,GlobalCharFormatLayer,*list);

	// Load the RichText object with 300 paragraphs
	_LIT(para, "A line of text\x2029");
	for (TInt Z=0; Z<300; Z++)
		{
		// a line of text foloowed by the paragraph separator	
		richText1->InsertL(0,para);
		}
	
	// create a formats for the style
	CParaFormatLayer* paraLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer=CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);
	
	// Set the richtext object to own the style list
	richText1->SetStyleListExternallyOwned( EFalse );
	
	// add more than 256 styles
	TInt StartOfNextPara=0;
	TInt Length=0;
	for ( TInt i= 0 ; i< 300; i++ )
		{
		// create a new style and add to the Style list
		CParagraphStyle*  paraStyle=CParagraphStyle::NewL(*paraLayer,*charLayer);
		RParagraphStyleInfo info(paraStyle);
		TRAPD(ret, list->AppendL(&info));
        UNUSED_VAR(ret);

		// find the next paragraph and add a style to it	
		StartOfNextPara = richText1->CharPosOfParagraph(Length, i);
		richText1->ApplyParagraphStyleL(*paraStyle,StartOfNextPara,1,
			CParagraphStyle::ERetainAllSpecificFormats);
		}
	
	// Compress it to a buffer.
	CBufFlat* buffer = CBufFlat::NewL(500000);
	CleanupStack::PushL(buffer);
	RBufWriteStream output_stream(*buffer);
	
	// Attempt to "externalize" the RichText object which has more then 256 styles
	// This caused the problem to occur and should leave "with KErrNotSupported"
	TRAPD( error, richText1->ExternalizeMarkupDataL(output_stream));
	test( error == KErrNotSupported);
	
	output_stream.Close();
	delete richText1;
	CleanupStack::PopAndDestroy(5); // GlobalParaFormatLayer, GlobalCharFormatLayer, paraLayer, 
									// charLayer, buffer
	__UHEAP_MARKEND;
	}
	
static void TestUnicodeCompressionL()
	{
	CPlainText* plain1 = CPlainText::NewL();
	CleanupStack::PushL(plain1);
	CPlainText* plain2 = CPlainText::NewL();
	CleanupStack::PushL(plain2);

	// Read complicated Unicode text from a file.
	TFileName raw_file_name = _L("z:\\test\\app-framework\\etext\\polyglot.txt");
	plain1->ImportTextFileL(0,raw_file_name,CPlainText::EOrganiseByParagraph);
	
	// Compress it to a buffer.
	CBufFlat* buffer = CBufFlat::NewL(1024);
	CleanupStack::PushL(buffer);
	RBufWriteStream output_stream(*buffer);
	plain1->ExternalizePlainTextL(output_stream);
	output_stream.Close();

	// Decompress it to a different plain text object.
	RBufReadStream input_stream(*buffer);
	plain2->InternalizePlainTextL(input_stream);
	input_stream.Close();

	// Test for equality.
	int length = plain1->DocumentLength();
	test(length == plain2->DocumentLength());
	int pos = 0;
	do
		{
		TPtrC p1 = plain1->Read(pos);
		TPtrC p2 = plain2->Read(pos);
		test(p1 == p2);
		pos += p1.Length();
		} while (pos < length);

	CleanupStack::PopAndDestroy(3); // plain1, plain2, buffer
	}

static void TestEncodingConversionL()
	{
	CPlainText* plain1 = CPlainText::NewL();
	CleanupStack::PushL(plain1);
	CPlainText* plain2 = CPlainText::NewL();
	CleanupStack::PushL(plain2);
	CBufSeg* buffer = CBufSeg::NewL(1024);
	CleanupStack::PushL(buffer);

	// Read multilingual text from a file.
	TFileName file_name = _L("z:\\test\\app-framework\\etext\\polyglot.txt");
	plain1->ImportTextFileL(0,file_name,CPlainText::EOrganiseByParagraph);

	// Convert to UTF8.
	RBufWriteStream output(*buffer,0);
	CPlainText::TImportExportParam param;
	param.iForeignEncoding = KCharacterSetIdentifierUtf8;
	CPlainText::TImportExportResult result;
	plain1->ExportTextL(0,output,param,result);
	output.Close();

	// Read it into another plain text object.
	RBufReadStream input(*buffer,0);
	plain2->ImportTextL(0,input,param,result);

	// Test for equality.
	int length1 = plain1->DocumentLength();
	TText* text1 = new(ELeave) TText[length1];
	TPtr ptr1(text1,length1);
	plain1->Extract(ptr1);
	int length2 = plain2->DocumentLength();
	TText* text2 = new(ELeave) TText[length2];
	TPtr ptr2(text2,length2);
	plain2->Extract(ptr2);
	test(length1 == length2);
	delete [] text1;
	delete [] text2;
	int pos = 0;
	do
		{
		TPtrC p1 = plain1->Read(pos);
		TPtrC p2 = plain2->Read(pos);
		test(p1 == p2);
		pos += p1.Length();
		} while (pos < length1);

    INFO_PRINTF1(_L("DEF058651 - Propagated:NTT - Received MMS with line break CR set does not cause line feed."));
    buffer->Reset();
    plain1->Reset();
    
    TBuf8<100>  testString(_L8("This Is Test  Message"));
    testString[4] = 0x0D;
    testString[7] = 0x0A;
    testString[12] = 0x0D;
    testString[13] = 0x0A;
    buffer->InsertL(0,testString.Ptr(),testString.Length());
    
    RBufReadStream insert(*buffer,0);
    plain1->ImportTextL(0,insert,param,result);
    insert.Close();

	TBuf<100> str;
	plain1->Extract(str);
    test(str[4] ==CEditableText::EParagraphDelimiter);
	CleanupStack::PopAndDestroy(3); // plain1, plain2, buffer
	}

LOCAL_C void DoTestL()
// Main routine
//
    {
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-LEGACY-T_IMPORT-0001 CPlainText "));
	CreateTextL();

	ImportText1();
	ImportText2();

	TestUnicodeCompressionL();
	TestEncodingConversionL();

	INFO_PRINTF1(_L("CRichText"));
	delete TheText;
	TheText=richText;

	ImportText1();
	ImportText2();

	INFO_PRINTF1(_L("CRichText - more than 256 styles, defect INC044582"));
	TestInc044582();
	
	DestroyText();
    }


LOCAL_C void setupCleanup()
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

CT_IMPORT::CT_IMPORT()
    {
    SetTestStepName(KTestStep_T_IMPORT);
    pTestStep = this;
    }

TVerdict CT_IMPORT::doTestStepL()
    {
    SetTestStepResult(EFail);

    INFO_PRINTF1(_L("Plain Text File Imports"));
    __UHEAP_MARK;
    setupCleanup();
    TRAPD(r, DoTestL());

    ::DeleteDataFile(KTestFileName1);       //deletion of data files must be before call to End() - DEF047652
    ::DeleteDataFile(KTestFileName2);   

    delete TheTrapCleanup;

    __UHEAP_MARKEND;
    
    if (r == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
