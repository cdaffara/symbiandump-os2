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







#ifndef __MWBXMLCONTENTHANDLER_H
#define __MWBXMLCONTENTHANDLER_H

#warning The XML API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Interface for SAX wbxml Parser Callback events.
*  If an application (client) needs to be informed of basic parsing events, 
*  it implements this interface and registers an instance with the SAX parser using the SetContentHandler() method.
*  The parser uses the instance to report basic document-related events like the start and end of elements.
*
*  @lib XMLInterface.dll
*  @since ?Series60_version
*/
class MWBXMLContentHandler
{
public:
/**
* Receive notification of the beginning of a wbxml document.
* @since ?Series60_version
* @param none
* @return 1 if sucess.
*/		
    virtual TInt StartDocument() = 0;

/**
* Receive notification of the end of a wbxml document.
* @since ?Series60_version
* @param none
* @return 1 if sucess.
*/
	virtual TInt EndDocument() = 0;

/**
* Receive notification of the beginning of an element.
* @since ?Series60_version
* @return 1 if sucess.
*/
	inline virtual TInt TagStart(){return 1;}

/**
* Receive notification of the end of an element.
* @since ?Series60_version
* @param none 
* @return 1 if sucess.
*/
	inline virtual TInt TagEnd(){return 1;}

/**
* Receive notification of the beginning of a processing instruction.
* @since ?Series60_version
* @param none 
* @return 1 if sucess.
*/
	inline virtual TInt ProcessingInstruction(){return 1;}

/**
* Receive notification of the end of a processing instruction.
* @since ?Series60_version
* @param none 
* @return 1 if sucess.
*/
	inline virtual TInt ProcessingInstructionEnd(){return 1;}
	
/**
* Receive notification of a attribute.
* @since ?Series60_version
* @param none 
* @return 1 if sucess.
*/
	inline virtual TInt AttributeStart(){return 1;}

/**
* Receive notification of a attribute value.
* @since ?Series60_version
* @param none 
* @return 1 if sucess.
*/
	inline virtual TInt AttributeValue(){return 1;}
/**
* Receive notification of content.
* @since ?Series60_version
* @param none 
* @return 1 if sucess.
*/
	inline virtual TInt Content(){return 1;}

/**
* Receive notification of a code page switch in the document.
* @since ?Series60_version
* @param none 
* @return 1 if sucess.
*/
	inline virtual TInt CodePageSwitch(){return 1;}

/**
* Receive notification of a extencion.
* @since ?Series60_version
* @param none 
* @return 1 if sucess.
*/
	inline virtual TInt Extension(){return 1;}
	
	// Handle data types

/**
* Receive notification of a Token.
* @since ?Series60_version
* @param aToken: binary token
* @param aString: string representation of aToken 
* @return 1 if sucess.
*/
	inline virtual TInt Token(TUint8 /*aToken*/, TDesC& /*aString*/){return 1;}
/**
* Receive notification of a Fully Qualifyed Token.
* @since ?Series60_version
* @param aToken: binary token
* @param aString: string representation of aToken 
* @return 1 if sucess.
*/
	inline virtual TInt FQToken(TUint32 /*aToken*/, TDesC& /*aString*/){return 1;}

/**
* Receive notification of a inline string.
* @since ?Series60_version
* @param aString: the inline string. 
* @return 1 if sucess.
*/
	inline virtual TInt InlineString(TDesC& /*aString*/){return 1;}

/**
* Receive notification of a table string.
* @since ?Series60_version
* @param aString: the table string. 
* @return 1 if sucess.
*/
	inline virtual TInt TableString(TDesC& /*aString*/){return 1;}
/**
* Receive notification of binary data.
* @since ?Series60_version
* @param aToken: binary data
* @return 1 if sucess.
*/
	inline virtual TInt Binary(TUint32 /*aToken*/){return 1;}

/**
* Receive notification of opaque data.
* @since ?Series60_version
* @param aToken: binary data
* @param aString: the string representation of aToken 
* @return 1 if sucess.
*/
	inline virtual TInt Opaque(TUint32 /*aToken*/, TDesC& /*aString*/){return 1;}

/**
* Receive notification of entity.
* @since ?Series60_version
* @param aToken: the entity
* @param aString: the string representation of aToken.
* @return 1 if sucess.
*/
	inline virtual TInt Entity(TUint32 /*aToken*/, TDesC& /*aString*/){return 1;}
	  	
/**
* Receive notification of parse error.
* @since ?Series60_version
* @param 
* @return 1 if sucess.
*/
	inline virtual TInt Exception(){return 1;}

};


#endif      // __MXMLCONTENTHANDLER_H   
            
// End of File
