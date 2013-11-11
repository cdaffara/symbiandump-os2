// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of XOP serializer
//

#include <e32math.h>
#include <badesca.h>

#include "xmlengserializerxop.h"
#include <xml/dom/xmlengerrors.h>
#include <xml/dom/xmlengdom.h>
#include "xmlengdomdefs.h"
#include <xml/dom/xmlengnode.h>
#include <xml/dom/xmlengdatacontainer.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <stdapis/libxml2/libxml2_tree.h>
#include "xmlengxopfileoutputstream.h"
#include "xmlengxopproxyoutputstream.h"
#include <xml/dom/xmlengserializeerrors.h>

#include <bsul/clientmessage.h>

_LIT8(KStartTag, "<");
// _LIT8(KCloseTag, "/>");
_LIT8(KXOPInclude, "xop:Include xmlns:xop=\"http://www.w3.org/2004/08/xop/include\"");
_LIT8(KXOPDocumentStart, "MIME-Version: 1.0\nContent-Type: Multipart/Related;boundary=MIME_boundary;\ntype=\"application/xop+xml\";\nstart=\"<root@nokia.com>\";\n\n");
_LIT8(KMimeRoot, "--MIME_boundary\nContent-ID: <root@nokia.com>\n\n");    
_LIT8(KMimeHeaderStart, "\n\n--MIME_boundary\n");
_LIT8(KMimeHeaderContentType, "Content-Type: ");
_LIT8(KMimeHeaderContentIdStart, "Content-ID: <");
_LIT8(KMimeHeaderContentIdEnd, ">\n");
// _LIT8(KMimeHeaderEnd, "\n\n");  
_LIT8(KXOPDocumentEnd, "\n--MIME_boundary--");
_LIT8(KInclude, "Include");
_LIT8(KXOPUri, "http://www.w3.org/2004/08/xop/include");
_LIT8(KNewLine, "\n");
_LIT8(KColon, ":");
_LIT8(KHrefStart, " href=\"cid:");
_LIT8(KHrefEnd, "\"/>");

// --------------------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------------------
//
EXPORT_C CXmlEngSerializerXOP* CXmlEngSerializerXOP::NewL( TBool aCleanXOPInfoset )
	{
	CXmlEngSerializerXOP* serializerXop = new (ELeave) CXmlEngSerializerXOP( aCleanXOPInfoset );
	CleanupStack::PushL( serializerXop ); 
	serializerXop->ConstructL();
	CleanupStack::Pop(); //serializerXop
	return serializerXop;
	}

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode
// --------------------------------------------------------------------------------------
//
TInt CXmlEngSerializerXOP::SerializeL(const TXmlEngNode aRoot)
    {
    TXmlEngSerializationOptions* defPtr = NULL;
    TXmlEngSerializationOptions def = TXmlEngSerializationOptions();
    if(iSerializationOptions )
    	{
    	defPtr = iSerializationOptions;
    	}
    else
        {
        defPtr = &def;
        }

	switch(iSerializationOutput)
		{
		case ESerializeToFile:
			{
		    if (!iOutFileName)
		        {
		        User::Leave(KXmlEngErrNoParameters);
		        } 			
			return SerializeL(iOutFileName->Des(), aRoot, *defPtr);
			}
		case ESerializeToBuffer:
			{
			return SerializeL(*iBuffer, aRoot, *defPtr);
			}
		case ESerializeToStream:
			{
			TXmlEngSXOPProxyOutputStream stream = TXmlEngSXOPProxyOutputStream(*iOutputStream);
			return StreamSerializeL(stream, aRoot, *defPtr); 
			}
		default: 
			{
			User::Leave(KErrNotSupported);
			} 
		}
	return 0;
    }
// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
TInt CXmlEngSerializerXOP::SerializeL(const TDesC& aFileName,
                                   const TXmlEngNode aRoot,
                                   const TXmlEngSerializationOptions& aOptions)
    {
	iRFs.Close();
	User::LeaveIfError(iRFs.Connect());
	return SerializeL(iRFs, aFileName, aRoot, aOptions);  
    }

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
TInt CXmlEngSerializerXOP::SerializeL(RFs& aRFs,
								   const TDesC& aFileName, 
								   const TXmlEngNode aRoot,									
	 						  	   const TXmlEngSerializationOptions& aOptions)
    { 
	User::LeaveIfError( iOutputFile.Replace(aRFs, aFileName, EFileWrite) );    
 	TXmlEngSXOPFileOutputStream fileStream = TXmlEngSXOPFileOutputStream(iOutputFile, aRFs);
	return StreamSerializeL(fileStream, aRoot, aOptions);  
    }

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to a given type of stream (file or stream)
// --------------------------------------------------------------------------------------
//
TInt CXmlEngSerializerXOP::StreamSerializeL(MXmlEngSXOPOutputStream& aOutputStream, 
											const TXmlEngNode aRoot,
											const TXmlEngSerializationOptions& aOptions)
    {
	LeaveIfXopIncludeL(aRoot);
    iDataContainerArray.Reset();
    if(aOptions.iDataSerializer)
    	{
    	return aRoot.OwnerDocument().SaveL(aOutputStream, aRoot, aOptions);
    	}

	iTmpOutputStream = &aOutputStream;
	iDataWritten = 0;

	if( !iCleanXOPInfoset )		
		{
		BufferedWriteL(KXOPDocumentStart, ETrue);
		BufferedWriteL(KMimeRoot, ETrue);					
		}
	    	
	TXmlEngSerializationOptions opt = aOptions;
	opt.iDataSerializer = this;

	iDataWritten += aRoot.OwnerDocument().SaveL(aOutputStream, aRoot, opt);

	if( !iCleanXOPInfoset )		
		{  
		for( TInt i = 0; i < iDataContainerArray.Count(); i++ )	
			{	
			BufferedWriteL(KMimeHeaderStart);
			TPtrC8 contentTypeStr;
			if(GetContentTypeValue(iDataContainerArray[i], contentTypeStr))
				{	
				BufferedWriteL(KMimeHeaderContentType);
				BufferedWriteL(contentTypeStr);
				BufferedWriteL(KNewLine);
				}
			BufferedWriteL(KMimeHeaderContentIdStart);
			BufferedWriteL(iDataContainerArray[i].Cid());
			BufferedWriteL(KMimeHeaderContentIdEnd);
			BufferedWriteL(KNewLine);
			switch(iDataContainerArray[i].NodeType())
				{
				case TXmlEngNode::EBinaryContainer:
					{
					if(opt.iOptions & TXmlEngSerializationOptions::KOptionDecodeBinaryContainers)	
						{
						HBufC8* decodedData = CreateDecodedBufLC(
							iDataContainerArray[i].AsBinaryContainer().Contents());
						BufferedWriteL(decodedData->Des());
						CleanupStack::PopAndDestroy(); //decodedData
						}
					else
						{
						BufferedWriteL(iDataContainerArray[i].AsBinaryContainer().Contents());														
						}
					break;
					}
				case TXmlEngNode::EChunkContainer:
					{
					TPtrC8 data = TPtrC8(
						((RChunk&)iDataContainerArray[i].AsChunkContainer().Chunk()).Base()
						+ iDataContainerArray[i].AsChunkContainer().ChunkOffset(),
						iDataContainerArray[i].AsChunkContainer().Size());
					if(opt.iOptions & TXmlEngSerializationOptions::KOptionDecodeBinaryContainers)	
						{						
						HBufC8* decodedData = CreateDecodedBufLC(data);					
						BufferedWriteL(decodedData->Des());
						CleanupStack::PopAndDestroy(); //decodedData
						}
					else
						{
						BufferedWriteL(data);
						}
					break;
					}
				case TXmlEngNode::EFileContainer:
					{
					TInt size = iDataContainerArray[i].AsFileContainer().Size();
					HBufC8* data = HBufC8::NewLC(size);
					TPtr8 dataPtr = data->Des();
					iDataContainerArray[i].AsFileContainer().File().Read(dataPtr, size);
					if(opt.iOptions & TXmlEngSerializationOptions::KOptionDecodeBinaryContainers )	
						{
						HBufC8* decodedData = CreateDecodedBufLC(dataPtr);
						BufferedWriteL(decodedData->Des());
						CleanupStack::PopAndDestroy(); //decodedData							
						}
					else
						{
						BufferedWriteL(dataPtr);
						}
					CleanupStack::PopAndDestroy(); //data					
					break;
					}														
				}			
			}
		BufferedWriteL(KXOPDocumentEnd);
		} //if( !iCleanXOPInfoset )	
	CommitWriteL();	
	return iDataWritten;
    }     

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to buffer
// --------------------------------------------------------------------------------------
//
TInt CXmlEngSerializerXOP::SerializeL( RBuf8& aBuffer, 
                                    const TXmlEngNode aRoot, 
									const TXmlEngSerializationOptions& aOptions )
    {
    if(aBuffer.Length())
        {
        aBuffer.Close();
        }
	LeaveIfXopIncludeL(aRoot);
    iDataContainerArray.Reset();	    
    if(aOptions.iDataSerializer)
    	{
    	return CXmlEngSerializer::SerializeL(aBuffer, aRoot, aOptions);
    	}    
    	
	TXmlEngSerializationOptions opt = aOptions;
	opt.iDataSerializer = this;
			
	RBuf8 xopDocument;
	CleanupClosePushL(xopDocument);
	aRoot.OwnerDocument().SaveL(xopDocument, aRoot, opt);

	if( iCleanXOPInfoset )		
		{
		if(xopDocument.Size() > aBuffer.MaxSize())
		    {
		    aBuffer.ReAllocL( xopDocument.Size() );
		    }
		aBuffer.Append(xopDocument);		
		}
	else		
		{    
	    // adjust buffer size
		TInt bufSize = KXOPDocumentStart().Size() 
					+ KMimeRoot().Size() 
					+ xopDocument.Size()
					+ KXOPDocumentEnd().Size();
		for(TInt j = 0; j < iDataContainerArray.Count(); j++)	
			{
			TPtrC8 contentTypeStr;
			if(GetContentTypeValue(iDataContainerArray[j], contentTypeStr))
				{
				bufSize += KMimeHeaderContentType().Size() 
						+ contentTypeStr.Size() 
						+ KNewLine().Size();					
				}
			bufSize += KMimeHeaderStart().Size() 
					+ KMimeHeaderContentIdStart().Size()
					+ iDataContainerArray[j].Cid().Length()
					+ KMimeHeaderContentIdEnd().Size()
					+ KNewLine().Size()
					+ iDataContainerArray[j].Size(); 
			}
		if (bufSize > aBuffer.MaxSize())
		    {
		    aBuffer.ReAllocL( bufSize );
		    }
		    
		// write to buffer
		aBuffer.Append(KXOPDocumentStart());
		aBuffer.Append(KMimeRoot());
		aBuffer.Append(xopDocument);
		for(TInt i = 0; i < iDataContainerArray.Count(); i++)	
			{
			aBuffer.Append(KMimeHeaderStart);
			TPtrC8 contentTypeStr;
			if(GetContentTypeValue(iDataContainerArray[i], contentTypeStr))
				{	
				aBuffer.Append(KMimeHeaderContentType);
				aBuffer.Append(contentTypeStr);
				aBuffer.Append(KNewLine);
				}
			aBuffer.Append(KMimeHeaderContentIdStart);				
			aBuffer.Append(iDataContainerArray[i].Cid());
			aBuffer.Append(KMimeHeaderContentIdEnd);
			aBuffer.Append(KNewLine);	
			switch(iDataContainerArray[i].NodeType())
				{
				case TXmlEngNode::EBinaryContainer:
					{
					if(opt.iOptions & TXmlEngSerializationOptions::KOptionDecodeBinaryContainers )	
						{						
						HBufC8* decodedData = CreateDecodedBufLC(iDataContainerArray[i].AsBinaryContainer().Contents());
						aBuffer.Append(decodedData->Des());
						CleanupStack::PopAndDestroy(); //decodedData
						}
					else
						{
						aBuffer.Append(iDataContainerArray[i].AsBinaryContainer().Contents());							
						}
					break;
					}
				case TXmlEngNode::EChunkContainer:
					{
					TPtrC8 data = TPtrC8(
						((RChunk&)iDataContainerArray[i].AsChunkContainer().Chunk()).Base()
						+ iDataContainerArray[i].AsChunkContainer().ChunkOffset(),
						iDataContainerArray[i].AsChunkContainer().Size());
					if(opt.iOptions & TXmlEngSerializationOptions::KOptionDecodeBinaryContainers )	
						{							
						HBufC8* decodedData = CreateDecodedBufLC(data); 
						aBuffer.Append(decodedData->Des());
						CleanupStack::PopAndDestroy(); //decodedData
						}
					else
						{
						aBuffer.Append(data);							
						}
					break;
					}
				case TXmlEngNode::EFileContainer:
					{
					TInt size = iDataContainerArray[i].AsFileContainer().Size();
					HBufC8* data = HBufC8::NewLC(size);
					TPtr8 dataPtr = data->Des();
					iDataContainerArray[i].AsFileContainer().File().Read(dataPtr, size);
					if(opt.iOptions & TXmlEngSerializationOptions::KOptionDecodeBinaryContainers )	
						{
						HBufC8* decodedData = CreateDecodedBufLC(dataPtr); 
						aBuffer.Append(decodedData->Des());
						CleanupStack::PopAndDestroy(); //decodedData							
						}
					else
						{
						aBuffer.Append(dataPtr);
						}
					CleanupStack::PopAndDestroy(); //data
					break;
					}														
				}						
			}
		aBuffer.Append(KXOPDocumentEnd);
		}
	CleanupStack::PopAndDestroy(); //xopDocument
	return aBuffer.Size(); 
    }
      
