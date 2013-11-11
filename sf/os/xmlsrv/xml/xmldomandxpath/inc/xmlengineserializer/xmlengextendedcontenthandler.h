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
// Content handler for binary data
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGEXTENDEDCONTENTHANDLER_H
#define XMLENGEXTENDEDCONTENTHANDLER_H

#include <xml/dom/xmlengdatacontainer.h>

/**
This class extends MContentHandler with functions for binary data handling.
Used by clients during XOP deserialization.

@see Xml::MContentHandler
*/
class MXmlEngExtendedHandler
	{
public:
	/** The uid identifying this extension interface. */
	enum TExtInterfaceUid {EExtInterfaceUid = 0x101F9794}; 

	/**
	Called when binary content has been parsed.
 	 
	@param aBytes The raw binary data of the element
	@param aCid The CID of the binary data
	@param aErrorcode The parsing error code.  If this is not KErrNone, special
	action may be required.
	@leave - One of the system-wide error codes
 	*/
	virtual void OnBinaryContentL(const TDesC8& aBytes,
	                              const TDesC8& aCid,
	                              TInt aErrorCode) = 0;
	
	/**
	Called when a reference to external data has been parsed.  
	@see CXmlEngDeserializer::ExternalData()

	Any class derived from TXmlEngDataContainer may be passed to this function.
	@see TXmlEngBinaryContainer
	@see TXmlEngChunkContainer
	@see TXmlEngFileContainer

	@param aContainer The container that holds the external binary data.
	@param aErrorcode The parsing error code.  If this is not KErrNone, special
	action may be required.
	@leave - One of the system-wide error codes
 	*/	
	virtual void OnDataContainerL( const TXmlEngDataContainer& aContainer, 
								   TInt aErrorCode) = 0;
	};

#endif //XMLENGINE_EXTENDEDCONTENTHANDLER_H

