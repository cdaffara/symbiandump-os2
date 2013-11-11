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
#include "xmlengtester.h"

#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <xml/dom/xmlengserializer.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include "xmlengtesterdef.h"
#include "TestFileOutputStream.h"

#include <InetProtTextUtils.h>
#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>

#include <EZDecompressor.h>
#include <EZCompressor.h>
#include <EZGzip.h>


/************************************** Serializer ***************************************/

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializerL
// loop test Serializer constructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::TestSerializerL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	
	// document setup
	SetupDocumentL();
	   
    
    
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TXmlEngSerializationOptions options ( nOption );
	
	TBuf8<16> buf = _L8("test data");
	TBuf8<32> cid;
	TXmlEngBinaryContainer bincont;
	
	TInt nIteration = 20;
	for(TInt i = 0; i < nIteration; i++ )
		{
		cid.Delete(0, 31);
		GenerateRandomCid(cid);
		bincont = iDoc.CreateBinaryContainerL(cid, buf );
		iDoc.DocumentElement().AppendChildL(bincont.CopyL());
		serializer->SerializeL(pOut, iDoc, nOption);
			
		}
	
	TInt nResult = KErrNone;

	RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
	TInt nList = list.Count();   
	if( nList != nIteration*2 ) nResult = KErrGeneral; 
	// nIteration*2 becouse "bincont.CopyL()" produces copy of containrer
	// what cause increment container list
	
	CleanupStack::PopAndDestroy( 2 );
	
	return nResult;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSetOutputBufferL
// test setting output buffer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
TInt CLibxml2Tester::TestSetOutputBufferL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	// document setup
	SetupDocumentL();
	
	
	RBuf8 buffer;
	CleanupClosePushL( buffer );
	
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	// set output
	serializer->SetOutput( buffer );	
	
	
	// serialize
	TInt nBytes = serializer->SerializeL( iDoc );
	// end processing
	
	if (!nBytes)
		{
		User::Leave( KErrWrite );
		}
		
	TInt bufferSize = buffer.Size();
	
	if ( bufferSize != nBytes )
		{
		User::Leave ( KErrTotalLossOfPrecision );
		}
	
	WriteFileFromBufferL(pOut,buffer);
	
	CleanupStack::PopAndDestroy( 2 );
	return KErrNone;
		
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSetOutputFileL
// test setting output file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   	
TInt CLibxml2Tester::TestSetOutputFileL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);

	TPtrC pOut;
	aItem.GetNextString(pOut);

	
	// document setup
	SetupDocumentL();
	
		
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	// set output
	serializer->SetOutputL( pOut );	
	
	
	// serialize
	TRAPD( trapResult, serializer->SerializeL( iDoc ); );
	// end processing
	
	CleanupStack::PopAndDestroy( 1 );
	if ( !trapResult )return KErrNone;
		else return trapResult;
	}
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSetOutputStreamL
// test setting output stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  	
TInt CLibxml2Tester::TestSetOutputStreamL(CStifItemParser& aItem)
	{
	
	TPtrC pType;
	aItem.GetNextString(pType);
	
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	// document setup
	SetupDocumentL();
	
	RFs aRFs;
	User::LeaveIfError(aRFs.Connect());
	CleanupClosePushL(aRFs);
	
	
	RFile outFile;	
	User::LeaveIfError ( outFile.Replace( aRFs, pOut, EFileWrite ) );
	CleanupClosePushL( outFile );
	
	CTestFileOutputStream stream(outFile,aRFs);
	CleanupClosePushL( stream );
	
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	

	// set output
	serializer->SetOutput( stream );	
	
	// set serialization options
	TXmlEngSerializationOptions options ( 	TXmlEngSerializationOptions::KOptionIndent  );
	serializer->SetSerializationOptions( options );
	
	// serialize
	TRAPD( trapResult, serializer->SerializeL( iDoc ); );
	// end processing
	
	CleanupStack::PopAndDestroy( 4 );
	
	if ( !trapResult )return KErrNone;
		else return trapResult;
	}
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeFile1L
// test serialize to file, using setter SetOutputL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestSerializeFile1L(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	SetupDocumentL();
	

	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	TXmlEngSerializationOptions options ( nOption );
	serializer->SetSerializationOptions( options );	
	
	// set output
	serializer->SetOutputL( pOut );
		
	// serialize1
	serializer->SerializeL( iDoc );
	
	CleanupStack::PopAndDestroy( 1 );
	return KErrNone;
		
	}
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeFile2L
// test convinience serialize to file 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 		
TInt CLibxml2Tester::TestSerializeFile2L(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	SetupDocumentL();
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	TXmlEngSerializationOptions options ( nOption );
	
	// serialize2
	serializer->SerializeL(pOut, iDoc, options );
	
	CleanupStack::PopAndDestroy( 1 );
	return KErrNone;
		
	}
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeFile3L
// test convinience serialize to file with file stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 		
TInt CLibxml2Tester::TestSerializeFile3L(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	SetupDocumentL();
	
	RFs aRFs;
	User::LeaveIfError(aRFs.Connect());
	CleanupClosePushL(aRFs);
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	TXmlEngSerializationOptions options ( nOption );
	
	
	// serialize
	serializer->SerializeL( aRFs, pOut, iDoc, options );
	
	CleanupStack::PopAndDestroy( 2 );
	return KErrNone;
		
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeBufferL
// test convinience serialize to buffer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestSerializeBufferL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	SetupDocumentL();
	
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	
	RBuf8 buffer;
	CleanupClosePushL( buffer );
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TXmlEngSerializationOptions options ( nOption );
		
	// serialize
	TInt nBytes = serializer->SerializeL(buffer, iDoc, options);
	
	
	// serialize
	if (!nBytes)
		{
		User::Leave( KErrWrite );
		}
		
	TInt bufferSize = buffer.Size();
	
	if ( bufferSize != nBytes )
		{
		User::Leave ( KErrWrite );
		}

	WriteFileFromBufferL(pOut,buffer);
	
	CleanupStack::PopAndDestroy( 2 );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializationOptionsL
// loop test serialize to file, same Serializer instance,
// different serialization options
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::TestSerializationOptionsL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);

	TBuf<256> start = pOut.Ptr();
	start.Delete(pOut.Length(),start.Length());
	TBuf<4> number = _L("1234");
	TBuf<4> extension = _L(".out");
	SetupDocumentL();
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	

	for(TInt i = 1; i < 5; i++)
		{
		// set serialization options
		TXmlEngSerializationOptions options ( i );
		
		start.Append(number[i-1]);
		start.Append(extension);
		
		// set output
		serializer->SetOutputL(start.Left(start.Length()));
		
		// serialize1
		serializer->SerializeL( iDoc );
		start.Delete(pOut.Length(),start.Length());
		}
	CleanupStack::PopAndDestroy( 1 );
	return KErrNone;
		
	}
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializePartTreeL
// serialize part of XML tree to file
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestSerializePartTreeL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	SetupDocumentL();
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	TXmlEngSerializationOptions options ( nOption );
	
	// set output
	serializer->SetOutputL( pOut );
		
	// serialize1
	serializer->SerializeL( iDoc.DocumentElement().FirstChild().NextSibling() );
	
	CleanupStack::PopAndDestroy( 1 );
	return KErrNone;
		
	}
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeFileBufferL
// test file than buffer serialization (mix setters)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 		
TInt CLibxml2Tester::TestSerializeFileBufferL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);

	SetupDocumentL();
	
	RBuf8 buffer;
	CleanupClosePushL( buffer );
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	TBuf8<16> buf = _L8("test data");
	TBuf8<32> cid;
	
	GenerateRandomCid(cid);
	TXmlEngBinaryContainer bincont = iDoc.CreateBinaryContainerL(cid, buf );
	iDoc.DocumentElement().AppendChildL(bincont.CopyL());
	
	// set output file
	serializer->SetOutputL(pOut);
		
	// serialize1
	serializer->SerializeL( iDoc );
	
	
	// set output bufer
	serializer->SetOutput( buffer );
	// serialize to buffer
	serializer->SerializeL( iDoc );
	
	HBufC8* fileBuffer = ReadFileToBufferL(pOut);
	CleanupStack::PushL( fileBuffer );
	
	if (pType.Compare(_L("gzip")))
	{
		if( buffer.Compare( fileBuffer->Des() )) User::Leave( KErrGeneral );
	}
	
	CleanupStack::PopAndDestroy( 3 );
	return KErrNone;
	
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeChildsL
// serialize XOP when container has child nodes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestSerializeChildsL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
		
	TPtrC pOut;
	aItem.GetNextString(pOut);

	SetupDocumentL();
	
	RBuf8 buffer;
	CleanupClosePushL( buffer );
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	TBuf8<16> buf = _L8("test data");
	
	TXmlEngBinaryContainer bincont = iDoc.CreateBinaryContainerL(CID_1(), buf );
	TXmlEngElement elem_1 = iDoc.CreateElementL(_L8("Child_1"));
	TXmlEngElement elem_2 = iDoc.CreateElementL(_L8("Child_2"));
	bincont.AppendChildL(elem_1);
	bincont.AppendChildL(elem_2);
	iDoc.DocumentElement().AppendChildL(bincont.CopyL());

	// set output bufer
	serializer->SetOutput( buffer );
	// serialize to buffer
	serializer->SerializeL( iDoc );
	
	WriteFileFromBufferL(pOut,buffer);
		
	CleanupStack::PopAndDestroy( 2 );
	return KErrNone;
	
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeFileConvinienceL
// serialize to file convinience, after set different output by setter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestSerializeFileConvinienceL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pOut1;
	aItem.GetNextString(pOut1);
	
	TPtrC pOut2;
	aItem.GetNextString(pOut2);
		
	SetupDocumentL();
	
	RBuf8 buffer;
	CleanupClosePushL( buffer );
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	TXmlEngSerializationOptions options ( nOption );
	
	
		// set output buffer
	serializer->SetOutput( buffer );
	
		// set output file
	serializer->SetOutputL( pOut2 );
				
	// serialize1
	serializer->SerializeL( pOut1, iDoc, options);
	
	//serialize 2
	
	serializer->SerializeL( iDoc );
	
	CleanupStack::PopAndDestroy( 2 );
	return KErrNone;
		
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeBufferConvinienceL
// serialize to buffer convinience, after set different output by setter
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::TestSerializeBufferConvinienceL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	TPtrC pOut2;
	aItem.GetNextString(pOut2);
	
	SetupDocumentL();
	
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	
	RBuf8 buffer1;
	CleanupClosePushL( buffer1 );
	
	RBuf8 buffer2;
	CleanupClosePushL( buffer2 );
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TXmlEngSerializationOptions options ( nOption );
	
	// set output
	serializer->SetOutputL( pOut );
		
	// set output
	serializer->SetOutput( buffer2 );	
		
	// serialize
	TInt nBytes = serializer->SerializeL(buffer1, iDoc, options);
	
	
	// serialize
	if (!nBytes)
		{
		User::Leave( KErrWrite );
		}
		
	TInt bufferSize = buffer1.Size();
	
	if ( bufferSize != nBytes )
		{
		User::Leave ( KErrWrite );
		}

	WriteFileFromBufferL(pOut,buffer1);
	
	nBytes = serializer->SerializeL( iDoc );
	
	if (!nBytes)
		{
		User::Leave( KErrWrite );
		}
		
	TInt bufferSize2 = buffer1.Size();
	
	if ( bufferSize2 != nBytes )
		{
		User::Leave ( KErrWrite );
		}

	WriteFileFromBufferL(pOut2,buffer2);
	
	CleanupStack::PopAndDestroy( 3 );
	
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestMultipleSerializeToBufferL
// loop test serialize to buffer (one Serializer instance)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CLibxml2Tester::TestMultipleSerializeToBufferL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	
	// document setup
	SetupDocumentL();
	   
    
	RBuf8 buffer;
	CleanupClosePushL( buffer );
    
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TXmlEngSerializationOptions options ( nOption );
	
	
	// set output
	serializer->SetOutput( buffer );	
	
	TBuf8<16> buf = _L8("test data");
	TBuf8<32> cid;
	TXmlEngBinaryContainer bincont;
	
	TInt nIteration = 20;
	for(TInt i = 0; i < nIteration; i++ )
		{
		cid.Delete(0, 31);
		GenerateRandomCid(cid);
		bincont = iDoc.CreateBinaryContainerL(cid, buf );
		iDoc.DocumentElement().AppendChildL(bincont.CopyL());
		
		serializer->SerializeL(iDoc);
			
		}
	
	TInt nResult = KErrNone;

	RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
	TInt nList = list.Count();   
	if( nList != nIteration*2 ) nResult = KErrGeneral; 
	// nIteration*2 becouse "bincont.CopyL()" produces copy of containrer
	// what cause increment container list
	
	
	// save faile from buffer
	WriteFileFromBufferL(pOut,buffer);
	
	CleanupStack::PopAndDestroy( 3 );
	
	return nResult;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializationOptionsBufferL
