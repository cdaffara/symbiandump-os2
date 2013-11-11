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
// Document node functions
//

#include <stdlib.h>
#include <bafl/sysutil.h>

#include <xml/utils/xmlengmem.h>
#include <xml/dom/xmlengdocument.h>
#include "xmlengfileoutputstream.h"
#include <xml/dom/xmlengerrors.h>
#include "xmlengownednodescontainer.h"
#include "xmlengdomdefs.h"
#include <xml/dom/xmlengdatacontainer.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include <xml/dom/xmlengfilecontainer.h>
#include <xml/dom/xmlengdataserializer.h>
#include <xml/dom/xmlengelement.h>
#include <xml/dom/xmlengcdatasection.h>
#include <xml/dom/xmlengprocessinginstruction.h>
#include <xml/dom/xmlengcomment.h>
#include <xml/dom/xmlengdocumentfragment.h>
#include <xml/dom/xmlengentityreference.h>
#include <xml/dom/xmlengoutputstream.h>
#include <xml/dom/xmlengdomimplementation.h>

#include <xml/utils/xmlengxestrings.h>
#include <stdapis/libxml2/libxml2_parser.h>
#include <stdapis/libxml2/libxml2_xmlio.h>
#include "libxml2_xmlsave_private.h"
#include "libxml2_tree_private.h"
#include "libxml2_globals_private.h"
#include <stdapis/libxml2/libxml2_parserinternals.h>
#include "libxml2_valid_private.h"

int XMLCALL NodeFilterCallback(void* aFilter, xmlNodePtr aNode);
void LibxmlDocumentCleanup(TAny* aAttrPtr);
void LibxmlNodeCleanup(TAny* aNodePtr);

// -----------------------------------------------------------------------------------------------
// "Write" callback registered in libxml2 for using with custom output stream
// during serializing XML. 
// 
// This callback is invoked repeatedly during serialization; 
// when whole data is serialized the "Close" callback is called for a output stream.
//
// @param aContext  User data that was used during callback registration 
//                  XML Engine uses here a pointer to MXmlEngOutputStream provided by client app
// @param aBuffer   Buffer where the next part of serialized XML has just been written
// @param aLen      Number of bytes written to the buffer
// 
// The callback is prototyped by libxml2's type declaration:
// @code
//     typedef int (*xmlOutputWriteCallback) (void* context, const char* buffer, int len);
// @endcode
// -----------------------------------------------------------------------------------------------
//
int XmlEngineIOWriteCallback(void* aContext, const char* aBuffer, int aLen)
    {
    MXmlEngOutputStream* stream = reinterpret_cast<MXmlEngOutputStream*>(aContext);
    return stream->Write(TPtrC8(reinterpret_cast<TUint8*>(const_cast<char*>(aBuffer)), aLen));
    }

// -----------------------------------------------------------------------------------------------
// "Close" callback registered in libxml2 for using with custom output stream
// during serializing XML. 
// 
// This callback is invoked after the last "Write" callback.
// 
// @param aContext  User data that was used during callback registration 
//                  XML Engine uses here a pointer to MXmlEngOutputStream provided by client app
// 
// The callback is prototyped by libxml2's type declaration:
// @code
//     typedef int (*xmlOutputCloseCallback) (void* context);
// @endcode
// -----------------------------------------------------------------------------------------------
//
int XmlEngineIOCloseCallback(void* aContext)
    {
    MXmlEngOutputStream* stream = reinterpret_cast<MXmlEngOutputStream*>(aContext);
    return stream->Close();
    }

// -----------------------------------------------------------------------------------------------
// Data serialization callback registered in libxml2 for custom node data serialization.
// 
// This callback is invoked whenever EText node contains data of given length, for example
// binary data or data stored in a memory chunk.
// 
// @param aContext  Data serializer provided by client app
// @param aNode  Node that contains data to be serialized 
// -----------------------------------------------------------------------------------------------
//
unsigned char* DataSerializationCallback(void* aContext, xmlNodePtr aNode, int* aWritten)
  	{
  	TPtrC8 ptr(KNullDesC8());
	MXmlEngDataSerializer* dataSerializer = reinterpret_cast<MXmlEngDataSerializer*>(aContext);	
	TRAPD(err, ptr.Set(dataSerializer->SerializeDataL(TXmlEngNode(aNode))));
	if(err)
	    {
	    *aWritten = err;
	    return NULL;
	    }
	    
    *aWritten = ptr.Size();
    return (unsigned char*)ptr.Ptr();
	}

#define LIBXML_DOC (static_cast<xmlDocPtr>(iInternal))

// -------------------------------------------------------------------------------------------------------
// Closes the RXmlEngDocument object: the whole document tree is destroyed and all memory is released.
// 
// All owned by the document nodes (created by it or unlinked from the document and not linked elsewhere)
// are destroyed too.
// -------------------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDocument::Close()
    {
    if(LIBXML_DOC)
        {
        LibxmlDocumentCleanup(LIBXML_DOC);
        iInternal = NULL;
        }
    }

