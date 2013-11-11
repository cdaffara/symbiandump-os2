/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include "TestBufferManager.h"
#include "xmlengtester.h"

#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>


#include <xml/dom/xmlengserializer.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include "xmlengtesterdef.h"

#include <InetProtTextUtils.h>
#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>

#include <EZDecompressor.h>
#include <EZCompressor.h>
#include <EZGzip.h>


/************************************** SerGZIP ***************************************/
// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeGZIPL
// test GZIP Serialize
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::SerializeGZIPL(CStifItemParser& aItem)
    {
    TInt error(KErrGeneral);
	
    //get test controls, input&output paths
    TPtrC pControl;
    aItem.GetNextString( pControl );
    
    TPtrC pBinary;
    aItem.GetNextString( pBinary );
 	
    TPtrC pXml;
    aItem.GetNextString( pXml );
 	
    TPtrC pSerialized;
    aItem.GetNextString( pSerialized );
 	 	
    TPtrC pGzipSerialized;
    aItem.GetNextString( pGzipSerialized );
 	
    TPtrC pOptions;
    aItem.GetNextString( pOptions );
 	
    // saves xml file with binary data added ( default serialize )
    RXmlEngDocument iDocument;
    CleanupClosePushL( iDocument );
    if( pControl.FindF( SINGLE ) != KErrNotFound )
        {
        error = SaveXmlWithBinaryDataL( iDocument, pOptions, pBinary, pXml, EMPTY_STRING() );
        }
    else if ( pControl.FindF( MULTI ) != KErrNotFound )
        {
        error = SaveXmlWithBinaryDataL( iDocument, pOptions,  pBinary, pXml, EMPTY_STRING(), TRUE );	
        }
    else
        {
        User::Leave(KErrArgument);	
        }
								
    if ( pControl.FindF( ONE_PARA ) != KErrNotFound )
        {
        User::LeaveIfError( TestSerialize1paraToFileL( iDocument, pGzipSerialized, pOptions, ESerializerGZip ) );	
        }
		
    if ( pControl.FindF( THREE_PARA ) != KErrNotFound )
        { 						  
        User::LeaveIfError ( TestSerialize3paraToFileL( iDocument, pGzipSerialized, pOptions, ESerializerGZip ) );
        }
		
    if ( pControl.FindF( TO_RFS ) != KErrNotFound )
        {						  
        User::LeaveIfError( TestSerializeRFsL( iDocument, pGzipSerialized, pOptions, ESerializerGZip ) );
        }
					
    if ( pControl.FindF( BUFFER ) != KErrNotFound )
        {						  				  	 					
        User::LeaveIfError( TestSerializeToBufferL( iDocument, pGzipSerialized, pOptions, ESerializerGZip ) );
        }
    CleanupStack::PopAndDestroy( 1 );
    return error;	
    }

