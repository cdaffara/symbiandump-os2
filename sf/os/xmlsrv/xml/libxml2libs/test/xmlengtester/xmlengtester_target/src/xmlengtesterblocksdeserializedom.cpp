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

#include <xml/dom/xmlengdeserializer.h>
#include <xml/dom/xmlengdeserializerdom.h>
#include "xmlengtesterdef.h"

#include <InetProtTextUtils.h>
#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>

#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengdatacontainer.h>


TInt CLibxml2Tester::DOMDeserializeL( CStifItemParser& aItem)
    {
    TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
	
    TPtrC pBinary;
   TPtrC pFile;
   if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
        }
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );
    
    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
     inputNum.Val(dirtyReturn);	
        
//    TInt nContainers = 3;
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
    RArray<TXmlEngDataContainer> list;
    if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	
	
    RXmlEngDocument doc;
    	
    if( pDeserializerType.FindF( DEFAULT ) != KErrNotFound )
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        des->SetInputFileL( pInputFile );
        TRAP(err,doc=des->DeserializeL());
        
        }
    else if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        des->SetInputFileL( pInputFile,EDeserializerXOP);
        TRAP(err,doc=des->DeserializeL());
        
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        des->UseExternalDataL( list );
        des->SetInputFileL( pInputFile,EDeserializerXOPInfoset );
        TRAP(err,doc=des->DeserializeL());
        }    
    else if( pDeserializerType.FindF( GZIP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        des->SetInputFileL( pInputFile,EDeserializerGZip);
        TRAP(err,doc=des->DeserializeL());
        }
      
    	SaveDocumentL(doc,pOutputFile);
        
    if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {   
        CleanupStack::PopAndDestroy( 6 );
        }
    else
        {
        CleanupStack::PopAndDestroy( 2 );      
        }
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;
    }
    
TInt CLibxml2Tester::DOMDeserialize2L( CStifItemParser& aItem)
    {
    TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
   TPtrC pBinary;
   TPtrC pFile;
   if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
        }
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );

    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
     inputNum.Val(dirtyReturn);
     	
//    TInt nContainers = 3;
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
    RArray<TXmlEngDataContainer> list;
    if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	
	
     HBufC8* buf = ReadFileL(pInputFile);
    CleanupStack::PushL(buf);
    RXmlEngDocument doc;
    if( pDeserializerType.FindF( DEFAULT ) != KErrNotFound )
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        des->SetInputBuffer(buf->Des());
        TRAP(err,doc=des->DeserializeL());
        }
    else if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL( );
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        des->SetInputBuffer(buf->Des(),EDeserializerXOP);
        TRAP(err,doc=des->DeserializeL());
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
         des->UseExternalDataL( list );
        des->SetInputBuffer(buf->Des(),EDeserializerXOPInfoset);
        TRAP(err,doc=des->DeserializeL());
        }
    else if( pDeserializerType.FindF( GZIP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
         
        des->SetInputBuffer(buf->Des(),EDeserializerGZip);
        TRAP(err,doc=des->DeserializeL());
        
        
        }

    	SaveDocumentL(doc,pOutputFile);
    	
     if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {   
        CleanupStack::PopAndDestroy( 7 );
        }
    else
        {
        CleanupStack::PopAndDestroy( 3 );      
        }
    		
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;
    }    

TInt CLibxml2Tester::DOMDeserializeFromFileL( CStifItemParser& aItem)
    {
    TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
    
   TPtrC pBinary;
   TPtrC pFile;
   if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
        }
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );

    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
     inputNum.Val(dirtyReturn);
     	