EXPORT_C void RXmlEngDocument::OpenL(RXmlEngDOMImplementation& aDOMImpl)
    {
    iImpl = &aDOMImpl;
    xmlDocPtr doc = xmlNewDoc(NULL);
    OOM_IF_NULL(doc);
    iInternal = doc;
    CleanupClosePushL(*this);
    InitOwnedNodeListL();
    CleanupStack::Pop();
    };
	
EXPORT_C void RXmlEngDocument::OpenL(RXmlEngDOMImplementation& aDOMImpl, void* aInternal)
    {
    iImpl = &aDOMImpl;
    iInternal = aInternal;
    CleanupStack::PushL(TCleanupItem(LibxmlDocumentCleanup,(TAny*)LIBXML_DOC));
    InitOwnedNodeListL();
    CleanupStack::Pop();
    };

EXPORT_C void RXmlEngDocument::OpenL(RXmlEngDOMImplementation& aDOMImpl, TXmlEngElement aRoot)
    {
    OpenL(aDOMImpl);
    xmlUnlinkNode(INTERNAL_NODEPTR(aRoot));
    xmlNodePtr node = xmlAddChild(LIBXML_NODE, INTERNAL_NODEPTR(aRoot));
    if(!node)
        {
        Close();
        OOM_HAPPENED;
        }
    };

EXPORT_C TXmlEngDocumentFragment RXmlEngDocument::CreateDocumentFragmentL()
    {
    xmlNodePtr dfNode = xmlNewDocFragment(LIBXML_DOC);
    OOM_IF_NULL(dfNode);
    TXmlEngNode df(dfNode);
    TakeOwnership(df);
    return df.AsDocumentFragment();
    }
    
// -----------------------------------------------------------------------------------------------
// Serializes document tree into a file
//
// @param aFileName    A file name (with path)
// @return            Number of byte written
// @note
// Method may leave with KErrNoMemory or KErrGeneral codes.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TInt RXmlEngDocument::SaveL( const TDesC& aFileName, 
							    TXmlEngNode aRoot,
							    const TXmlEngSerializationOptions& aSaveOptions ) const
    {
    if(IsNull())
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
    if (!aFileName.Length())
        {
        WRONG_USE_OF_API;
        }    
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    TInt size = SaveL(fs, aFileName, aRoot, aSaveOptions);
    CleanupStack::PopAndDestroy(&fs);
    return size;
    }

// -----------------------------------------------------------------------------------------------
// Serializes node to a file
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TInt RXmlEngDocument::SaveL( RFs& aRFs,
    							const TDesC& aFileName, 
    							TXmlEngNode aRoot,
    							const TXmlEngSerializationOptions& aSaveOptions ) const
    {
    if(IsNull())
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}    
    if (!aFileName.Length())
        {
        WRONG_USE_OF_API;
        }  
    TInt size = -1;        
    RFile outputFile;
    
    User::LeaveIfError(outputFile.Replace(aRFs,aFileName,EFileShareAny|EFileWrite));
    CleanupClosePushL(outputFile);
        
    TXmlEngFileOutputStream outputStream(outputFile,aRFs);
    RBuf8 buffer;
    // TString is null so it does not need to be closed
    size = SaveNodeL( INTERNAL_NODEPTR(aRoot), buffer, &outputStream, aSaveOptions);
    User::LeaveIfError(outputStream.CheckError());
    CleanupStack::PopAndDestroy(&outputFile);
    
    return size;   
    }

// -----------------------------------------------------------------------------------------------
// Serializes a document tree into provided output stream, which supports
// progressive writing of data.
//
// @param aStream  An output stream to write serialized DOM tree
// @param aRoot Root node to be serialized
// @param aSaveOptions Options that control how serialization is performed
// @see MXmlEngOutputStream
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TInt RXmlEngDocument::SaveL( MXmlEngOutputStream& aStream,
								TXmlEngNode aRoot,
								const TXmlEngSerializationOptions& aSaveOptions ) const
    {
	RBuf8 placeHolder;
    if(IsNull())
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}
	if(!&aStream)
		{
		User::Leave(KXmlEngErrWrongUseOfAPI);
		}
	
	return SaveNodeL( aRoot, placeHolder, &aStream, aSaveOptions );   
    }


// -----------------------------------------------------------------------------------------------
// Serializes DOM subtree into memory buffer
// 
// @param  aBuffer  A buffer to write output data.
// @param  aRoot    Root note to serialize.
// @param  aSaveOptions Various options that control how serialization is performed
//
// @return Number of bytes in updated buffer
//
// Buffer contents is freed prior serialization
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TInt RXmlEngDocument::SaveL( RBuf8& aBuffer, 
    							TXmlEngNode aRoot, 
    							const TXmlEngSerializationOptions& aSaveOptions ) const
    {
    if(IsNull())
    	{
    	User::Leave(KXmlEngErrWrongUseOfAPI);
    	}  
    return SaveNodeL(aRoot, aBuffer, NULL, aSaveOptions);
    }
   
