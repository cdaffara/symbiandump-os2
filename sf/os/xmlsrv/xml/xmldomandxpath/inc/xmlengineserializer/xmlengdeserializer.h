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
// XML Deserializer
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGDESERIALIZER_H
#define XMLENGDESERIALIZER_H

#include <e32base.h>
#include <xml/dom/xmlengparsingoptions.h>

#include <xml/contenthandler.h>
#include <xml/parser.h>
#include <e32std.h>

class RFs;
class TXmlEngDataContainer;

/** Controls the format of deserialization */
enum TXmlEngDeserializerType
	{
	/** Default deserialization (XML) */
	EDeserializerDefault,
	/** Deserialization from MIME Multipart containing XOP */
	EDeserializerXOP,
	/** Deserialization from XOP Infoset */
	EDeserializerXOPInfoset,
	/** Deserialization from GZip */
	EDeserializerGZip
	};

/** Controls the source of the deserialization */
enum TXmlEngDeserializationSource
	{
	EDeserializeFromFile,
	EDeserializeFromBuffer,
	EDeserializeFromStream
	};

/**
Provides deserialization and parsing for XML content using a SAX callback
interface.  Derived classes provide deserialization for specific formats,
whereas this class defines the interface and provides default (plain XML)
deserialization.

@see TXmlEngDeserializerType

Deserialization generates SAX events that are passed to a MContentHandler. 

@see Xml::MContentHandler

For deserialization to a DOM tree, see CXmlEngDeserializerDOM.
@see CXmlEngDeserializerDOM

Support for data stored outside the DOM tree, such as data linked by
xop:include references, is provided.  The external data must be set with
UseExternalDataL() prior to deserialization.  The data is then returned through
the MContentHandler interface.
*/
class CXmlEngDeserializer: public CBase
{
	friend class CXmlEngDeserializerXOP;
	friend class CXmlEngDeserializerGZIP;

 public:
    /**
	Creates a deserializer of the given type.  Returns an instance of this
	class or a derived class.

    @param aHandler Content handler for SAX events triggered by deserialization
    @param aType Deserializer type    
    @return The deserializer
	@leave - One of the system-wide error codes
    */
	IMPORT_C static CXmlEngDeserializer* NewL( Xml::MContentHandler& aContentHandler, TXmlEngDeserializerType aType = EDeserializerDefault);
												 
	/**
	Sets the input type to file and saves the file name for later
	deserialization.
	@param aFileName The file name of the file to deserialize
	@leave - One of the system-wide error codes
    */
	IMPORT_C void SetInputFileL(const TDesC& aFileName);

	/**
	Sets the input type to buffer and saves the buffer for later deserialization.
	@param aBuffer The buffer to deserialize
    */
	IMPORT_C void SetInputBuffer(const TDesC8& aBuffer);

	/**
	Sets the content handler which handles SAX events raised during
	deserialization.  Clients must derive from Xml::MContentHandler in order to
	receive SAX callbacks.

	@param aContentHandler The content handler
	@leave - One of the system-wide error codes
    */
	IMPORT_C virtual void SetContentHandlerL(Xml::MContentHandler& aContentHandler);
	
    /**
	Sets parsing options used for deserialization.
	@param aOptions Parsing options to set.  A copy of the options is stored.
    */	
	IMPORT_C virtual void SetParsingOptions(TXmlEngParsingOptions& aOptions);

    /**
	Gets the parsing options
	@return The parsing options
    */	
	IMPORT_C virtual const TXmlEngParsingOptions& ParsingOptions();

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
	Enables parser features.

	@see Xml::CParser::EnableFeature()
	@see Xml::CParser::TParserFeature
	@param aParserFeature The parser feature to enable
	@return KErrNone if successful, otherwise one of the system-wide error codes
    */	
	IMPORT_C virtual TInt EnableFeature(TInt aParserFeature);
	