// loop test, serialize to buffer, one serializer instance, 
// different serialization options
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 	
TInt CLibxml2Tester::TestSerializationOptionsBufferL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);

	iDoc = parser.ParseFileL(pDoc);
	
	TBuf<256> start = pOut.Ptr();
	start.Delete(pOut.Length(),start.Length());
	TBuf<4> number = _L("1234");
	TBuf<4> extension = _L(".out");
	
	
	RFs aRFs;
    User::LeaveIfError(aRFs.Connect());
    CleanupClosePushL(aRFs);
	RFile fOut;
	RBuf8 buffer;
	CleanupClosePushL( buffer );
    
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set output
	serializer->SetOutput(buffer);
	
	for(TInt i = 1; i < 5; i++)
		{
		// set serialization options
		TXmlEngSerializationOptions options ( i );
		
		start.Append(number[i-1]);
		start.Append(extension);
	
		
		
		// serialize1
		serializer->SerializeL( iDoc );
		User::LeaveIfError ( fOut.Replace( aRFs, start.Left(start.Length()), EFileWrite ) );
		CleanupClosePushL( fOut );
		User::LeaveIfError ( fOut.Write(buffer ) );
		start.Delete(pOut.Length(),start.Length());
		CleanupStack::PopAndDestroy( &fOut );
		}
	
	CleanupStack::PopAndDestroy( 3 );
	return KErrNone;
		
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestContentTypeL
// test if element Content-Type is included in each MIME_boundary
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 		
TInt CLibxml2Tester::TestContentTypeL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC optionPtr;
	aItem.GetNextString(optionPtr);
	
	TPtrC pDocument;
	aItem.GetNextString(pDocument);
	
	TPtrC pData1;
	aItem.GetNextString(pData1);
	
	TPtrC pData2;
	aItem.GetNextString(pData2);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	TLex inputNum (optionPtr);
    TInt nOption;
    inputNum.Val(nOption);	
	
	iDoc = parser.ParseFileL(pDocument);
	
	RBuf8 buffer;
	CleanupClosePushL( buffer );
	
		
  	HBufC8* buffer1 = ReadFileToBufferL(pData1);
	CleanupStack::PushL(buffer1);
	
	HBufC8* buffer2 = ReadFileToBufferL(pData2);
	CleanupStack::PushL(buffer2);
	
	TXmlEngNode aNode = FindNodeL(iDoc,_L8("photo"));
	TXmlEngBinaryContainer bincont1 = iDoc.CreateBinaryContainerL(CID_1(), buffer1->Des());
	aNode.AppendChildL(bincont1);
	
	aNode = FindNodeL(iDoc,_L8("sig"));
	TXmlEngBinaryContainer bincont2 = iDoc.CreateBinaryContainerL(CID_2(), buffer2->Des());
	aNode.AppendChildL(bincont2);
	
	// procesing
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TXmlEngSerializationOptions options ( nOption );
		
	
	// serialize
	TInt nBytes = serializer->SerializeL(buffer, iDoc, options);
		

	// serialize
	if (!nBytes)
		{
		User::Leave( KErrWrite );
		}
		
	TInt bufferSize = buffer.Size();
	
	if ( bufferSize != nBytes )
		{
		User::Leave ( KErrWrite );
		}

	WriteFileFromBufferL(pOut,buffer);
	
	
	
	for (TInt i = 0; i < 3; i++)
		{
		if(buffer.FindC(_L8("Content-Type:")) == KErrNotFound)
			{
			TestModuleIf().Printf( infoNum++,KTEST_NAME,KContentTypeError);
     		iLog->Log((TDesC)KContentTypeError);
        	return KErrNotFound;
			}
		}
	CleanupStack::PopAndDestroy( 4 );
	
	return KErrNone;
	}

// End of file
