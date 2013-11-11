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
*
*/

#include "TestContentHandler.h"
#include "xmlengtester.h"

#include <e32svr.h>
#include <e32std.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <xml/dom/xmlengserializer.h>
#include <Xml\ParserFeature.h>
#include "xmlengtesterdef.h"

#include <InetProtTextUtils.h>
#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>


#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengdatacontainer.h>

#include <xml/dom/xmlengparsingoptions.h>


// -----------------------------------------------------------------------------
// CLibxml2Tester::DeserializeL
// deserialize from file, using setter SetInputFileL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::DeserializeL( CStifItemParser& aItem)
    {
    TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );
    
    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
     inputNum.Val(dirtyReturn);	
    
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOutputFile, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
	
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    
    if( pDeserializerType.FindF( DEFAULT ) != KErrNotFound )
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerDefault );
        CleanupStack::PushL( des );
        des->SetInputFileL( pInputFile );
        TRAP(err,des->DeserializeL());
        }
    else if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOP );
        CleanupStack::PushL( des );
        des->SetInputFileL( pInputFile );
        TRAP(err,des->DeserializeL());
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOPInfoset );
        CleanupStack::PushL( des );
        des->SetInputFileL( pInputFile );
        TRAP(err,des->DeserializeL());
        }    
    else if( pDeserializerType.FindF( GZIP ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerGZip );
        CleanupStack::PushL( des );
        des->SetInputFileL( pInputFile );
        TRAP(err,des->DeserializeL());
        }
    	
    CleanupStack::PopAndDestroy( 4 );
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;
    }
// -----------------------------------------------------------------------------
// CLibxml2Tester::Deserialize2L
// deserialize from buffer, using setter SetInputBuffer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TInt CLibxml2Tester::Deserialize2L( CStifItemParser& aItem)
    {
    TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );

    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
     inputNum.Val(dirtyReturn);
     	
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOutputFile, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
    
     HBufC8* buf = ReadFileL(pInputFile);
    CleanupStack::PushL(buf);
		
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    
    if( pDeserializerType.FindF( DEFAULT ) != KErrNotFound )
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerDefault );
        CleanupStack::PushL( des );
        des->SetInputBuffer(buf->Des());
        TRAP(err,des->DeserializeL());
        }
    else if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOP );
        CleanupStack::PushL( des );
        des->SetInputBuffer(buf->Des());
        TRAP(err,des->DeserializeL());
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOPInfoset );
        CleanupStack::PushL( des );
        des->SetInputBuffer(buf->Des());
        TRAP(err,des->DeserializeL());
        }
    else if( pDeserializerType.FindF( GZIP ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle,EDeserializerGZip );
        CleanupStack::PushL( des );
        des->SetInputBuffer(buf->Des());
        TRAP(err,des->DeserializeL());
        }
    	
    CleanupStack::PopAndDestroy( 5 );		
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;
    }    

