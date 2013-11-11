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
#include <e32std.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengchunkcontainer.h>

#include "xmlengtesterdef.h"

#include <xml/dom/xmlengserializer.h>
#include <xml/dom/xmlengdeserializer.h>


#include <InetProtTextUtils.h>
#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>
#include <xml/dom/xmlengdomparser.h>


/************************************** SerXOP ****************************************/
// -----------------------------------------------------------------------------
// CLibxml2Tester::SaveXmlWithBinaryDataL
// saves xml file with added binary data, returns RDocument
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::SaveXmlWithBinaryDataL(RXmlEngDocument& iDocument, TPtrC serOptions, 
											TPtrC pData, TPtrC pXml, TPtrC pOut,TBool multiNode)
    {
   	
    //parse input xml file
    iDocument = parser.ParseFileL(pXml);	
	
    HBufC8* containerBuffer = ReadFileToBufferL(pData);
	CleanupStack::PushL( containerBuffer );
 		
    // check if input binary data file is empty or not	
    if ( containerBuffer->Length()  != 0 )
        {	
	//add binary container node to xml document
        if (!multiNode)
            {
		   //create binary container  
		   TXmlEngBinaryContainer binaryData = iDocument.CreateBinaryContainerL(CID_1(),containerBuffer->Des());
		   iDocument.DocumentElement().AppendChildL(binaryData);
		   }
	        else
	            {
	            //create binary container 1
	            TXmlEngBinaryContainer binaryData1 = iDocument.CreateBinaryContainerL(CID_1(),containerBuffer->Des());
	            		
			    //create binary container 2
	            TXmlEngBinaryContainer binaryData2 = iDocument.CreateBinaryContainerL(CID_2(),containerBuffer->Des());
	            		
	            //create binary container 3
	            TXmlEngBinaryContainer binaryData3 = iDocument.CreateBinaryContainerL(CID_3(),containerBuffer->Des());
					
	            TPtrC8 nodeName;
	            nodeName.Set(Node1);
	            TXmlEngNode currentNode = FindNodeL(iDocument,nodeName);
	            currentNode.AppendChildL(binaryData1);
					
	            nodeName.Set(Node2);
	            currentNode = FindNodeL(iDocument,nodeName);
	            currentNode.AppendChildL(binaryData2);
					
	            nodeName.Set(Node3);
	            currentNode = FindNodeL(iDocument,nodeName);
	            currentNode.AppendChildL(binaryData3);						
	            }
	        }
		
	        TInt saveResult( KErrNone );
	        if ( pOut.Size() )
	            {
	            saveResult = KErrGeneral;
			
	            if( serOptions != EMPTY_STRING() )
	                {
	              			    
	                TXmlEngSerializationOptions options ( GetHexFromPointerL( serOptions )); 	
	                saveResult = iDocument.SaveL(pOut, iDocument, options);
	                }
	            else 
	                {
	                TXmlEngElement root = iDocument.DocumentElement();
	                if(root.NotNull())
	                	{
	              	    saveResult = iDocument.SaveL( pOut );
					    }
			    	}
			 
			}
    
	CleanupStack::PopAndDestroy( 1 );
	if (saveResult >= 0)
		{
		return KErrNone;
		}
		else
			{
			return saveResult;
			}
		
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerialize1paraToFileL
// test "TInt SerializeL(const TNode aRoot);"
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CLibxml2Tester::TestSerialize1paraToFileL( RXmlEngDocument& iDocument, TPtrC pXml, TPtrC pSerOpt,TXmlEngSerializerType serializerType )
	{
	TInt error(KErrGeneral);
	
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(serializerType);
	//CDOMSerializer* serializer = CDOMSerializer::SerializerL();
	CleanupStack::PushL(serializer);
	
	
	// check if serialization options are avaliable 
	if( !pSerOpt.operator==(EMPTY_STRING) )
		{
		// set serialization options
		TXmlEngSerializationOptions options ( GetHexFromPointerL(pSerOpt ));
		
		// set output
		TFileName srcFileName( pXml );
		serializer->SetOutputL( srcFileName );	
		// set serialization options 
		serializer->SetSerializationOptions(options);
		// serialize
		error = serializer->SerializeL( iDocument );
		}
		else	
			// serialization without setting serialization options
			//
			{
			// set output
			TFileName srcFileName( pXml );
			serializer->SetOutputL( srcFileName );	
			// serialize
			error = serializer->SerializeL( iDocument );
			}
	
	CleanupStack::PopAndDestroy( 1 );
	
	return error;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerialize3paraToFileL
// test "TInt SerializeL(const TNode aRoot);"
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CLibxml2Tester::TestSerialize3paraToFileL( RXmlEngDocument& iDocument, TPtrC pXml, TPtrC pSerOpt, TXmlEngSerializerType serializerType )
	{
	TInt error(KErrGeneral);
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(serializerType);
	CleanupStack::PushL(serializer);
	
	
	// check if serialization options are avaliable 
	if( !pSerOpt.operator==(EMPTY_STRING) )
		{
		// set serialization options
		TXmlEngSerializationOptions options ( GetHexFromPointerL(pSerOpt ));
		
		// serialize
		error = serializer->SerializeL(pXml, iDocument, options);
		}
		else	
			// serialization without setting serialization options
			//
			{
			error = KErrArgument;
			}

	CleanupStack::PopAndDestroy( 1 );
	return error;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeRFsL
// test "TInt SerializeL(const TNode aRoot);"
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CLibxml2Tester::TestSerializeRFsL( RXmlEngDocument& iDocument, TPtrC pXml, TPtrC pSerOpt, TXmlEngSerializerType serializerType )
	{
	TInt error(KErrGeneral);
	
	// connecting to RFs
	RFs aRFs;
	User::LeaveIfError(aRFs.Connect());
   	CleanupClosePushL(aRFs);
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(serializerType);
	CleanupStack::PushL(serializer);
	
	
	// check if serialization options are avaliable 
	if( !pSerOpt.operator==(EMPTY_STRING) )
		{
		// set serialization options
		TXmlEngSerializationOptions options ( GetHexFromPointerL(pSerOpt ));
		
	
		// serialize
		error = serializer->SerializeL(aRFs, pXml, iDocument, options);
		}
		else	
			// serialization without setting serialization options
			//
			{
			error = KErrArgument;
			}
	
	CleanupStack::PopAndDestroy( 2 );
	return error;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSerializeToBufferL
// test "TInt SerializeL(const TNode aRoot);"
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CLibxml2Tester::TestSerializeToBufferL( RXmlEngDocument& iDocument, TPtrC pXml, TPtrC pSerOpt, TXmlEngSerializerType serializerType )
	{
	TInt nBytes;
	
	RBuf8 buffer;
	CleanupClosePushL( buffer );
	
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(serializerType);
	CleanupStack::PushL(serializer);
	
	
	TXmlEngSerializationOptions options ( GetHexFromPointerL(pSerOpt ));
	nBytes = serializer->SerializeL(buffer, iDocument, options);
	
	// serialize
	
	
	if (!nBytes)
		{
		User::Leave( KErrWrite );
		}
		
	TPtrC8 bufferPtr = buffer.Ptr();
	TInt bufferSize = buffer.Size();
	
	if ( bufferSize != nBytes )
		{
		User::Leave ( KErrWrite );
		}
	
	WriteFileFromBufferL(pXml,buffer);
	
	CleanupStack::PopAndDestroy( 2 );
	
	return nBytes;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeXopL
// test binary container method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CLibxml2Tester::SerializeXOPL(CStifItemParser& aItem)
	{

	TInt error(KErrGeneral);
	
	//	get test controls, input&output paths
	TPtrC p_Control;
	aItem.GetNextString( p_Control );
	
	TPtrC p_BinaryData;
 	aItem.GetNextString( p_BinaryData );
 	
	TPtrC p_XMLin;
 	aItem.GetNextString( p_XMLin );
 	
 	TPtrC p_XMLbefore;
 	aItem.GetNextString( p_XMLbefore );
 	 	
 	TPtrC p_XMLafter;
 	aItem.GetNextString( p_XMLafter );
 	
 	TPtrC p_SerializationOptions;
 	aItem.GetNextString( p_SerializationOptions );
 	
 
	// saves xml file with binary data added ( default serialize )
	RXmlEngDocument iDocument;
	CleanupClosePushL( iDocument );
	if( p_Control.FindF( SINGLE ) != KErrNotFound )
		{
		error = SaveXmlWithBinaryDataL(iDocument, p_SerializationOptions, p_BinaryData, p_XMLin, p_XMLbefore);
		
		}
		else if ( p_Control.FindF( MULTI ) != KErrNotFound )
			{
			error = SaveXmlWithBinaryDataL(iDocument, p_SerializationOptions, p_BinaryData, p_XMLin, p_XMLbefore,TRUE);
		
			}
			else
				{
				User::Leave(KErrArgument);	
				}
		
			
	if ( p_Control.FindF( ONE_PARA ) != KErrNotFound )
		{
		User::LeaveIfError( TestSerialize1paraToFileL( iDocument, p_XMLafter, p_SerializationOptions, ESerializerXOP ) );	
		}
	
	
	if ( p_Control.FindF( THREE_PARA ) != KErrNotFound )
		{ 						  
		User::LeaveIfError ( TestSerialize3paraToFileL( iDocument, p_XMLafter, p_SerializationOptions, ESerializerXOP) );
		}
	
	
	if ( p_Control.FindF( TO_RFS ) != KErrNotFound )
		{						  
		User::LeaveIfError( TestSerializeRFsL( iDocument, p_XMLafter, p_SerializationOptions, ESerializerXOP ) );
		}
		
			
	if ( p_Control.FindF( BUFFER ) != KErrNotFound )
		{						  				  	 					
		User::LeaveIfError( TestSerializeToBufferL( iDocument, p_XMLafter, p_SerializationOptions, ESerializerXOP ) );
		}


	
	// pop and destroy pushed objects
	CleanupStack::PopAndDestroy( 1 ); 
	
	return error;
	}
// -----------------------------------------------------------------------------
// CLibxml2Tester::DirtySerializeXopL
// test binary container method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CLibxml2Tester::DirtySerializeXOPL(CStifItemParser& aItem)
	{

	TInt error(KErrGeneral);
	 TPtrC p_Control;
	aItem.GetNextString( p_Control );
	
	TPtrC p_binaryData;
 	aItem.GetNextString( p_binaryData );
	 	
	TPtrC p_XMLin;
 	aItem.GetNextString( p_XMLin );
 	  	TPtrC p_XMLbefore;
 	aItem.GetNextString( p_XMLbefore );	 	
 	TPtrC p_XMLafter;
 	aItem.GetNextString( p_XMLafter );
 	
	// saves xml file with binary data added ( default serialize )
/*	TDOMImplementation iDomImpl = DOMImplementationSource::DOMImplementationL(KImplementationID);
         RDocument iDocument = iDomImpl.CreateDocumentL();
	CleanupClosePushL( iDocument );
		
        _LIT8( cid, "123456789@123456789" );
        _LIT( chunkName, "test chunk" );
    
        TInt size1 = 1000;
        TInt maxSize = 5000;
        TInt offset = 100;
        TInt binarySize = 500;
        TBool isReadOnly = EFalse;
        RChunk chunk;
        chunk.CreateGlobal(chunkName(), size1, maxSize);
        CleanupClosePushL(chunk);
	XmlEngine::DOM::TChunkContainer binData = iDocument.CreateChunkContainerL(
											cid, chunk,	offset, binarySize);
											
	XmlEngine::DOM::TElement el = iDocument.CreateDocumentElementL( "doc" );
	XmlEngine::DOM::TElement el2 = iDocument.CreateElementL("test1");
	el.AppendChildL(el2);										
         XmlEngine::DOM::TElement el3 = iDocument.CreateElementL("chunkcontainer");
         iDocument.DocumentElement().AppendChildL(el3); 									
	
        el3.AppendChildL(binData);
         RFs aRFs;
   	User::LeaveIfError(aRFs.Connect());
   	CleanupClosePushL(aRFs);
 	TInt size;
 	RFile fileHandle;
 	User::LeaveIfError(fileHandle.Open(aRFs, p_binaryData, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(fileHandle);
	User::LeaveIfError(fileHandle.Size(size));
	HBufC8* bufferData = HBufC8::NewLC(size);
	TPtr8 dataPtr = bufferData->Des();
	User::LeaveIfError(fileHandle.Read(dataPtr, size)); 
	
	XmlEngine::DOM::TBinaryContainer binaryData1 = iDocument.CreateBinaryContainerL(_L8("123456789@01223444"),dataPtr);
	XmlEngine::DOM::TElement el4 = iDocument.CreateElementL("binarycontainer");
         iDocument.DocumentElement().AppendChildL(el4); 							
	el4.AppendChildL(binaryData1);
*/
	RXmlEngDocument iDocument;
	CleanupClosePushL( iDocument );
    error = SaveXmlWithBinaryDataL(iDocument, _L("1"), p_binaryData, p_XMLin, p_XMLbefore);

		
    if ( p_Control.FindF( NONE ) == KErrNotFound )
        {

        			
	if ( p_Control.FindF( FILE ) != KErrNotFound )
		{
		//invalid output file
		//User::LeaveIfError( TestSerialize1paraToFileL( iDocument, p_XMLafter, p_SerializationOptions, ESerializerXOP ) );	
		}
			
	if ( p_Control.FindF( BUFFER ) != KErrNotFound )
		{
		//invalid output buffer						  				  	 					
		//User::LeaveIfError( TestSerializeToBufferL( iDocument, p_XMLafter, p_SerializationOptions, ESerializerXOP ) );
		}
        }
    	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(ESerializerXOP);
	//CDOMSerializer* serializer = CDOMSerializer::SerializerL();
	CleanupStack::PushL(serializer);
	
	// set output
	TFileName srcFileName( p_XMLafter);
	serializer->SetOutputL( srcFileName );	
	// serialize
	error = serializer->SerializeL( iDocument );
	iDocument.SaveL(_L("c:\\serxoptest.xml"));
	
	// pop and destroy pushed objects
	CleanupStack::PopAndDestroy( 3 ); 
	
	return error;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeXopSoapL
// serialize soap documents
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	

TInt CLibxml2Tester::SerializeXopSoapL(CStifItemParser& aItem)
	{
	TPtrC pDocument;
	aItem.GetNextString(pDocument);
	
	TPtrC pData;
	aItem.GetNextString(pData);
		
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
    iDoc = parser.ParseFileL(pDocument);

      
  	TXmlEngNode result = FindNodeL(iDoc, _L8("photo"));
  	
  	HBufC8* buffer = ReadFileToBufferL(pData);
	CleanupStack::PushL(buffer);
	
	TXmlEngBinaryContainer bincont = iDoc.CreateBinaryContainerL(CID_1(), buffer->Des());
	result.AppendChildL(bincont);
	
	// procesing
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(ESerializerXOP);
	CleanupStack::PushL(serializer);
	
	// set output
	TFileName srcFileName( pOut );
	serializer->SetOutputL( srcFileName );	
	
	// set serialization options
	TXmlEngSerializationOptions options ( TXmlEngSerializationOptions::KOptionDecodeBinaryContainers );
	serializer->SetSerializationOptions( options );
	TInt serializedBits;
	// serialize
	serializedBits = serializer->SerializeL( iDoc );
		
	CleanupStack::PopAndDestroy( 2 );
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::MultiSerializeXopSoapL
// Multi serialiazation soap documents
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	

TInt CLibxml2Tester::MultiSerializeXopSoapL(CStifItemParser& aItem)
	{
	TPtrC pDocument;
	aItem.GetNextString(pDocument);
	
	TPtrC pData1;
	aItem.GetNextString(pData1);
	
	TPtrC pData2;
	aItem.GetNextString(pData2);
		
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
    iDoc = parser.ParseFileL(pDocument);

      
  		
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
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(ESerializerXOP);
	CleanupStack::PushL(serializer);
	
	// set output
	TFileName srcFileName( pOut );
	serializer->SetOutputL( srcFileName );	
	
	// set serialization options
	TXmlEngSerializationOptions options ( TXmlEngSerializationOptions::KOptionDecodeBinaryContainers );
	serializer->SetSerializationOptions( options );
	
	// serialize
	serializer->SerializeL( iDoc );
		
	CleanupStack::PopAndDestroy( 3 );
	return KErrNone;
	}
// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeXopFileL
// serialization to file test function 
// specification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	

TInt CLibxml2Tester::SerializeXopFileL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString( pType );
		
	TPtrC pOption;
	aItem.GetNextString( pOption );
	
	TPtrC pData;
	aItem.GetNextString(pData);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
    iDoc = parser.ParseFileL(pDoc);
       
    // set serialization options
	TLex inputNum (pOption);
    TInt nOption;
    inputNum.Val(nOption);
    
    //open binary file
    HBufC8* buffer = ReadFileToBufferL(pData);
	CleanupStack::PushL( buffer );
 		
    //create binary container  
	TXmlEngBinaryContainer binaryData = iDoc.CreateBinaryContainerL(CID_1(),buffer->Des());
	iDoc.DocumentElement().AppendChildL(binaryData);
	
	// procesing
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	// set output
	TFileName srcFileName( pOut );
	serializer->SetOutputL( srcFileName );	
	
	// set serialization options
	TXmlEngSerializationOptions options ( nOption );
	serializer->SetSerializationOptions( options );
	
	// serialize
	TInt serializedBits = serializer->SerializeL( iDoc );
	// end processing
	
	CleanupStack::PopAndDestroy( 2 );
	return KErrNone;
	}
		
// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeXopFileDirtyL
// dirty functional serialization test function 
// specification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	

TInt CLibxml2Tester::SerializeXopFileDirtyL(CStifItemParser& aItem)
	{
	TInt nResult = KErrNone;
	
	TPtrC pData;
	aItem.GetNextString(pData);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	TPtrC pDirtyReturn;
	aItem.GetNextString(pDirtyReturn);
	
	TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
    inputNum.Val(dirtyReturn);
	
	iDoc = parser.ParseFileL(pDoc);
       
    //open binary file
    HBufC8* buffer = ReadFileToBufferL(pData);
	CleanupStack::PushL( buffer );
 		
    //create binary container  
	TXmlEngBinaryContainer binaryData = iDoc.CreateBinaryContainerL(CID_1(),buffer->Des());
	iDoc.DocumentElement().AppendChildL(binaryData);
	
	// procesing
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(ESerializerXOP);
	CleanupStack::PushL(serializer);
	
	// set output
	TFileName srcFileName( pOut );
	serializer->SetOutputL( srcFileName );	
	
	TInt serializedBits;
	// serialize
	TRAPD(trapResult, serializedBits = serializer->SerializeL( iDoc ) );
	
	CleanupStack::PopAndDestroy( 2 );
	if (trapResult == dirtyReturn ) nResult = KErrNone;
			else if (trapResult != KErrNone) nResult = trapResult;
				else nResult = KErrGeneral;
	
	
	return nResult;
	}
	
	// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeXopDirtySoapL
// special conditions serialization test function			
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	

TInt CLibxml2Tester::SerializeXopDirtySoapL(CStifItemParser& aItem)
	{
	TInt nResult = KErrNone;
	
	TPtrC pDocument;
	aItem.GetNextString(pDocument);
	
	TPtrC pData;
	aItem.GetNextString(pData);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	TPtrC pDirtyReturn;
	aItem.GetNextString(pDirtyReturn);
	
	TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
    inputNum.Val(dirtyReturn);
    
	iDoc = parser.ParseFileL(pDocument);
    
    
    TXmlEngNode result = FindNodeL(iDoc, _L8("photo"));
  	
  	HBufC8* buffer = ReadFileToBufferL(pData);
	CleanupStack::PushL(buffer);
	
	TXmlEngBinaryContainer bincont = iDoc.CreateBinaryContainerL(CID_1(), buffer->Des());
	result.AppendChildL(bincont);
    
    // procesing
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(ESerializerXOP);
	CleanupStack::PushL(serializer);
	
	// set output
	TFileName srcFileName( pOut );
	serializer->SetOutputL( srcFileName );	
	
		// set serialization options
	//TXmlEngSerializationOptions options ( TXmlEngSerializationOptions::KOptionDecodeBinaryContainers );
	TXmlEngSerializationOptions options ( TXmlEngSerializationOptions::KOptionStandalone );
	serializer->SetSerializationOptions( options );
	
	// serialize
	TRAPD(trapResult, serializer->SerializeL( iDoc ) );
	
	if (trapResult == dirtyReturn ) nResult = KErrNone;
			else if (trapResult != KErrNone) nResult = trapResult;
				else nResult = KErrGeneral;
				
	CleanupStack::PopAndDestroy( 2 );
	
	return nResult;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeXopBufferL
// serialization to buffer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	

TInt CLibxml2Tester::SerializeXopBufferL(CStifItemParser& aItem)
	{
	TPtrC pType;
	aItem.GetNextString( pType );
	
	TPtrC pOption;
	aItem.GetNextString( pOption );
	
	TPtrC pData;
	aItem.GetNextString(pData);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);

	
	iDoc = parser.ParseFileL(pDoc);
       
    // set serialization options
	TLex inputNum (pOption);
    TInt nOption;
    inputNum.Val(nOption);
    
    //open binary file
    HBufC8* containerBuffer = ReadFileToBufferL(pData);
	CleanupStack::PushL( containerBuffer );
 		
    //create binary container  
	TXmlEngBinaryContainer binaryData = iDoc.CreateBinaryContainerL(CID_1(),containerBuffer->Des());
	iDoc.DocumentElement().AppendChildL(binaryData);
	
	
	RBuf8 buffer;
	CleanupClosePushL( buffer );
	
	// procesing
	
	// create serializer 	
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(RetSerializerType(pType));
	CleanupStack::PushL(serializer);
	
	
	// set serialization options
	TXmlEngSerializationOptions options ( nOption );
		
	
	// serialize
	TInt nBytes = serializer->SerializeL(buffer, iDoc, options);
	// end processing
	
	if (!nBytes)
		{
		User::Leave( KErrWrite );
		}
		
	TPtrC8 bufferPtr = buffer.Ptr();
	TInt bufferSize = buffer.Size();
	
	if ( bufferSize != nBytes )
		{
		User::Leave ( KErrWrite );
		}
	
	WriteFileFromBufferL(pOut,buffer);
	
	CleanupStack::PopAndDestroy( 3 );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeXopBufferL
// serialization to file, XML contains chunk & binary & file containers
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//		
TInt CLibxml2Tester::Serxop3diffContL(CStifItemParser& aItem)
	{
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	
	TPtrC pBinary;
	aItem.GetNextString(pBinary);
	
	TPtrC pFile;
	aItem.GetNextString(pFile);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	iDoc = parser.ParseFileL(pDoc);	
	  
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
    
    RFs aRFs;
	User::LeaveIfError(aRFs.Connect());
	CleanupClosePushL(aRFs);
	
	RFile fp;
	User::LeaveIfError( fp.Open(aRFs, pFile, EFileRead) );
	CleanupClosePushL(fp); 	
  
  
	
    TXmlEngBinaryContainer bincont = iDoc.CreateBinaryContainerL(CID_1(), binbuf->Des());
    TXmlEngChunkContainer chunkcont = iDoc.CreateChunkContainerL(CID_2(), chunk, offset, binarySize);
	TXmlEngFileContainer filecont = iDoc.CreateFileContainerL(CID_3(), fp);
		
	
	TXmlEngNode aNode = FindNodeL(iDoc,_L8("bincont"));
	aNode.AppendChildL(bincont);
	
	aNode = FindNodeL(iDoc,_L8("chunkcont"));
	aNode.AppendChildL(chunkcont);
		
	aNode = FindNodeL(iDoc,_L8("filecont"));
	aNode.AppendChildL(filecont);
	
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL(ESerializerXOP);
	//CXmlEngSerializer* serializer = CXmlEngSerializer::NewL();
	CleanupStack::PushL(serializer);
	
	// set output
	TFileName srcFileName( pOut );
	serializer->SetOutputL( srcFileName );	
	
	// set serialization options
	TXmlEngSerializationOptions options ( 	TXmlEngSerializationOptions::KOptionIndent  );
	serializer->SetSerializationOptions( options );
	
	// serialize
	serializer->SerializeL( iDoc );
		
	CleanupStack::PopAndDestroy( 5 );
	return KErrNone;
    
	}

// End of file
