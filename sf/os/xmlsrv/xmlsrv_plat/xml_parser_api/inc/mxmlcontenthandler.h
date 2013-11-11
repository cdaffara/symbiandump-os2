/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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







/**
 * DO NOT USE THIS API. DEPRECATED AND REMOVED IN S60 5.0. USE SYMBIAN XML FRAMEWORK INSTEAD.
 */











#ifndef __MXMLCONTENTHANDLER_H
#define __MXMLCONTENTHANDLER_H

#warning The XML API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information


//  INCLUDES
#include <xml/MXMLAttributes.h>
#include <xml/cxml/cxml_nw2cxmltypes.h>

// CLASS DECLARATION

/**
*  Interface for SAX Parser Callback events.
*  If an application (client) needs to be informed of basic parsing events, 
*  it implements this interface and registers an instance with the SAX parser using the SetContentHandler() method.
*  The parser uses the instance to report basic document-related events like the start and end of elements.
*
*  @lib XMLInterface.dll
*  @since ?Series60_version
*/
class MXMLContentHandler
{
public:
/**
* Receive notification of the beginning of a document.
* @since ?Series60_version
* @param none
* @return none
*/		
    virtual TInt StartDocument() = 0;

/**
* Receive notification of the end of a document.
* @since ?Series60_version
* @param none
* @return none
*/
	virtual TInt EndDocument() = 0;

/**
* Receive notification of the beginning of an element.
* @since ?Series60_version
* @param aURI: The Namespace URI, or the empty string if the element has no Namespace URI or if Namespace
*              processing is not being performed.
* @param aLocalName: The local name (without prefix), or the empty string if Namespace processing is not being
*                    performed.
* @param aName: The qualified name, or the empty string if qualified names are not available.
* @param aAttributeList: The attributes attached to the element. If there are no attributes, it shall be an 
*                        empty Attributes object. 
*        
* @return 1 if success 0 if failure.
*/
	inline virtual TInt StartElement(TDesC& /*aURI*/, TDesC& /*aLocalName*/, TDesC& /*aName*/, MXMLAttributes* /*aAttributeList*/){return 1;}

/**
* Receive notification of the end of an element.
* @since ?Series60_version
* @param aURI: The Namespace URI, or the empty string if the element has no Namespace URI or if Namespace
*              processing is not being performed.
* @param aLocalName: The local name (without prefix), or the empty string if Namespace processing is not being
*                    performed.
* @param aName: The qualified name, or the empty string if qualified names are not available.
* @return 1 if success 0 if failure.
*/
	inline virtual TInt EndElement(TDesC& /*aURI*/, TDesC& /*aLocalName*/, TDesC& /*aName*/){return 1;}

/**
* Receive notification of character data inside an element.
* @since ?Series60_version
* @param aBuff: The characters.
* @param aStart: The start position in the character buffer.
* @param aLength: The number of characters to use from the character buffer.
* @return 1 if success 0 if failure.
*/
	inline virtual TInt Charecters(TDesC& /*aBuf*/, TInt /*aStart*/, TInt /*aLength*/){return 1;}

/**
* Receive notification of XML comment.
* @since ?Series60_version
* @param aComment: Buffer containing the comment.
* @return 1 if success 0 if failure.
*/
	inline virtual TInt Comment(TDesC& /*aComment*/){return 1;}

/**
* Receive notification of a processing instruction
* @since ?Series60_version
* @param aTarget: The processing instruction target.
* @param aData: The processing instruction data, or null if none is supplied.
* @return 1 if success 0 if failure.
*/
    inline virtual TInt ProcessingInstructions(TDesC& /*aTarget*/, TDesC& /*aData*/){return 1;}

/**
* Receive notification of ignorable whitespace in element content.
* @since ?Series60_version
* @param aString: The whitespace characters.
* @return 1 if success 0 if failure.
*/
	// R&D: Feature: IgnoreWhiteSpace missing from XMLInterface
	inline virtual TInt IgnoreWhiteSpace(TDesC& /*aString*/){return 1;}; 

/**
* Receive notification of beginning of some internal and external XML entities.
* @since ?Series60_version
* @param aName: The name of the entity.
* @return 1 if success 0 if failure.
*/

	inline virtual TInt StartEntity(TDesC& /*aName*/){return 1;};  

/**
* Receive notification of end of XML entities.
* @since ?Series60_version
* @param aName: The name of the entity.
* @return 1 if success 0 if failure.
*/

	inline virtual TInt EndEntity(TDesC& /*aName*/){return 1;}  

/**
* Receive notification of a skipped entity.
* @since ?Series60_version
* @param aName: The name of the skipped entity.
* @return 1 if success 0 if failure.
*/
	// R&D: Feature: SkippedEntity missing from XMLInterface
	inline virtual TInt SkippedEntity(TDesC& /*aName*/){return 1;} 

/**
* Receive notification of beginning of CData section.
* @since ?Series60_version
* @param none
* @return 1 if success 0 if failure.
*/
	inline virtual TInt StartCDATA(){return 1;}

/**
* Receive notification of end of CData section.
* @since ?Series60_version
* @param aName: none.
* @return 1 if success 0 if failure.
*/
	inline virtual TInt EndCDATA(){return 1;}

/**
* Receive notification of error situation during parsing.
* @since ?Series60_version
* @param aErrorCode: Error status code.
* @param aSeverity: Error Severity.
* @return 1 if success 0 if failure.
*/
	inline virtual TInt Error(TInt /*aErrorCode*/, TInt /*aSeverity*/){return 1;}
    
/**
* Returns parsed buffer to client.
* @since Series60 3.1
* @param aBuf Buffer with parsed data.
*/
	inline virtual void DataBuffer(const TDesC16& /*aBuf*/){return;}

/**
* Returns reader index
* @since Series60 3.1
* @param aIndex - reader index
*/
	inline virtual void ReaderIndex(NW_Uint32 /*aIndex*/){return;}


};


#endif      // __MXMLCONTENTHANDLER_H   
            
// End of File
