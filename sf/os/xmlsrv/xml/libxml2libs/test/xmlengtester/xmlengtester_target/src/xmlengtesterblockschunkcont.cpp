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
//#include "XmlEngDOMChunkParser.h"

#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengfilecontainer.h>

#include <xml/dom/xmlengnode.h>
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <Xml\Parser.h>
#include <Xml\ParserFeature.h>

#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>

using namespace Xml;

/************************************** CHUNK ****************************************/
// -----------------------------------------------------------------------------
// CLibxml2Tester::ParseFileChunkL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ParseFileChunkL( CStifItemParser& aItem )
    {
    _LIT(KSourceFileErrorName,"No source file name.");
    _LIT(KEndParsing,"Parsing end.");
    
    TPtrC SrcFile;
    TPtrC tmp;
//    TBool map = TRUE;
    if ( aItem.GetNextString(SrcFile) != KErrNone )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KSourceFileErrorName);
        iLog->Log((TDesC)KSourceFileErrorName);
        return KErrGeneral;
        }
    TPtrC pChunkSize;
    aItem.GetNextString( pChunkSize );
    
    TLex lexer (pChunkSize);
    TInt chunkSize;
     lexer.Val(chunkSize);
     
    TFileName srcFileName;
    srcFileName.Copy(SrcFile);

	RXmlEngDOMParser chunkparser;
    chunkparser.Open(DOM_impl);
    CleanupClosePushL(chunkparser);
    TInt err;
    
    
    TRAP(err,iDoc = chunkparser.ParseFileL(srcFileName,chunkSize));
	
    if(err == KXmlEngErrParsing)
        {
        iLastError = chunkparser.GetLastParsingError();	
        }
    else
        {
        iLastError = err;		
        }   
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(&chunkparser);
    return KErrNone;    
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_GetListL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_GetListL( CStifItemParser& /*aItem*/ )
    {
    //checks how array of nodes is filled with data, and realloc if list > iDoc->dataNodeMax-1
    _LIT(KChunkError,"Chunk Error GetList.");
    _LIT(KEndParsing,"Parsing end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 200;
    TInt maxSize = 1000;
    TInt offset = 0;
    TInt binarySize = cid.Length();
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    TInt multiplicity = 19;
    for(TInt i = 0; i < multiplicity; i++)								
        {
        TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
        TXmlEngNode ref = iDoc.DocumentElement().AppendChildL(binData);
        if(!ref.IsSameNode(binData))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
            iLog->Log((TDesC)KChunkError);
            CleanupStack::PopAndDestroy();//chunk
            return KErrGeneral;
            }
        if( ref.AsChunkContainer().Cid() != cid)
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
            iLog->Log((TDesC)KChunkError);
            CleanupStack::PopAndDestroy();// chunk
            return KErrGeneral;   
            }
        }
   
   RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    //TInt maxsize = iDoc->dataNodeMax; ???????
    if(count != multiplicity)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk
        return KErrGeneral;  
        }    
    for(TInt i = 0; i < count; i++)								
        {
        if( list[i].Cid() != cid)
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
            iLog->Log((TDesC)KChunkError);
            CleanupStack::PopAndDestroy(2);//list, chunk
            return KErrGeneral;  
            }            
       if( list[i].AsChunkContainer().Chunk().Name() != chunk.Name() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
            iLog->Log((TDesC)KChunkError);
            CleanupStack::PopAndDestroy(2);//list, chunk
            return KErrGeneral;  
            }
        if( list[i].AsChunkContainer().ChunkOffset() != offset )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
            iLog->Log((TDesC)KChunkError);
            CleanupStack::PopAndDestroy(2);//list, chunk
            return KErrGeneral;  
            }
        if( list[i].Size() != binarySize )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
            iLog->Log((TDesC)KChunkError);
            CleanupStack::PopAndDestroy(2);//list, chunk
            return KErrGeneral;  
            }		
        }
    
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(2);//list, chunk
    return KErrNone;    
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_CidL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_CidL( CStifItemParser& /*aItem*/ )
    {
    //check working of Cid() for ChunkContainer
    _LIT(KChunkError,"Chunk Cid() Error.");
//    _LIT(KChunkError2,"Chunk Cid() Error with binary content.");
    _LIT(KEndParsing,"Checking end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 0;
    TInt binarySize = cid.Length();
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
									cid,	 chunk,		 offset,	 binarySize);        
    
    TXmlEngNode ref = iDoc.DocumentElement().AppendChildL(binData);
    
    TPtrC8 fetchedCid = binData.Cid();
    if (fetchedCid !=cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk   
        return KErrGeneral;       
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy();//chunk
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_CidL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_CidErrorL( CStifItemParser& /*aItem*/ )
    {
    //create 2 cid with the same name, do some operation on tree ->ReplaceWith, 
    _LIT(KChunkError,"Chunk Cid() Error.");
    _LIT(KEndParsing,"Checking Cid() end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid2(_L8("133456789@123456789") );
    TBufC8<100> cid3(_L8("100000000@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 0;
    TInt binarySize = cid.Length();
    TInt offset2 = binarySize;
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk); 
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunk container"));
    iDoc.DocumentElement().AppendChildL(el);
    
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
									cid,	 chunk,		 offset,	 binarySize);        
    
    el.AppendChildL(binData);
    TPtrC8 fetchedCid = binData.Cid();
    if (fetchedCid !=cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk   
        return KErrGeneral;       
        }
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
									cid,	 chunk,		 offset2,	 binarySize);        
    
    el.AppendChildL(binData2);
    TPtrC8 fetchedCid2 = binData2.Cid();
    if (fetchedCid2 !=cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk  
        return KErrGeneral;       
        }
        
    el.AppendChildL(binData); // this nothing does to tree
    
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);

    binData2.ReplaceWith(binData3);	//by replacing, removes cid-matching chunks from list 
                                                                            	//and appends new chunk to list								
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
   if(count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk ,list 
        return KErrGeneral;       
        }  
    TPtrC8 fetchedCid3 = binData3.Cid();
    if (fetchedCid3 !=cid3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list  
        return KErrGeneral;       
        }   
                        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//chunk,list
    
    return KErrNone;         
    }       
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_ChunkL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_ChunkL( CStifItemParser& /*aItem*/ )
    {
    //check working of Chunk() for ChunkContainer
    _LIT(KChunkError,"Chunk Chunk() Error.");
    _LIT(KEndParsing,"Checking end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 0;
    TInt binarySize = cid.Length();
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);      
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
										cid,	 chunk,	 offset, binarySize);
    RChunk fetchedChunk = binData.Chunk();
    TBuf<100> fetchedName = fetchedChunk.Name();
    if (fetchedName != chunkName)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        return KErrGeneral;       
        }
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy();//chunk
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_ChunkOffsetL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_OffsetL( CStifItemParser& /*aItem*/ )
    {
    //check working of ChunkOffset() for ChunkContainer
    _LIT(KChunkError,"Chunk ChunkOffset() Error.");
    _LIT(KEndParsing,"Checking end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 0;
    TInt binarySize = cid.Length();
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);      
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
										cid,	 chunk,	 offset, binarySize);
    if (binData.ChunkOffset() != offset)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        return KErrGeneral;       
        }
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy();//chunk
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_SizeL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_SizeL( CStifItemParser& /*aItem*/ )
    {
    //check working of Size() for ChunkContainer
    _LIT(KChunkError,"Chunk ChunkOffset() Error.");
    _LIT(KEndParsing,"Checking end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 0;
    TInt binarySize = cid.Length();
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);      
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
										cid,	 chunk,	 offset, binarySize);
    if (binData.Size() != binarySize)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
         CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;       
        }
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy();//chunk
    
    return KErrNone;         
    }   
 
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_NodeTypeL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_NodeTypeL( CStifItemParser& /*aItem*/ )
    {
    //check working of NodeType() for ChunkContainer
    _LIT(KChunkError,"Chunk NodeType() Error.");
    _LIT(KEndParsing,"Checking end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 0;
    TInt binarySize = cid.Length();
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);      
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
										cid,	 chunk,	 offset, binarySize);
    if (binData.NodeType() != TXmlEngNode::EChunkContainer) //XmlEngine::DOM::TNodeType
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        return KErrGeneral;       
        }
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy();//chunk
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_RemoveL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_RemoveL( CStifItemParser& /*aItem*/ )
    {
    //check working of Removing ChunkContainer
    //1. input 3 chunks, removes second, check if cid of 1 and 3 comapare with test data - returns 2 chunks in list
    _LIT(KChunkError,"Chunk Remove Error.");
    _LIT(KEndParsing,"Checking RemoveChunk end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    // (1.) removes chunk with matching cid
    binData1.Remove();								
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    if ( (list[0].Cid() != cid) && (list[0].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    if ( (list[1].Cid() != cid) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//chunk,list
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_Remove2L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_Remove2L( CStifItemParser& /*aItem*/ )
    {
    //check working of Removing ChunkContainer
    //2. input third chunk with the same cid as first chunk, remove it - returns 2 chunks in list
    _LIT(KChunkError,"Chunk Remove Error - containers with the same cids removed.");
    _LIT(KEndParsing,"Checking RemoveChunk end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
   //XmlEngine::DOM::TElement  element = iDoc.CreateDocumentElementL("doc");
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    
    // (2.) input chunk with the same cid as first one
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData3);
    // removes chunk
    binData3.Remove();
    RArray<TXmlEngDataContainer> list2;
    CleanupClosePushL(list2);
    iDoc.GetDataContainerList(list2);
    TInt count2 = list2.Count();
    if ( count2 != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
   
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//chunk,list2
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_Remove3L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_Remove3L( CStifItemParser& /*aItem*/ )
    {
    //check working of Removing ChunkContainer
    //3. input some other data and move it before chunk container end then remove some chunk from list
    _LIT(KChunkError,"Chunk Remove Error.");
    _LIT(KEndParsing,"Checking RemoveChunk end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
   //XmlEngine::DOM::TElement  element = iDoc.CreateDocumentElementL("doc");
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    // (3.) some other data 
    _LIT(KInputFile1, "f:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg"); 
    _LIT8(KCid, "111");      
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    TXmlEngBinaryContainer container = iDoc.CreateBinaryContainerL(KCid(), *str1);   
    TXmlEngElement el2 = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el2);
    el2.AppendChildL(container); 
    //move binary container before chunk container
    //get last
    TXmlEngNode node = elem.LastChild();	
    //move last node before first
    node.MoveBeforeSibling(elem.FirstChild());
    
    //remove chunk
    binData2.Remove();
    RArray<TXmlEngDataContainer> list3;
    CleanupClosePushL(list3);
    iDoc.GetDataContainerList(list3);
    TInt count3 = list3.Count();
    if ( count3 != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk,list3,str1
        return KErrGeneral;         
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(3);//chunk,str1,list3
    
    return KErrNone;         
    }   

    
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_UnlinkL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_UnlinkL( CStifItemParser& /*aItem*/ )
    {
    //check working of Unlinking ChunkContainer
    //1. input 3 chunks, unlink one, list.count must be 2
    _LIT(KChunkError,"Chunk Unlink Error.");
    _LIT(KEndParsing,"Checking UnlinkChunk end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    // (1.)unlink one chunk
    TXmlEngNode unlinked1=binData1.Unlink();
    if(unlinked1.AsChunkContainer().Cid() != cid1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    if( unlinked1.AsChunkContainer().Chunk().Name() != chunk.Name() )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    if( unlinked1.AsChunkContainer().ChunkOffset() != offset )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    if( unlinked1.AsChunkContainer().Size() != binarySize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    									
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//chunk,list,
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_ReplaceWithL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_ReplaceWithL( CStifItemParser& /*aItem*/ )
    {
    //check working of Replacing ChunkContainer with another
    //1. input 3 chunks, replace second one (cid1) with 4 (cid3)
    _LIT(KChunkError,"Chunk ReplaceWith Error.");
    _LIT(KEndParsing,"Checking ReplaceWithkChunk end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    // (1.)replace second with 4
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
     
    binData1.ReplaceWith(binData3);	//by replacing, removes  chunk from list 
								//and appends new chunk to list
  									
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    if( (list[0].Cid() == cid1) || (list[1].Cid() == cid1) || (list[2].Cid() == cid1))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;
        }
 
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//chunk,list,
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_SubstitiuteForL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_SubstituteForL( CStifItemParser& /*aItem*/ )
    {
    //check working of Substitiuting chunk containers
    //1. input 3 chunks,substitiute second for 4
    _LIT(KChunkError,"Chunk SubstituteFor Error.");
    _LIT(KEndParsing,"Checking SubstituteForkChunk end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    // (1.)substitute for
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    
    TXmlEngNode unlinked1 = binData1.SubstituteForL(binData3);	
										//substituting doesn't remove cid-matching chunks from list 
										//but appends new chunk to list					
  if(unlinked1.AsChunkContainer().Cid() != cid1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;         
        }
    if( unlinked1.AsChunkContainer().Chunk().Name() != chunk.Name() )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;         
        }
    if( unlinked1.AsChunkContainer().ChunkOffset() != offset )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;         
        }
    if( unlinked1.AsChunkContainer().Size() != binarySize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;         
        }
    //unlinked1.Remove();     
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    if(  (list[0].Cid() == cid1) || (list[1].Cid() == cid1) || (list[2].Cid() == cid1))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;
        }
  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//chunk,list,
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_CopyL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_CopyL( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //1. copy container, check its data: cid, chunk...
    _LIT(KChunkError,"Chunk Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
     iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    // (1.)copy container, list will be 3
    
    TXmlEngNode copyOfBinData1 = binData1.CopyL();								
  if(copyOfBinData1.AsChunkContainer().Cid() != cid1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;         
        }
    if( copyOfBinData1.AsChunkContainer().Chunk().Name() != chunk.Name() )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;         
        }
    if( copyOfBinData1.AsChunkContainer().ChunkOffset() != offset )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;         
        }
    if( copyOfBinData1.AsChunkContainer().Size() != binarySize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;         
        }
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,list
        return KErrGeneral;         
        }
    //after append, list should be 4
    el.AppendChildL(copyOfBinData1);	   
    RArray<TXmlEngDataContainer> list2;
    CleanupClosePushL(list2);
    iDoc.GetDataContainerList(list2);
    TInt count2 = list2.Count();
    if ( count2 != 4)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk,list,list2
        return KErrGeneral;         
        }
              TInt i=0;
        TInt j=0;
        for (i=0;i<count2;i++ )
            {
                if( list2[i].Cid() == cid1)  j++;

            }

     if( j!=2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk,doc,list2
        return KErrGeneral;
        }	
    	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(3);//chunk,list,list3
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_Copy2L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_Copy2L( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //2. copy node with chunk containers, check its element data
    _LIT(KChunkError,"Chunk Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
     iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    CleanupStack::PopAndDestroy();
    
    TXmlEngElement elCopy = el.CopyL().AsElement();
  
    if(!elCopy.HasChildNodes())
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if(elCopy.FirstChild().AsChunkContainer().Cid() != cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if(elCopy.FirstChild().AsChunkContainer().Chunk().Name() != chunk.Name() )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.FirstChild().AsChunkContainer().ChunkOffset() != offset )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.FirstChild().AsChunkContainer().Size() != binarySize )	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.LastChild().AsChunkContainer().Cid() != cid2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.LastChild().AsChunkContainer().Chunk().Name() != chunk.Name() )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.LastChild().AsChunkContainer().ChunkOffset() != offset )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.LastChild().AsChunkContainer().Size() != binarySize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy();//chunk
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_Copy3L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_Copy3L( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //3. copy node with chunk containers to another RDocument
    _LIT(KChunkError,"Chunk Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
     iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    TXmlEngElement elCopy = el.CopyL().AsElement();								
   
    RXmlEngDocument doc;
    doc.OpenL(DOM_impl);
    CleanupClosePushL(doc);
   
     // (3.) copy node to another RDocument
    doc.AppendChildL(elCopy);
     
    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    doc.GetDataContainerList(list1);
    TInt count2 = list1.Count();
    if ( count2 != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk,doc,,list2
        return KErrGeneral;         
        }
    if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk,doc,list2
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk,doc,list2
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk,doc,list2
        return KErrGeneral;
        } 
	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(3);//chunk,doc,list2
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_Copy4L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_Copy4L( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //2. copy grandparents , check if chunk containers are there and not changed
    _LIT(KChunkError,"Chunk Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
     iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
     TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    elem2.AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    CleanupStack::PopAndDestroy();
    
    TXmlEngElement elCopy = elem3.CopyL().AsElement();
  
    if(!elCopy.HasChildNodes())
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if(elCopy.FirstChild().FirstChild().FirstChild().AsChunkContainer().Cid() != cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if(elCopy.FirstChild().FirstChild().FirstChild().AsChunkContainer().Chunk().Name() != chunk.Name() )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.FirstChild().FirstChild().FirstChild().AsChunkContainer().ChunkOffset() != offset )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.FirstChild().FirstChild().FirstChild().AsChunkContainer().Size() != binarySize )	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.FirstChild().FirstChild().LastChild().AsChunkContainer().Cid() != cid2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.FirstChild().FirstChild().LastChild().AsChunkContainer().Chunk().Name() != chunk.Name() )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.FirstChild().FirstChild().LastChild().AsChunkContainer().ChunkOffset() != offset )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    if( elCopy.FirstChild().FirstChild().LastChild().AsChunkContainer().Size() != binarySize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy();//chunk
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_CopyToL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_CopyToL( CStifItemParser& /*aItem*/ )
    {
    //check working of CopyTo containers
    //1. copy chunk container  to another chunk container 
    _LIT(KChunkError,"Chunk CopyToError.");
    _LIT(KEndParsing,"Checking CopyTo end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    

    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    el.AppendChildL(binData3);	
    
    
    // (1.) copy chunk container  to another chunk container 
    TXmlEngNode copyOfBinData1 = binData1.CopyToL(binData1.OwnerDocument());					
    if( copyOfBinData1.AsChunkContainer().Cid() != cid1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk,
        return KErrGeneral;
        }    
    if( copyOfBinData1.AsChunkContainer().Chunk().Name() != chunk.Name() )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk,
        return KErrGeneral;
        }    
    if( copyOfBinData1.AsChunkContainer().ChunkOffset() != offset )
         {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;
        }
    if( copyOfBinData1.AsChunkContainer().Size() != binarySize )			
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk,
        return KErrGeneral;
        }
        
        copyOfBinData1.Remove();
  	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy();//chunk
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_CloneL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_CloneL( CStifItemParser& /*aItem*/ )
    {
    //check working of Clone containers
    //2. clone document to another document  
    _LIT(KChunkError,"Chunk CloneError.");
    _LIT(KEndParsing,"Checking Clone end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    

    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    el.AppendChildL(binData3);	
    
   // (2.) clone document to another document     
    RXmlEngDocument doc = iDoc.CloneDocumentL();
    CleanupClosePushL(doc);
  	
    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    doc.GetDataContainerList(list1);
    TInt count = list1.Count();
    if ( count != 4)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//list1, doc, chunk
        return KErrGeneral;         
        }													
    if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) && (list1[0].Cid() != cid3) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//list1, doc, chunk
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) && (list1[1].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//list1, doc, chunk
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) && (list1[2].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
           CleanupStack::PopAndDestroy(3);//list1, doc, chunk
        return KErrGeneral;
        } 
    if( (list1[3].Cid() != cid) && (list1[3].Cid() != cid1) && (list1[3].Cid() != cid2) && (list1[3].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//list1, doc, chunk
        return KErrGeneral;
        }       
        	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(3);//list1, doc, chunk
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_AdoptNodeL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_AdoptNodeL( CStifItemParser& /*aItem*/ )
    {
    //check working of Clone containers
    //3. append document to another
    _LIT(KChunkError,"Chunk AdoptNodeError.");
    _LIT(KEndParsing,"Checking AdoptNode end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    

    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    el.AppendChildL(binData3);	
     
    RArray<TXmlEngDataContainer> list4;
    CleanupClosePushL(list4);
    iDoc.GetDataContainerList(list4);
    TInt count4 = list4.Count();
    if ( count4 != 4)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//chunk,  list4
        return KErrGeneral;         
        }						
   CleanupStack::PopAndDestroy();//list4   
       
 // (3.) append document to another
    RXmlEngDocument doc2;
    doc2.OpenL(DOM_impl); 
    CleanupClosePushL(doc2);
    el.Unlink();
    doc2.AdoptNodeL(el);
	
    RArray<TXmlEngDataContainer> list2;
    CleanupClosePushL(list2);
    doc2.GetDataContainerList(list2);
    TInt count2 = list2.Count();
    if ( count2 != 4)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk, doc2, list2
        return KErrGeneral;         
        }													
    if( (list2[0].Cid() != cid) && (list2[0].Cid() != cid1) && (list2[0].Cid() != cid2) && (list2[0].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk, doc2, list2
        return KErrGeneral;
        }
    if(  (list2[1].Cid() != cid) && (list2[1].Cid() != cid1) && (list2[1].Cid() != cid2) && (list2[1].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);// chunk, doc2, list2
        return KErrGeneral;
        }	
    if(  (list2[2].Cid() != cid) && (list2[2].Cid() != cid1) && (list2[2].Cid() != cid2) && (list2[2].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
           CleanupStack::PopAndDestroy(3);//chunk, doc2, list2
        return KErrGeneral;
        } 
    if(  (list2[3].Cid() != cid) && (list2[3].Cid() != cid1) && (list2[3].Cid() != cid2) && (list2[3].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(3);//chunk, doc2, list2
        return KErrGeneral;
        }       														
	
    RArray<TXmlEngDataContainer> list3;
    CleanupClosePushL(list3);
    iDoc.GetDataContainerList(list3);
    TInt count3 = list3.Count();
    if ( count3 != 0)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(4);//chunk, doc2, list2, list3
        return KErrGeneral;         
        }	
        	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(4);//list1, doc, chunk, list3
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_SetAsFirstSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_SetAsFirstSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of SetAsFirstSibling  on parent  node with chunk containers

    _LIT(KChunkError,"Chunk SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem6 = iDoc.CreateElementL(_L8("parent1"));
    elem3.AppendChildL(elem6);
     TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent2"));
    elem3.AppendChildL(elem2);
    TXmlEngElement elem4 = iDoc.CreateElementL(_L8("child1"));
    elem2.AppendChildL(elem4); 
    TXmlEngElement elem5 = iDoc.CreateElementL(_L8("child2"));
    elem2.AppendChildL(elem5); 
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    elem2.AppendChildL(el); 
    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    el.AppendChildL(binData3);	
   
    el.SetAsFirstSibling();

    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();	
    if( count != 4)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list,chunk
        return KErrGeneral;         
        }
      if((list[0].Cid() != cid) && (list[0].Cid() != cid1) && (list[0].Cid() != cid2) && (list[0].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }
    if( (list[1].Cid() != cid) && (list[1].Cid() != cid1) && (list[1].Cid() != cid2) && (list[1].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk,
        return KErrGeneral;
        }	
    if((list[2].Cid() != cid) && (list[2].Cid() != cid1) && (list[2].Cid() != cid2) && (list[2].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
           CleanupStack::PopAndDestroy(2);//list,chunk,
        return KErrGeneral;
        } 
    if( (list[3].Cid() != cid) && (list[3].Cid() != cid1) && (list[3].Cid() != cid2) && (list[3].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }       							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(2);//list,  chunk
    
    return KErrNone;         
    }   
 // -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_SetAsFirstSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_SetAsLastSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of SetAsLastSibling  on parent  node with chunk containers

    _LIT(KChunkError,"Chunk SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem6 = iDoc.CreateElementL(_L8("parent1"));
    elem3.AppendChildL(elem6);
     TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent2"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    elem2.AppendChildL(el); 
    TXmlEngElement elem4 = iDoc.CreateElementL(_L8("child1"));
    elem2.AppendChildL(elem4); 
    TXmlEngElement elem5 = iDoc.CreateElementL(_L8("child2"));
    elem2.AppendChildL(elem5); 

    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    el.AppendChildL(binData3);	
   
    el.SetAsLastSibling();

    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 4)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list,chunk
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) && (list1[0].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) && (list1[1].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk,
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) && (list1[2].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
           CleanupStack::PopAndDestroy(2);//list,chunk,
        return KErrGeneral;
        } 
    if( (list1[3].Cid() != cid) && (list1[3].Cid() != cid1) && (list1[3].Cid() != cid2) && (list1[3].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }       							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(2);//list,  chunk
    
    return KErrNone;         
    }     
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_MoveBeforeSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_MoveBeforeSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of MoveBeforeSibling  on parent  node with chunk containers

    _LIT(KChunkError,"Chunk SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem6 = iDoc.CreateElementL(_L8("parent1"));
    elem3.AppendChildL(elem6);
     TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent2"));
    elem3.AppendChildL(elem2);
    TXmlEngElement elem4 = iDoc.CreateElementL(_L8("child1"));
    elem2.AppendChildL(elem4); 
    TXmlEngElement elem5 = iDoc.CreateElementL(_L8("child2"));
    elem2.AppendChildL(elem5); 
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    elem2.AppendChildL(el); 

    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    el.AppendChildL(binData3);	
   
    el.MoveBeforeSibling(elem5);

    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 4)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list,chunk
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) && (list1[0].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) && (list1[1].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk,
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) && (list1[2].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
           CleanupStack::PopAndDestroy(2);//list,chunk,
        return KErrGeneral;
        } 
    if( (list1[3].Cid() != cid) && (list1[3].Cid() != cid1) && (list1[3].Cid() != cid2) && (list1[3].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }       							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(2);//list,  chunk
    
    return KErrNone;         
    }     
 // -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_MoveAfterSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_MoveAfterSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of MoveAfterSibling  on parent  node with chunk containers

    _LIT(KChunkError,"Chunk SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem6 = iDoc.CreateElementL(_L8("parent1"));
    elem3.AppendChildL(elem6);
     TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent2"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    elem2.AppendChildL(el); 
    TXmlEngElement elem4 = iDoc.CreateElementL(_L8("child1"));
    elem2.AppendChildL(elem4); 
    TXmlEngElement elem5 = iDoc.CreateElementL(_L8("child2"));
    elem2.AppendChildL(elem5); 

    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    el.AppendChildL(binData3);	
   
    el.MoveAfterSibling(elem4);
    
    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 4)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list,chunk
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) && (list1[0].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) && (list1[1].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk,
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) && (list1[2].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
           CleanupStack::PopAndDestroy(2);//list,chunk,
        return KErrGeneral;
        } 
    if( (list1[3].Cid() != cid) && (list1[3].Cid() != cid1) && (list1[3].Cid() != cid2) && (list1[3].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }       							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(2);//list,  chunk
    
    return KErrNone;         
    }          
   
// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_MoveToL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_MoveToL( CStifItemParser& /*aItem*/ )
    {
    //check working of MoveTo on parent  node with chunk containers

    _LIT(KChunkError,"Chunk SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem6 = iDoc.CreateElementL(_L8("parent1"));
    elem3.AppendChildL(elem6);
     TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent2"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"));
    elem2.AppendChildL(el); 
    TXmlEngElement elem4 = iDoc.CreateElementL(_L8("child1"));
    elem2.AppendChildL(elem4); 
    TXmlEngElement elem5 = iDoc.CreateElementL(_L8("child2"));
    elem2.AppendChildL(elem5); 

    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    el.AppendChildL(binData3);	
   
    el.MoveTo(elem3);

    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 4)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list,chunk
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) && (list1[0].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) && (list1[1].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk,
        return KErrGeneral;
        }	
    if((list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) && (list1[2].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
           CleanupStack::PopAndDestroy(2);//list,chunk,
        return KErrGeneral;
        } 
    if( (list1[3].Cid() != cid) && (list1[3].Cid() != cid1) && (list1[3].Cid() != cid2) && (list1[3].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }       							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(2);//list,  chunk
    
    return KErrNone;         
    }     

// -----------------------------------------------------------------------------
// CLibxml2Tester::ChunkContainer_ReconcileNamespacesL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::ChunkContainer_ReconcileNamespacesL( CStifItemParser& /*aItem*/ )
    {
    //check working of namespace operations for chunk containers

    _LIT(KChunkError,"Chunk SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    TBufC<100> chunkName(_L("test chunk") );
    TInt size = 1000;
    TInt maxSize = 5000;
    TInt offset = 100;
    TInt binarySize = 25; 
//    TBool isReadOnly = EFalse;
    RChunk chunk;
    chunk.CreateGlobal(chunkName, size, maxSize);
    CleanupClosePushL(chunk);
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem6 = iDoc.CreateElementL(_L8("parent1"));
    elem3.AppendChildL(elem6);
     TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent2"), _L8("http://testuri.com"), _L8("ns"));
    elem3.AppendChildL(elem2);
    TXmlEngElement elementNode = iDoc.CreateElementL(_L8("child1"), _L8("http://testuri.com"), _L8("ns"));
    elem2.AppendChildL(elementNode);
    TXmlEngElement el = iDoc.CreateElementL(_L8("chunkcontainer"), _L8("http://testuri.com"), _L8("ns"));
    elem2.AppendChildL(el); 
    TXmlEngElement elem4 = iDoc.CreateElementL(_L8("child3"), _L8("http://testuri.com"), _L8("ns"));
    elem2.AppendChildL(elem4); 
    TXmlEngElement elem5 = iDoc.CreateElementL(_L8("child4"), _L8("http://testuri.com"), _L8("ns"));
    elem2.AppendChildL(elem5); 

    TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
											cid, chunk,offset, binarySize);
    el.AppendChildL(binData);
    TXmlEngChunkContainer binData1 = iDoc.CreateChunkContainerL(
											cid1, chunk,offset, binarySize);
    el.AppendChildL(binData1);
    TXmlEngChunkContainer binData2 = iDoc.CreateChunkContainerL(
											cid2, chunk,offset, binarySize);
    el.AppendChildL(binData2);	
    
    TXmlEngChunkContainer binData3 = iDoc.CreateChunkContainerL(
											cid3, chunk,offset, binarySize);
    el.AppendChildL(binData3);	

    if (elem2.NamespaceUri().Compare(_L8("http://testuri.com")))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
	
    //unlink elem2 (parent of node - el)
    TXmlEngNode unlinked = elem2.Unlink();
    unlinked.ReconcileNamespacesL();
		
    if (el.NamespaceUri().Compare(_L8("http://testuri.com")))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy();//chunk
        return KErrGeneral;    
        }
    elem3.AppendChildL(unlinked);  

    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 4)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list,chunk
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) && (list1[0].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) && (list1[1].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk,
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) && (list1[2].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
           CleanupStack::PopAndDestroy(2);//list,chunk,
        return KErrGeneral;
        } 
    if( (list1[3].Cid() != cid) && (list1[3].Cid() != cid1) && (list1[3].Cid() != cid2) && (list1[3].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KChunkError);
        iLog->Log((TDesC)KChunkError);
        CleanupStack::PopAndDestroy(2);//list, chunk, 
        return KErrGeneral;
        }       							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(2);//list,  chunk
    
    return KErrNone;         
    }
