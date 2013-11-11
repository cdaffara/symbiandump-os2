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

#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengfilecontainer.h>

#include "xmlengtesterdef.h"

#include <InetProtTextUtils.h>
#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>


/************************************** BinCont ****************************************/



// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainerL
// test binary container method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::BinaryContainerL(CStifItemParser& aItem)
	{

	//	get test controls, input&output paths
	TPtrC p_Cid;
 	aItem.GetNextString(p_Cid);
 	HBufC8* aCid=CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Cid);
 	
 	TPtrC8 cidPtr = aCid->Ptr(); 
 	
 	TPtrC p_BinaryData;
 	aItem.GetNextString(p_BinaryData);
 	
	TPtrC p_XmlFile;
 	aItem.GetNextString(p_XmlFile);
 	
 	TPtrC p_Out;
 	aItem.GetNextString(p_Out);
 	
 	TPtrC p_XmlOut;
 	aItem.GetNextString(p_XmlOut);
 	
	
 	//	open binary file
 	RFs aRFs;
   	User::LeaveIfError(aRFs.Connect());
   	CleanupClosePushL(aRFs);
 	TInt size;
 	RFile fileHandle;
 	User::LeaveIfError(fileHandle.Open(aRFs, p_BinaryData, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(fileHandle);
	User::LeaveIfError(fileHandle.Size(size));
	HBufC8* buffer = HBufC8::NewLC(size);
	TPtr8 bufferPtr = buffer->Des();
	User::LeaveIfError(fileHandle.Read(bufferPtr, size)); 
 	
			
	
	//	parse xml file
	iDoc = parser.ParseFileL(p_XmlFile);	

	//	create binary container
	TXmlEngBinaryContainer binaryData = iDoc.CreateBinaryContainerL(cidPtr,bufferPtr);
	
	//	add binary container node to xml document
	iDoc.DocumentElement().AppendChildL(binaryData);
    
    //	save xml document to file
    User::LeaveIfError( SaveDocumentL (iDoc, p_XmlOut ) );
	
	//	get binary data content to TDOMString and set pointer on data
	TPtrC8 p_OutString8 = binaryData.TXmlEngBinaryContainer::Contents();
	
	
	//	save binary data to file
	RFs aRFs2;
	User::LeaveIfError(aRFs2.Connect());
	CleanupClosePushL(aRFs2);
	RFile fileHandle2;
	User::LeaveIfError(fileHandle2.Replace(aRFs2, p_Out, EFileWrite));
	CleanupClosePushL(fileHandle2);
	User::LeaveIfError(fileHandle2.Write(p_OutString8));

	//	cleanup all pushed objests
	CleanupStack::PopAndDestroy( 6 );


	return KErrNone;
	}
	
	

	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestCreateBinaryContainerL
// test craeting binary container
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::TestCreateBinaryContainerL( TPtrC8 aCid, TPtrC8 binaryDataPtr, TPtrC pOut )
	{
	TPtrC8 bincontName = Node1();
	RXmlEngDocument iDocument;
	iDocument.OpenL(DOM_impl);
	CleanupClosePushL( iDocument );
	iDocument.CreateDocumentElementL(bincontName);
	
	//	create binary container 
	TXmlEngBinaryContainer binaryDataNode = iDocument.CreateBinaryContainerL(aCid, binaryDataPtr);
	
	if( !binaryDataNode.IsNull() ) 
		{
		iDocument.DocumentElement().AppendChildL(binaryDataNode);
		User::LeaveIfError( SaveDocumentL (iDocument, pOut ) );
		CleanupStack::PopAndDestroy( &iDocument );
		return KErrNone;
		}
		else 
			{
			CleanupStack::PopAndDestroy( &iDocument );
			return KErrGeneral;
			}
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestLenghtOfBinaryContainerL
// test functin lenght of binary container
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::TestSizeOfBinaryContainerL( RXmlEngDocument& iDocument, TPtrC8 aCid, TPtrC8 binaryDataPtr, TInt size )
	{
	TInt aLength(-1);

	TXmlEngBinaryContainer binaryDataNode = iDocument.CreateBinaryContainerL( aCid, binaryDataPtr );
	
	// size of binary container
	aLength = binaryDataNode.Size();

	return aLength - size;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestBinaryContainerNodeTypeL
// check if created binary container is a text node
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::TestBinaryContainerNodeTypeL( RXmlEngDocument& iDocument, TPtrC8 aCid, TPtrC8 binaryDataPtr )
	{

	//	create binary container 
	TXmlEngBinaryContainer binaryDataNode = iDocument.CreateBinaryContainerL( aCid, binaryDataPtr );
	
	
	return 30 - binaryDataNode.NodeType();
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestSetContentsOfBinaryContainerL
// check if function SetContentsL & Contents of Binary Container
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::TestContentsOfBinaryContainerL( RXmlEngDocument& iDocument, TPtrC8 aCid, TPtrC8 binaryDataPtr, TPtrC pOut1 )
	{
	TXmlEngBinaryContainer binaryDataNode = iDocument.CreateBinaryContainerL( aCid, binaryDataPtr );
	
	
	
	//  init parser		
	RXmlEngDOMImplementation iDomImpl;
	iDomImpl.OpenL();
	RXmlEngDOMParser iParser; 
	iParser.Open(iDomImpl);
	
	TXmlEngBinaryContainer binaryDataNode2 = iDocument.CreateBinaryContainerL( aCid, binaryDataPtr );
	
	binaryDataNode2.SetContentsL(binaryDataPtr);
	
	
	
	TInt compareStrings = binaryDataNode.Contents().Compare(binaryDataNode2.Contents());
	
	User::LeaveIfError( SaveDocumentL (iDocument, pOut1 ) );

	
	return compareStrings;
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestMoveBinaryContainerL
// test craeting binary container
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::TestMoveBinaryContainerL( RXmlEngDocument& iDoc1, RXmlEngDocument& iDoc2, TPtrC8 aCid, TPtrC8 binaryDataPtr , TPtrC pOut1, TPtrC pOut2)
	{
	TInt error;
	

	TXmlEngBinaryContainer binaryDataNode = iDoc1.CreateBinaryContainerL( aCid, binaryDataPtr );
	
	
	iDoc1.DocumentElement().FirstChild().NextSibling().AppendChildL(binaryDataNode);
	
	User::LeaveIfError( SaveDocumentL (iDoc1, pOut1 ) );
		
	binaryDataNode.Unlink();
	binaryDataNode.ReconcileNamespacesL();
	
	binaryDataNode.MoveTo(iDoc2.DocumentElement());
	

	User::LeaveIfError( SaveDocumentL (iDoc2, pOut2 ) );	
	
	error = binaryDataNode.IsNull();

	return error;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestremoveBinaryContainerL
// test craeting binary container
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::TestRemoveBinaryContainerL( RXmlEngDocument& iDocument,TPtrC8 aCid, 
										TPtrC8 binaryDataPtr, TPtrC pOut1, TPtrC pOut2)
	{
	TXmlEngBinaryContainer binaryDataNode = iDocument.CreateBinaryContainerL( aCid, binaryDataPtr );

	iDocument.DocumentElement().FirstChild().NextSibling().AppendChildL(binaryDataNode);
	
	User::LeaveIfError( SaveDocumentL (iDocument, pOut1 ) );
	
	iDocument.DocumentElement().FirstChild().NextSibling().Remove();	
	
	User::LeaveIfError( SaveDocumentL (iDocument, pOut2 ) );
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestCopyBinaryContainerL
// test craeting binary container
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::TestCopyBinaryContainerL( RXmlEngDocument& iDocument,TPtrC8 aCid, TPtrC8 binaryDataPtr, TPtrC pOut1, TPtrC pOut2 )
	{
	TXmlEngBinaryContainer binaryDataNode = iDocument.CreateBinaryContainerL( aCid, binaryDataPtr );

	iDocument.DocumentElement().FirstChild().NextSibling().AppendChildL(binaryDataNode);
	
	
	User::LeaveIfError( SaveDocumentL (iDocument, pOut1 ) );
	
	TXmlEngNode binNode = iDocument.DocumentElement().FirstChild().NextSibling().CopyL();

	iDocument.DocumentElement().AppendChildL(binNode);	
	
	binNode.Unlink();
	binNode.ReconcileNamespacesL();
	
	User::LeaveIfError( SaveDocumentL (iDocument, pOut2 ) );
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::TestReplaceBinaryContainerL
// test craeting binary container
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::TestReplaceBinaryContainerL( RXmlEngDocument& iDoc1, RXmlEngDocument& iDoc2,
 										TPtrC8 aCid, TPtrC8 binaryDataPtr, TPtrC pOut1, TPtrC pOut2 )
	{
	TInt error;
	

	TXmlEngBinaryContainer binaryDataNode = iDoc1.CreateBinaryContainerL( aCid, binaryDataPtr );

	iDoc1.DocumentElement().FirstChild().NextSibling().AppendChildL(binaryDataNode);
	
	User::LeaveIfError( SaveDocumentL(iDoc1, pOut1) );
	
	binaryDataNode.Unlink();
	binaryDataNode.ReconcileNamespacesL();
	
	
	iDoc2.DocumentElement().LastChild().ReplaceWith(binaryDataNode);	
	
	User::LeaveIfError( SaveDocumentL(iDoc2, pOut2) );
	
		
	error = binaryDataNode.IsNull();


	return error;
	}	
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::PartialBinaryContainerL
// test binary container method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::PartialBinaryContainerL(CStifItemParser& aItem)
	{
	TInt error(KErrGeneral);
	
	//	get test controls, input&output paths
	TPtrC p_Control;
	aItem.GetNextString( p_Control );
		
	TPtrC p_Cid;
 	aItem.GetNextString(p_Cid);
    HBufC8* aCid=CnvUtfConverter::ConvertFromUnicodeToUtf8L(p_Cid);
 	TPtrC8 cidPtr = aCid->Ptr(); 
 	TPtrC p_BinaryData;
 	aItem.GetNextString(p_BinaryData);
 	
	TPtrC p_XmlFile1;
 	aItem.GetNextString(p_XmlFile1);
 	
 	TPtrC p_XmlFile2;
 	aItem.GetNextString(p_XmlFile2);
 	
 	TPtrC p_Out1;					// return value or xml file
 	aItem.GetNextString(p_Out1);
 	
 	TPtrC p_Out2;					// xml file 
 	aItem.GetNextString(p_Out2);
 	
  	//	open binary file
 	RFs aRFs;
   	User::LeaveIfError(aRFs.Connect());
   	CleanupClosePushL(aRFs);
 	TInt size;
 	RFile fileHandle;
 	User::LeaveIfError(fileHandle.Open(aRFs, p_BinaryData, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(fileHandle);
	User::LeaveIfError(fileHandle.Size(size));
	HBufC8* buffer = HBufC8::NewLC(size);
	TPtr8 bufferPtr = buffer->Des();
	User::LeaveIfError(fileHandle.Read( bufferPtr, size )); 

	//	parse xml file
	RXmlEngDocument iDoc1;
 	CleanupClosePushL(iDoc1);
	if( p_XmlFile1[0] != 48 )
		{
		iDoc1 = parser.ParseFileL(p_XmlFile1);	
		}
	//	parse xml file
	RXmlEngDocument iDoc2;
	CleanupClosePushL(iDoc2);
	if( p_XmlFile2[0] != 48 )
		{
		iDoc2 = parser.ParseFileL(p_XmlFile2);	
		}
 	
 	if ( p_Control == CREATE_BC )
 		{
 		error = TestCreateBinaryContainerL( cidPtr, bufferPtr, p_Out1 );
 		}
 		else if ( p_Control == CONTENTS_BC )
 			{	
 			error =  TestContentsOfBinaryContainerL( iDoc1, cidPtr, bufferPtr, p_Out1 );
 			}
 			else if ( p_Control == SIZE_BC )
 				{
 				error =  TestSizeOfBinaryContainerL( iDoc1, cidPtr,bufferPtr, size );
 				}
 				else if ( p_Control == NODETYPE_BC )
 					{
 					error =  TestBinaryContainerNodeTypeL( iDoc1, cidPtr, bufferPtr );
 					}
 					else if (p_Control == MOVE_BC)
 						{
						error = TestMoveBinaryContainerL(iDoc1, iDoc2, cidPtr, bufferPtr, p_Out1, p_Out2 ); 						
 						}
 						else if ( p_Control == REMOVE_BC )
 							{
							error = TestRemoveBinaryContainerL( iDoc1, cidPtr, bufferPtr, p_Out1, p_Out2 );
 							}
 							else if ( p_Control == COPY_BC )
	 							{
								error = TestCopyBinaryContainerL( iDoc1, cidPtr, bufferPtr, p_Out1, p_Out2 );
	 							}
	 							else if ( p_Control == REPLACE_BC )
		 							{
									error = TestReplaceBinaryContainerL( iDoc1, iDoc2, cidPtr, bufferPtr, 
																			p_Out1, p_Out2 );
		 							}
				 					else
				 						{
				 						User::Leave( KErrArgument );
				 						}

 	CleanupStack::PopAndDestroy(5 );
  	return error;
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::TestAppendContentsL
// test appending contents to binary container
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::TestAppendContentsL(CStifItemParser& aItem)
	{
	TPtrC pData;
	aItem.GetNextString(pData);
	
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	
	
	HBufC8* buffer = ReadFileToBufferL(pData);
	CleanupStack::PushL(buffer);
		
	iDoc.OpenL(DOM_impl);
	TXmlEngElement elem = iDoc.CreateElementL(_L8("binary container"));
	iDoc.SetDocumentElement(elem);
	
	
	TXmlEngBinaryContainer bincont = iDoc.CreateBinaryContainerL(CID_1(), buffer->Left(100)); 
	
	bincont.AppendContentsL(buffer->Mid(100,100));
	bincont.AppendContentsL(buffer->Right(buffer->Length()-200));
	elem.AppendChildL(bincont);
    
    if(elem.NotNull())
    	{	
    	iDoc.SaveL(pOut);
    	}
    	
    TInt compareStrings = bincont.Contents().Compare(buffer->Des());
    if (compareStrings) return KErrGeneral;
    
    compareStrings = bincont.Cid().Compare(CID_1());
    
	CleanupStack::PopAndDestroy( 1 );
	
	if (compareStrings != 0) return KErrGeneral;
		else return KErrNone;
	}
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_GetListL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_GetListL( CStifItemParser& /*aItem*/ )
    {
    //checks how array of nodes is filled with data, and realloc if list > iDoc->dataNodeMax-1
    _LIT(KBinaryError,"BinaryContainer Error GetList..");
    _LIT(KEndParsing,"Checking BinaryContainer GetList end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
     HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);

     iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    TInt multiplicity = 19;
    for(TInt i = 0; i < multiplicity; i++)								
        {
        TXmlEngBinaryContainer container = iDoc.CreateBinaryContainerL(cid,*str1);   
        TXmlEngNode ref = el.AppendChildL(container);
        if(!ref.IsSameNode(container))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy();//str1
            return KErrGeneral;
            }
        if( ref.AsBinaryContainer().Cid() != cid)
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy();//str1
            return KErrGeneral;   
            }
        }
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if(count != multiplicity)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(2);//list, str1
        return KErrGeneral;  
        }    
    for(TInt i = 0; i < count; i++)								
        {
        if( list[i].Cid() != cid)
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(2);//list,str1
            return KErrGeneral;  
            }                  
       if(   list[i].AsBinaryContainer().Size() != str1->Length() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(2);//list,  str1
            return KErrGeneral;  
            }
        if(   list[i].AsBinaryContainer().Contents().CompareC(*str1))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(2);//list,  str1
            return KErrGeneral;  
            }     
            
        }
    
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(2);//list,  str1
    return KErrNone;    
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_GetList2L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_GetList2L( CStifItemParser& /*aItem*/ )
    {
    //checks if after creation binarycontainer (without appendchild) getdatacontainerlist is updated
    _LIT(KBinaryError,"BinaryContainer Error GetList..");
    _LIT(KEndParsing,"Checking BinaryContainer GetList end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
     HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);

     iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el); 
    
        TXmlEngBinaryContainer container = iDoc.CreateBinaryContainerL(cid,*str1);   
     //   TXmlEngNode ref = el.AppendChildL(container);

    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
        if ( count != 1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(2);//str1,list
        return KErrGeneral;         
        }
    
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(2);//list,  str1
    return KErrNone;    
    }       
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_CidL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_CidErrorL( CStifItemParser& /*aItem*/ )
    {
    //create 2 cid with the same name, do some operation on tree ->ReplaceWith, 
    _LIT(KBinaryError,"Binary Cid() Error.");
    _LIT(KEndParsing,"Checking BinaryContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");

    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

    TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
    TPtrC8 fetchedCid = binData.Cid();
    if (fetchedCid !=cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;       
        }
        
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid,*str2);  
    el.AppendChildL(binData1);
    TPtrC8 fetchedCid1 = binData1.Cid();
    if (fetchedCid1 !=cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;       
        }
        
    el.AppendChildL(binData); // this nothing does to tree
    
    TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str2);  

    binData1.ReplaceWith(binData2);	//by replacing, removes binaryconteiners from list 
                                                                            	//and appends newbinary conteiner to list								
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
   if(count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;       
        }  
    TPtrC8 fetchedCid2 = binData2.Cid();
    if (fetchedCid2 !=cid2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;       
        }   
                        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
    
    return KErrNone;         
    }    
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_RemoveL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_RemoveL( CStifItemParser& /*aItem*/ )
    {
    //check working of Removing BinaryContainer
    //1. input 3 binarycontainers, removes second, check if cid of 1 and 3 comapare with test data - returns 2 bins in list
    _LIT(KBinaryError,"BinaryContainer Remove Error.");
    _LIT(KEndParsing,"Checking BinaryContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid2,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid3,*str3);  
    el.AppendChildL(binData2);
    
    // (1.) removes file 
    binData1.Remove();								
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[0].Cid() != cid) && (list[0].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[0].Size() != str1->Length()) && (list[0].Size() != str3->Length()))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }    
    if ( (list[1].Cid() != cid) && (list[1].Cid() != cid3))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[1].Size() != str1->Length()) && (list[1].Size() != str3->Length()))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }      
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_Remove2L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_Remove2L( CStifItemParser& /*aItem*/ )
    {
    //check working of Removing BinaryContainer
    //2. input third binary with the same cid as first file, remove it - returns 2 binary on list
     _LIT(KBinaryError,"BinaryContainer Remove Error.");
    _LIT(KEndParsing,"Checking BinaryContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid1,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid2,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid1,*str3);  
    el.AppendChildL(binData2);
    
    // (1.) removes file 
    binData2.Remove();								
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[0].Cid() != cid1) && (list[0].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[0].Size() != str1->Length()) && (list[0].Size() != str2->Length()))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }    
    if ( (list[1].Cid() != cid1) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[1].Size() != str1->Length()) && (list[1].Size() != str2->Length()))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }      
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_Remove3L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_Remove3L( CStifItemParser& /*aItem*/ )
    {
    //check working of Removing BinaryContainer
   //3. input some other data and move it before binarycontainer end then remove some bin from list
    _LIT(KBinaryError,"BinaryContainer Remove Error.");
    _LIT(KEndParsing,"Checking BinaryContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid2,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid3,*str3);  
    el.AppendChildL(binData2);
    
        // (3.) some other data 
    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);      
    RFile file1;
    User::LeaveIfError(file1.Open(rfs, KInputFile1, EFileRead));
    CleanupClosePushL(file1);
    TXmlEngFileContainer filecont = iDoc.CreateFileContainerL(cid, file1);
    TXmlEngElement el2 = iDoc.CreateElementL(_L8("filecontainer"));
    iDoc.DocumentElement().AppendChildL(el2);
    el2.AppendChildL(filecont); 
    //move file container before binary container
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
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(6);//rfs,file,str1,str2,str3,list
        return KErrGeneral;         
        }
                  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(6);//rfs,file1,str1,str2,str3,list
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_UnlinkL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_UnlinkL( CStifItemParser& /*aItem*/ )
    {
    //check working of Unlinking BinaryContainer
    //1. input 3 files, unlink one, list.count must be 2
    _LIT(KBinaryError,"BinaryContainer Unlink Error.");
    _LIT(KEndParsing,"Checking BinaryContainer end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
    
    // (1.)unlink one binary container
    TXmlEngNode unlinked1=binData1.Unlink();
    if( unlinked1.AsBinaryContainer().Cid() != cid1)
        {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
        }                  
       if(   unlinked1.AsBinaryContainer().Size() != str2->Length() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }
        if(   unlinked1.AsBinaryContainer().Contents().CompareC(*str2))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }         									
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_ReplaceWithL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_ReplaceWithL( CStifItemParser& /*aItem*/ )
    {
    //check working of Replacing BinaryContainer with another
    //1. input 2 binary, replace second one (cid1) with 3 (cid2)
    _LIT(KBinaryError,"Binary ReplaceWith Error.");
    _LIT(KEndParsing,"Checking ReplaceWithBinary end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
     
    binData.ReplaceWith(binData2);	    //by replacing, removes  binary from list 
								//and appends new binaryto list
  									
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[0].Cid() != cid1) && (list[0].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[0].Size() != str2->Length()) && (list[0].Size() != str3->Length()))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }    
    if ( (list[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list[0].AsBinaryContainer().Contents().CompareC(*str3)))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }        
    if ( (list[1].Cid() != cid1) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[1].Size() != str2->Length()) && (list[1].Size() != str3->Length()))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }      
  if ( (list[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list[1].AsBinaryContainer().Contents().CompareC(*str3)))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_SubstitiuteForL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_SubstituteForL( CStifItemParser& /*aItem*/ )
    {
    //check working of Substitiuting binary containers
    //1. input 2 binary,substitiute first for third
    _LIT(KBinaryError,"Binary SubstituteFor Error.");
    _LIT(KEndParsing,"Checking SubstituteForBinary end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    
    TXmlEngNode unlinked1 = binData.SubstituteForL(binData2);	
    if( unlinked1.AsBinaryContainer().Cid() != cid)
        {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
        }                  
       if(   unlinked1.AsBinaryContainer().Size() != str1->Length() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }
        if(   unlinked1.AsBinaryContainer().Contents().CompareC(*str1))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }         									  
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
     if ( count != 2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[0].Cid() != cid1) && (list[0].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[0].Size() != str2->Length()) && (list[0].Size() != str3->Length()))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }   
        if ( (list[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list[0].AsBinaryContainer().Contents().CompareC(*str3)))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }             
    if ( (list[1].Cid() != cid1) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }
    if ( (list[1].Size() != str2->Length()) && (list[1].Size() != str3->Length()))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }      
 if ( (list[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list[1].AsBinaryContainer().Contents().CompareC(*str3)))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
        return KErrGeneral;         
        }        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_CopyL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_CopyL( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //1. copy container, check its data: cid, size
        _LIT(KBinaryError,"Binary Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
    
    // (1.)copy container, list will be 3
    TXmlEngNode copyOfBinData1 = binData1.CopyL();								
  if(copyOfBinData1.AsBinaryContainer().Cid() != cid1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3,
        return KErrGeneral;         
        }
          if(   copyOfBinData1.AsBinaryContainer().Size() != str2->Length() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }
        if(   copyOfBinData1.AsBinaryContainer().Contents().CompareC(*str2))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }         									

    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list
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
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,list.list2
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
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,list.list2
        return KErrGeneral;
        }	
    	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(5);//str1,str2,str3,list.list2
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_Copy2L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_Copy2L( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //2. copy node with binary containers, check its element data
    _LIT(KBinaryError,"Binary Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
    
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    CleanupStack::PopAndDestroy();
    
    TXmlEngElement elCopy = el.CopyL().AsElement();
  
    if(!elCopy.HasChildNodes())
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;    
        }
    if(elCopy.FirstChild().AsBinaryContainer().Cid() != cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;    
        }
        if(   elCopy.FirstChild().AsBinaryContainer().Size() != str1->Length() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }
        if(   elCopy.FirstChild().AsBinaryContainer().Contents().CompareC(*str1))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }         			

   if( elCopy.LastChild().AsFileContainer().Cid() != cid2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;    
        }
          if(   elCopy.LastChild().AsBinaryContainer().Size() != str3->Length() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }
        if(   elCopy.LastChild().AsBinaryContainer().Contents().CompareC(*str3))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }     
  
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(3);//str1,str2,str3
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_Copy3L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_Copy3L( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //3. copy node with binary containers to another RDocument
    _LIT(KBinaryError,"Binary Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainer"));
    iDoc.DocumentElement().AppendChildL(el);

   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
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
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;         
        }
    if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
    if( (list1[0].Size() != str1->Length()) && (list1[0].Size() != str2->Length()) && (list1[0].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list1[0].AsBinaryContainer().Contents().CompareC(*str1)) && (list1[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list1[0].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }        
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list1[1].Size() != str1->Length()) && (list1[1].Size() != str2->Length()) && (list1[1].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list1[1].AsBinaryContainer().Contents().CompareC(*str1)) && (list1[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list1[1].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }           	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        } 
   if( (list1[2].Size() != str1->Length()) && (list1[2].Size() != str2->Length()) && (list1[2].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
    if( (list1[2].AsBinaryContainer().Contents().CompareC(*str1)) && (list1[2].AsBinaryContainer().Contents().CompareC(*str2)) && (list1[2].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }        	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
    
    return KErrNone;         
    }   
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_Copy4L
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_Copy4L( CStifItemParser& /*aItem*/ )
    {
    //check working of Copying containers
    //2. copy grandparents , check if binary containers are there and not changed
    _LIT(KBinaryError,"Binary Copy Error.");
    _LIT(KEndParsing,"Checking Copy end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainers"));
    elem2.AppendChildL(el);
  
   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
     
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();
    CleanupStack::PopAndDestroy();
    
    TXmlEngElement elCopy = elem3.CopyL().AsElement();
  
    if(!elCopy.HasChildNodes())
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;    
        }
    if(elCopy.FirstChild().FirstChild().FirstChild().AsBinaryContainer().Cid() != cid)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;    
        }   
   if(   elCopy.FirstChild().FirstChild().FirstChild().AsBinaryContainer().Size() != str1->Length() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }
        if(   elCopy.FirstChild().FirstChild().FirstChild().AsBinaryContainer().Contents().CompareC(*str1))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }     
    if( elCopy.FirstChild().FirstChild().LastChild().AsFileContainer().Cid() != cid2)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;    
        }
       if(   elCopy.FirstChild().FirstChild().LastChild().AsBinaryContainer().Size() != str3->Length() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }
        if(   elCopy.FirstChild().FirstChild().LastChild().AsBinaryContainer().Contents().CompareC(*str3))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }     
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
   CleanupStack::PopAndDestroy(3);//str1,str2,str3
    
    return KErrNone;         
    }   

// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_CopyToL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_CopyToL( CStifItemParser& /*aItem*/ )
    {
    //check working of CopyTo containers
    //1. copy binary container  to another binary container 
    _LIT(KBinaryError,"Binary CopyToError.");
    _LIT(KEndParsing,"Checking CopyTo end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainers"));
    elem2.AppendChildL(el);
  
   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
    
    // (1.) copy file container  to another file container 
    TXmlEngNode copyOfBinData1 = binData1.CopyToL(binData1.OwnerDocument());					
    if( copyOfBinData1.AsBinaryContainer().Cid() != cid1)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;
        }    
  if(   copyOfBinData1.AsBinaryContainer().Size() != str2->Length() )
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }
        if(   copyOfBinData1.AsBinaryContainer().Contents().CompareC(*str2))
            {
            TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
            iLog->Log((TDesC)KBinaryError);
            CleanupStack::PopAndDestroy(3);//str1,str2,str3
            return KErrGeneral;  
            }     
      
        copyOfBinData1.Remove();
  	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(3);//str1,str2,str3
    
    return KErrNone;         
    }   
  // -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_CloneL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_CloneL( CStifItemParser& /*aItem*/ )
    {
    //check working of Clone containers
    //2. clone document to another document  
    _LIT(KBinaryError,"Binary CloneError.");
    _LIT(KEndParsing,"Checking Clone end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainers"));
    elem2.AppendChildL(el);
  
   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
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
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,list1, doc
        return KErrGeneral;         
        }													
      if( (list1[0].Cid() != cid) && (list1[0].Cid() != cid1) && (list1[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
       if( (list1[0].Size() != str1->Length()) && (list1[0].Size() != str2->Length()) && (list1[0].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list1[0].AsBinaryContainer().Contents().CompareC(*str1)) && (list1[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list1[0].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }            
    if( (list1[1].Cid() != cid) && (list1[1].Cid() != cid1) && (list1[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
           if( (list1[1].Size() != str1->Length()) && (list1[1].Size() != str2->Length()) && (list1[1].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list1[1].AsBinaryContainer().Contents().CompareC(*str1)) && (list1[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list1[1].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }        	
    if( (list1[2].Cid() != cid) && (list1[2].Cid() != cid1) && (list1[2].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        } 
       if( (list1[2].Size() != str1->Length()) && (list1[2].Size() != str2->Length()) && (list1[2].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list1[2].AsBinaryContainer().Contents().CompareC(*str1)) && (list1[2].AsBinaryContainer().Contents().CompareC(*str2)) && (list1[2].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }           	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(5);//str1,str2,str3,list1, doc
    
    return KErrNone;         
    }   
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_AdoptNodeL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_AdoptNodeL( CStifItemParser& /*aItem*/ )
    {
    //check working of Clone containers
    //3. append document to another
    _LIT(KBinaryError,"Binary AdoptNodeError.");
    _LIT(KEndParsing,"Checking AdoptNode end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
    iDoc.OpenL(DOM_impl);
    TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
    TXmlEngElement elem3 = iDoc.CreateElementL(_L8("grandparent"));
    iDoc.DocumentElement().AppendChildL(elem3);
    TXmlEngElement elem2 = iDoc.CreateElementL(_L8("parent"));
    elem3.AppendChildL(elem2);
    TXmlEngElement el = iDoc.CreateElementL(_L8("binarycontainers"));
    elem2.AppendChildL(el);
  
   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
     
    RArray<TXmlEngDataContainer> list4;
    CleanupClosePushL(list4);
    iDoc.GetDataContainerList(list4);
    TInt count4 = list4.Count();
    if ( count4 != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str2,str2,str3,  list4
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
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,list1, doc
        return KErrGeneral;         
        }													
      if( (list2[0].Cid() != cid) && (list2[0].Cid() != cid1) && (list2[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list2[0].Size() != str1->Length()) && (list2[0].Size() != str2->Length()) && (list2[0].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list2[0].AsBinaryContainer().Contents().CompareC(*str1)) && (list2[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list2[0].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }     
    if( (list2[1].Cid() != cid) && (list2[1].Cid() != cid1) && (list2[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list2[1].Size() != str1->Length()) && (list2[1].Size() != str2->Length()) && (list2[1].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list2[1].AsBinaryContainer().Contents().CompareC(*str1)) && (list2[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list2[1].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }     
    if( (list2[2].Cid() != cid) && (list2[2].Cid() != cid1) && (list2[2].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        } 
         if( (list2[2].Size() != str1->Length()) && (list2[2].Size() != str2->Length()) && (list2[2].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list2[2].AsBinaryContainer().Contents().CompareC(*str1)) && (list2[2].AsBinaryContainer().Contents().CompareC(*str2)) && (list2[2].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }     													
	
    RArray<TXmlEngDataContainer> list3;
    CleanupClosePushL(list3);
    iDoc.GetDataContainerList(list3);
    TInt count3 = list3.Count();
    if ( count3 != 0)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(6);//str1,str2,str3,doc,,list2,list3
        return KErrGeneral;         
        }	
        	
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(6);//str1,str2,str3,doc,,list2,list3
    
    return KErrNone;         
    }   
 // -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_SetAsFirstSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_SetAsFirstSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of SetAsFirstSibling  on parent  node with binery containers

    _LIT(KBinaryError,"Binary SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
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
    
   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
   
    el.SetAsFirstSibling();

    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();	
    if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;         
        }													
      if( (list[0].Cid() != cid) && (list[0].Cid() != cid1) && (list[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
           if( (list[0].Size() != str1->Length()) && (list[0].Size() != str2->Length()) && (list[0].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[0].AsBinaryContainer().Contents().CompareC(*str1)) && (list[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list[0].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }             
    if( (list[1].Cid() != cid) && (list[1].Cid() != cid1) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
           if( (list[1].Size() != str1->Length()) && (list[1].Size() != str2->Length()) && (list[1].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[1].AsBinaryContainer().Contents().CompareC(*str1)) && (list[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list[1].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }               	
    if( (list[2].Cid() != cid) && (list[2].Cid() != cid1) && (list[2].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        } 
           if( (list[2].Size() != str1->Length()) && (list[2].Size() != str2->Length()) && (list[2].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[2].AsBinaryContainer().Contents().CompareC(*str1)) && (list[2].AsBinaryContainer().Contents().CompareC(*str2)) && (list[2].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }           
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
    
    return KErrNone;         
    }   
 // -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_SetAsFirstSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_SetAsLastSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of SetAsLastSibling  on parent  node with filecontainers

    _LIT(KBinaryError,"Binary SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
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
   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
    
    el.SetAsLastSibling();

    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();	
     if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;         
        }													
      if( (list[0].Cid() != cid) && (list[0].Cid() != cid1) && (list[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
               if( (list[0].Size() != str1->Length()) && (list[0].Size() != str2->Length()) && (list[0].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[0].AsBinaryContainer().Contents().CompareC(*str1)) && (list[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list[0].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }           
    if( (list[1].Cid() != cid) && (list[1].Cid() != cid1) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
                   if( (list[1].Size() != str1->Length()) && (list[1].Size() != str2->Length()) && (list[1].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[1].AsBinaryContainer().Contents().CompareC(*str1)) && (list[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list[1].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }       	
    if( (list[2].Cid() != cid) && (list[2].Cid() != cid1) && (list[2].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        } 			
              if( (list[2].Size() != str1->Length()) && (list[2].Size() != str2->Length()) && (list[2].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[2].AsBinaryContainer().Contents().CompareC(*str1)) && (list[2].AsBinaryContainer().Contents().CompareC(*str2)) && (list[2].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }            
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
    
    return KErrNone;         
    }     
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_MoveBeforeSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_MoveBeforeSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of MoveBeforeSibling  on parent  node with file containers

    _LIT(KBinaryError,"Binary SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
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
   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);

    el.MoveBeforeSibling(elem5);

    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();	
      if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;         
        }
        													
         if( (list[0].Cid() != cid) && (list[0].Cid() != cid1) && (list[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
               if( (list[0].Size() != str1->Length()) && (list[0].Size() != str2->Length()) && (list[0].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[0].AsBinaryContainer().Contents().CompareC(*str1)) && (list[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list[0].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }           
    if( (list[1].Cid() != cid) && (list[1].Cid() != cid1) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
                   if( (list[1].Size() != str1->Length()) && (list[1].Size() != str2->Length()) && (list[1].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[1].AsBinaryContainer().Contents().CompareC(*str1)) && (list[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list[1].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }       	
    if( (list[2].Cid() != cid) && (list[2].Cid() != cid1) && (list[2].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        } 			
              if( (list[2].Size() != str1->Length()) && (list[2].Size() != str2->Length()) && (list[2].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[2].AsBinaryContainer().Contents().CompareC(*str1)) && (list[2].AsBinaryContainer().Contents().CompareC(*str2)) && (list[2].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }            

 						
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
    
    return KErrNone;         
    }     
 // -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_MoveAfterSiblingL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_MoveAfterSiblingL( CStifItemParser& /*aItem*/ )
    {
    //check working of MoveAfterSibling  on parent  node with binary containers

    _LIT(KBinaryError,"Binary SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
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
   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
   
    el.MoveAfterSibling(elem4);
    
    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();	
       if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;         
        }													
          if( (list[0].Cid() != cid) && (list[0].Cid() != cid1) && (list[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
               if( (list[0].Size() != str1->Length()) && (list[0].Size() != str2->Length()) && (list[0].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[0].AsBinaryContainer().Contents().CompareC(*str1)) && (list[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list[0].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }           
    if( (list[1].Cid() != cid) && (list[1].Cid() != cid1) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
                   if( (list[1].Size() != str1->Length()) && (list[1].Size() != str2->Length()) && (list[1].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[1].AsBinaryContainer().Contents().CompareC(*str1)) && (list[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list[1].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }       	
    if( (list[2].Cid() != cid) && (list[2].Cid() != cid1) && (list[2].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        } 			
              if( (list[2].Size() != str1->Length()) && (list[2].Size() != str2->Length()) && (list[2].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[2].AsBinaryContainer().Contents().CompareC(*str1)) && (list[2].AsBinaryContainer().Contents().CompareC(*str2)) && (list[2].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }            

    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
    
    return KErrNone;         
    }          
   
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_MoveToL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_MoveToL( CStifItemParser& /*aItem*/ )
    {
    //check working of MoveTo on parent  node with binarycontainers

    _LIT(KBinaryError,"Binary SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
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
   TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);
   
    el.MoveTo(elem3);

    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();	
        if ( count != 3)
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;         
        }													
        if( (list[0].Cid() != cid) && (list[0].Cid() != cid1) && (list[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
               if( (list[0].Size() != str1->Length()) && (list[0].Size() != str2->Length()) && (list[0].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[0].AsBinaryContainer().Contents().CompareC(*str1)) && (list[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list[0].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }           
    if( (list[1].Cid() != cid) && (list[1].Cid() != cid1) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
                   if( (list[1].Size() != str1->Length()) && (list[1].Size() != str2->Length()) && (list[1].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[1].AsBinaryContainer().Contents().CompareC(*str1)) && (list[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list[1].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }       	
    if( (list[2].Cid() != cid) && (list[2].Cid() != cid1) && (list[2].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        } 			
              if( (list[2].Size() != str1->Length()) && (list[2].Size() != str2->Length()) && (list[2].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[2].AsBinaryContainer().Contents().CompareC(*str1)) && (list[2].AsBinaryContainer().Contents().CompareC(*str2)) && (list[2].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }            
				
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
    
    return KErrNone;         
    }     
// -----------------------------------------------------------------------------
// CLibxml2Tester::BinaryContainer_ReconcileNamespacesL
// test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::BinaryContainer_ReconcileNamespacesL( CStifItemParser& /*aItem*/ )
    {
    //check working of namespace operations for chunk containers

    _LIT(KBinaryError,"Binary SiblingsError.");
    _LIT(KEndParsing,"Checking Siblings end.");
    TBufC8<100> cid(_L8("123456789@123456789") );
    TBufC8<100> cid1(_L8("11111111@123456789") );
    TBufC8<100> cid2(_L8("22222222@123456789") );
    TBufC8<100> cid3(_L8("33333333@123456789") );
    _LIT(KInputFile1,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.jpg");
    _LIT(KInputFile2,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\icon.gif");
    _LIT(KInputFile3,"c:\\testing\\data\\xmleng\\efute\\input\\filecontainer\\test.doc");
  
    HBufC8* str1=ReadFileL(KInputFile1);
    CleanupStack::PushL(str1);
    HBufC8* str2=ReadFileL(KInputFile2);
    CleanupStack::PushL(str2);
    HBufC8* str3=ReadFileL(KInputFile3);
    CleanupStack::PushL(str3);
      
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

    TXmlEngBinaryContainer binData= iDoc.CreateBinaryContainerL(cid,*str1);  
    el.AppendChildL(binData);
   TXmlEngBinaryContainer binData1= iDoc.CreateBinaryContainerL(cid1,*str2);  
    el.AppendChildL(binData1);
   TXmlEngBinaryContainer binData2= iDoc.CreateBinaryContainerL(cid2,*str3);  
    el.AppendChildL(binData2);


    if (elem2.NamespaceUri().Compare(_L8("http://testuri.com")))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;    
        }
	
    //unlink elem2 (parent of node - el)
    TXmlEngNode unlinked = elem2.Unlink();
    unlinked.ReconcileNamespacesL();
		
    if (el.NamespaceUri().Compare(_L8("http://testuri.com")))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(3);//str1,str2,str3
        return KErrGeneral;    
        }
    elem3.AppendChildL(unlinked);  

    RArray<TXmlEngDataContainer> list;
    CleanupClosePushL(list);
    iDoc.GetDataContainerList(list);
    TInt count = list.Count();	
    if( count != 3)	
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3.list
        return KErrGeneral;         
        }
         if( (list[0].Cid() != cid) && (list[0].Cid() != cid1) && (list[0].Cid() != cid2) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
               if( (list[0].Size() != str1->Length()) && (list[0].Size() != str2->Length()) && (list[0].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[0].AsBinaryContainer().Contents().CompareC(*str1)) && (list[0].AsBinaryContainer().Contents().CompareC(*str2)) && (list[0].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }           
    if( (list[1].Cid() != cid) && (list[1].Cid() != cid1) && (list[1].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        }
                   if( (list[1].Size() != str1->Length()) && (list[1].Size() != str2->Length()) && (list[1].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[1].AsBinaryContainer().Contents().CompareC(*str1)) && (list[1].AsBinaryContainer().Contents().CompareC(*str2)) && (list[1].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }       	
    if( (list[2].Cid() != cid) && (list[2].Cid() != cid1) && (list[2].Cid() != cid2))
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
        return KErrGeneral;
        } 			
              if( (list[2].Size() != str1->Length()) && (list[2].Size() != str2->Length()) && (list[2].Size() != str3->Length()) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }
        if( (list[2].AsBinaryContainer().Contents().CompareC(*str1)) && (list[2].AsBinaryContainer().Contents().CompareC(*str2)) && (list[2].AsBinaryContainer().Contents().CompareC(*str3) ) )
        {
        TestModuleIf().Printf( infoNum++,KTEST_NAME,KBinaryError);
        iLog->Log((TDesC)KBinaryError);
        CleanupStack::PopAndDestroy(5);//str1,str2,str3,doc,,list1
        return KErrGeneral;
        }            
				
        
    TestModuleIf().Printf( infoNum++,KTEST_NAME,KEndParsing);
    iLog->Log((TDesC)KEndParsing);
    CleanupStack::PopAndDestroy(4);//str1,str2,str3,list1
    
    return KErrNone;         
    }     


// End of file