// -----------------------------------------------------------------------------------------------
// Creates complete copy of the document
//
// @return Complete copy of the document tree
// -----------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument RXmlEngDocument::CloneDocumentL() const
    {
    xmlDocPtr doc = NULL;
    if (iInternal)
        {
        doc = xmlCopyDoc(LIBXML_DOC, 1);
        OOM_IF_NULL(doc); 
        }
    RXmlEngDocument retDoc(doc);
    CleanupStack::PushL(TCleanupItem(LibxmlDocumentCleanup,(TAny*)doc));
    retDoc.InitOwnedNodeListL();
    CleanupStack::Pop();
    return retDoc;
    }

// -----------------------------------------------------------------------------------------------
// Creates new element from specific namespace to be a root of the document tree.
//
// Any existing document element of the document is destroyed
//@return A new root element
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement RXmlEngDocument::CreateDocumentElementL( const TDesC8& aName, 
												     const TDesC8& aNamespaceUri, 
												     const TDesC8& aPrefix )
    {
    TXmlEngElement newE = CreateElementL(aName, aNamespaceUri, aPrefix);
    SetDocumentElement(newE);
    return newE;
    }

// -----------------------------------------------------------------------------------------------
// Replaces (and closes) document element with another one
//
// New document element is added as the last child to the document node
// @param aNewDocElement New document tree
// @note Use TXmlEngElement::ReconcileNamespacesL() on the new document element
//      if it or its descendants can contain references to namespace declarations
//      out of the element
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDocument::SetDocumentElement( TXmlEngElement aNewDocElement )
    {
    TXmlEngElement oldE = DocumentElement();
    if (oldE.NotNull())
        {
        oldE.Remove();
        }
    if(aNewDocElement.ParentNode().IsNull())
        aNewDocElement.OwnerDocument().RemoveOwnership(aNewDocElement);
    //
    xmlNodePtr child =  xmlAddChild(LIBXML_NODE, INTERNAL_NODEPTR(aNewDocElement));
    }


// -----------------------------------------------------------------------------------------------
// @return A document element - the top-most element in the document tree
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement RXmlEngDocument::DocumentElement() const
    {
    return TXmlEngElement(xmlDocGetRootElement(LIBXML_DOC));
    }

// -----------------------------------------------------------------------------------------------
// @return Encoding of the source XML data.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 RXmlEngDocument::XmlEncoding() const
    {
    return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_DOC->encoding)).PtrC8();
    }

// -----------------------------------------------------------------------------------------------
// @return Whether standalone="true" was specified in XML declaration in the source XML file.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TBool RXmlEngDocument::IsStandalone() const
    {
    return LIBXML_DOC->standalone != 0;
    }

// -----------------------------------------------------------------------------------------------
// Sets 'standalone' attribute of XML declaration for a document
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDocument::SetStandalone( TBool aStandalone )
    {
    LIBXML_DOC->standalone = aStandalone;
    }

// -----------------------------------------------------------------------------------------------
// @return Version number of XML taken from XML declaration
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 RXmlEngDocument::XmlVersion() const
    {
    return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_DOC->version)).PtrC8();
    }

// -----------------------------------------------------------------------------------------------
// Sets XML version number to be shown in XML declaration when document is serialized.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDocument::SetXmlVersionL( const TDesC8& aVersion )
    {
    if(aVersion.Length())
        {
        xmlChar* ver = xmlCharFromDesC8L(aVersion);
        xmlFree((void*)LIBXML_DOC->version);
        LIBXML_DOC->version = ver;
        }
    }

// -----------------------------------------------------------------------------------------------
// Retrieves base URI (if defined) of the document or NULL
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 RXmlEngDocument::DocumentUri() const
    {
    return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_DOC->URL)).PtrC8();
    }

// -----------------------------------------------------------------------------------------------
// Sets location of the document.
//
// Document's URI is used as top-level base URI definition.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDocument::SetDocumentUriL( const TDesC8& aUri )
    {
    xmlChar* uri = xmlCharFromDesC8L(aUri);
    if(LIBXML_DOC->URL)
        xmlFree((void*)LIBXML_DOC->URL);
    LIBXML_DOC->URL = uri;
    }

// -----------------------------------------------------------------------------------------------
// @return Object that represents current DOM implementation
// @note There is no practical use of implementation object in this version
//      of API other than for creating new RXmlEngDocument instances, but
//      it will change in the future, when an implementation object 
//      is used for changing configuration settings at run-time.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngDOMImplementation RXmlEngDocument::Implementation() const
    {
    return *iImpl;
    }