// -----------------------------------------------------------------------------
// CLibxml2Tester::DeserializeFromFileL
// deserialize from file, convinience function
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::DeserializeFromFileL( CStifItemParser& aItem)
    {
    TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );

    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
     inputNum.Val(dirtyReturn);
     	
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOutputFile, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
	
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    
    if( pDeserializerType.FindF( DEFAULT ) != KErrNotFound )
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerDefault );
        CleanupStack::PushL( des );
        TRAP(err,des->DeserializeL( pInputFile ));
        }
    else if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle,EDeserializerXOP );
        CleanupStack::PushL( des );
        TRAP(err,des->DeserializeL( pInputFile ));
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle,EDeserializerXOPInfoset );
        CleanupStack::PushL( des );
        TRAP(err,des->DeserializeL( pInputFile ));
        }
    else if( pDeserializerType.FindF( GZIP ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerGZip );
        CleanupStack::PushL( des );
        RFs fs; 
        CleanupClosePushL(fs);
        User::LeaveIfError(fs.Connect());    				
        TRAP(err,des->DeserializeL( fs ,pInputFile ));			
        CleanupStack::PopAndDestroy(&fs);
        }
    	
    CleanupStack::PopAndDestroy( 4 );		
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;
    }
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::DeserializeFromBufferL
// deserialize from buffer, convinience function
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::DeserializeFromBufferL(CStifItemParser& aItem)
    {    
    TInt err;    
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );
    
    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex lexer (pDirtyReturn);
    TInt dirtyReturn;
    lexer.Val(dirtyReturn);
    
    HBufC8* buf = ReadFileL(pInputFile);
    CleanupStack::PushL(buf);
		
    RFile handleOutput;
    RFs rfOutput;
    rfOutput.Connect();
    CleanupClosePushL( rfOutput );
	
    User::LeaveIfError( handleOutput.Replace( rfOutput, pOutputFile, EFileWrite));
    CleanupClosePushL( handleOutput );	
		
    CTestHandler* testHandle = CTestHandler::NewLC( handleOutput );
    if( pDeserializerType.FindF( DEFAULT ) != KErrNotFound )
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerDefault );
        CleanupStack::PushL( des );
        TRAP(err,des->DeserializeL(buf->Des()));
        }
    else if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOP );
        CleanupStack::PushL( des );
        TRAP(err,des->DeserializeL(buf->Des()));
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOPInfoset );
        CleanupStack::PushL( des );
        TRAP(err,des->DeserializeL(buf->Des()));
        }
    else if( pDeserializerType.FindF( GZIP ) != KErrNotFound )     
        {
        CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerGZip );
        CleanupStack::PushL( des );
        TRAP(err,des->DeserializeL(buf->Des()));
        }

    CleanupStack::PopAndDestroy( 5 );
    
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;	
    }

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestDeserializerL
// loop test, deserialize from file, one deserializer instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::TestDeserializerL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOut, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
	
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, RetDeserializerType(pType) );
    CleanupStack::PushL( des );
    
    for (TInt i = 0; i < 20; i++ )
    	{
    	des->SetInputFileL(pDoc);
    	des->DeserializeL();
    	des->DeserializeL();
    	des->EnableFeature(	EReportNamespaces );
    	des->DeserializeL();
    	if( des->IsFeatureEnabled( EReportNamespaces )) 
    		{
    	   	des->DisableFeature( EReportNamespaces );
    		}
    		else User::Leave( KErrGeneral );
    	des->EnableFeature( EReportNamespaceMapping );
    	des->EnableFeature( EReportUnrecognisedTags );
    	}
	
	
	CleanupStack::PopAndDestroy( 4 );
	return KErrNone;	
	}
		
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSetInputFileL
// test setting input file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::TestSetInputFileL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOut, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
	
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, RetDeserializerType(pType) );
    CleanupStack::PushL( des );

   	des->SetInputFileL(pDoc);
   	des->DeserializeL();
    	
	CleanupStack::PopAndDestroy( 4 );
	return KErrNone;	
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSetInputBufferL
// test setting input buffer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::TestSetInputBufferL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	HBufC8* buf = ReadFileL(pDoc);
    CleanupStack::PushL(buf);
		
	RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOut, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
	
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, RetDeserializerType(pType) );
    CleanupStack::PushL( des );
    
   	des->SetInputBuffer(buf->Des());
   	des->DeserializeL();
	
	CleanupStack::PopAndDestroy( 5 );
	return KErrNone;	
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSetContentHandlerL
// test setting content handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::TestSetContentHandlerL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut1;
	aItem.GetNextString(pOut1);
	
	TPtrC pOut2;
	aItem.GetNextString(pOut2);
	

	RFile fileHandle_1;
	RFile fileHandle_2;
	
	// create session 1
    RFs fs_1;
    fs_1.Connect();
    CleanupClosePushL( fs_1 );
    // create session 2
	RFs fs_2;
    fs_2.Connect();
    CleanupClosePushL( fs_2 );
	
	User::LeaveIfError( fileHandle_1.Replace( fs_1, pOut1, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_1 );
	CTestHandler* testHandle_1 = CTestHandler::NewLC( fileHandle_1 );
	
	User::LeaveIfError( fileHandle_2.Replace( fs_2, pOut2, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_2 );
	CTestHandler* testHandle_2 = CTestHandler::NewLC( fileHandle_2 );
	
	CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle_1, RetDeserializerType(pType) );
	CleanupStack::PushL( des );
	des->DeserializeL( pDoc );
	des->SetContentHandlerL( *testHandle_2 );
	des->DeserializeL( fs_2, pDoc );
	
	CleanupStack::PopAndDestroy( 7 );	
	return KErrNone;	
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestEnableFeatureL
// test setter enable feature
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::TestEnableFeatureL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut1;
	aItem.GetNextString(pOut1);
	
	TPtrC pOut2;
	aItem.GetNextString(pOut2);
	

	RFile fileHandle_1;
	RFile fileHandle_2;
	
	// create session 1
    RFs fs_1;
    fs_1.Connect();
    CleanupClosePushL( fs_1 );
    // create session 2
	RFs fs_2;
    fs_2.Connect();
    CleanupClosePushL( fs_2 );
	
	User::LeaveIfError( fileHandle_1.Replace( fs_1, pOut1, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_1 );
	CTestHandler* testHandle_1 = CTestHandler::NewLC( fileHandle_1 );
	
	User::LeaveIfError( fileHandle_2.Replace( fs_2, pOut2, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_2 );
	CTestHandler* testHandle_2 = CTestHandler::NewLC( fileHandle_2 );
	
	CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle_1, RetDeserializerType(pType) );
	CleanupStack::PushL( des );
	des->DeserializeL( pDoc );
	des->EnableFeature( EReportNamespaceMapping );
	des->SetContentHandlerL( *testHandle_2 );
	des->DeserializeL( fs_2, pDoc );
	if(!des->IsFeatureEnabled(EReportNamespaceMapping)) User::Leave(KErrGeneral);
	
	CleanupStack::PopAndDestroy( 7 );	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestDisableFeatureL
// test setter disable feature
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::TestDisableFeatureL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut1;
	aItem.GetNextString(pOut1);
	
	TPtrC pOut2;
	aItem.GetNextString(pOut2);
	

	RFile fileHandle_1;
	RFile fileHandle_2;
	
	// create session 1
    RFs fs_1;
    fs_1.Connect();
    CleanupClosePushL( fs_1 );
    // create session 2
	RFs fs_2;
    fs_2.Connect();
    CleanupClosePushL( fs_2 );
	
	User::LeaveIfError( fileHandle_1.Replace( fs_1, pOut1, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_1 );
	CTestHandler* testHandle_1 = CTestHandler::NewLC( fileHandle_1 );
	
	User::LeaveIfError( fileHandle_2.Replace( fs_2, pOut2, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_2 );
	CTestHandler* testHandle_2 = CTestHandler::NewLC( fileHandle_2 );
	
	CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle_1, RetDeserializerType(pType) );
	CleanupStack::PushL( des );
	des->EnableFeature( EReportNamespaceMapping );
	des->DisableFeature( EReportNamespaceMapping );
	des->DisableFeature( 0x1111); //attempt an invalid feature
	if ( des->IsFeatureEnabled( EReportNamespaceMapping ) ) User::Leave(KErrGeneral);
	des->DeserializeL( pDoc );
	des->EnableFeature( EReportNamespaceMapping );
	des->SetContentHandlerL( *testHandle_2 );
	des->DisableFeature(EReportNamespaceMapping );
	des->DeserializeL( fs_2, pDoc );
	if(des->IsFeatureEnabled(EReportNamespaceMapping)) User::Leave(KErrGeneral);
	
	CleanupStack::PopAndDestroy( 7 );	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestIsFeatureEnabledL
// test is feature enabled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestIsFeatureEnabledL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	

	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	RFile fileHandle;
	
    RFs fs;
    fs.Connect();
    CleanupClosePushL( fs );
    
	User::LeaveIfError( fileHandle.Replace( fs, pOut, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle );
	CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
	
	CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, RetDeserializerType(pType) );
	CleanupStack::PushL( des );
	
	TBuf8<16> features = _L8("0000000000");

	if(des->IsFeatureEnabled( EConvertTagsToLowerCase )) features[0] = '1';
	if(des->IsFeatureEnabled( EErrorOnUnrecognisedTags )) features[1] = '1';
	if(des->IsFeatureEnabled( EReportUnrecognisedTags )) features[2] = '1';
	if(des->IsFeatureEnabled( EReportNamespaces )) features[3] = '1';
	if(des->IsFeatureEnabled( EReportNamespacePrefixes )) features[4] = '1';
	if(des->IsFeatureEnabled( ESendFullContentInOneChunk )) features[5] = '1';
	if(des->IsFeatureEnabled( EReportNamespaceMapping )) features[6] = '1';
	if(des->IsFeatureEnabled( ERawContent )) features[7] = '1';
	if(des->IsFeatureEnabled( EReplaceIntEntityFromExtSubsetByRef )) features[8] = '1';
	if(des->IsFeatureEnabled( EXmlVersion_1_1 )) features[9] = '1';
	if(des->IsFeatureEnabled( ELastFeature )) features[10] = '1';
	
	
	User::LeaveIfError( fileHandle.Write(features) );
	

	CleanupStack::PopAndDestroy( 4 );	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestUseExternalDataL
// test deserialize from file, using external data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestUseExternalDataL(CStifItemParser& aItem)
	{
	TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
	
	TPtrC pBinary;
	aItem.GetNextString( pBinary );
	
	TPtrC pFile;
	aItem.GetNextString(pFile);
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );
    
    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
     inputNum.Val(dirtyReturn);	
    
    TInt nContainers = 3;
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
	SetupDocumentL();
	  
    HBufC8* binbuf = ReadFileToBufferL(pBinary);
	CleanupStack::PushL(binbuf);
    
	
	TBufC<100> chunkName(_L("ChunkContainer") );
    TInt size = 2000;
    TInt maxSize = 10000;
    TInt offset = 0;
    TInt binarySize = CID_2().Length();
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);

	
	RFile fp;
	User::LeaveIfError( fp.Open(aRFs, pFile, EFileRead) );
	CleanupClosePushL(fp); 	
  
  
	
    TXmlEngBinaryContainer bincont = iDoc.CreateBinaryContainerL(CID_1(), binbuf->Des());
    TXmlEngChunkContainer chunkcont = iDoc.CreateChunkContainerL(CID_2(), chunk, offset, binarySize);
	TXmlEngFileContainer filecont = iDoc.CreateFileContainerL(CID_3(), fp);
		
	iDoc.DocumentElement().AppendChildL(bincont);
	iDoc.DocumentElement().AppendChildL(chunkcont);
	iDoc.DocumentElement().AppendChildL(filecont);
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOutputFile, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
	
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    RArray<TXmlEngDataContainer> list;
	CleanupClosePushL(list);	////
	iDoc.GetDataContainerList(list); 
	CXmlEngDeserializer* des;
    if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOP );
        CleanupStack::PushL( des );
        des->SetInputFileL( pInputFile );
        des->UseExternalDataL( list );
        TRAP(err,des->DeserializeL());
        
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOPInfoset );
        CleanupStack::PushL( des );
        des->SetInputFileL( pInputFile );
        des->UseExternalDataL( list );
        TRAP(err,des->DeserializeL());
        }
    if(list.Count() != nContainers) User::Leave(KErrGeneral);

    CleanupStack::PopAndDestroy( 8 );
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;

	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestExternalDataL
// test deserialize from file, drawing external data array
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestExternalDataL(CStifItemParser& aItem)
	{
	TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
	
	TPtrC pBinary;
	aItem.GetNextString( pBinary );
	
	TPtrC pFile;
	aItem.GetNextString(pFile);
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );
    
    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
     inputNum.Val(dirtyReturn);	
    
    TInt nContainers = 3;
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
	SetupDocumentL();
	  
    HBufC8* binbuf = ReadFileToBufferL(pBinary);
	CleanupStack::PushL(binbuf);
    
	
	TBufC<100> chunkName(_L("ChunkContainer") );
    TInt size = 2000;
    TInt maxSize = 10000;
    TInt offset = 0;
    TInt binarySize = CID_2().Length();
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);

	
	RFile fp;
	User::LeaveIfError( fp.Open(aRFs, pFile, EFileRead) );
	CleanupClosePushL(fp); 	
  
  
	
    TXmlEngBinaryContainer bincont = iDoc.CreateBinaryContainerL(CID_1(), binbuf->Des());
    TXmlEngChunkContainer chunkcont = iDoc.CreateChunkContainerL(CID_2(), chunk, offset, binarySize);
	TXmlEngFileContainer filecont = iDoc.CreateFileContainerL(CID_3(), fp);
		
	iDoc.DocumentElement().AppendChildL(bincont);
	iDoc.DocumentElement().AppendChildL(chunkcont);
	iDoc.DocumentElement().AppendChildL(filecont);
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOutputFile, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
	
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    RArray<TXmlEngDataContainer> list;
	CleanupClosePushL(list);	////
	iDoc.GetDataContainerList(list); 
	CXmlEngDeserializer* des = NULL;
    if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOP );
        CleanupStack::PushL( des );
        des->SetInputFileL( pInputFile );
        des->UseExternalDataL( list );
        TRAP(err,des->DeserializeL());
        
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        des = CXmlEngDeserializer::NewL( *testHandle, EDeserializerXOPInfoset );
        CleanupStack::PushL( des );
        des->SetInputFileL( pInputFile );
        des->UseExternalDataL( list );
        TRAP(err,des->DeserializeL());
        }
    if(list.Count() != nContainers) User::Leave(KErrGeneral);
	RArray<TXmlEngDataContainer>* fromDeserializer = des->ExternalData();
	
	for(TInt i = 0; i < nContainers; i++ )
		{
		if (fromDeserializer->operator[](i).NodeType() != list.operator[](i).NodeType() ) User::Leave( KErrGeneral );
		if (fromDeserializer->operator[](i).Size() != list.operator[](i).Size() ) User::Leave( KErrGeneral );
		}
    CleanupStack::PopAndDestroy( 8 );
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;

	}	

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestDesFileConvinienceL
// test convinience function, before use set input file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestDesFileConvinienceL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc1;
	aItem.GetNextString(pDoc1);
	
	TPtrC pDoc2;
	aItem.GetNextString(pDoc2);
		
	TPtrC pOut1;
	aItem.GetNextString(pOut1);
	
	TPtrC pOut2;
	aItem.GetNextString(pOut2);
	
	RFile fileHandle_1;
	RFile fileHandle_2;
	
	// create session 1
    RFs fs_1;
    fs_1.Connect();
    CleanupClosePushL( fs_1 );
    // create session 2
	RFs fs_2;
    fs_2.Connect();
    CleanupClosePushL( fs_2 );
	
	User::LeaveIfError( fileHandle_1.Replace( fs_1, pOut1, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_1 );
	CTestHandler* testHandle_1 = CTestHandler::NewLC( fileHandle_1 );
	
	User::LeaveIfError( fileHandle_2.Replace( fs_2, pOut2, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_2 );
	CTestHandler* testHandle_2 = CTestHandler::NewLC( fileHandle_2 );
	
	CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle_1, RetDeserializerType(pType) );
	CleanupStack::PushL( des );
	
   	des->SetInputFileL(pDoc2);
   	des->DeserializeL(pDoc1);
   	des->SetContentHandlerL( *testHandle_2 );
   	des->DeserializeL();   	
   	    	
	CleanupStack::PopAndDestroy( 7 );
	return KErrNone;	
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestDesBufferConvinienceL
// test convinience function, before use set input buffer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestDesBufferConvinienceL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc1;
	aItem.GetNextString(pDoc1);
	
	TPtrC pDoc2;
	aItem.GetNextString(pDoc2);
		
	TPtrC pOut1;
	aItem.GetNextString(pOut1);
	
	TPtrC pOut2;
	aItem.GetNextString(pOut2);
	
	HBufC8* buffer1 = ReadFileL(pDoc1);
    CleanupStack::PushL(buffer1);
    
    HBufC8* buffer2 = ReadFileL(pDoc2);
    CleanupStack::PushL(buffer2);
	
	RFile fileHandle_1;
	RFile fileHandle_2;
	
	// create session 1
    RFs fs_1;
    fs_1.Connect();
    CleanupClosePushL( fs_1 );
    // create session 2
	RFs fs_2;
    fs_2.Connect();
    CleanupClosePushL( fs_2 );
	
	User::LeaveIfError( fileHandle_1.Replace( fs_1, pOut1, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_1 );
	CTestHandler* testHandle_1 = CTestHandler::NewLC( fileHandle_1 );
	
	User::LeaveIfError( fileHandle_2.Replace( fs_2, pOut2, EFileStream | EFileWrite | EFileShareExclusive ) );
	CleanupClosePushL( fileHandle_2 );
	CTestHandler* testHandle_2 = CTestHandler::NewLC( fileHandle_2 );
	
	CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle_1, RetDeserializerType(pType) );
	CleanupStack::PushL( des );
	
   	des->SetInputFileL(pDoc2);
   	des->SetInputBuffer(buffer2->Des());
   	des->DeserializeL(buffer1->Des());
   	des->SetContentHandlerL( *testHandle_2 );
   	des->DeserializeL();   	
   	    	
	CleanupStack::PopAndDestroy( 9 );
	return KErrNone;	
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestDeserializerBufferL
// loop test, deserialize from buffer, one deserializer instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 		
TInt CLibxml2Tester::TestDeserializerBufferL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	HBufC8* buffer = ReadFileL(pDoc);
    CleanupStack::PushL(buffer);
	
	RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOut, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
	
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, RetDeserializerType(pType) );
    CleanupStack::PushL( des );

	des->SetInputBuffer(buffer->Des());
	    
    for (TInt i = 0; i < 20; i++ )
    	{
    	
    	des->DeserializeL();
    	des->DeserializeL();
    	des->EnableFeature(	EReportNamespaces );
    	des->DeserializeL();
    	if( des->IsFeatureEnabled( EReportNamespaces )) 
    		{
    	   	des->DisableFeature( EReportNamespaces );
    		}
    		else User::Leave( KErrGeneral );
    	des->EnableFeature( EReportNamespaceMapping );
    	des->EnableFeature( EReportUnrecognisedTags );
    	}
	
	
	CleanupStack::PopAndDestroy( 5 );
	return KErrNone;	
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSetParsingOptions1L
// test setting parsing options
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 		
TInt CLibxml2Tester::TestSetParsingOptions1L(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	HBufC8* buf = ReadFileL(pDoc);
    CleanupStack::PushL(buf);
	
	RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    User::LeaveIfError( fileHandle.Replace( aRFs, pOut, EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL( fileHandle );
	
    CTestHandler* testHandle = CTestHandler::NewLC( fileHandle );
    CXmlEngDeserializer* des = CXmlEngDeserializer::NewL( *testHandle, RetDeserializerType(pType) );
    CleanupStack::PushL( des );
    TXmlEngParsingOptions options(TXmlEngParsingOptions::KOptionEncodeToBase64);
    
	des->SetParsingOptions( options );
	des->SetInputBuffer(buf->Des());
   	des->DeserializeL();
    	
	CleanupStack::PopAndDestroy( 5 );
	return KErrNone;	
	
	}
	
// End of file