// --------------------------------------------------------------------------------------
// Determines how binary data should be serialized
// --------------------------------------------------------------------------------------
//
TPtrC8 CXmlEngSerializerXOP::SerializeDataL(TXmlEngNode aNode)
    {
	User::LeaveIfError(iDataContainerArray.Append(aNode.AsDataContainer()));
	iXopStubPtr->Zero();
	iXopStubPtr->Append(KStartTag);
	TXmlEngNamespace xopNs = aNode.AsElement().LookupNamespaceByUriL(KXOPUri);
	if(xopNs.IsNull())
		{
		iXopStubPtr->Append(KXOPInclude);
		}
	else
		{
		iXopStubPtr->Append(xopNs.Prefix());
		iXopStubPtr->Append(KColon);
		iXopStubPtr->Append(KInclude);
		}
	iXopStubPtr->Append(KHrefStart);	
	iXopStubPtr->Append(aNode.AsDataContainer().Cid());
	iXopStubPtr->Append(KHrefEnd);
	iXopStubPtr->ZeroTerminate();
	return *iXopStubPtr;
    }
   
// --------------------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------------------
//
CXmlEngSerializerXOP::CXmlEngSerializerXOP( TBool aCleanXOPInfoset ) : 
	iCleanXOPInfoset(aCleanXOPInfoset)
    {   
    }

