/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include <xml/dom/xmlengbinarycontainer.h>
#include "xmlengtesterdef.h"

#include <InetProtTextUtils.h>
#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>
#include <xml/dom/xmlengserializer.h>


/************************************** AddFun ****************************************/


// --------------------------------------------------------------------------------------
// Read file to buffer
// --------------------------------------------------------------------------------------
//

HBufC8* CLibxml2Tester::ReadFileToBufferL(TDesC& aFile)
	{
	RFs aRFs;
    User::LeaveIfError(aRFs.Connect());
    CleanupClosePushL(aRFs);
    
    TInt dataSize;
    RFile fileHandle;
    User::LeaveIfError(fileHandle.Open(aRFs, aFile, EFileRead | EFileShareReadersOnly));
    CleanupClosePushL(fileHandle);
    User::LeaveIfError(fileHandle.Size(dataSize));
    HBufC8* bufferData = HBufC8::NewL(dataSize);
    TPtr8 dataPtr = bufferData->Des();
    User::LeaveIfError(fileHandle.Read(dataPtr, dataSize)); 
    
    CleanupStack::PopAndDestroy( 2 );
    return bufferData;
	}

// --------------------------------------------------------------------------------------
// Write file from buffer
// --------------------------------------------------------------------------------------
//

void CLibxml2Tester::WriteFileFromBufferL(TDesC& aFile, RBuf8& buffer)
	{
	RFs aRFs;
    User::LeaveIfError(aRFs.Connect());
    CleanupClosePushL(aRFs);
    
	RFile fOut;
	User::LeaveIfError ( fOut.Replace( aRFs, aFile, EFileWrite ) );
	CleanupClosePushL( fOut );
	User::LeaveIfError ( fOut.Write(buffer ) );
	
	CleanupStack::PopAndDestroy( 2 );
	return;
	}

// --------------------------------------------------------------------------------------
// Generates random CID
// --------------------------------------------------------------------------------------
//
void CLibxml2Tester::GenerateRandomCid(TDes8& aCid)
    {
    _LIT8(KAt, "@");
    //generate random CID as <randomNumber>@<homeTime>
	TTime now;
	now.HomeTime();
	TInt64 homeTime = now.Int64();
	TUint32 randomNumber = Math::Random();
	aCid.AppendNum(randomNumber);
	aCid.Append(KAt);
	aCid.AppendNum(now.Int64());
	aCid.SetLength(25);
    }


// -----------------------------------------------------------------------------
// CLibxml2Tester::SaveDocument
// saves xml document
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CLibxml2Tester::SaveDocumentL(RXmlEngDocument& iDocument, TPtrC& pOut)
	{
	TInt error(KErrArgument);
	if (pOut.Length()  != 0 )
		{
		//	save xml document to file
		TXmlEngElement root = iDocument.DocumentElement();
	    if(root.NotNull())
		   	{
		    error = iDocument.SaveL( pOut);
		    }
		
		}
	return error;
	}


// -----------------------------------------------------------------------------
// CLibxml2Tester::CArrayCleanup
// A function to use with TCleanupItem to push c-style arrays
// on cleanup stack. It simply calls delete[] aCArray;
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

 void CLibxml2Tester::CArrayCleanup( TAny* aCArray )
    {
    delete[] aCArray;
    }
    
// -----------------------------------------------------------------------------
// CXmlSecTester::FindNodeL				defect
// Finding node
// Copied function from xmlsectester
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TXmlEngNode CLibxml2Tester::FindNodeL(RXmlEngDocument document, TPtrC8 pName8)
	{
   
    CleanupClosePushL(document);
    TPtrC8 paNode8;
    TXmlEngNode retNode = NULL;
	RXmlEngNodeList<TXmlEngNode> aList;
	document.GetChildNodes(aList);
	TXmlEngNode tNode;
	tNode = aList.Next();
	if (!tNode.NotNull())
		{
		TestModuleIf().Printf( infoNum++, KTEST_NAME, 
                                KSourceFileError);
            	iLog->Log((TDesC) KSourceFileError);
     
     	
		}
		else
			{
			while (tNode.NotNull())
				{
				paNode8.Set(tNode.Name());
				
				if(paNode8 == pName8)
					{
					retNode = tNode;
					break;
					}
					
				if(tNode.HasChildNodes())
					{
					tNode = tNode.FirstChild();
					}
					else if(tNode.NextSibling().NotNull())
						{
						tNode = tNode.NextSibling();
						}
						else
							{
							while(!tNode.NextSibling().NotNull() && !tNode.HasChildNodes())
								{
								tNode = tNode.ParentNode();
								}
								tNode = tNode.NextSibling();
							}
				}
		}
	CleanupStack::Pop(&document);
	return retNode;
	}

//---------------------------------------------------------------------------
// Find element in DOM tree by its name element name	defect (?)
//---------------------------------------------------------------------------
//
TInt CLibxml2Tester::FindElement(const TDesC8& aElementName, const TXmlEngElement& aParent, TXmlEngElement& aResult)
    {

    RXmlEngNodeList<TXmlEngElement> nodeList;
    aParent.GetElementsByTagNameL(nodeList, aElementName);
    TInt error(KErrNone);
    if (nodeList.Count() > 1)
        {
        return KErrOverflow;
        }
    else if(nodeList.Count() == 1)
        {
        aResult = nodeList.Next();
        return KErrNone;
        }
    else if(nodeList.Count() == 0)
        {
        aParent.GetChildElements(nodeList);
        for (TInt i=0;i<nodeList.Count();i++)
            {
            error = FindElement(aElementName, nodeList.Next() , aResult);
            if (!error)
                return KErrNone;
            }
        return KErrNotFound;
        }
    else
        {
        return KErrUnderflow;
        }
    }