//       TInt nContainers = 3;
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    RArray<TXmlEngDataContainer> list;
    if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	

    RXmlEngDocument doc;
    
    if( pDeserializerType.FindF( DEFAULT ) != KErrNotFound )
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL( pInputFile ));
        }
    else if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL( );
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL( pInputFile,EDeserializerXOP ));
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
     des->UseExternalDataL( list );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL( pInputFile,EDeserializerXOPInfoset ));
        }
    else if( pDeserializerType.FindF( GZIP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);				
        TRAP(err,doc=des->DeserializeL(pInputFile , EDeserializerGZip));			
        }
    		SaveDocumentL(doc,pOutputFile);
         if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {   
        CleanupStack::PopAndDestroy( 6 );
        }
    else
        {
        CleanupStack::PopAndDestroy( 2 );      
        }		
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;
    }
    
TInt CLibxml2Tester::DOMDeserializeFromFile2L( CStifItemParser& aItem)
    {
    TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
    
      TPtrC pBinary;
   TPtrC pFile;
   if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
        }
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );

    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex inputNum (pDirtyReturn);
    TInt dirtyReturn;
     inputNum.Val(dirtyReturn);
     	
//       TInt nContainers = 3;
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
    RArray<TXmlEngDataContainer> list;
    if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	
	
    RXmlEngDocument doc;
    
    if( pDeserializerType.FindF( DEFAULT ) != KErrNotFound )
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL( aRFs,pInputFile ));
        }
    else if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL( );
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL( aRFs,pInputFile,EDeserializerXOP ));
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
         des->UseExternalDataL( list );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL( aRFs,pInputFile,EDeserializerXOPInfoset ));
        }
    else if( pDeserializerType.FindF( GZIP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);				
        TRAP(err,doc=des->DeserializeL(aRFs,pInputFile , EDeserializerGZip));			
        }
  	SaveDocumentL(doc,pOutputFile);  	
         if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {   
        CleanupStack::PopAndDestroy( 6 );
        }
    else
        {
        CleanupStack::PopAndDestroy( 2 );      
        }		
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;
    }    

TInt CLibxml2Tester::DOMDeserializeFromBufferL(CStifItemParser& aItem)
    {    
    TInt err;    
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
    
   TPtrC pBinary;
   TPtrC pFile;
   if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
        }
	
    TPtrC pOutputFile;
    aItem.GetNextString( pOutputFile );
    
    TPtrC pDirtyReturn;
    aItem.GetNextString( pDirtyReturn );
    
    TLex lexer (pDirtyReturn);
    TInt dirtyReturn;
     lexer.Val(dirtyReturn);
    
    HBufC8* buf = ReadFileL(pInputFile);
    CleanupStack::PushL(buf);
		
//       TInt nContainers = 3;
    RFile fileHandle;
    RFs aRFs;
    aRFs.Connect();
    CleanupClosePushL( aRFs );
	
    RArray<TXmlEngDataContainer> list;
    if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	
	

    RXmlEngDocument doc;
		
    if( pDeserializerType.FindF( DEFAULT ) != KErrNotFound )
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL(buf->Des()));
        }
    else if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL(buf->Des(),EDeserializerXOP));
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseExternalDataL( list );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL(buf->Des(), EDeserializerXOPInfoset ));
        }
    else if( pDeserializerType.FindF( GZIP ) != KErrNotFound )     
        {
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        TRAP(err,doc=des->DeserializeL(buf->Des(),EDeserializerGZip));
        }
	SaveDocumentL(doc,pOutputFile);
         if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )   
        {   
        CleanupStack::PopAndDestroy( 7 );
        }
    else
        {
        CleanupStack::PopAndDestroy( 3 );      
        }
    
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;	
    }

