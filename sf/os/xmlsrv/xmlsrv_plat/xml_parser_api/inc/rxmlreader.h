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













#ifndef __RXMLREADER_H
#define __RXMLREADER_H

#warning The XML API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information


//  INCLUDES
#include <F32File.h>

#include <xml/MXMLContentHandler.h>
#include <xml/MWBXMLContentHandler.h>
#include <xml/XMLUtils.h>


// CLASS DECLARATION
class CXMLReader;
/**
*  Interface for XML Parser.
*  
*  @lib XMLInterface.dll
*  @since ?Series60_version
*/
class RXMLReader
{
public: // New functions

	inline RXMLReader();

/** 
* Creates the parser. 
* This function leaves with an apropriate code if the create failed.
* 
*/
	IMPORT_C void CreateL();

/** 
* Destroys the parser. 
* 
*/
	IMPORT_C void Destroy();

/** 
* Destroys the parser. 
* 
*/
	IMPORT_C void Close();
	

/**
* Enables/Dissables a feature.
* @since ?Series60_version
* @param aFeature: The feature to Enable/Dissable.
* @param aStatus: TRUE Enables the feature, FALSE Dissables.
* @return KErrNone if OK, KErrNotSupported if feature not supported.
*/
	IMPORT_C TInt SetFeature(TXMLFeature aFeature, TBool aStatus);

/**
* Check if a feature is Enabled.
* @since ?Series60_version
* @param aFeature: The feature to check.
* @param aStatus: TRUE if aFeature is Enabled. 
* @return KErrNone if OK, KErrNotSupported if feature not supported.
*/
	IMPORT_C TInt GetFeature(TXMLFeature aFeature, TBool& aStatus);

/**
* Allow an application to register a content event handler (for XML).
* @since ?Series60_version
* @param aContentHandler: The handler.
* @return Status, KErrNone if OK.
*/
	IMPORT_C TInt SetContentHandler(MXMLContentHandler* aContentHandler);

/**
* Allow an application to register a content event handler (for WBXML).
* @since ?Series60_version
* @param aContentHandler: The handler.
* @return Status, KErrNone if OK.
*/
	IMPORT_C TInt SetContentHandler(MWBXMLContentHandler* aContentHandler);

/**
* Parse an XML document.
* @since ?Series60_version
* @param aRFs: An open filesession.
* @param aFileToParse: Filename of file to parse.
* @return
*/
	IMPORT_C void ParseL(RFs &aRFs, const TDesC& aFileToParse);

/**
* Parse an XML document.
* @since ?Series60_version
* @param aBuff: Buffer containing document to parse. 
* @return 
*/
	IMPORT_C void ParseL(const TDesC8& aBuff);

/**
* Parse an XML document.
* @since ?Series60_version
* @param aOpenedFile: Opened file containing document to parse. 
* @return 
*/
	IMPORT_C void ParseL(RFile& aOpenedFile);

private:
	friend class CXMLReader;

	CXMLReader* iImplementation;
};

inline RXMLReader::RXMLReader()
		: iImplementation(0)
	{
	}

#endif      // __RXMLREADER_H   
            
// End of File