// --------------------------------------------------------------------------------------
// Second phase constructor
// --------------------------------------------------------------------------------------
//
void CXmlEngSerializerXOP::ConstructL()
    { 
    //create stub buffer
    iXopStub = HBufC8::NewL(KXopStubMaxSize);
    iXopStubPtr = new ( ELeave ) TPtr8(iXopStub->Des());
    //create output buffer
    iOutputBuffer = HBufC8::NewL(KInitBufferSize);
    iOutputBufferPtr = new ( ELeave ) TPtr8(iOutputBuffer->Des());

    }
        
// --------------------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------------------
//
CXmlEngSerializerXOP::~CXmlEngSerializerXOP()
    {
	iDataContainerArray.Close();
	iRFs.Close();
   	delete iXopStub;	
    delete iXopStubPtr;	
   	delete iOutputBuffer;	
   	delete iOutputBufferPtr;
    }

// --------------------------------------------------------------------------------------
// Leaves if a DOM tree contains 'Include' elements from XOP 
// namespace 'http://www.w3.org/2004/08/xop/include'
// --------------------------------------------------------------------------------------
//
void CXmlEngSerializerXOP::LeaveIfXopIncludeL(TXmlEngNode aRoot)
	{
	if(aRoot.NodeType() == XML_DOCUMENT_NODE)
		{
	    if(HasXopInclude(aRoot.OwnerDocument().DocumentElement()))
	    	{
	    	User::Leave(KXmlEngErrBadInfoset);
	    	}		
		}
	else if(HasXopInclude(aRoot))
	    {
	    User::Leave(KXmlEngErrBadInfoset);
	    }	
	}

// --------------------------------------------------------------------------------------
// Verifies if a DOM tree contains 'Include' elements from XOP 
// namespace 'http://www.w3.org/2004/08/xop/include'
// --------------------------------------------------------------------------------------
//
TBool CXmlEngSerializerXOP::HasXopInclude(TXmlEngNode aRoot)
	{
	if(aRoot.NodeType() == XML_ELEMENT_NODE)
		{
		if(	aRoot.NamespaceDeclaration().NotNull() && 
			aRoot.Name().Compare(KInclude) == 0 && 
			aRoot.NamespaceUri().Compare(KXOPUri) == 0 )
			{
			return ETrue;
			}
		if(aRoot.HasChildNodes())
			{
			RXmlEngNodeList<TXmlEngNode> childList;
			aRoot.GetChildNodes(childList);
			while(childList.HasNext())
				{
				if(HasXopInclude(childList.Next()))
					return ETrue;				
				} 
			}
		}
	return EFalse;
	}
	