TInt CLibxml2Tester::TestDOMDeserializerL(CStifItemParser& aItem)
	{
	// multiple deserialization in one row
	TPtrC pType;
	aItem.GetNextString(pType);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	        TPtrC pBinary;
        TPtrC pFile;
        if( pType.FindF( INFOSET ) != KErrNotFound )   
            {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
            }
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
//  TInt nContainers = 3;
        RFile fileHandle;
        RFs aRFs;
        aRFs.Connect();
        CleanupClosePushL( aRFs );
	
        RArray<TXmlEngDataContainer> list;
        if( pType.FindF( INFOSET ) != KErrNotFound )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	
	

        RXmlEngDocument doc;
	
    CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
    CleanupStack::PushL( des );
    des->UseDOMImplementationL( DOM_impl);
    for (TInt i = 0; i < 20; i++ )
    	{
    	des->SetInputFileL(pDoc, RetDeserializerType(pType));
    	    if( pType.FindF( INFOSET ) != KErrNotFound )   
            {
            des->UseExternalDataL( list );
            }
    	doc=des->DeserializeL();
    	doc=des->DeserializeL();
    	}
    	
    	SaveDocumentL(doc,pOut);
    	
	   if( pType.FindF( INFOSET ) != KErrNotFound )   
            {
            CleanupStack::PopAndDestroy( 6 );
            }
            else
            {
            CleanupStack::PopAndDestroy( 2 );    
            }
	return KErrNone;	
	}
		

TInt CLibxml2Tester::TestDOMSetInputFileL(CStifItemParser& aItem)
        {
        TPtrC pType;
        aItem.GetNextString(pType);
	
        TPtrC pDoc;
        aItem.GetNextString(pDoc);
        
        TPtrC pBinary;
        TPtrC pFile;
        if( pType.FindF( INFOSET ) != KErrNotFound )   
            {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
            }
	
        TPtrC pOut;
        aItem.GetNextString(pOut);
		
//        TInt nContainers = 3;
        RFile fileHandle;
        RFs aRFs;
        aRFs.Connect();
        CleanupClosePushL( aRFs );
	
        RArray<TXmlEngDataContainer> list;
        if( pType.FindF( INFOSET ) != KErrNotFound )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	
	

        RXmlEngDocument doc;
        
       
        CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
        if( pType.FindF( INFOSET ) != KErrNotFound )   
            {
            des->UseExternalDataL( list );
            }
        des->SetInputFileL(pDoc, RetDeserializerType(pType) );
        doc=des->DeserializeL();
        SaveDocumentL(doc,pOut);
       
        if( pType.FindF( INFOSET ) != KErrNotFound )   
            {
            CleanupStack::PopAndDestroy( 6 );
            }
            else
            {
            CleanupStack::PopAndDestroy( 2 );    
            }
        return KErrNone;	
        }

TInt CLibxml2Tester::TestDOMSetInputBufferL(CStifItemParser& aItem)
        {
        TPtrC pType;
        aItem.GetNextString(pType);
	
        TPtrC pDoc;
        aItem.GetNextString(pDoc);
        
        TPtrC pBinary;
        TPtrC pFile;
        if( pType.FindF( INFOSET ) != KErrNotFound )   
            {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
            }
        	
        TPtrC pOut;
        aItem.GetNextString(pOut);
	
        HBufC8* buf = ReadFileL(pDoc);
        CleanupStack::PushL(buf);
		
//             TInt nContainers = 3;
        RFile fileHandle;
        RFs aRFs;
        aRFs.Connect();
        CleanupClosePushL( aRFs );
	
        RArray<TXmlEngDataContainer> list;
        if( pType.FindF( INFOSET ) != KErrNotFound )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	
	

        RXmlEngDocument doc;
	
       CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
       CleanupStack::PushL( des );
       des->UseDOMImplementationL( DOM_impl);
               if( pType.FindF( INFOSET ) != KErrNotFound )   
            {
            des->UseExternalDataL( list );
            }
       des->SetInputBuffer(buf->Des(), RetDeserializerType(pType));
       doc=des->DeserializeL();
       SaveDocumentL(doc,pOut);
          if( pType.FindF( INFOSET ) != KErrNotFound )   
            {
            CleanupStack::PopAndDestroy( 7 );
            }
            else
            {
            CleanupStack::PopAndDestroy( 3 );    
            }
       return KErrNone;	
       }

