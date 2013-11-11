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
// Content handler implementation
//

#include <xml/parser.h>
#include <xml/documentparameters.h>
#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengdocument.h>
#include "xmlengdomdefs.h"
#include <xml/dom/xmlengbinarycontainer.h>
#include <stdapis/libxml2/libxml2_globals.h>	//xmlFree
#include <xml/utils/xmlengxestd.h>
#include "xmlengdomcontenthandler.h"

xmlChar* xmlCharFromDesC8L(const TDesC8& aDesc)
    {
    if(!aDesc.Length())
        {
        return NULL;
        }
    xmlChar* newContent = (xmlChar*)new(ELeave) TUint8[aDesc.Length() + 1];
    *(Mem::Copy((TAny*)newContent, aDesc.Ptr(), aDesc.Length())) = 0;
    return newContent;
    }

CXmlEngDOMContentHandler* CXmlEngDOMContentHandler::NewL(RXmlEngDocument& aDoc)
	{
	CXmlEngDOMContentHandler* self = new (ELeave) CXmlEngDOMContentHandler(aDoc);
	return self;
	}

CXmlEngDOMContentHandler::~CXmlEngDOMContentHandler()
	{
	}

CXmlEngDOMContentHandler::CXmlEngDOMContentHandler(RXmlEngDocument& aDoc):
iDoc(aDoc)
	{
	iLastElement = iDoc;
	iTmpElement = TXmlEngElement();
	}

/**
This method is a callback to indicate the start of the document.
@param				aDocParam Specifies the various parameters of the document.
@param				aDocParam.iCharacterSetName The character encoding of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode)
    {
    if(aErrorCode)
        {
        return;
        }
        
	INTERNAL_DOCPTR(iDoc)->encoding = xmlCharFromDesC8L(aDocParam.CharacterSetName().DesC());
	iTmpElement = iDoc.CreateDocumentElementL(_L8("fake"));	
    }


/**
This method is a callback to indicate the end of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnEndDocumentL(TInt /*aErrorCode*/)
    {
    }


/**
This method is a callback to indicate an element has been parsed.
@param				aElement is a handle to the element's details.
@param				aAttributes contains the attributes for the element.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnStartElementL(const RTagInfo& aElement,
								 const RAttributeArray& aAttributes, 
								 TInt aErrorCode)
    {
    if(aErrorCode)
        {
        return;
        }
        
    TXmlEngElement element;
    if(iTmpElement.IsNull())
        {
        element = iDoc.CreateElementL(aElement.LocalName().DesC(),
                                      aElement.Uri().DesC(),
                                      aElement.Prefix().DesC());
        }
    else
        {
        element = iTmpElement;

         CXmlEngDOMContentHandler::RenameElementL( element,
                		aElement.LocalName().DesC(),
                            aElement.Uri().DesC(),
                            aElement.Prefix().DesC() );
                           
        iTmpElement = TXmlEngElement();
        }
	
	RArray <RAttribute> array = aAttributes;
	TInt size = array.Count();
	RAttribute attr;
		
	for ( TInt i = 0; i < size; i++)
		{
		attr = array[i];
		if(attr.Attribute().Prefix().DesC().Length())
		{
			element.AddNewAttributeL(attr.Attribute().LocalName().DesC(),
		                         attr.Value().DesC(),
		                         attr.Attribute().Uri().DesC(),		
		                         attr.Attribute().Prefix().DesC());
		}
		else
		{
			element.AddNewAttributeL(attr.Attribute().LocalName().DesC(),
		                         attr.Value().DesC());
		}
	}
	iLastElement.AppendChildL(element);
	iLastElement = element;	
}
	
/**
This method is a callback to indicate the end of the element has been reached.
@param				aElement is a handle to the element's details.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnEndElementL(const RTagInfo& /*aElement*/, TInt aErrorCode)
    {
    if(aErrorCode)
        {
        return;
        }
    iLastElement = iLastElement.ParentNode();
    }

