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
* Description:  ?Description
*
*/



#ifndef __CXMLOBSERVER_H
#define __CXMLOBSERVER_H

//  INCLUDES
#include <e32std.h>
#include <txtetext.h>
#include "cxmlattributes.h"
#include <xml/mxmlcontenthandler.h>
#include <xml/mwbxmlcontenthandler.h>


// CLASS DECLARATION

/**
*  Help class for CXMLReader.
*  @lib XMLInterface.dll
*  @since ?Series60_version
*/
class CXMLObserver : public CBase
    {
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXMLObserver* NewL();

		static CXMLObserver* NewLC();
        
        /**
        * Destructor.
        */
        ~CXMLObserver();

public: // New functions
        
/**
* Registers content handler
* @since ?Series60_version
* @param aContentHandler: The ContentHandler to use.
* @return none.
*/
	inline void SetContentHandler(MXMLContentHandler* aContentHandler){iContentHandler = aContentHandler;}

/**
* Registers a binary content handler
* @since ?Series60_version
* @param aContentHandler: The ContentHandler to use.
* @return none.
*/
	inline void SetContentHandler(MWBXMLContentHandler* aContentHandler){iBinaryContentHandler = aContentHandler;}

/**
* Gets content handler
* @since ?Series60_version
* @param none.
* @return ContentHandler.
*/
	inline MXMLContentHandler* GetContentHandler(){return iContentHandler;}

	/**
* Gets binary content handler
* @since ?Series60_version
* @param none.
* @return ContentHandler.
*/
	inline MWBXMLContentHandler* GetBinaryContentHandler(){return iBinaryContentHandler;}


/**
* Gets current attribute list
* @since ?Series60_version
* @return pointer to current attribute list
*/
	inline CXMLAttributes* GetAttributes(){return iAttributes;}

/**
* Get current element
* @since ?Series60_version
* @return Current element
*/
	TPtrC GetElement() const;

/**
* Set current element
* @since ?Series60_version
* @param Current element
* @return none.
*/
	void SetElementL(const TPtrC &aName);

/**
* Remove current element
* @since ?Series60_version
* @return none
*/
	void RemoveElement();

/**
* Set element URI
* @since ?Series60_version
* @param aURI: the URI.
* @param aNSextension: namespace extension mapped to aURI.
* @return none
*/
	void SetURIL(const TPtrC &aURI, const TPtrC &aNSextension);

/**
* Get element URI
* @since ?Series60_version
* @param aNSextension: namespace extension mapped to aURI.
* @return the URI.
*/
	TPtrC GetURI(const TPtrC &aNSextension) const;

/**
* ResetURI, reset URI list
* @since ?Series60_version
* @return none.
*/
	void ResetURI();


private:
/**
* C++ default constructor.
*/
	CXMLObserver(); 
	
	void ConstructL();

public:	   // Data
	// Keeps current attribute name
	TPtrC* iCurrentAttribute;	

private:  // Data
	// Parser CallBacks
	MXMLContentHandler* iContentHandler;
	// Binary call backs
	MWBXMLContentHandler* iBinaryContentHandler;
	// Current attribute list
	CXMLAttributes* iAttributes;
	// Keeps track of elements (needed for binary parsing)
	CDesCArraySeg* iDescElements;

	// Keep track off namespace extensions and URIs.
	CDesCArraySeg* iNSextensions;
	CDesCArraySeg* iURIs;
};

#endif      // __CXMLOBSERVER_H   
            
// End of File
