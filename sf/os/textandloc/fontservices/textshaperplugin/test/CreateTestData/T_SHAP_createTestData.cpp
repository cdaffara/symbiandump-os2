/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Creates test data files that are used by TC_SHAP_shapeText.cpp test. 
* This tool uses the Unicode test input text defined in inputData.h and
* shapes the text by calling the IcuLayoutEngine API ShapeText().
* The result, along with the input text is stored in the test data files with 
* extensions .dat
* First, the reference input data is stored in the data files in the following format, seperated by a delimiter - | :
* Length (of the input data string)	Font typeface name | Font file name | Starting offset in the input string | End offset in the input string | Input text string
* Then the result of the shaping is stored in data files in the following format:
* Output glyph count	Output glyphs	Glyph X and Y coordinates	Glyph advances	Glyph indices	Output character count
* The TC_SHAP_shape_Text test is programmed to read in files in this format, and this format only.
* This tool needs to be run, and new test data files need to be created everytime the 
* IcuLayoutEngine shaping rules are changed/edited/appended.
* IN ORDER TO CREATE NEW TEST DATA FILES USING THIS UTILITY, YOU WILL NEED TO:
* 1. Add a new TInputData entry in inputData.txt
* 2. Delete any old data files in %EPOCROOT%epoc32\winscw\c\
* 3. Run the test.
* IN ORDER TO GET THE TC_SHAP_shapeText TEST FILE TO USE THIS NEW DATA, YOU WILL NEED TO:
* 1. Add your new test file in perforce to:
* IcuLayoutEngine/test/testdata/...
* They will have been added to %EPOCROOT%epoc32\winscw\c
* 2. Update the bld.inf file for IcuLayoutEngine to export this file to z:\test\data\...
* 3. Update the .cpp file by adding the a new literal at the top of the file with the new filename to be used.
* 4. Update the TC_SHAP_shapeText.iby file in IcuLayoutEngine/test to include the new file.
* THIS TOOL SHOULD ONLY BE RUN BY SOMEONE WHO KNOWS WHAT HE/SHE IS DOING!!
*
*/


#include <e32def.h>
#include <e32test.h>
#include <e32std.h>
#include <f32file.h>
#include <fntstore.h>
#include <s32file.h>

#include <graphics/openfontrasterizer.h>
#include <graphics/openfontconstants.h>
#include <graphics/shapeimpl.h>
#include "inputData.h"

const TUint32 KDevanagariScriptCode = 0x64657661;
const TUint32 KKannadaScriptCode = 0x6B6E6461;
const TUint32 KHindiLanguageCode = 0x48494E20;
const TUint32 KKannadaLanguageCode = 0x4B414E20;
const TUint32 KGujaratiScriptCode = 0x67756A72;
const TUint32 KGujaratiLanguageCode = 0x47554A20;
const TUint32 KBengaliScriptCode = 0x62656E67;
const TUint32 KBengaliLanguageCode = 0x42454E20;
const TUint32 KTamilScriptCode = 0x74616D6C;
const TUint32 KTamilLanguageCode = 0x54414D20;
const TUint32 KTeluguScriptCode = 0x74656C75;
const TUint32 KTeluguLanguageCode = 0x54454C20;
const TUint32 KGurmukhiScriptCode = 0x67757275;
const TUint32 KGurmukhiLanguageCode = 0;
const TUint32 KMalayalamScriptCode = 0x6d6c796d;
const TUint32 KMalayalamLanguageCode = 0x4d4c5220;


_LIT16(KDelimiter, "|");

//RTest Macros etc
static RTest TheTest(_L("T_SHAP_createTestData"));
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}

static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)	
#define TEST(arg) ::Check((arg), __LINE__)

/* 
Used for cleanup of RImplInfoArray implementationArray below 
This method is stolen from FbsTop.cpp
*/
LOCAL_C void ResetAndDestroyRImplInfoPtrArray(TAny* aPtr)
	{
	RImplInfoPtrArray* array = reinterpret_cast <RImplInfoPtrArray*> (aPtr);
	array->ResetAndDestroy();
	}
	
