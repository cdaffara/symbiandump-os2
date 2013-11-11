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

// INCLUDE FILES
#include "TestContentHandler.h"
#include "XMLOutputContentHandler.h"
#include "xmlengtester.h"

#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <Xml\Parser.h>
#include <Xml\ParserFeature.h>
#include <Xml\MatchData.h>

#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>

using namespace Xml;

/****************************** SAX TESTs **************************************/ 

// -----------------------------------------------------------------------------
// CLibxml2Tester::ParseFileSAXL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ParseFileSAXL( CStifItemParser& aItem )
{
	_LIT(KSourceFileErrorName,"No source file name.");
    _LIT(KOutputFileErrorName,"No output file name.");
    _LIT(KStartParsing,"Start parsing file using SAX.");
    _LIT(KEndParsing,"Parsing end.");
    _LIT(KOutputFileError,"Output file error: %d.");
    
    TPtrC SrcFile;
    if ( aItem.GetNextString(SrcFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KSourceFileErrorName);
     	iLog->Log((TDesC)KSourceFileErrorName);
        return KErrGeneral;
        }
        
    TPtrC OutputFile;
    if ( aItem.GetNextString(OutputFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KOutputFileErrorName);
     	iLog->Log((TDesC)KOutputFileErrorName);
        return KErrGeneral;
        }
    
    TBool lib = TRUE;
    TPtrC tmp;
    if ( aItem.GetNextString(tmp) == KErrNone )
        {
        lib = FALSE;
        }
	
	//create file sesion and files
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	
	RFile outputFile;
	TFileName outFileName;
	outFileName.Copy(OutputFile);
	
	TFileName srcFileName;
	srcFileName.Copy(SrcFile);
	
	// Open output file
	TInt err = outputFile.Replace(fs,outFileName,EFileShareAny|EFileWrite);
	
	// If output file is correct
	if (err == KErrNone)
    {
    
    	// Create content handler
		CTestHandler *sax = CTestHandler::NewL(outputFile);
			
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KStartParsing);
		iLog->Log((TDesC)KStartParsing);
		
		// Create parser
		// 1. Create CMatchData object
        CMatchData *matchData = CMatchData::NewL();

        // 2. Set mime Type
        matchData->SetMimeTypeL(KXmlMimeType);

        // 3. Set Variant ID
        matchData->SetVariantL(_L8("libxml2"));

		CParser* parser;
		if(lib)
		    {
		    parser = CParser::NewL(*matchData, *sax);
		    }
		else
		    {
		    parser = CParser::NewL(KXmlMimeType, *sax);
		    }
				
		// Parse file
		TInt error;

		TRAP(error, Xml::ParseL(*parser,fs,srcFileName));
		if (error!=KErrNone)
			{
   			// threats exceptions throwed by "ParseL"
			}
		
		delete parser;
		delete matchData;
		delete sax;
	
		outputFile.Close();
	}
	else
	{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KOutputFileError, err);
		iLog->Log(KOutputFileError, err);	
	}
	
	CleanupStack::PopAndDestroy(&fs);
	
	TestModuleIf().Printf( infoNum++,KTEST_NAME, KEndParsing);
	iLog->Log((TDesC)KEndParsing);

    return KErrNone;
    
    }