// -----------------------------------------------------------------------------------------------
// Creates new text node and copies the content string into it.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngTextNode RXmlEngDocument::CreateTextNodeL( const TDesC8& aCharacters )
    {
    xmlChar* con = xmlCharFromDesC8L(aCharacters);
    xmlNodePtr textNode = xmlNewText(NULL);
    if(!textNode)
        {
        delete con;
        OOM_HAPPENED;
        }
    textNode->doc = LIBXML_DOC;
    textNode->content = con;
    TXmlEngNode text(textNode);
    TakeOwnership(text);
    return text.AsText();
    }

// -----------------------------------------------------------------------------------------------
// Creates new binary container and copies the content string into it.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngBinaryContainer RXmlEngDocument::CreateBinaryContainerL( const TDesC8& aCid,
                                                             const TDesC8& aData )
    {
    // Note: TXmlEngBinaryContainer is treated internally by Libxml2 as text node.

    TUint len = aCid.Size();
    unsigned char* cid = new(ELeave) unsigned char[len + 1];
    memcpy(cid, aCid.Ptr(), len);
    cid[len] = 0;
    len = aData.Size();
    
    CleanupStack::PushL(cid);
    unsigned char* str = new(ELeave) unsigned char[len + 1];
    CleanupStack::Pop(cid);
    
    memcpy(str, aData.Ptr(), len);
    str[len] = 0;
    
    xmlNodePtr contNode = xmlNewText(NULL);
    if(!contNode)
        {
        delete cid;
        delete str;
        OOM_HAPPENED;
        }
        
    contNode->ns = (xmlNs*) cid;    
    contNode->content = (xmlChar*) str;
    contNode->properties = (xmlAttr*) len;
	contNode->psvi = (void*) EBinaryContainer;
    contNode->doc = LIBXML_DOC;
    contNode->name = xmlStringTextNoenc;
    xmlAppendDataList(contNode, LIBXML_DOC);
    if(OOM_FLAG)
    	{
        xmlFreeNode(contNode);
        OOM_HAPPENED;    	
    	}    
    TXmlEngNode container(contNode);
    TakeOwnership(container);
    return container.AsBinaryContainer();
    }

// -----------------------------------------------------------------------------------------------
// Creates new binary container that stores reference to memory chunk mapped with descriptor
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngChunkContainer RXmlEngDocument::CreateChunkContainerL( const TDesC8& aCid, 
	    												   const RChunk& aChunk,
						                                   const TInt aChunkOffset,
						                                   const TInt aDataSize )
    {
    // Note: TXmlEngChunkContainer is treated internally by Libxml2 as text node.
    TUint len = aCid.Size();
    unsigned char* cid = new(ELeave) unsigned char[len + 1];
    memcpy(cid, aCid.Ptr(), len);
    cid[len] = 0;
    
    xmlNodePtr contNode = xmlNewText(NULL);
    if(!contNode)
        {
        delete cid;
        OOM_HAPPENED;
        }
    contNode->ns = (xmlNs*) cid;
    contNode->content = (xmlChar*) &aChunk;    
    contNode->properties = (xmlAttr*) aDataSize;
    contNode->nsDef = (xmlNs*) aChunkOffset;    
    contNode->psvi = (void*) EChunkContainer;
    contNode->doc = LIBXML_DOC;
    contNode->name = xmlStringTextNoenc;
    xmlAppendDataList(contNode, LIBXML_DOC);
    if(OOM_FLAG)
    	{
        xmlFreeNode(contNode);
        OOM_HAPPENED;    	
    	}
    TXmlEngNode container(contNode);
    TakeOwnership(container);
    return container.AsChunkContainer();
    }

// -----------------------------------------------------------------------------------------------
// Creates new binary container that stores reference to memory chunk mapped with descriptor
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngFileContainer RXmlEngDocument::CreateFileContainerL( const TDesC8& aCid, 
	    												 const RFile& aFile )
						                                 
    {
    // Note: TXmlEngChunkContainer is treated internally by Libxml2 as text node.
    TUint len = aCid.Size();
    unsigned char* cid = new(ELeave) unsigned char[len + 1];
    memcpy(cid, aCid.Ptr(), len);
    cid[len] = 0;
    
    xmlNodePtr contNode = xmlNewText(NULL);
    if(!contNode)
        {
        delete cid;
        OOM_HAPPENED;
        }
    contNode->ns = (xmlNs*) cid;
    contNode->content = (xmlChar*) &aFile;    
    TInt fileSize;
    aFile.Size(fileSize);
    contNode->properties = (xmlAttr*) fileSize;
    contNode->psvi = (void*) EFileContainer;
    contNode->doc = LIBXML_DOC;
    contNode->name = xmlStringTextNoenc;
    xmlAppendDataList(contNode, LIBXML_DOC);
    if(OOM_FLAG)
    	{
        xmlFreeNode(contNode);
        OOM_HAPPENED;    	
    	}
    TXmlEngNode container(contNode);
    TakeOwnership(container);
    return container.AsFileContainer();
    }
