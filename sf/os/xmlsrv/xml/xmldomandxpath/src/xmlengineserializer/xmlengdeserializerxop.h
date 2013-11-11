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
// XML XOP Deserializer
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGDESERIALIZERXOP_H
#define XMLENGDESERIALIZERXOP_H

#include <xml/utils/xmlengxestd.h>
#include <xml/dom/xmlengdeserializer.h>

class RFs;
class CBodyPart;

using namespace Xml;

class CXmlEngContentHandler;

/**
 * XOP deserializer
 * 
 */
class CXmlEngDeserializerXOP: public CXmlEngDeserializer
{
 public:
    /**
     * Creates an instance of CXmlEngDeserializerXOP
	 * @param aContentHandler Content handler
	 * @param aCleanXOPInfoset Indicates that XOP package contains only clean XOP infoset	      
     *
     */
    static CXmlEngDeserializerXOP* NewL( MContentHandler& aContentHandler, TBool aCleanXOPInfoset );

    /**
     * Enables CParser feature
     *
	 * @param aParserFeature Paser feature flag
     */	
	IMPORT_C TInt EnableFeature(TInt aParserFeature);
	
    /**
     * Disables CParser feature
     *
	 * @param aParserFeature Paser feature flag
     */	
	IMPORT_C TInt DisableFeature(TInt aParserFeature);

    /**
     * Checks if a feature is enabled
     *
	 * @param aParserFeature Paser feature flag
     */	
	IMPORT_C TBool IsFeatureEnabled(TInt aParserFeature) const;

    /**
     * Deserializes XML
     *
     *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	void DeserializeL();  

    /**
     * Deserializes XML
     *
     * @param aFileName File to be parsed
     * @param aHandler Content handler for SAX callbacks
     * @param aOptions Parsing options
	 *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	void DeserializeL( const TDesC& aFileName, 
					   const TXmlEngParsingOptions& aOptions 
					   	= TXmlEngParsingOptions());	   

    /**
     * Deserializes XML
     *
     * @param aRFs File Server session
     * @param aFileName File to be parsed
     * @param aHandler Content handler for SAX callbacks
     * @param aOptions Parsing options
	 *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
	void DeserializeL( RFs& aRFs, 
					   const TDesC& aFileName, 
					   const TXmlEngParsingOptions& aOptions 
					   	= TXmlEngParsingOptions());     
	
    /**
     * Deserializes XML
     *
     * @param aBuffer Buffer to be parsed
     * @param aHandler Content handler for SAX callbacks  
     * @param aOptions Parsing options   
	 *
     * @leave KErrNoMemory, KErrGeneral, KXmlEngErrWrongEncoding.
     */
    void DeserializeL( const TDesC8& aBuffer, 
					   const TXmlEngParsingOptions& aOptions 
					   	= TXmlEngParsingOptions() );
    
    /**
     * Retrieves pointer to data referenced by CID from parsed multipart package
     *
     * @param aCid Content ID   
	 *
 	 * @return Pointer to referenced data, or NULL if data is not found
     */    
    TInt GetData(const TDesC8& aCid, TPtrC8& aData);
      
    /**
     * Destructor.
     *
     */
    ~CXmlEngDeserializerXOP();

protected:
    

private:
    /**
     * Constructor
     *
	 * @param aCleanXOPInfoset Indicates that XOP package contains only clean XOP infoset     
     */
    CXmlEngDeserializerXOP( TBool aCleanXOPInfoset );

    /**
     * 2nd Phase constructor
     */	 
	 void ConstructL( MContentHandler& aContentHandler );

	/**
	 * Read the file
	 *
	 * @param aRFs File Server session
	 * @param aFileName File name to be read
	 *
 	 * @return Pointer to memory buffer with file content 
	 */
	 HBufC8* ReadFileL( RFs& aRFs, const TDesC& aFileName );
	   

//	TXmlEngDeserializationSource SerializationOutput();
	

public: //data

protected: //data

	/** body of multipart package **/
	HBufC8* iData;

	/** body part array for parsed multipart document **/
	RPointerArray<CBodyPart> iBodyPartArray;
	
	/** pointer to single binary data **/
	TPtrC8 iDataPtr;
	
	/** internal content handler **/
	CXmlEngContentHandler* iInternalHandler;
	
	/**	Indicates that XOP package contains only clean XOP infoset **/
	TBool iCleanXOPInfoset;	

	//Features mode
	TInt iParserMode;		
		
private: //data

}; //class CXmlEngDeserializerXOP



#endif /* XMLENGDESERIALIZERXOP_H */