// -----------------------------------------------------------------------------
// CLibxml2Tester::DecompressGZIPL
// test GZIP Decompress
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::DecompressGZIPL(CStifItemParser& aItem)
    {
    TBool unzipLoop = TRUE;
	
    TPtrC pInput;
    aItem.GetNextString( pInput );
		
    TPtrC pOutput;
    aItem.GetNextString( pOutput );
    HBufC8* bufferOutput = CnvUtfConverter::ConvertFromUnicodeToUtf8L(pOutput);
    CleanupStack::PushL( bufferOutput );
    TPtr8 pOutput8 = bufferOutput->Des();
    	
    RFs aRFs;
    User::LeaveIfError(aRFs.Connect());
    CleanupClosePushL(aRFs);
   	
    RFile fileHandle;
    User::LeaveIfError(fileHandle.Replace(aRFs, pOutput, EFileRead | EFileWrite));
    CleanupClosePushL(fileHandle);
     
	
    CEZGZipToFile* decompressFile = CEZGZipToFile::NewLC(aRFs, pInput, fileHandle);
	
    while ( unzipLoop )
        {
        unzipLoop = decompressFile->InflateL();
        }		
    
	
    CleanupStack::PopAndDestroy( 4 );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLibxml2Tester::DecompressBufferGZIPL
// test GZIP Decompress
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::DecompressBufferGZIPL(CStifItemParser& aItem)
    {
    TPtrC pInput;
    aItem.GetNextString( pInput );
		
    TPtrC pOutput;
    aItem.GetNextString( pOutput );
    HBufC8* bufferOutput = CnvUtfConverter::ConvertFromUnicodeToUtf8L(pOutput);
    CleanupStack::PushL( bufferOutput );
    TPtr8 pOutput8 = bufferOutput->Des();
  
   	
    HBufC8* buffer = ReadFileToBufferL( pInput );
    CleanupStack::PushL( buffer );
    
   // CVtcpBufferManager::NewL( in.Ptr() ,in.Length());

    
        CTestBufferManager* bm = CTestBufferManager::NewLC(  buffer->Ptr(), buffer->Size() );
    CEZDecompressor* decompressor = CEZDecompressor::NewLC( *bm );
    
//    decompressor->DecompressL( pOutput8, buffer->Des() );
	
while ( decompressor->InflateL() )
        {
        /* empty */
        }

TPtrC8 out=bm->OutputData();

	RFs aRFs;
    User::LeaveIfError(aRFs.Connect());
    CleanupClosePushL(aRFs);
    
	RFile fOut;
	User::LeaveIfError ( fOut.Replace( aRFs, pOutput, EFileWrite ) );
	CleanupClosePushL( fOut );
	User::LeaveIfError ( fOut.Write(out ) );
	

//WriteFileFromBufferL(pOutput,out)
	
    CleanupStack::PopAndDestroy( 6 );
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CLibxml2Tester::CompressBufferGZIPL
// test GZIP Compress to buffer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::CompressBufferGZIPL(CStifItemParser& aItem)
    {
   	_LIT(tem,"Error in code");
	
	// resize heap
	he = UserHeap::ChunkHeap(&KNullDesC(), 0, 20000000);
    if(!he)
        {
        User::Leave(100);
        }
    oldHeap = User::SwitchHeap(he);// Change heaps
    cleanup=CTrapCleanup::New(); 
    //--
    TInt trapResult;
    
    // start trap
	TRAP(trapResult,

    TPtrC pInput;
    aItem.GetNextString( pInput );
		
    TPtrC pOutput;
    aItem.GetNextString( pOutput );
    HBufC8* bufferOutput = CnvUtfConverter::ConvertFromUnicodeToUtf8L(pOutput);
    CleanupStack::PushL( bufferOutput );
    TPtr8 pOutput8 = bufferOutput->Des();
  
   	
    HBufC8* buffer = ReadFileToBufferL( pInput );
    CleanupStack::PushL( buffer );
    
   // CVtcpBufferManager::NewL( in.Ptr() ,in.Length());

    
        CTestBufferManager* bm = CTestBufferManager::NewLC(  buffer->Ptr(), buffer->Size() );
    CEZCompressor* compressor = CEZCompressor::NewLC( *bm );
    
//    decompressor->DecompressL( pOutput8, buffer->Des() );
	
while ( compressor->DeflateL() )
        {
        /* empty */
        }

TPtrC8 out=bm->OutputData();

	RFs aRFs;
    User::LeaveIfError(aRFs.Connect());
    CleanupClosePushL(aRFs);
    
	RFile fOut;
	User::LeaveIfError ( fOut.Replace( aRFs, pOutput, EFileWrite ) );
	CleanupClosePushL( fOut );
	User::LeaveIfError ( fOut.Write(out ) );
	

//WriteFileFromBufferL(pOutput,out)
	
    CleanupStack::PopAndDestroy( 6 );
    );
    // end trap
    
    
    if (trapResult)
        User::Panic(tem,trapResult);

	
	// delete cheap
	delete cleanup; // destroy clean-up stack
    User::SwitchHeap(oldHeap); // Change to old heap
    he->Close();
    he = NULL;  
	// --
	return KErrNone;
    }


// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeDirtyGzipL
// test serialize to file GZIP, error code trapping
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
 TInt CLibxml2Tester::SerializeDirtyGzipL(CStifItemParser& aItem)
 	{
 	TInt nResult = KErrNone;
	
	TPtrC pDocument;
	aItem.GetNextString(pDocument);
		
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	TPtrC pDirtyReturn;
	aItem.GetNextString(pDirtyReturn);
	
	TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
    inputNum.Val(dirtyReturn);
    
    
    TRAPD(trapResult,
	iDoc = parser.ParseFileL(pDocument);
        
    // procesing
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(ESerializerGZip);
	CleanupStack::PushL(serializer);
	
	// set output
	TFileName srcFileName( pOut );
	serializer->SetOutputL( srcFileName );	
	
	// set serialization options
	TXmlEngSerializationOptions options ( TXmlEngSerializationOptions::KOptionStandalone );
	serializer->SetSerializationOptions( options );
	
	// serialize
	 serializer->SerializeL( iDoc );
	 CleanupStack::PopAndDestroy( 1 );
	);
	
	if (trapResult == dirtyReturn ) nResult = KErrNone;
			else if (trapResult != KErrNone) nResult = trapResult;
				else nResult = KErrGeneral;
				
	
	
	return nResult;
 	}
// End of file