// -----------------------------------------------------------------------------------------------
// Creates new comment node and copies the content string into it.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngComment RXmlEngDocument::CreateCommentL( const TDesC8& aText )
    {
    xmlChar* con = xmlCharFromDesC8L(aText);
    xmlNodePtr commentNode = xmlNewComment(NULL);
    if(!commentNode)
        {
        delete con;
        OOM_HAPPENED;
        } 
    commentNode->doc = LIBXML_DOC;
    commentNode->content = con;
    TXmlEngNode comment(commentNode);
    TakeOwnership(comment);
    return comment.AsComment();
    }

// -----------------------------------------------------------------------------------------------
// Creates new processing instruction node and set its "target" and "data" values
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngProcessingInstruction RXmlEngDocument::CreateProcessingInstructionL( const TDesC8& aTarget, 
    																	 const TDesC8& aData )
    {
    if ( aTarget.Length( ) <= 0 ) 
    	{
    	User::Leave( KXmlEngErrWrongUseOfAPI );
    	}
    
    xmlChar* target = xmlCharFromDesC8L(aTarget);
    CleanupStack::PushL(target);
    xmlChar* data = xmlCharFromDesC8L(aData);
    
    xmlNodePtr piNode = xmlNewPI( target,
                            	  NULL );
    OOM_IF_NULL(piNode);
    CleanupStack::PopAndDestroy(target);
    piNode->content = data;
    TXmlEngNode pi(piNode);
    TakeOwnership(pi);
    return pi.AsProcessingInstruction();
    }

// -----------------------------------------------------------------------------------------------
// Creates new CDATA section node and copies the content into it.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngCDATASection RXmlEngDocument::CreateCDATASectionL( const TDesC8& aContents )
    {
    xmlChar* con = xmlCharFromDesC8L(aContents);
    xmlNodePtr cdNode = xmlNewCDataBlock( LIBXML_DOC,
			                              NULL,
			                              0);
	if(!cdNode)
        {
        delete con;
        OOM_HAPPENED;
        } 
    cdNode->content = con;
    TXmlEngNode cd(cdNode);
    TakeOwnership(cd);
    return cd.AsCDATASection();
    }

// -----------------------------------------------------------------------------------------------
// Creates new entity reference node for aEntityName entity
//
// aEntityName is a string in one of the forms:
//    -  <i>name</i>
//    -  <b>&</b><i>name</i>
//    -  <b>&</b><i>name</i><b>;</b>
//
// where <i>name</i> is the name of the entity
//
// @note &lt; , &gt; , &apos; , &quot; and other <b>predefined</b> entity references
//      should not be created with this method. These entity refs are rather 
//      "character references" and encoded/decoded automatically.
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngEntityReference RXmlEngDocument::CreateEntityReferenceL( const TDesC8& aEntityRef )
    {
    if ( aEntityRef.Length( ) <= 0 ) 
    	{
    	User::Leave( KXmlEngErrWrongUseOfAPI );
    	}    
    
    xmlChar* er = xmlCharFromDesC8L(aEntityRef);
    xmlNodePtr erNode = xmlNewReference( LIBXML_DOC,
                        				 er);
    delete er;
    OOM_IF_NULL(erNode);
    TXmlEngNode eref(erNode);
    TakeOwnership(eref);
    return eref.AsEntityReference();
    }

// -----------------------------------------------------------------------------------------------
// Creates new element node that belongs to specific namespace.
// A namespace declaration node is created on the element.
//
// @param aNamespaceUri Namespace of new element
// @param aPrefix       Prefix to use for namespace binding and QName of the element
// @param aLocalName    Local name of the element 
//
// @note If null namespace uri is provided element will be created without namespace.
// -----------------------------------------------------------------------------------------------

EXPORT_C TXmlEngElement RXmlEngDocument::CreateElementL( const TDesC8& aLocalName, 
										     const TDesC8& aNamespaceUri, 
										     const TDesC8& aPrefix )
    {        
    if ( aLocalName.Length( ) <= 0 ) 
    	{
    	User::Leave( KXmlEngErrWrongUseOfAPI );
    	}
    //
    xmlChar* name = xmlCharFromDesC8L(aLocalName);
    xmlNodePtr nodeEl = xmlNewNode( NULL, 
                            		name);
    delete name;
    OOM_IF_NULL(nodeEl);
    //
//    if(aNamespaceUri.Length() || aPrefix.Length())
	  if(aNamespaceUri.Length())
        {
        CleanupStack::PushL(TCleanupItem(LibxmlNodeCleanup,(TAny*)nodeEl));
        xmlChar* nsU = xmlCharFromDesC8L(aNamespaceUri);
        CleanupStack::PushL(nsU);
        xmlChar* nsP = NULL;
        if(aPrefix.Length())
            {
            nsP = xmlCharFromDesC8L(aPrefix);
            }
        CleanupStack::Pop(nsU);
        xmlNsPtr ns = xmlNewNs( NULL,
                    		    NULL,
                    		    NULL);
        
        if (!ns)
            {
            delete nsP;
            delete nsU;
            OOM_HAPPENED;
            }
        ns->href   = nsU;
        ns->prefix = nsP;
        nodeEl->nsDef = ns;
        nodeEl->ns = ns;
        CleanupStack::Pop(); // nodeEl
        }
    nodeEl->doc = LIBXML_DOC;

    TXmlEngElement el(nodeEl);
    TakeOwnership(el);
    return el;
    }