// -----------------------------------------------------------------------------
// CLibxml2Tester::GetHexFromPointerL
// returns TInt from TPtrC(pointer to hexadecimal value) 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLibxml2Tester::GetHexFromPointerL(TPtrC pSerOpt)
	{
	//	get serialization options from file
	TInt serOpt;
	TInt size;
	RFs aRFs;
	User::LeaveIfError(aRFs.Connect());
	CleanupClosePushL(aRFs);
	RFile fileHandle;
	User::LeaveIfError(fileHandle.Open(aRFs, pSerOpt, EFileRead | EFileShareReadersOnly ));
	CleanupClosePushL(fileHandle);
	User::LeaveIfError(fileHandle.Size(size));
	HBufC8* optionBuffer = HBufC8::NewLC(size);
	TPtr8 optionPtr = optionBuffer->Des();
	User::LeaveIfError(fileHandle.Read(optionPtr, size)); 
	InetProtTextUtils::ConvertDescriptorToHex(optionPtr, serOpt);
		
	// pop and destroy pushed objects
	CleanupStack::PopAndDestroy( 3 );
	return serOpt;
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::ReadFileL
// returns HBufC8 pointer to content of file with TDesC filename 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CLibxml2Tester::ReadFileL(const TDesC& aFileName)
    {
    RFs rfs;
    RFile file;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    User::LeaveIfError(file.Open(rfs, aFileName, EFileRead));
    CleanupClosePushL(file);
    TInt size;
    User::LeaveIfError(file.Size(size));
    HBufC8* buf = HBufC8::NewLC(size);
    TPtr8 bufPtr(buf->Des());
    User::LeaveIfError(file.Read(bufPtr));
    CleanupStack::Pop(); // buf
    CleanupStack::PopAndDestroy(2); // file, rfs
    return buf;
    }
    
// -----------------------------------------------------------------------------
// CLibxml2Tester::RetSerializerType
// returns serializer type from TPtrC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TXmlEngSerializerType CLibxml2Tester::RetSerializerType(TPtrC& type)
	{
	if (!type.Compare(DEFAULT()))
		{
		return ESerializerDefault;	
		}
	if(!type.Compare(XOP()))
		{
		return ESerializerXOP;
		}
	if(!type.Compare(INFOSET()))
		{
		return ESerializerXOPInfoset;
		}
	if(!type.Compare(GZIP()))
		{
		return ESerializerGZip;
		}
	return ESerializerDefault;	
	}

// -----------------------------------------------------------------------------
// CLibxml2Tester::RetDeserializerType
// returns deserializer type from TPtrC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TXmlEngDeserializerType CLibxml2Tester::RetDeserializerType(TPtrC& type)
	{
	if (!type.Compare(DEFAULT()))
		{
		return EDeserializerDefault;	
		}
	if(!type.Compare(XOP()))
		{
		return EDeserializerXOP;
		}
	if(!type.Compare(INFOSET()))
		{
		return EDeserializerXOPInfoset;
		}
	if(!type.Compare(GZIP()))
		{
		return EDeserializerGZip;
		}
	return EDeserializerDefault;	
	}
	
// -----------------------------------------------------------------------------
// CLibxml2Tester::SetupDocumentL
// setup test DOM tree
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLibxml2Tester::SetupDocumentL(  )
	{
    _LIT8(KElementName,"doc"); 
    _LIT8(KTarget,"test");
    _LIT8(KComment,"test comment");		   
    _LIT8(KValue,"test value");
    _LIT8(KNamespace,"testNamespace");
//    _LIT8(KNamespaceUri,"http://testuri.com");	
    _LIT8(KPrefix,"testPref");
    _LIT8(KStringName,"doc");
    _LIT8(KStringValue,"ATTR_value");
    _LIT8(KStringText,"test content");	
    _LIT8(KCData,"test < test2 & \r\n> ;");						

   
    iDoc.OpenL(DOM_impl);
		
 
    iElement = iDoc.CreateDocumentElementL( KElementName,KNullDesC8 ,KNullDesC8 );
    TXmlEngTextNode textNode = iDoc.CreateTextNodeL(KStringText);
    iElement.AppendChildL(textNode);
    TXmlEngCDATASection cdNode = iDoc.CreateCDATASectionL(KCData);
    iElement.AppendChildL(cdNode);
    TXmlEngAttr atNode = iDoc.CreateAttributeL(KStringName, KStringValue);
    iElement.AppendChildL(atNode);
    TXmlEngComment commentNode = iDoc.CreateCommentL(KComment);
    iElement.AppendChildL(commentNode);
    TXmlEngProcessingInstruction piNode = iDoc.CreateProcessingInstructionL(KTarget, KValue);
    iElement.AppendChildL(piNode);	
    TXmlEngElement elementNode = iDoc.CreateElementL(KElementName, KNamespace, KPrefix);
    iElement.AppendChildL(elementNode);	
    }  
// End of file