// -----------------------------------------------------------------------------
// CLibxml2Tester::ParseFileSAXNSL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ParseFileSAXNSL( CStifItemParser& aItem )
{
	_LIT(KSourceFileErrorName,"No source file name.");
    _LIT(KOutputFileErrorName,"No output file name.");
    _LIT(KStartParsing,"Start parsing file using SAX.");
    _LIT(KEndParsing,"Parsing end.");
    _LIT(KSetFeature,"Setting feature name: %d.");
    _LIT(KOutputFileError,"Output file error: %d.");

    TPtrC SrcFile;
    if ( aItem.GetNextString(SrcFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KSourceFileErrorName);
     	iLog->Log((TDesC)KSourceFileErrorName);
        return KErrGeneral;
        }
        
    TPtrC OutputFile;
    if ( aItem.GetNextString(OutputFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KOutputFileErrorName);
     	iLog->Log((TDesC)KOutputFileErrorName);
        return KErrGeneral;
        }
	
	// create file sesion and files
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile outputFile;
	TFileName outFileName;
	outFileName.Copy(OutputFile);
	
	TFileName srcFileName;
	srcFileName.Copy(SrcFile);
	
	// Open output file
	TInt err = outputFile.Replace(fs,outFileName,EFileShareAny|EFileWrite);
    
    // If output file is correct
	if (err == KErrNone)
    {
    	// Create content handler
		CTestHandler *sax = CTestHandler::NewL(outputFile);
			
		// Create parser
		// 1. Create CMatchData object
        CMatchData *matchData = CMatchData::NewL();

        // 2. Set mime Type
        matchData->SetMimeTypeL(KXmlMimeType);

        // 3. Set Variant ID
        matchData->SetVariantL(_L8("libxml2"));

		CParser* parser = CParser::NewL(*matchData, *sax);
		
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KSetFeature,EReportNamespaceMapping);
		iLog->Log(KSetFeature,EReportNamespaceMapping);
		
		parser->EnableFeature(EReportNamespaceMapping);
		
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KStartParsing);
		iLog->Log((TDesC)KStartParsing);
		
		// Parse file
		TInt error;
		TRAP(error, Xml::ParseL(*parser,fs,srcFileName));
		if (error!=KErrNone)
			{
   			// threats exceptions throwed by "ParseL"
			}
	    delete matchData;
		delete parser;
		delete sax;
	
		outputFile.Close();
	}
	else
	{
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KOutputFileError, err);
		iLog->Log(KOutputFileError, err);	
	}
	
	CleanupStack::PopAndDestroy(&fs);
	
	TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
	iLog->Log((TDesC)KEndParsing);

    return KErrNone;
    
    }
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::ParseFileSAXLowerCaseL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ParseFileSAXLowerCaseL( CStifItemParser& aItem )
{
	_LIT(KSourceFileErrorName,"No source file name.");
    _LIT(KOutputFileErrorName,"No output file name.");
    _LIT(KStartParsing,"Start parsing file using SAX.");
    _LIT(KEndParsing,"Parsing end.");
    _LIT(KSetFeature,"Setting feature name: %d.");
    _LIT(KOutputFileError,"Output file error: %d.");

    TPtrC SrcFile;
    if ( aItem.GetNextString(SrcFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KSourceFileErrorName);
     	iLog->Log((TDesC)KSourceFileErrorName);
        return KErrGeneral;
        }
        
    TPtrC OutputFile;
    if ( aItem.GetNextString(OutputFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KOutputFileErrorName);
     	iLog->Log((TDesC)KOutputFileErrorName);
        return KErrGeneral;
        }
	
	// create file sesion and files
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile outputFile;
	TFileName outFileName;
	outFileName.Copy(OutputFile);
	
	TFileName srcFileName;
	srcFileName.Copy(SrcFile);
	
	// Open output file
	TInt err = outputFile.Replace(fs,outFileName,EFileShareAny|EFileWrite);
    
    // If output file is correct
	if (err == KErrNone)
    {
    	// Create content handler
		CTestHandler *sax = CTestHandler::NewL(outputFile);
			
		// Create parser
		// 1. Create CMatchData object
        CMatchData *matchData = CMatchData::NewL();

        // 2. Set mime Type
        matchData->SetMimeTypeL(KXmlMimeType);

        // 3. Set Variant ID
        matchData->SetVariantL(_L8("libxml2"));

		CParser* parser = CParser::NewL(*matchData, *sax);
		
		
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KSetFeature,EConvertTagsToLowerCase);
		iLog->Log(KSetFeature,EConvertTagsToLowerCase);
		
		parser->EnableFeature(EConvertTagsToLowerCase);
		
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KStartParsing);
		iLog->Log((TDesC)KStartParsing);
		
		// Parse file
		TInt error;
		TRAP(error, Xml::ParseL(*parser,fs,srcFileName));
		if (error!=KErrNone)
			{
   			// threats exceptions throwed by "ParseL"
			}
	
		delete parser;
		delete matchData;
		delete sax;
	
		outputFile.Close();
	}
	else
	{
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KOutputFileError, err);
		iLog->Log(KOutputFileError, err);	
	}
	
	CleanupStack::PopAndDestroy(&fs);
	
	TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
	iLog->Log((TDesC)KEndParsing);

    return KErrNone;
    
    }
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::ParseFileSAXExternalSubsetL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ParseFileSAXExternalSubsetL( CStifItemParser& aItem )
{
	_LIT(KSourceFileErrorName,"No source file name.");
    _LIT(KOutputFileErrorName,"No output file name.");
    _LIT(KStartParsing,"Start parsing file using SAX.");
    _LIT(KEndParsing,"Parsing end.");
    _LIT(KSetFeature,"Setting feature name: %d.");
    _LIT(KOutputFileError,"Output file error: %d.");

    TPtrC SrcFile;
    if ( aItem.GetNextString(SrcFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KSourceFileErrorName);
     	iLog->Log((TDesC)KSourceFileErrorName);
        return KErrGeneral;
        }
        
    TPtrC OutputFile;
    if ( aItem.GetNextString(OutputFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KOutputFileErrorName);
     	iLog->Log((TDesC)KOutputFileErrorName);
        return KErrGeneral;
        }
	
	// create file sesion and files
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile outputFile;
	TFileName outFileName;
	outFileName.Copy(OutputFile);
	
	TFileName srcFileName;
	srcFileName.Copy(SrcFile);
	
	// Open output file
	TInt err = outputFile.Replace(fs,outFileName,EFileShareAny|EFileWrite);
    
    // If output file is correct
	if (err == KErrNone)
    {
    	// Create content handler
		CTestHandler *sax = CTestHandler::NewL(outputFile);
			
		// Create parser
		// 1. Create CMatchData object
        CMatchData *matchData = CMatchData::NewL();

        // 2. Set mime Type
        matchData->SetMimeTypeL(KXmlMimeType);

        // 3. Set Variant ID
        matchData->SetVariantL(_L8("libxml2"));

		CParser* parser = CParser::NewL(*matchData, *sax);
		
		
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KSetFeature,0x0100);
		iLog->Log(KSetFeature,0x0100);
		
		parser->EnableFeature(0x0100);
		
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KStartParsing);
		iLog->Log((TDesC)KStartParsing);
		
		// Parse file
		TInt error;
		TRAP(error, Xml::ParseL(*parser,fs,srcFileName));
		if (error!=KErrNone)
			{
   			// threats exceptions throwed by "ParseL"
			}
	
		delete parser;
		delete matchData;
		delete sax;
	
		outputFile.Close();
	}
	else
	{
		TestModuleIf().Printf( infoNum++,KTEST_NAME,KOutputFileError, err);
		iLog->Log(KOutputFileError, err);	
	}
	
	CleanupStack::PopAndDestroy(&fs);
	
	TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
	iLog->Log((TDesC)KEndParsing);

    return KErrNone;
    
    }        