// -----------------------------------------------------------------------------------------------
// Creates new attribute,
//
// @param  aName   Name of the atribute; no prefix allowed
// @param  aValue  Value of the attribute (optional)
// @return Handler to the newly created attribute 
//
// @note 
// aValue should represent a correct value of an attribute if it is put as is into XML file
// (with all characters correctly escaped with entity references when XML spec requires)
//
// TXmlEngElement class provides a rich set of attribute creation methods, which not
// just create attribute nut also link it into element.
//
// There is no way to create attributes with namespace (despite the DOM spec); 
// you have to use one of the TXmlEngElement::AddNewAttributeL(..) methods instead
//
// Returned handler is the only reference to the allocated memory
// until you have attached the attribute to some element node
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngAttr RXmlEngDocument::CreateAttributeL( const TDesC8& aName, 
    										const TDesC8& aValue )
    {
    if ( aName.Length( ) <= 0 ) 
    	{
    	User::Leave( KXmlEngErrWrongUseOfAPI );
    	}
    xmlChar* name = xmlCharFromDesC8L(aName);
    CleanupStack::PushL(name);
    xmlChar* value = xmlCharFromDesC8L(aValue);
    
    xmlAttrPtr attrNode = xmlNewDocProp( LIBXML_DOC,
                            			 name,
                            	 		 value );
    delete value;
    CleanupStack::PopAndDestroy(name);
    OOM_IF_NULL(attrNode);
    TXmlEngNode attr(attrNode);
    TakeOwnership(attr);
    return attr.AsAttr();
    }

// -----------------------------------------------------------------------------------------------
// Sets "document" property on the node and all its descendants to be this RXmlEngDocument node 
// -----------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode RXmlEngDocument::AdoptNodeL(TXmlEngNode aSource)
    {
    if ( aSource.ParentNode().NotNull() || aSource.OwnerDocument().IsSameNode( *this )
    		|| aSource.NodeType() == TXmlEngNode::EDocument ) 
    	{
    	User::Leave( KXmlEngErrWrongUseOfAPI );
    	}
    
    aSource.OwnerDocument().RemoveOwnership(aSource);
    xmlSetTreeDoc(INTERNAL_NODEPTR(aSource), LIBXML_DOC);
    TakeOwnership(aSource);
    return aSource;
    }