/**
This method is a callback that sends the content of the element.
Not all the content may be returned in one go. The data may be sent in chunks.
When an OnEndElementL is received this means there is no more content to be sent.
@param				aBytes is the raw content data for the element. 
					The client is responsible for converting the data to the 
					required character set if necessary.
					In some instances the content may be binary and must not be converted.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
    {
    if(aErrorCode)
        {
        return;
        }
    iLastElement.AppendChildL(iDoc.CreateTextNodeL(aBytes));
    }
	
/**
This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
This method is always called before the corresponding OnStartElementL method.
@param				aPrefix is the Namespace prefix being declared.
@param				aUri is the Namespace URI the prefix is mapped to.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
									   TInt aErrorCode)
    {
    if(aErrorCode)
        {
        return;
        }
        
    if(iTmpElement.IsNull())
        {
        iTmpElement = iDoc.CreateElementL(_L8("fake"));
        }
    iTmpElement.AddNamespaceDeclarationL(aUri.DesC(),aPrefix.DesC());
    }

/**
This method is a notification of the end of the scope of a prefix-URI mapping.
This method is called after the corresponding DoEndElementL method.
@param				aPrefix is the Namespace prefix that was mapped.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
    {
    }

/**
This method is a notification of ignorable whitespace in element content.
@param				aBytes are the ignored bytes from the document being parsed.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
    {
    }

/**
This method is a notification of a skipped entity. If the parser encounters an 
external entity it does not need to expand it - it can return the entity as aName 
for the client to deal with.
@param				aName is the name of the skipped entity.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
    {
    
    }

/**
This method is a receive notification of a processing instruction.
@param				aTarget is the processing instruction target.
@param				aData is the processing instruction data. If empty none was supplied.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngDOMContentHandler::OnProcessingInstructionL(const TDesC8& aTarget,
										  const TDesC8& aData, 
										  TInt aErrorCode)
    {
    if(aErrorCode)
        {
        return;
        }
        
	iLastElement.AppendChildL(iDoc.CreateProcessingInstructionL(aTarget,aData));	
    }

/**
 * This method is a callback that sends the binary content of the element.
 *
 * @param				aBytes is the raw binary data of the element 
 *						and must not be converted.
 * @param				aErrorCode is the error code. If this is not KErrNone
 *						then special action may be required.
 */
void CXmlEngDOMContentHandler::OnBinaryContentL(const TDesC8& aBytes, 
                                                const TDesC8& aCid,
                                                TInt aErrorCode)
    {
    if(aErrorCode)
        {
        return;
        }
        
	iLastElement.AppendChildL(iDoc.CreateBinaryContainerL(aCid,aBytes));    
    }
	
/**
 * This method is a callback that provides data container reference
 *
 * @param				aContainer Container encapsulating binary data
 * @param				aErrorCode is the error code. If this is not KErrNone
 *						then special action may be required.
 */	
void CXmlEngDOMContentHandler::OnDataContainerL( const TXmlEngDataContainer& aContainer, 
								   TInt aErrorCode)
    {
    if(aErrorCode)
        {
        return;
        }
        
	iLastElement.AppendChildL(aContainer);
    }

/**
This method indicates an error has occurred.
@param				aError is the error code
*/
void CXmlEngDOMContentHandler::OnError(TInt /*aErrorCode*/)
    {
    }

/**
This method obtains the interface matching the specified uid.
@return				0 if no interface matching the uid is found.
					Otherwise, the this pointer cast to that interface.
@param				aUid the uid identifying the required interface.
*/
TAny* CXmlEngDOMContentHandler::GetExtendedInterface(const TInt32 aUid)
    {
    if (aUid == MXmlEngExtendedHandler::EExtInterfaceUid)
        {
        return static_cast<MXmlEngExtendedHandler*>(this);
        }
    return 0;	
    }

/**
   Renames TXmlEngElement nodes only.	(static helper method)
    
  @param aElement element to rename
  @param aLocalName New name
  @param aNamespaceUri New namespace uri
  @param aPrefix New namespace prefix
*/
void CXmlEngDOMContentHandler::RenameElementL(TXmlEngElement& aElement,
    const TDesC8& aLocalName,
    const TDesC8& aNamespaceUri,
    const TDesC8& aPrefix )
    {
    	if ( !INTERNAL_NODEPTR(aElement)->type == XML_ELEMENT_NODE ) 
    		{
    		User::Leave(KXmlEngErrWrongUseOfAPI);	
    		}
    
    TXmlEngNode::TXmlEngDOMNodeType type = aElement.NodeType();
    if (type == TXmlEngNode::EElement)
        {
        SetNameL(aElement, aLocalName);
        if (aNamespaceUri.Length() || aPrefix.Length())
            {
            TXmlEngElement nsElem = aElement.AsElement();
            TXmlEngNamespace ns = nsElem.FindOrCreateNsDeclL(aNamespaceUri, aPrefix);
            INTERNAL_NODEPTR(aElement)->ns = INTERNAL_NSPTR(ns);
            }
        }
    }
  
// ---------------------------------------------------------------------------------------------
// Changes name of the element so that Name() on it will return new name
// ---------------------------------------------------------------------------------------------  
    void CXmlEngDOMContentHandler::SetNameL( TXmlEngElement& aElement,
    const TDesC8& aName )
    {
       
    if ( !INTERNAL_NODEPTR(aElement) )
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    const xmlChar* KOldName = INTERNAL_NODEPTR(aElement)->name;

        if ( aName.Length() <= 0 )
    		{
    		User::Leave(KXmlEngErrWrongUseOfAPI);
    		}
        INTERNAL_NODEPTR(aElement)->name = xmlCharFromDesC8L(aName);
        xmlFree((void *)KOldName);
    
    }