/*
Load all ECOM implemented rasterizer DLLs. 
This method is stolen from CFbTop::LoadOpenFontLibraries()
*/
LOCAL_C void LoadOpenFontLibraries(CFontStore* aFontStore)
	{

	RImplInfoPtrArray implementationArray;
	TCleanupItem cleanup(ResetAndDestroyRImplInfoPtrArray, &implementationArray);
	CleanupStack::PushL(cleanup);
	TInt error;
	TInt ecomerror;
	TInt ecomnotready;
	TUid uid = {KUidOpenFontRasterizerPlunginInterface};

	// Making sure that no race situation arises
	// If ECom is not ready, give it another chance and try again. if it still doesn't work 
	// after the third try, then it just carries on quietly and fails... 
	for (ecomnotready =0; ecomnotready <3; ecomnotready++)
		{
		TRAP(ecomerror,REComSession::ListImplementationsL(uid,implementationArray));
		if (!ecomerror)
			{
			break;
			}
		else
			{
			ecomerror = 0; 	
			User::After(0);
			}
		}

	const TInt availCount = implementationArray.Count();
	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		TUid rasterizerUid = info->ImplementationUid();
 		// Create a rasterizer
 		COpenFontRasterizer* rasterizer=0;
 		TRAP(error,rasterizer = COpenFontRasterizer::NewL(rasterizerUid));
 		if (!error)
   			{
   			// Install it in the font store.
   			TRAP(error,aFontStore->InstallRasterizerL(rasterizer));
   			if (error)
   				delete rasterizer;
   			}
		}
	CleanupStack::PopAndDestroy(&implementationArray); 	

	}
	
LOCAL_C void WriteInputData(RWriteStream& aStream, const TInputData& aInputData)
	{
	//output the input data to the file
	TPtrC input = aInputData.TextInput();
	TInt length = input.Length();
	TInt start = aInputData.iStart;
	if (start < 0)
		start += length;
	TInt end = aInputData.iEnd;
	if (end <= 0)
		end += length;

	aStream.WriteInt16L(length);

	aStream.WriteL(aInputData.TypeFaceName());
	aStream.WriteL(KDelimiter);

	aStream.WriteL(aInputData.FontFilename());
	aStream.WriteL(KDelimiter);
	
	aStream.WriteInt16L(start);
	aStream.WriteL(KDelimiter);
	aStream.WriteInt16L(end);
	aStream.WriteL(KDelimiter);

	aStream.WriteL(input);
	aStream.WriteL(KDelimiter);
	}