// -----------------------------------------------------------------------------------------------
// Main implementation of SaveL() functions that puts together all common code
// and serializes to buffer or output stream.
//	@param aNode Root node to be serialized     
//	@param aBuffer buffer with serialized data.
//	@param aOutputStream stream that should be used during serialization
//	@param aOpt Options that control how serialization is performed
//  @return Number of bytes written
//	@leave KXmlEngErrWrongEncoding The encoding is not understood
//  @leave KXmlEngErrNegativeOutputSize The data to be serialized has a negative size
//	@leave - One of the system-wide error codes
// -----------------------------------------------------------------------------------------------
//
TInt RXmlEngDocument::SaveNodeL( TXmlEngNode aRoot,
					   	   RBuf8& aBuffer,
						   MXmlEngOutputStream* aOutputStream,
						   TXmlEngSerializationOptions aOpt) const				
    {
    TInt size = -1;
    const unsigned char* encoding;
    xmlCharEncodingHandlerPtr encoder = NULL;
    xmlSaveCtxt ctxt;
    xmlOutputBufferPtr buf = NULL;
    if (aBuffer.Length())
        {
        aBuffer.Close();    
        }
    xmlNodePtr node = INTERNAL_NODEPTR(aRoot);
    if(!node)
    	{
    	node = LIBXML_NODE;
    	}
    xmlChar* enc =NULL;
    if(aOpt.iEncoding.Length())
        enc =  xmlCharFromDesC8L(aOpt.iEncoding);
    CleanupStack::PushL(enc);
    
    encoding = enc;	        
    if (!encoding)
        {
        encoding = node->doc->encoding; 
        } 
    if (encoding) 
        {
        encoder = xmlFindCharEncodingHandler((char*)encoding);
        if (encoder == NULL)
            {
            TEST_OOM_FLAG;
            XmlEngLeaveL(KXmlEngErrWrongEncoding);
            }
        }
    
    if(aOutputStream)
       		{
		    buf = xmlOutputBufferCreateIO( XmlEngineIOWriteCallback,
		            					   XmlEngineIOCloseCallback,
		            					   aOutputStream, // a "context" know by the library 
		            					   encoder );       		
       		}
       	else
       		{
       		buf = xmlAllocOutputBuffer(encoder); 
       		}
    OOM_IF_NULL(buf);
    
    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.doc = node->doc;
    ctxt.encoding = encoding;
    ctxt.buf = buf;
    ctxt.level = 0;
    ctxt.format = (aOpt.iOptions & TXmlEngSerializationOptions::KOptionIndent) ? 1 : 0; 
    if(aOpt.iDataSerializer)
    	{
    	ctxt.textNodeCallback = (xmlSaveTextNodeCallback) DataSerializationCallback;	
    	ctxt.context = aOpt.iDataSerializer;
    	}
    xmlSaveCtxtInit(&ctxt);
    
	xmlNodeFilterData filterData = {0,0}; // Fix to DEF133066.
    // The scope of 'afilterData' (now 'filterData') has been increased so that it is valid when 
	// passed to 'xmlNodeDumpOutputInternal(&ctxt, node)' as a member of 'ctxt'
    if (aOpt.iNodeFilter)
        {
		filterData.fn = aOpt.iNodeFilter;
		filterData.proxyFn = NodeFilterCallback;
        ctxt.nodeFilter = &filterData;
        }

    if(!(aOpt.iOptions & TXmlEngSerializationOptions::KOptionOmitXMLDeclaration))
        {
        const TInt KXmlOpenTagLength = 14;
        const TInt KXmlOpenTagEncLength = 10;
        const TInt KXmlOpenTagStdLength1 = 16;
        const TInt KXmlOpenTagStdLength2 = 17;
        const TInt KXmlCloseTagEncLength = 1;
        const TInt KXmlCloseTagLength = 2;
        
        xmlOutputBufferWrite(buf,KXmlOpenTagLength,"<?xml version=");
        if (node->doc->version != NULL)
            xmlBufferWriteQuotedString(buf->buffer, node->doc->version);
        else
            xmlOutputBufferWrite(buf, 5, "\"1.0\"");

        if (encoding && (aOpt.iOptions & TXmlEngSerializationOptions::KOptionEncoding))
            {
            xmlOutputBufferWrite(buf,KXmlOpenTagEncLength," encoding="); 
            xmlBufferWriteQuotedString(buf->buffer,encoding);
            }
            
        if (aOpt.iOptions & TXmlEngSerializationOptions::KOptionStandalone)
            {
            switch (node->doc->standalone) 
                {
                case 0:
                    xmlOutputBufferWrite(buf, KXmlOpenTagStdLength1, " standalone=\"no\"");
                    break;
                case 1:
                    xmlOutputBufferWrite(buf, KXmlOpenTagStdLength2, " standalone=\"yes\"");
                    break;
                }    
            }
        xmlOutputBufferWrite(buf,KXmlCloseTagLength,"?>");
        if(ctxt.format)
            {
            xmlOutputBufferWrite(buf,KXmlCloseTagEncLength,"\n");
            }
        }
    xmlNodeDumpOutputInternal(&ctxt, node);
    
    if(xmlOOMFlag())
        {
        xmlOutputBufferClose(buf);
        OOM_HAPPENED;
        }
    
    if(!aOutputStream)
        {
        if(buf->encoder)
            {
            xmlCharEncOutFunc(buf->encoder, buf->conv, buf->buffer);
            size = buf->conv->use;
            aBuffer.Assign(buf->conv->content,size,size); // frees any previous contents of aBuffer argument
            buf->conv->content = NULL; // To prevent it from freeing
            }
        else
            {
            size = buf->buffer->use;
            aBuffer.Assign(buf->buffer->content,size,size); // frees any previous contents of aBuffer argument       
            buf->buffer->content = NULL; // To prevent it from freeing
            }
        }
    else
        {
        size = buf->buffer->use;
        }
    TInt res = xmlOutputBufferClose(buf);
    
    if ((size < 0) || (res == -1))
        {
        XmlEngLeaveL(KXmlEngErrNegativeOutputSize);
        }
    CleanupStack::PopAndDestroy(enc);
    if(!aOutputStream)
        {    
    	return size;   
        }
    else
    	{
    	return res;
    	}
    }