    /**
	Disables parser features.

	@see Xml::CParser::DisableFeature()
	@see Xml::CParser::TParserFeature
	@param aParserFeature The parser feature to disable
	@return KErrNone if successful, otherwise one of the system-wide error codes
    */	
	IMPORT_C virtual TInt DisableFeature(TInt aParserFeature);

    /**
	Checks if a parser feature is enabled.

	@see Xml::CParser::IsFeatureEnabled()
	@see Xml::CParser::TParserFeature
	@param aParserFeature The parser feature to check
	@return ETrue if feature is enabled, otherwise EFalse
    */	
	IMPORT_C virtual TBool IsFeatureEnabled(TInt aParserFeature) const;

	/**
	Deserializes the file or buffer previously set by SetInputFileL() or
	SetInputBuffer().

	The result of deserialization is passed to the content handler as SAX
	events.

	@leave KXmlEngErrNoParameters No file or buffer has been previously set
	@leave - One of the system wide error codes or one of the Xml specific
	ones defined in XmlFrameworkErrors.h
    */
	IMPORT_C virtual void DeserializeL();     

    /**
	Deserializes the given file.  Any filename previously set with
	SetInputFileL() is ignored. 

	The result of deserialization is passed to the content handler as SAX
	events.

	@param aFileName The file to be parsed
	@param aOptions Parsing options
	@leave - One of the system wide error codes or one of the Xml specific
	ones defined in XmlFrameworkErrors.h
    */
	IMPORT_C virtual void DeserializeL( const TDesC& aFileName,
										const TXmlEngParsingOptions& aOptions = 
											TXmlEngParsingOptions()); 

    /**
	Deserializes the given file.  Any filename previously set with
	SetInputFileL() is ignored.  

	The result of deserialization is passed to the content handler as SAX
	events.

    @param aRFs File Server session
	@param aFileName The file to be parsed
	@param aOptions Parsing options
	@leave - One of the system wide error codes or one of the Xml specific
	ones defined in XmlFrameworkErrors.h
    */
	IMPORT_C virtual void DeserializeL( RFs& aRFs, 
										const TDesC& aFileName,
										const TXmlEngParsingOptions& aOptions = 
											TXmlEngParsingOptions()); 
	
    /**
	Deserializes the given buffer.  Any buffer previously set with
	SetInputBuffer() is ignored. 

	The result of deserialization is passed to the content handler as SAX
	events.

    @param aBuffer The buffer to be parsed 
	@param aOptions Parsing options
	@leave - One of the system wide error codes or one of the Xml specific
	ones defined in XmlFrameworkErrors.h
    */
    IMPORT_C virtual void DeserializeL( const TDesC8& aBuffer,
    									const TXmlEngParsingOptions& aOptions = 
											TXmlEngParsingOptions()); 

    /** Destructor */
    virtual ~CXmlEngDeserializer();

    /**
    Retrieves a pointer to the data container referenced by CID.
	@param aCid The aCid corresponding to the data container required
	@return The data container or NULL if no matching container is found
    */
	TXmlEngDataContainer* GetDataContainer(const TDesC8& aCid);

private:
    /** Default constructor */
    CXmlEngDeserializer( );
  
    /** 2nd phase constructor */
	virtual void ConstructL( Xml::MContentHandler& aContentHandler );
	
private:
	/** SAX Parser */
	Xml::CParser* iParser;
	
	/** File to be parsed */
	HBufC*	iInputFileName;	
	
	/** Buffer to be parsed */
	TPtrC8 iBuffer;
	
	/** Deserialization format */
	TXmlEngDeserializationSource iSerializationOutput;
	
	/** Array of external data */
	RArray<TXmlEngDataContainer>* iDataList;

	/** Features */
	TUint iFeatures;

	/** Parsing options */
	TXmlEngParsingOptions iParsingOptions;
	
}; //class CXmlEngDeserializer



#endif /* XMLENGDESERIALIZER_H */

