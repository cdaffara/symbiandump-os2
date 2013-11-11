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
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengbinarycontainer.h>
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

/************************************** FILE ****************************************/  
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_GetListL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_GetListL( CStifItemParser& /*aItem*/ )
    {
    //checks how array of nodes is filled with data, and realloc if list > iDoc->dataNodeMax-1
    _LIT(KFileError,"FileContainer Error GetList..");
    _LIT(KEndParsing,"Checking FileContainer GetList end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
     iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("filecontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TInt multiplicity = 19;
    for(TInt i = 0; i < multiplicity; i++)								
        {
        TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);    
        TXmlEngNode ref = el.AppendChildL(binData);
        if(!ref.IsSameNode(binData))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
            iLog->Log((TDesC)KFileError);
            CleanupStack::PopAndDestroy(2);//rfs,file
            return KErrGeneral;
            }
        if( ref.AsFileContainer().Cid() != cid)
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
            iLog->Log((TDesC)KFileError);
            CleanupStack::PopAndDestroy(2);// rfs,file
            return KErrGeneral;   
            }
        }
    TBuf<100> fileName;
    file1.Name(fileName);
    TBuf<100> refName;
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if(count != multiplicity)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(3);//list,  rfs,file
        return KErrGeneral;  
        }    
    for(TInt i = 0; i < count; i++)								
        {
        if( list[i].Cid() != cid)
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
            iLog->Log((TDesC)KFileError);
            CleanupStack::PopAndDestroy(3);//list,  rfs,file
            return KErrGeneral;  
            }                  
        list[i].AsFileContainer().File().Name(refName);
       if(  refName != fileName )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
            iLog->Log((TDesC)KFileError);
            CleanupStack::PopAndDestroy(3);//list,  rfs,file
            return KErrGeneral;  
            }
        }
    
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(3);//list,  rfs,file
    return KErrNone;    
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_CidL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_CidL( CStifItemParser& /*aItem*/ )
    {
    //check working of Cid() for FileContainer
    _LIT(KFileError,"File Cid() Error.");
    _LIT(KEndParsing,"Checking FileContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("filecontainer"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
									    
    TXmlEngNode ref = el.AppendChildL(binData);
    
    TPtrC8 fetchedCid = binData.Cid();
    if (fetchedCid !=cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(2);//rfs,file
        return KErrGeneral;               
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//rfs,file
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_CidL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_CidErrorL( CStifItemParser& /*aItem*/ )
    {
    //create 2 cid with the same name, do some operation on tree ->ReplaceWith, 
    _LIT(KFileError,"File Cid() Error.");
    _LIT(KEndParsing,"Checking FileContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("filecontainer"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TPtrC8 fetchedCid = binData.Cid();
    if (fetchedCid !=cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;       
        }
        
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid, file2);
    el.AppendChildL(binData1);
    TPtrC8 fetchedCid1 = binData1.Cid();
    if (fetchedCid1 !=cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;       
        }
        
    el.AppendChildL(binData); // this nothing does to tree
    
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);

    binData1.ReplaceWith(binData2);	//by replacing, removes fileconteiners from list 
                                                                            	//and appends new file conteiner to list								
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
   if(count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;       
        }  
    TPtrC8 fetchedCid2 = binData2.Cid();
    if (fetchedCid2 !=cid2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;       
        }   
                        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
    
    return KErrNone;         
    }    
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_SizeL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_SizeL( CStifItemParser& /*aItem*/ )
    {
    //check working of Size() for FileContainer
    _LIT(KFileError,"File Size() Error.");
    _LIT(KEndParsing,"Checking FileContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("filecontainer"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
									    
    TXmlEngNode ref = el.AppendChildL(binData);
    TInt fileSize;
    file1.Size(fileSize); 
    if (binData.Size() != fileSize)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(2);//rfs,file1
        return KErrGeneral;       
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//rfs,file1
    
    return KErrNone;             
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_FileL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_FileL( CStifItemParser& /*aItem*/ )
    {
    //check working of File().Name() for FileContainer
    _LIT(KFileError,"File File().Name() Error.");
    _LIT(KEndParsing,"Checking FileContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("filecontainer"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
									    
    TXmlEngNode ref = el.AppendChildL(binData);
    TBuf<100> fileName;
    file1.Name(fileName);
    TBuf<100> refName;
    binData.File().Name(refName);
    if (refName != fileName)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(2);//rfs,file1
        return KErrGeneral;       
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//rfs,file1
    
    return KErrNone;             
    }      
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_NodeTypeL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_NodeTypeL( CStifItemParser& /*aItem*/ )
    {
    //check working of NodeType() for FileContainer
     _LIT(KFileError,"File NodeType() Error.");
    _LIT(KEndParsing,"Checking FileContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("filecontainer"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);									    
    el.AppendChildL(binData);

    if (binData.NodeType() != TXmlEngNode::EFileContainer)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(2);//rfs,file1
        return KErrGeneral;       
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(2);//rfs,file1
    
    return KErrNone;       
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_RemoveL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_RemoveL( CStifItemParser& /*aItem*/ )
    {
    //check working of Removing FileContainer
    //1. input 3 filecontainers, removes second, check if cid of 1 and 3 comapare with test data - returns 2 files in list
    _LIT(KFileError,"FileContainer Remove Error.");
    _LIT(KEndParsing,"Checking FileContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);	
    
    // (1.) removes file 
    binData1.Remove();								
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
    if ( (list[0].Cid() == cid1) || ( list[1].Cid() == cid1))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_Remove2L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_Remove2L( CStifItemParser& /*aItem*/ )
    {
    //check working of Removing FileContainer
    //2. input third file with the same cid as first file, remove it - returns 2 files in list
    _LIT(KFileError,"FileContainer Remove Error.");
    _LIT(KEndParsing,"Checking FileContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid, file3);
    el.AppendChildL(binData2);	
    
    // (1.) removes file 
    binData2.Remove();								
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
    if ( (list[0].Cid() != cid) && (list[0].Cid() != cid1) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
    if ( (list[1].Cid() != cid) && (list[1].Cid() != cid1))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_Remove3L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_Remove3L( CStifItemParser& /*aItem*/ )
    {
    //check working of Removing FileContainer
   //3. input some other data and move it before filecontainer end then remove some file from list
    _LIT(KFileError,"FileContainer Remove Error.");
    _LIT(KEndParsing,"Checking FileContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    
    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);	
    
        // (3.) some other data 
    _LIT8(KCid, "111");      

    TXmlEngBinaryContainer container = iDoc.CreateBinaryContainerL(KCid(), *str1);   
    TXmlEngElement el2 = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el2);
    el2.AppendChildL(container); 
    //move binary container before chunk container
    //get last
    TXmlEngNode node = elem.LastChild();	
    //move last node before first
    node.MoveBeforeSibling(elem.FirstChild());
    
    binData2.Remove();								
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,list,str1
        return KErrGeneral;         
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,list,str1
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_UnlinkL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_UnlinkL( CStifItemParser& /*aItem*/ )
    {
    //check working of Unlinking FileContainer
    //1. input 3 files, unlink one, list.count must be 2
    _LIT(KFileError,"File Unlink Error.");
    _LIT(KEndParsing,"Checking UnlinkFile end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);	
    
    // (1.)unlink one file container
    TXmlEngNode unlinked1=binData1.Unlink();
    if(unlinked1.AsFileContainer().Cid() != cid1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TBuf<100> fileName;
    file2.Name(fileName);
    TBuf<100> refName;
    unlinked1.AsFileContainer().File().Name(refName);
    if (refName != fileName)    
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TInt fileSize;
    file2.Size(fileSize);      
    if( unlinked1.AsFileContainer().Size() != fileSize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    									
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);///rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
    
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_ReplaceWithL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_ReplaceWithL( CStifItemParser& /*aItem*/ )
    {
    //check working of Replacing FileContainer with another
    //1. input 2 files, replace second one (cid1) with 3 (cid2)
    _LIT(KFileError,"File ReplaceWith Error.");
    _LIT(KEndParsing,"Checking ReplaceWithFile end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
     
    binData.ReplaceWith(binData2);	    //by replacing, removes  files from list 
								//and appends new file to list
  									
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
    if( (list[0].Cid() == cid) ||  (list[1].Cid() == cid))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;
        }	

    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_SubstitiuteForL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_SubstituteForL( CStifItemParser& /*aItem*/ )
    {
    //check working of Substitiuting file containers
    //1. input 2 files,substitiute first for third
    _LIT(KFileError,"File SubstituteFor Error.");
    _LIT(KEndParsing,"Checking SubstituteForFile end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    
    TXmlEngNode unlinked1 = binData.SubstituteForL(binData2);	
										//substituting doesn't remove cid-matching files from list 
										//but appends new file to list					
  if(unlinked1.AsFileContainer().Cid() != cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TBuf<100> fileName;
    file1.Name(fileName);
    TBuf<100> refName;
    unlinked1.AsFileContainer().File().Name(refName);
    if (refName != fileName)    
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TInt fileSize;
    file1.Size(fileSize);      
    if( unlinked1.AsFileContainer().Size() != fileSize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }

    unlinked1.Remove();     
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
    if( (list[0].Cid() == cid) ||  (list[1].Cid() == cid))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;
        }
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_CopyL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_CopyL( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //1. copy container, check its data: cid, size
    _LIT(KFileError,"File Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
    
    // (1.)copy container, list will be 3
    TXmlEngNode copyOfBinData1 = binData1.CopyL();								
  if(copyOfBinData1.AsFileContainer().Cid() != cid1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TBuf<100> fileName;
    file2.Name(fileName);
    TBuf<100> refName;
    copyOfBinData1.AsFileContainer().File().Name(refName);
    if (refName != fileName)    
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TInt fileSize;
    file2.Size(fileSize);      
    if( copyOfBinData1.AsFileContainer().Size() != fileSize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);///rfs,file1,file2,file3,list
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
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);///rfs,file1,file2,file3,list,list2
        return KErrGeneral;         
        }
         TInt i=0;
        TInt j=0;
        for (i=0;i<count2;i++ )
            {
                if( list2[i].Cid() == cid1)  j++;

            }
    if( j !=2 )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);///rfs,file1,file2,file3,list,list2
        return KErrGeneral;
        }	
    	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(6);///rfs,file1,file2,file3,list,list2
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_Copy2L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_Copy2L( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //2. copy node with file containers, check its element data
    _LIT(KFileError,"File Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
    
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    CleanupStack::PopAndDestroy();
    
    TXmlEngElement elCopy = el.CopyL().AsElement();
  
    if(!elCopy.HasChildNodes())
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;    
        }
    if(elCopy.FirstChild().AsFileContainer().Cid() != cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;    
        }
    TBuf<100> fileName;
    file1.Name(fileName);
    TBuf<100> refName;
    elCopy.FirstChild().AsFileContainer().File().Name(refName);
    if (refName != fileName)    
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TInt fileSize;
    file1.Size(fileSize);      
    if( elCopy.FirstChild().AsFileContainer().Size() != fileSize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }

   if( elCopy.LastChild().AsFileContainer().Cid() != cid2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;    
        }
  
    TBuf<100> fileName2;
    file3.Name(fileName2);
    TBuf<100> refName2;
    elCopy.LastChild().AsFileContainer().File().Name(refName2);
    if (refName2 != fileName2)    
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TInt fileSize2;
    file3.Size(fileSize2);      
    if( elCopy.LastChild().AsFileContainer().Size() != fileSize2 )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_Copy3L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_Copy3L( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //3. copy node with files containers to another RDocument
    _LIT(KFileError,"File Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
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
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//chunk,doc,,list2
        return KErrGeneral;         
        }
    if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,doc,list2
        return KErrGeneral;
        }
    if((list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,doc,list2
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,doc,list2
        return KErrGeneral;
        } 
	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,doc,list2
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_Copy4L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_Copy4L( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //2. copy grandparents , check if file containers are there and not changed
    _LIT(KFileError,"File Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    elem2.AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
    
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    CleanupStack::PopAndDestroy();
    
    TXmlEngElement elCopy = elem3.CopyL().AsElement();
  
    if(!elCopy.HasChildNodes())
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;    
        }
    if(elCopy.FirstChild().FirstChild().FirstChild().AsFileContainer().Cid() != cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;    
        }   
    TBuf<100> fileName;
    file1.Name(fileName);
    TBuf<100> refName;
    elCopy.FirstChild().FirstChild().FirstChild().AsFileContainer().File().Name(refName);
    if (refName != fileName)    
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TInt fileSize;
    file1.Size(fileSize);      
    if( elCopy.FirstChild().FirstChild().FirstChild().AsFileContainer().Size() != fileSize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    if( elCopy.FirstChild().FirstChild().LastChild().AsFileContainer().Cid() != cid2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;    
        }
    TBuf<100> fileName2;
    file3.Name(fileName2);
    TBuf<100> refName2;
    elCopy.FirstChild().FirstChild().LastChild().AsFileContainer().File().Name(refName2);
    if (refName != fileName)    
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TInt fileSize2;
    file3.Size(fileSize2);      
    if( elCopy.FirstChild().FirstChild().LastChild().AsFileContainer().Size() != fileSize2 )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_CopyToL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_CopyToL( CStifItemParser& /*aItem*/ )
    {
    //check working of CopyTo containers
    //1. copy file container  to another file container 
    _LIT(KFileError,"File CopyToError.");
    _LIT(KEndParsing,"Checking CopyTo end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    elem2.AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
    
    // (1.) copy file container  to another file container 
    TXmlEngNode copyOfBinData1 = binData1.CopyToL(binData1.OwnerDocument());					
    if( copyOfBinData1.AsFileContainer().Cid() != cid1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;
        }    
    TBuf<100> fileName;
    file2.Name(fileName);
    TBuf<100> refName;
    copyOfBinData1.AsFileContainer().File().Name(refName);
    if (refName != fileName)    
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
    TInt fileSize;
    file2.Size(fileSize);      
    if( copyOfBinData1.AsFileContainer().Size() != fileSize )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;         
        }
      
        copyOfBinData1.Remove();
  	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_CloneL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_CloneL( CStifItemParser& /*aItem*/ )
    {
    //check working of Clone containers
    //2. clone document to another document  
    _LIT(KFileError,"File CloneError.");
    _LIT(KEndParsing,"Checking Clone end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    elem2.AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
    
   // (2.) clone document to another document     
    RXmlEngDocument doc = iDoc.CloneDocumentL();
    CleanupClosePushL(doc);
  	
    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    doc.GetDataContainerList(list1);
    TInt count = list1.Count();
    if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,list1, doc
        return KErrGeneral;         
        }													
    if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,list1, doc, 
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,list1, doc, 
        return KErrGeneral;
        }	
    if((list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
           CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,list1, doc, 
        return KErrGeneral;
        } 
        	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3,list1, doc,
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_AdoptNodeL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_AdoptNodeL( CStifItemParser& /*aItem*/ )
    {
    //check working of Clone containers
    //3. append document to another
    _LIT(KFileError,"File AdoptNodeError.");
    _LIT(KEndParsing,"Checking AdoptNode end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    elem2.AppendChildL(el);

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
     
    RArray<TXmlEngDataContainer> list4;
    CleanupClosePushL(list4);
    iDoc.GetDataContainerList(list4);
    TInt count4 = list4.Count();
    if ( count4 != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,  list4
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
    if ( count2 != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3, doc2, list2
        return KErrGeneral;         
        }													
    if( (list2[0].Cid() != cid) && (list2[0].Cid() != cid1) && (list2[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3, doc2, list2
        return KErrGeneral;
        }
    if( (list2[1].Cid() != cid) && (list2[1].Cid() != cid1) && (list2[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3, doc2, list2
        return KErrGeneral;
        }	
    if( (list2[2].Cid() != cid) && (list2[2].Cid() != cid1) && (list2[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
           CleanupStack::PopAndDestroy(6);//rfs,file1,file2,file3, doc2, list2
        return KErrGeneral;
        } 														
	
    RArray<TXmlEngDataContainer> list3;
    CleanupClosePushL(list3);
    iDoc.GetDataContainerList(list3);
    TInt count3 = list3.Count();
    if ( count3 != 0)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(7);//rfs,file1,file2,file3, doc2, list2,list3
        return KErrGeneral;         
        }	
        	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(7);//rfs,file1,file2,file3, doc2, list2,list3
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_SetAsFirstSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_SetAsFirstSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of SetAsFirstSibling  on parent  node with file containers

    _LIT(KFileError,"File SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
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
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    elem2.AppendChildL(el);
    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
   
    el.SetAsFirstSibling();

    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();	
    if( count != 3)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
      if( (list[0].Cid() != cid) && (list[0].Cid() != cid1) && (list[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;
        }
    if( (list[1].Cid() != cid) && (list[1].Cid() != cid1) && (list[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;
        }	
    if( (list[2].Cid() != cid) && (list[2].Cid() != cid1) && (list[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
           CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;
        } 							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
    
    return KErrNone;         
    }   
 // -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_SetAsFirstSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_SetAsLastSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of SetAsLastSibling  on parent  node with filecontainers

    _LIT(KFileError,"File SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem6 = iDoc.CreateElementL(_L8("parent1"));
    elem3.AppendChildL(elem6);
     TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent2"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    elem2.AppendChildL(el);
    TXmlEngElement elem4 = iDoc.CreateElementL(_L8("child1"));
    elem2.AppendChildL(elem4); 
    TXmlEngElement elem5 = iDoc.CreateElementL(_L8("child2"));
    elem2.AppendChildL(elem5); 
    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
    
    el.SetAsLastSibling();

    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 3)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
           CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;
        }  							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
    
    return KErrNone;         
    }     
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_MoveBeforeSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_MoveBeforeSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of MoveBeforeSibling  on parent  node with file containers

    _LIT(KFileError,"File SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
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
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    elem2.AppendChildL(el);
    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);

    el.MoveBeforeSibling(elem5);

    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 3)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
           CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        } 
 						
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
    
    return KErrNone;         
    }     
 // -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_MoveAfterSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_MoveAfterSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of MoveAfterSibling  on parent  node with file containers

    _LIT(KFileError,"File SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem6 = iDoc.CreateElementL(_L8("parent1"));
    elem3.AppendChildL(elem6);
     TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent2"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    elem2.AppendChildL(el);
    TXmlEngElement elem4 = iDoc.CreateElementL(_L8("child1"));
    elem2.AppendChildL(elem4); 
    TXmlEngElement elem5 = iDoc.CreateElementL(_L8("child2"));
    elem2.AppendChildL(elem5); 
    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
   
    el.MoveAfterSibling(elem4);
    
    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 3)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
           CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        } 							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
    
    return KErrNone;         
    }          
   
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_MoveToL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_MoveToL( CStifItemParser& /*aItem*/ )
    {
    //check working of MoveTo on parent  node with filecontainers

    _LIT(KFileError,"File SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
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
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"));
    elem2.AppendChildL(el);
    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);
   
    el.MoveTo(elem3);

    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 3)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
           CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
        return KErrGeneral;
        }     							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list,
    
    return KErrNone;         
    }     
// -----------------------------------------------------------------------------
// CLibxml2Tester::FileContainer_ReconcileNamespacesL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::FileContainer_ReconcileNamespacesL( CStifItemParser& /*aItem*/ )
    {
    //check working of namespace operations for chunk containers

    _LIT(KFileError,"File SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    RFile file2;
    User::LeaveIfError(file2.Open(rfs, KInputFile2, EFileRead));
    CleanupClosePushL(file2);
    RFile file3;
    User::LeaveIfError(file3.Open(rfs, KInputFile3, EFileRead));
    CleanupClosePushL(file3);
      
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
    TXmlEngElement el = iDoc.CreateElementL(_L8("containers"), _L8("http://testuri.com"), _L8("ns"));
    elem2.AppendChildL(el);
    TXmlEngElement elem4 = iDoc.CreateElementL(_L8("child3"), _L8("http://testuri.com"), _L8("ns"));
    elem2.AppendChildL(elem4); 
    TXmlEngElement elem5 = iDoc.CreateElementL(_L8("child4"), _L8("http://testuri.com"), _L8("ns"));
    elem2.AppendChildL(elem5); 

    TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
    el.AppendChildL(binData);
    TXmlEngFileContainer binData1 = iDoc.CreateFileContainerL(cid1, file2);
    el.AppendChildL(binData1);
    TXmlEngFileContainer binData2 = iDoc.CreateFileContainerL(cid2, file3);
    el.AppendChildL(binData2);

    if (elem2.NamespaceUri().Compare(_L8("http://testuri.com")))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;    
        }
	
    //unlink elem2 (parent of node - el)
    TXmlEngNode unlinked = elem2.Unlink();
    unlinked.ReconcileNamespacesL();
		
    if (el.NamespaceUri().Compare(_L8("http://testuri.com")))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(4);//rfs,file1,file2,file3
        return KErrGeneral;    
        }
    elem3.AppendChildL(unlinked);  

    RArray<TXmlEngDataContainer> list1;
    CleanupClosePushL(list1);
    iDoc.GetDataContainerList(list1);
    TInt count = list1.Count();	
    if( count != 3)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;         
        }
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list 
        return KErrGeneral;
        }
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
        CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;
        }	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KFileError);
        iLog->Log((TDesC)KFileError);
           CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
        return KErrGeneral;
        }  							
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(5);//rfs,file1,file2,file3,list
    
    return KErrNone;         
    }     