// -----------------------------------------------------------------------------------------------
// "Secondary" constructor that should be called on every newly created document node.
// Initializes container for nodes owned by the document.
// 
// The need for such secondary constructor is in the fact that underlying libxml2
// library knows nothing about ownership of unlinked nodes -- this feature is
// implemented in C++ DOM wrapper.
// -----------------------------------------------------------------------------------------------
//
void RXmlEngDocument::InitOwnedNodeListL()
    {
    if(!LIBXML_DOC->ownedNodes)
        LIBXML_DOC->ownedNodes = CXmlEngOwnedNodesContainer::NewL();
    }

// -----------------------------------------------------------------------------------------------
// Adds aNode to the list of owned nodes - the nodes that are not linked yet into a 
// document tree, but still destroyed with the document that owns them.
//
// In case of OOM (during growing node list container) the argument node is freed with
// xmlFreeNode()
// -----------------------------------------------------------------------------------------------
//
void RXmlEngDocument::TakeOwnership(TXmlEngNode aNode)
    {
    CXmlEngOwnedNodesContainer* nc = (CXmlEngOwnedNodesContainer*) LIBXML_DOC->ownedNodes;
    if(aNode.ParentNode().NotNull())
        return;
    RXmlEngDocument owner = aNode.OwnerDocument();
    if(owner.NotNull() && !owner.IsSameNode(*this))
        {
        owner.RemoveOwnership(aNode);
        }

    nc->Add(INTERNAL_NODEPTR(aNode));
    INTERNAL_NODEPTR(aNode)->doc = LIBXML_DOC;
    }

void RXmlEngDocument::RemoveOwnership(TXmlEngNode aNode)
    {
    if(LIBXML_DOC)
        ((CXmlEngOwnedNodesContainer*)(LIBXML_DOC->ownedNodes))->Remove(INTERNAL_NODEPTR(aNode));
    }

// -----------------------------------------------------------------------------------------------
// Registers specified attribute as xml:id.
// First parametr allows user, to specify sub-tree, not to search whole document.
// To search whole tree see @see RegisterXmlId(const TDesC8&,const TDesC8&)
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDocument::RegisterXmlIdL(TXmlEngElement aStartElement,     
                                        const TDesC8& aLocalName, 
										const TDesC8& aNamespaceUri)
    {
    if ( !aLocalName.Compare(KNullDesC8) || aStartElement.IsNull()
         || !aStartElement.OwnerDocument().IsSameNode(*this) || aStartElement.OwnerDocument().IsNull())
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);   
        }        
    
    xmlChar* name = xmlCharFromDesC8L(aLocalName);
    CleanupStack::PushL(name);
    xmlChar* nsU = NULL;
    if(aNamespaceUri.Length())
        nsU=xmlCharFromDesC8L(aNamespaceUri);
    
    const xmlChar* ids[3];
    ids[0] = name;
    ids[1] = nsU;
    ids[2] = NULL;
    
    TInt error = xmlAddIDs(INTERNAL_NODEPTR(aStartElement), ids);
    
    delete nsU;
    CleanupStack::PopAndDestroy(name);
    
    switch (error) 
        {
        case 0:            
        break;
        case -1:
            User::Leave(KErrAlreadyExists);
        break;
        case -2:
            User::Leave(KErrNoMemory);
        break;
        case -3:
            User::Leave(KErrArgument);
        break;
        default:
            //User::Panic(); ?
        break;            
        }

    }

// -----------------------------------------------------------------------------------------------
// Registers specified attribute as xml:id.
// Not to search whole tree see @see RegisterXmlId(TXmlEngElement,const TDesC8&,const TDesC8&)
// -----------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngDocument::RegisterXmlIdL(const TDesC8& aLocalName, 
										const TDesC8& aNamespaceUri)
    {
    if (IsNull())
        {
        User::Leave(KXmlEngErrWrongUseOfAPI);   
        }        
    RegisterXmlIdL(DocumentElement(),aLocalName, aNamespaceUri);
    }

// ---------------------------------------------------------------------------------------------
// Looks for element with specified value of xmlId
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngElement RXmlEngDocument::FindElementByXmlIdL( const TDesC8& aValue ) const
    {
    xmlChar* value = xmlCharFromDesC8L(aValue);
    xmlAttrPtr attr = xmlGetID( LIBXML_DOC, value );
    delete value;
    if (attr==NULL)
        {
        return TXmlEngElement(NULL);
        }
    return TXmlEngElement(attr->parent);
    }
    
// ---------------------------------------------------------------------------------------------
// Retrieves an array of chunk containers
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TInt RXmlEngDocument::GetDataContainerList( RArray<TXmlEngDataContainer>& aList )
	{
	TInt i = 0;
	TInt error = 0;
	while( LIBXML_DOC->dataNodeList[i] )
		{
		TXmlEngNode node(LIBXML_DOC->dataNodeList[i]);
		error = aList.Append( node.AsDataContainer() );
		if (error) break;
		++i;		
	    }
	return error;
	}
 
// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C RXmlEngDocument::RXmlEngDocument():TXmlEngNode(NULL), iImpl(NULL)
    {
    } 
