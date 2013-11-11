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
// XML Deserializer to DOM tree
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGDESERIALIZERDOM_H
#define XMLENGDESERIALIZERDOM_H

#include <xml/dom/xmlengdeserializer.h>

#include <e32std.h>
#include <e32base.h>


class RFs;
class RXmlEngDOMImplementation;
class RXmlEngDocument;

/**
Deserializes a XML file to a DOM tree.

Sample code for deserialization to a DOM tree:
@code
     // create deserializer
     CXmlEngDeserializerDOM* deserializer = CXmlEngDeserializerDOM::NewL();
     CleanupStack::PushL(deserializer);
     // configure deserializer
     deserializer->SetInputFileL(KInputFile, EDeserializerGZip);
     // Set the DOM Implementation
     deserializer->UseDOMImplementationL(iDomImpl);
     // deserialize
     RXmlEngDocument doc = deserializer->DeserializeL();
     CleanupStack::PopAndDestroy();// deserializer
@endcode 
*/
class CXmlEngDeserializerDOM: public CBase
{
 public:
    /**
    Creates an instance of CXmlEngDeserializerDOM.
    @return The DOM deserializer instance
	@leave KErrNoMemory Memory allocation failure
    */
    IMPORT_C static CXmlEngDeserializerDOM* NewL();
    												 
    /**
	Sets the input type to file and saves the file name for later
	deserialization.
	@param aFileName The file name of the file to deserialize
	@param aType The deserialization format
	@leave - One of the system-wide error codes
    */
	IMPORT_C void SetInputFileL(const TDesC& aFileName, TXmlEngDeserializerType aType = EDeserializerDefault);

    /**
	Sets the input to to buffer and saves the buffer for later deserialization.
	@param aBuffer The buffer to deserialize
	@param aType The deserialization format
    */
	IMPORT_C void SetInputBuffer(const TDesC8& aBuffer, TXmlEngDeserializerType aType = EDeserializerDefault);

    /**
	Sets a list of data containers and indicates that the XML to be
	deserialized contains references (such as xop:include) to data stored
	outside the DOM tree.	

	Upon deserialization, the references are substituted with the matching data
	containers and returned via the content handler.

	This often occurs when a multipart MIME message is received.  The text XML
	content may be in one part, while the binary content is in another part.
	Each part containing binary content must be placed into a data container using
	RXmlEngDocument::CreateBinaryContainerL() or similar.  When deserialization
	of the XML occurs, the data containers are retrieved by content-id.

	Ownership is not transferred and the list must stay in scope for the
	lifetime of the deserializer.
	
	@param aList The list of data containers
	@leave - Any system-wide error code
    */
	IMPORT_C void UseExternalDataL(RArray<TXmlEngDataContainer>& aList);

    /**
	Gets the list of external data containers.
	@return The list of data containers or NULL if no external data is registered
    */
	IMPORT_C RArray<TXmlEngDataContainer>* ExternalData();  
	
	/**
    Sets the DOM implementation that will be used to create the DOM tree
	@param aImpl The DOM implementation
	@leave - Does not leave
    */
	IMPORT_C void UseDOMImplementationL(RXmlEngDOMImplementation& aImpl);

    /**
	Deserializes the file or buffer previously set by SetInputFileL() or
	SetInputBuffer() and creates a new document that holds the DOM tree.

	@return The document that holds the DOM tree
	@leave KXmlEngErrNoParameters No file or buffer has been previously set
	@leave KXmlEngErrWrongUseOfAPI DOM Implementation not set
	@leave - One of the system-wide error codes 
    */
	IMPORT_C RXmlEngDocument DeserializeL();     

	/**
	Deserializes the given file and creates a new document that holds the DOM
	tree.  Any filename previously set with SetInputFileL() is ignored.  

	@param aFileName The file to be deserialized
	@param aType The deserialization format
	@return The document that holds the DOM tree
	@leave KXmlEngErrWrongUseOfAPI DOM Implementation not set
	@leave - One of the system wide error codes
    */
	IMPORT_C RXmlEngDocument DeserializeL( const TDesC& aFileName, TXmlEngDeserializerType aType = EDeserializerDefault); 

    /**
	Deserializes the given file and creates a new document that holds the DOM
	tree.  Any filename previously set with SetInputFileL() is ignored.  

    @param aRFs File Server session
	@param aFileName The file to be deserialized
	@param aType The deserialization format
	@return The document that holds the DOM tree
	@leave KXmlEngErrWrongUseOfAPI DOM Implementation not set
	@leave - One of the system wide error codes
    */
	IMPORT_C RXmlEngDocument DeserializeL( RFs& aRFs, const TDesC& aFileName, TXmlEngDeserializerType aType = EDeserializerDefault); 
	
    /**
	Deserializes the given buffer and creates a new document that holds the DOM
	tree.  Any buffer previously set with SetInputBuffer() is ignored.  

    @param aBuffer Buffer to be parsed  
    @param aType Input type 
	@return The document that holds the DOM tree
	@leave KXmlEngErrWrongUseOfAPI DOM Implementation not set
	@leave - One of the system wide error codes
    */
    IMPORT_C RXmlEngDocument DeserializeL( const TDesC8& aBuffer, TXmlEngDeserializerType aType = EDeserializerDefault); 

    /** Destructor. */
    ~CXmlEngDeserializerDOM();

protected:
    /** Default constructor. */
    CXmlEngDeserializerDOM();
  
protected: 
	/** File to be parsed */
	HBufC* iInputFileName;	
	/** Buffer to be parsed */
	TPtrC8 iBuffer;
	/** Array of external data */
	RArray<TXmlEngDataContainer>* iDataList;
	/** Deserializer type */
	TXmlEngDeserializerType iType;
	/** DOM implementation */
	RXmlEngDOMImplementation* iImpl;

}; //class CXmlEngDeserializerDOM

#endif /* XMLENGDESERIALIZERDOM_H */