static void WriteOutputData(RWriteStream& aStream, const TInputData& aInputData, 
							TUint32 aScript, TUint32 aLanguage)
	{
	//SHAPE THE TEXT

	TPtrC input = aInputData.TextInput();
	TInt length = input.Length();
	TInt start = aInputData.iStart;
	if (start < 0)
		start += length;
	TInt end = aInputData.iEnd;
	if (end <= 0)
		end += length;

	//set up font store and install rasterizer(s)
	CFontStore* fontStore = CFontStore::NewL(&User::Heap());
	fontStore->iKPixelWidthInTwips = 11860;
	fontStore->iKPixelHeightInTwips = 11860;
	CleanupStack::PushL(fontStore);
	LoadOpenFontLibraries(fontStore);

	//add required font file
	TRAPD( err, fontStore->AddFileL(aInputData.FontFilename()) );
	TEST(err==KErrNone);
	
	CBitmapFont* font;
	TFontSpec fontSpec(aInputData.TypeFaceName(), 12);
	fontStore->GetNearestFontToDesignHeightInPixels((CFont*&)font, fontSpec);
	CleanupStack::PushL(font);

	CShaper* theShaper = NULL;
	CShaperFactory* shaperFactory = NULL;

	RImplInfoPtrArray implementationArray;
	TCleanupItem cleanup(ResetAndDestroyRImplInfoPtrArray, &implementationArray);
	CleanupStack::PushL(cleanup);
	TInt error;
	TInt ecomerror;
	TInt ecomnotready;
	TUid uid = {KUidShaperFactoryPlunginInterface};

	// Making sure that no race situation arises between FBserv and Ecom
	// If ECom is not ready, give it another chance and try again. if it still doesn't work 
	// after the third try, then it just carries on quietly and fails... 
	for (ecomnotready =0; ecomnotready <3; ecomnotready++)
		{
       		TRAP(ecomerror,REComSession::ListImplementationsL(uid,implementationArray));
		if (!ecomerror)
			{
			break;
			}
		else
			{
			ecomerror = 0; 	
			User::After(0);
			}
		}

	const TInt availCount = implementationArray.Count();
	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		TUid shaperFactoryUid = info->ImplementationUid();
 		// Create a shaper factory
 		//CShaperFactory* shaperFactory = 0;
 		TRAP(error,shaperFactory = CShaperFactory::NewL(shaperFactoryUid));
   		TEST2(error, KErrNone);
   			// Create a shaper
   		TRAPD(err, theShaper = shaperFactory->NewShaperL(font, aScript, aLanguage, &User::Heap()));
   		TEST2(err, KErrNone);
    	}
	CleanupStack::PopAndDestroy(&implementationArray); 	

	CleanupStack::PushL(shaperFactory);
	CleanupStack::PushL(theShaper);

	// create the data to be shaped
	// this uses testData, defined in testData.h
	CShaper::TInput shaperInput;
	shaperInput.iText = &input;
	shaperInput.iStart = start;
	shaperInput.iEnd = end;

	TShapeHeader* output = NULL;
	err = theShaper->ShapeText(output, shaperInput, &User::Heap());
	TEST2(err,KErrNone);
	CleanupStack::PushL(output);
	
	
	//AND FILL THE FILE UP WITH THE RESULTS
	
	//first the glyph count
	aStream.WriteInt16L(output->iGlyphCount);
	TInt glyphCount(output->iGlyphCount);
	TInt i;
	
	//then the Glyphs
	TUint32* shapePtr32 = (reinterpret_cast<TUint32*>(&output->iBuffer[0]));
	for(i=0; i <glyphCount; i++)
		{
		aStream.WriteInt32L(*shapePtr32);
		shapePtr32++;
		}
	
	//then the X and Y positions
	TUint16* shapePtr16 = (reinterpret_cast<TUint16*>(shapePtr32));
	for(i=0; i<glyphCount*2; i++)
		{
		aStream.WriteInt16L(*shapePtr16);
		shapePtr16++;		
		}
	
  	//then the advance
  	aStream.WriteInt16L(*shapePtr16);
	shapePtr16++;		
  	aStream.WriteInt16L(*shapePtr16);
	shapePtr16++;		
 	
	//then the indices
	for(i=0; i<glyphCount; i++)
		{
		aStream.WriteInt16L(*shapePtr16);
		shapePtr16++;		
		}	
	
	//then the character count
	aStream.WriteInt16L(output->iCharacterCount);

	CleanupStack::PopAndDestroy(output);
	CleanupStack::PopAndDestroy(theShaper);
	CleanupStack::PopAndDestroy(shaperFactory);
	REComSession::FinalClose();

	CleanupStack::Pop(font);  
	fontStore->ReleaseFont(font);
	CleanupStack::PopAndDestroy(fontStore);

	//close the ecom session opened by LoadOpenFontLibraries()
	REComSession::FinalClose();
	}

