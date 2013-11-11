// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TESTCONTENTHANDLER_H__
#define __TESTCONTENTHANDLER_H__

#include <xml/dom/xmlengextendedcontenthandler.h>
#include <Xml\DocumentParameters.h>
#include <Xml\ContentHandler.h>
#include <f32file.h>

using namespace Xml;

const TInt KShortInfoSize = 256;
const TInt KLongInfoSize = 1000;

class CTestHandler : public CBase, public MContentHandler, public MXmlEngExtendedHandler
{

public:

	static CTestHandler* NewL(const RFile & aFileName);
	static CTestHandler* NewLC(const RFile & aFileName);
	
	~CTestHandler();

/**
This method is a callback to indicate the start of the document.
@param				aDocParam Specifies the various parameters of the document.
					aDocParam.iCharacterSetName The character encoding of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
	void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);


/**
This method is a callback to indicate the end of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
	void OnEndDocumentL(TInt aErrorCode);


/**
This method is a callback to indicate an element has been parsed.
@param				aElement is a handle to the element's details.
@param				aAttributes contains the attributes for the element.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, 
								 TInt aErrorCode);

	
/**
This method is a callback to indicate the end of the element has been reached.
@param				aElement is a handle to the element's details.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);


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
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);


/**
This method is a callback that sends the binary content of the element.
@param				aBytes is the raw binary content data for the element. 
@param                               aCid is the content data id
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	void OnBinaryContentL(const TDesC8& aBytes, const TDesC8& aCid, TInt aErrorCode);

/**
This method is a callback that sends data container reference
@param				aContainer Container encapsulating binary data
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	void OnDataContainerL(const TXmlEngDataContainer& aContainer, TInt aErrorCode);
	
/**
This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
This method is always called before the corresponding OnStartElementL method.
@param				aPrefix is the Namespace prefix being declared.
@param				aUri is the Namespace URI the prefix is mapped to.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
									   TInt aErrorCode);


/**
This method is a notification of the end of the scope of a prefix-URI mapping.
This method is called after the corresponding DoEndElementL method.
@param				aPrefix is the Namespace prefix that was mapped.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);


/**
This method is a notification of ignorable whitespace in element content.
@param				aBytes are the ignored bytes from the document being parsed.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);


/**
This method is a notification of a skipped entity. If the parser encounters an 
external entity it does not need to expand it - it can return the entity as aName 
for the client to deal with.
@param				aName is the name of the skipped entity.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode);


/**
This method is a receive notification of a processing instruction.
@param				aTarget is the processing instruction target.
@param				aData is the processing instruction data. If empty none was supplied.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, 
										  TInt aErrorCode);


/**
This method indicates an error has occurred.
@param				aError is the error code
*/
	void OnError(TInt aErrorCode);
	
	
/**
This method obtains the interface matching the specified uid.
@return				0 if no interface matching the uid is found.
					Otherwise, the this pointer cast to that interface.
@param				aUid the uid identifying the required interface.
*/
	TAny* GetExtendedInterface(const TInt32 aUid);
	
	CTestHandler();
	void ConstructL(const RFile & aFileName);
	
	private:
		RFile iLog;
		
};

#endif //__CONTENTHANDLER_H__