// --------------------------------------------------------------------------------------
// Verifies if container's parent node has xmlmime:contentType 
// attribute information item. If found, the function returns ETrue 
// and xmlmime:contentType attribute value. Otherwise, function
// returns EFalse and aContentType is undefined.
// --------------------------------------------------------------------------------------
//
TBool CXmlEngSerializerXOP::GetContentTypeValue(TXmlEngDataContainer aContainer, TPtrC8& aContentType)
	{
	_LIT8(KMimeNsUri, "http://www.w3.org/2004/11/xmlmime");
	_LIT8(KContentTypeName, "contentType");
	
	TXmlEngElement parent = aContainer.ParentNode().AsElement();
	if(parent.HasAttributes())
		{
		RXmlEngNodeList<TXmlEngAttr> attrList;
		parent.GetAttributes(attrList);
		for(TInt i = 0; i < attrList.Count(); i++)
			{
			TXmlEngAttr attr = attrList.Next();
			if(attr.Name().Compare(KContentTypeName) == 0 
				&& attr.NamespaceUri().Compare(KMimeNsUri) == 0)
				{
				aContentType.Set(attr.Value());
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// --------------------------------------------------------------------------------------
// Decodes input data from base64 to binary octets and creates
// heap buffer with the decoded data. The buffer is pushed on the cleanup stack.
// --------------------------------------------------------------------------------------
//
HBufC8* CXmlEngSerializerXOP::CreateDecodedBufLC(TPtrC8 aEncodedData)
	{
	// The decoded length of base64 is about half (use same) encoded length
	HBufC8* decodedData = HBufC8::NewLC(aEncodedData.Size());
	TPtr8 decodedDataPtr = decodedData->Des();
	// Decode the base64 Content-Transfer-Encoding
	using namespace BSUL;
	Base64Codec::Decode(aEncodedData, decodedDataPtr);
	if(decodedDataPtr.Length() == 0)
		{
		User::Leave(KXmlEngErrDecodingFailed);
		}
	return decodedData;	
	}

// --------------------------------------------------------------------------------------
// Appends given string to output buffer. If the serializer's 
// output type is file, the output buffer is released to the output stream 
// after reaching a specified threshold and the output buffer is cleared.
// --------------------------------------------------------------------------------------
//
void CXmlEngSerializerXOP::BufferedWriteL(const TDesC8& aString, TBool aFlush)
	{
	if(aFlush)
		{
		if(iOutputBufferPtr->Size() > 0)
			{
			if( iTmpOutputStream->Write(iOutputBuffer->Des()) == -1)
				{
				User::Leave(iTmpOutputStream->CheckError());
				}
			iOutputBufferPtr->Delete(0, iOutputBufferPtr->Size());							
			}
		if( iTmpOutputStream->Write(aString) == -1)
			{
			User::Leave(iTmpOutputStream->CheckError());
			}												
		iDataWritten += aString.Size();
		return;			
		}

	// first, check if data is in the buffer and added string will not fit
	// -> flush buffer
	if(iOutputBufferPtr->Size() > 0 &&
	   iOutputBufferPtr->Size() + aString.Size() > iOutputBufferPtr->MaxSize())
		{
		if( iTmpOutputStream->Write(iOutputBuffer->Des()) == -1)
			{
			User::Leave(iTmpOutputStream->CheckError());
			}
		iOutputBufferPtr->Delete(0, iOutputBufferPtr->Size());
		}
	// if string alone doesn't fit in the buffer -> save string
	if(aString.Size() > iOutputBufferPtr->MaxSize())
		{
		if( iTmpOutputStream->Write(aString) == -1)
			{
			User::Leave(iTmpOutputStream->CheckError());
			}
		}
	// if string fits into the buffer -> append it
	else
		{
		iOutputBufferPtr->Append(aString);
		}
	iDataWritten += aString.Size();					
	}

// --------------------------------------------------------------------------------------
// Flushes the output buffer and closes the stream
// --------------------------------------------------------------------------------------
//
void CXmlEngSerializerXOP::CommitWriteL()
	{
	// release cached data to the output stream
	if(iOutputBufferPtr->Size() > 0)		
		{
		User::LeaveIfError(iTmpOutputStream->Write(iOutputBuffer->Des()));
		iOutputBufferPtr->Delete(0, iOutputBufferPtr->MaxSize());	
		}
	iTmpOutputStream->CloseAll();	
	}

    
// --------------------------------------------------------------------------------------
// Generates random CID
// --------------------------------------------------------------------------------------
//
void CXmlEngSerializerXOP::GenerateRandomCid(TDes8& aCid)
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
    }

// --------------------------------------------------------------------------------------
// Sets callback for text nodes in serialization options
// --------------------------------------------------------------------------------------
//
void CXmlEngSerializerXOP::SetCallbackL( TXmlEngSerializationOptions& /* aOptions */)
    {/*
  	aOptions.iSerializationCallback = (void*)XOPSerializationCallback;
	aOptions.iHandler = this;*/
    }    