static void MainL()
	{
	RFs fs;
	RFile file;
	RFileBuf buf;

	// Create the Devanagari test data files
	const TInt numberOfTests
		= sizeof(TNR_Dev_OTInputData)/sizeof(TNR_Dev_OTInputData[0]);
	for (TInt i = 0; i != numberOfTests; ++i)
		{
		const TInputData& data = TNR_Dev_OTInputData[i];
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		// note this uses RFile::Create so that users of this utility cannot accidentally write over
		// files created before. If this test fails, you need to ensure that the file specified by 
		// KFileName is correct, and if so delete it before running this utility.
		TEST2(file.Create(fs, data.OutputFilename(), EFileWrite), KErrNone);
				
		CleanupClosePushL(buf);
		buf.Attach(file);
		RWriteStream stream(&buf);

		//write to the stream
		WriteInputData(stream, data);
		WriteOutputData(stream, data, KDevanagariScriptCode, KHindiLanguageCode);

		CleanupStack::PopAndDestroy(2);	//buf, fs
		}
	
	// Create the Kannada test data files
	const TInt numberOfKanTests = sizeof(Kannada_InputData)/sizeof(Kannada_InputData[0]);
	for (TInt i = 0; i != numberOfKanTests; i++)
		{
		const TInputData& kan_data = Kannada_InputData[i];
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TEST2(file.Create(fs, kan_data.OutputFilename(), EFileWrite), KErrNone);
		CleanupClosePushL(buf);
		buf.Attach(file);
		RWriteStream stream(&buf);
			
		WriteInputData(stream, kan_data);
		WriteOutputData(stream, kan_data, KKannadaScriptCode, KKannadaLanguageCode);
		
		CleanupStack::PopAndDestroy(2);
		}

	
	// Create the Gujarati test data files
	const TInt numberOfGujTests = sizeof(Gujarati_InputData)/sizeof(Gujarati_InputData[0]);
	for (TInt i = 0; i != numberOfGujTests; i++)
		{
		const TInputData& kan_data = Gujarati_InputData[i];
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TEST2(file.Create(fs, kan_data.OutputFilename(), EFileWrite), KErrNone);
		CleanupClosePushL(buf);
		buf.Attach(file);
		RWriteStream stream(&buf);
			
		WriteInputData(stream, kan_data);
		WriteOutputData(stream, kan_data, KGujaratiScriptCode, KGujaratiLanguageCode);
		
		CleanupStack::PopAndDestroy(2);
		}
	
	
	// Create the Bengali test data files
	const TInt numberOfBenTests = sizeof(Bengali_InputData)/sizeof(Bengali_InputData[0]);
	for (TInt i = 0; i != numberOfBenTests; i++)
		{
		const TInputData& ben_data = Bengali_InputData[i];
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TEST2(file.Create(fs, ben_data.OutputFilename(), EFileWrite), KErrNone);
		CleanupClosePushL(buf);
		buf.Attach(file);
		RWriteStream stream(&buf);
			
		WriteInputData(stream, ben_data);
		WriteOutputData(stream, ben_data, KBengaliScriptCode, KBengaliLanguageCode);
		
		CleanupStack::PopAndDestroy(2);
		}
	
	// Create the Tamil test data files
	const TInt numberOfTamTests = sizeof(Tamil_InputData)/sizeof(Tamil_InputData[0]);
	for (TInt i = 0; i != numberOfTamTests; i++)
		{
		const TInputData& tam_data = Tamil_InputData[i];
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TEST2(file.Create(fs, tam_data.OutputFilename(), EFileWrite), KErrNone);
		CleanupClosePushL(buf);
		buf.Attach(file);
		RWriteStream stream(&buf);
			
		WriteInputData(stream, tam_data);
		WriteOutputData(stream, tam_data, KTamilScriptCode, KTamilLanguageCode);
		
		CleanupStack::PopAndDestroy(2);
		}
	
	// Create the Telugu test data files
	const TInt numberOfTelTests = sizeof(Telugu_InputData)/sizeof(Telugu_InputData[0]);
	for (TInt i = 0; i != numberOfTelTests; i++)
		{
		const TInputData& tel_data = Telugu_InputData[i];
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TEST2(file.Create(fs, tel_data.OutputFilename(), EFileWrite), KErrNone);
		CleanupClosePushL(buf);
		buf.Attach(file);
		RWriteStream stream(&buf);
			
		WriteInputData(stream, tel_data);
		WriteOutputData(stream, tel_data, KTeluguScriptCode, KTeluguLanguageCode);
		
		CleanupStack::PopAndDestroy(2);
		}
	
	// Create the Gurmukhi test data files
	const TInt numberOfGurTests = sizeof(Gurmukhi_InputData)/sizeof(Gurmukhi_InputData[0]);
	for (TInt i = 0; i != numberOfGurTests; i++)
		{
		const TInputData& tel_data = Gurmukhi_InputData[i];
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TEST2(file.Create(fs, tel_data.OutputFilename(), EFileWrite), KErrNone);
		CleanupClosePushL(buf);
		buf.Attach(file);
		RWriteStream stream(&buf);
			
		WriteInputData(stream, tel_data);
		WriteOutputData(stream, tel_data, KGurmukhiScriptCode, KGurmukhiLanguageCode);
		
		CleanupStack::PopAndDestroy(2);
		}
	
	// 	Create the Malayalam test data files
	const TInt numberOfMalaTests = sizeof(Malayalam_InputData)/sizeof(Malayalam_InputData[0]);
	for (TInt i = 0; i != numberOfMalaTests; i++)
		{
		const TInputData& tel_data = Malayalam_InputData[i];
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TEST2(file.Create(fs, tel_data.OutputFilename(), EFileWrite), KErrNone);
		CleanupClosePushL(buf);
		buf.Attach(file);
		RWriteStream stream(&buf);
			
		WriteInputData(stream, tel_data);
		WriteOutputData(stream, tel_data, KMalayalamScriptCode, KMalayalamLanguageCode);
		
		CleanupStack::PopAndDestroy(2);
		}
	}

TInt E32Main()
	{
	__UHEAP_MARK;
		
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;
	
	TRAPD(err, MainL());
	TEST2(err, KErrNone);
	
	delete cleanup;
	__UHEAP_MARKEND;

	return KErrNone;
	}
	
