// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __CONTENTHANDLER_H__
#define __CONTENTHANDLER_H__


#include <xml/attribute.h> // needed for RAttributeArray

class RString;

namespace Xml
{

class RDocumentParameters;
class RTagInfo;


class MContentHandler
/**
This class defines the interface required by a client of the xml framework.
It allows a client to be placed in a chain with other clients, i.e. a Parser, a Validator, 
or a User, and therefore allows the flow of information between these links.
It provides callbacks analogous to that of the SAX 2.0 interface.
@see http://www.saxproject.org/apidoc/

@publishedAll
@released
*/
	{
public:

/**
This method is a callback to indicate the start of the document.
@param				aDocParam Specifies the various parameters of the document.
@param				aDocParam.iCharacterSetName The character encoding of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
	virtual void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode) = 0;


/**
This method is a callback to indicate the end of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
	virtual void OnEndDocumentL(TInt aErrorCode) = 0;


/**
This method is a callback to indicate an element has been parsed.
@param				aElement is a handle to the element's details.
@param				aAttributes contains the attributes for the element.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	virtual void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, 
								 TInt aErrorCode) = 0;

	
/**
This method is a callback to indicate the end of the element has been reached.
@param				aElement is a handle to the element's details.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	virtual void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode) = 0;


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
	virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode) = 0;

	
/**
This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
This method is always called before the corresponding OnStartElementL method.
@param				aPrefix is the Namespace prefix being declared.
@param				aUri is the Namespace URI the prefix is mapped to.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	virtual void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
									   TInt aErrorCode) = 0;


/**
This method is a notification of the end of the scope of a prefix-URI mapping.
This method is called after the corresponding DoEndElementL method.
@param				aPrefix is the Namespace prefix that was mapped.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	virtual void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode) = 0;


/**
This method is a notification of ignorable whitespace in element content.
@param				aBytes are the ignored bytes from the document being parsed.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	virtual void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode) = 0;


/**
This method is a notification of a skipped entity. If the parser encounters an 
external entity it does not need to expand it - it can return the entity as aName 
for the client to deal with.
@param				aName is the name of the skipped entity.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	virtual void OnSkippedEntityL(const RString& aName, TInt aErrorCode) = 0;


/**
This method is a receive notification of a processing instruction.
@param				aTarget is the processing instruction target.
@param				aData is the processing instruction data. If empty none was supplied.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	virtual void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, 
										  TInt aErrorCode) = 0;


/**
This method indicates an error has occurred.
@param				aErrorCode is the error code
*/
	virtual void OnError(TInt aErrorCode) = 0;


/**
This method obtains the interface matching the specified uid.
@return				0 if no interface matching the uid is found.
					Otherwise, the this pointer cast to that interface.
@param				aUid the uid identifying the required interface.
*/
	virtual TAny* GetExtendedInterface(const TInt32 aUid) = 0;

	};

}


#endif //__CONTENTHANDLER_H__