TInt CLibxml2Tester::TestDOMUseExternalDataL(CStifItemParser& aItem)
	{
 TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
	
    TPtrC pBinary;
   TPtrC pFile;
   if( (pDeserializerType.FindF( INFOSET ) != KErrNotFound ) || (pDeserializerType.FindF( XOP ) != KErrNotFound )  )   
        {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
        }
	
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
    RArray<TXmlEngDataContainer> list;
    if( (pDeserializerType.FindF( INFOSET ) != KErrNotFound ) || (pDeserializerType.FindF( XOP ) != KErrNotFound ) )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	
	
    RXmlEngDocument doc;
    	
         CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
    if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        des->UseExternalDataL( list );
        des->SetInputFileL( pInputFile,EDeserializerXOP);
        TRAP(err,doc=des->DeserializeL());
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        des->UseExternalDataL( list );
        des->SetInputFileL( pInputFile,EDeserializerXOPInfoset );
        TRAP(err,doc=des->DeserializeL());
        }    
          
    if(list.Count() != nContainers) User::Leave(KErrGeneral);
    
    SaveDocumentL(doc,pOutputFile);
		
    CleanupStack::PopAndDestroy( 6 ); 
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;

	}	
TInt CLibxml2Tester::TestDOMExternalDataL(CStifItemParser& aItem)
	{
 TInt err;
    TPtrC pDeserializerType;
    aItem.GetNextString( pDeserializerType );
	
    TPtrC pInputFile;
    aItem.GetNextString( pInputFile );
	
    TPtrC pBinary;
   TPtrC pFile;
   if( (pDeserializerType.FindF( INFOSET ) != KErrNotFound ) || (pDeserializerType.FindF( XOP ) != KErrNotFound )  )   
        {
        aItem.GetNextString( pBinary );
        aItem.GetNextString(pFile);	
        }
	
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
    RArray<TXmlEngDataContainer> list;
    if( (pDeserializerType.FindF( INFOSET ) != KErrNotFound ) || (pDeserializerType.FindF( XOP ) != KErrNotFound ) )   
        {
        SetupDocumentL();	  
        HBufC8* binbuf = ReadFileToBufferL(pBinary);
        CleanupStack::PushL(binbuf);
        TBufC<100> chunkName(_L("ChunkContainer") );
        TInt size = 2000;
        TInt maxSize = 10000;
        TInt offset = 0;
        TInt binarySize = CID_2().Length();
//        TBool isReadOnly = EFalse;
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
        CleanupClosePushL(list);	////
        iDoc.GetDataContainerList(list); 
        }	
	
    RXmlEngDocument doc;
    	
         CXmlEngDeserializerDOM* des = CXmlEngDeserializerDOM::NewL();
        CleanupStack::PushL( des );
        des->UseDOMImplementationL( DOM_impl);
    if( pDeserializerType.FindF( XOP ) != KErrNotFound )     
        {
        des->UseExternalDataL( list );
        des->SetInputFileL( pInputFile,EDeserializerXOP);
        TRAP(err,doc=des->DeserializeL());
        }
    else if( pDeserializerType.FindF( INFOSET ) != KErrNotFound )     
        {
        des->UseExternalDataL( list );
        des->SetInputFileL( pInputFile,EDeserializerXOPInfoset );
        TRAP(err,doc=des->DeserializeL());
        }    
          
    if(list.Count() != nContainers) User::Leave(KErrGeneral);
    
    RArray<TXmlEngDataContainer>* fromDeserializer = des->ExternalData();
    
    if(fromDeserializer->Count() != nContainers) User::Leave(KErrGeneral);	
    	for(TInt i = 0; i < nContainers; i++ )
		{
		if (fromDeserializer->operator[](i).NodeType() != list.operator[](i).NodeType() ) User::Leave( KErrGeneral );
		if (fromDeserializer->operator[](i).Size() != list.operator[](i).Size() ) User::Leave( KErrGeneral );
		}
//    fromDeserializer->Close();

    SaveDocumentL(doc,pOutputFile);
		
    CleanupStack::PopAndDestroy( 6 ); 
    if ( err == dirtyReturn ) return KErrNone;		
    else return err;

	}	

// End of file