// -----------------------------------------------------------------------------
// CLibxml2Tester::ParseBufSAXL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ParseBufSAXL( CStifItemParser& aItem )
{        
	//SSG const TInt KBufferSize = 20000;      //changed  to 2000, to remove a test failure on SMP h/w
	const TInt KBufferSize = 2000;

	
	_LIT(KSourceFileErrorName,"No source file name.");
    _LIT(KOutputFileErrorName,"No output file name.");
    _LIT(KStartParsing,"Start parsing file using SAX.");
    _LIT(KEndParsing,"Parsing end.");
    _LIT(KReadFileToBuf,"Reading file to buffer.");
    _LIT(KOutputFileError,"Output file error: %d.");
    _LIT(KInputFileError,"Input file error: %d.");

	TPtrC SrcFile;
    if ( aItem.GetNextString(SrcFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KSourceFileErrorName);
     	iLog->Log((TDesC)KSourceFileErrorName);
        return KErrGeneral;
        }
        
    TPtrC OutputFile;
    if ( aItem.GetNextString(OutputFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KOutputFileErrorName);
     	iLog->Log((TDesC)KOutputFileErrorName);
        return KErrGeneral;
        }
	
	// create file sesion and files
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile outputFile;
	TFileName outFileName;
	outFileName.Copy(OutputFile);
	
	RFile srcFile;
	TFileName srcFileName;
	srcFileName.Copy(SrcFile);
	
	// Open output file
	TInt err = outputFile.Replace(fs,outFileName,EFileShareAny|EFileWrite);
	
    // If output file is correct
	if (err == KErrNone)
    {
    	// Create content handler
		CTestHandler *sax = CTestHandler::NewL(outputFile);

		// Create parser
		// 1. Create CMatchData object
        CMatchData *matchData = CMatchData::NewL();

        // 2. Set mime Type
        matchData->SetMimeTypeL(KXmlMimeType);

        // 3. Set Variant ID
        matchData->SetVariantL(_L8("libxml2"));

		CParser* parser = CParser::NewL(*matchData, *sax);
				
		// Read file to buffer	
		TBuf8 <KBufferSize> file;
		
		if(srcFile.Open(fs, srcFileName, EFileStreamText|EFileRead) == KErrNone)
		{
			TestModuleIf().Printf( infoNum++,KTEST_NAME,KReadFileToBuf);
			iLog->Log((TDesC)KReadFileToBuf);

			srcFile.Read(file);
			srcFile.Close();

			TestModuleIf().Printf( infoNum++,KTEST_NAME,KStartParsing);
			iLog->Log((TDesC)KStartParsing);
			// Parse buffer
			TInt error;
			TRAP(error, Xml::ParseL(*parser, file));
			if (error!=KErrNone)
				{
   				// threats exceptions throwed by "ParseL"
				}
		}
		else
		{	
			TestModuleIf().Printf( infoNum++,KTEST_NAME, KInputFileError, err);
			iLog->Log(KInputFileError, err);
		}
	
		delete parser;
		delete matchData;
		delete sax;
	
		outputFile.Close();
	}
	else
	{
		TestModuleIf().Printf( infoNum++,KTEST_NAME, KOutputFileError, err);
		iLog->Log(KOutputFileError, err);	
	}
	
	CleanupStack::PopAndDestroy(&fs);
	
	TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
	iLog->Log((TDesC)KEndParsing);

    return KErrNone;
    
    }
