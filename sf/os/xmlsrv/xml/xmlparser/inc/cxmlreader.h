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



#ifndef __CXMLREADER_H
#define __CXMLREADER_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include <utf.h>
#include <xml/cxml/nw_cxml_parser.h>

#include "cxmlattributes.h"
#include <xml/mxmlcontenthandler.h>
#include <xml/mwbxmlcontenthandler.h>
#include "cxmlobserver.h"
#include <xml/xmlutils.h>


// CLASS DECLARATION

/**
*  Implementation of RXMLReader interface.
*  @lib XMLInterface.dll
*  @since ?Series60_version
*/
class CXMLReader : public CBase
{
public:  // Constructors and destructor
        
/**
* Two-phased constructor.
*/
	static CXMLReader* NewL();

	static CXMLReader* NewLC();
        
/**
* Destructor.
*/
	~CXMLReader();

public: // Implementation of RXMLAttributes Functions
        
/**
* Implementation of RXMLReader::SetFeature(TXMLFeature aFeature, TBool aStatus)
*/
	TInt SetFeature(TXMLFeature aFeature, TBool aStatus);

/**
* Implementation of RXMLReader::GetFeature(TXMLFeature aFeature)
*/
	TInt GetFeature(TXMLFeature aFeature, TBool& aStatus);

/**
* Implementation of RXMLReader::SetContentHandler(MXMLContentHandler& aContentHandler)
*/
	TInt SetContentHandler(MXMLContentHandler* aContentHandler);

/**
* Implementation of RXMLReader::SetContentHandler(MWBXMLContentHandler& aContentHandler)
*/
	TInt SetContentHandler(MWBXMLContentHandler* aContentHandler);

/**
* Implementation of RXMLReader::ParseL(RFs& aRFs, const TDesC& aFileToParse)
*/
	void ParseL(RFs& aRFs, const TDesC& aFileToParse);

/**
* Implementation of RXMLReader::ParseL(const TDesC& aBuff)
*/
	void ParseL(const TDesC8& aBuff);

/**
* Implementation of RXMLReader::ParseL(RFile& aOpenedFile)
*/
	void ParseL(RFile& aOpenedFile);


private:

/**
* C++ default constructor.
*/
	CXMLReader();

    void ConstructL();

/**
* Parse XML document
* @since ?Series60_version
* @param 
* @return status.
*/
	TInt ParseXML(const TDesC& aBuff);

/**
* Parse WBXML document
* @since ?Series60_version
* @param 
* @return status.
*/
	TInt ParseWBXML(const TDesC8& aBuff);


protected:  // Data

	// Parser CallBacks
	CXMLObserver* iParserObserver;
	// wbxml or xml input
	TBool iIsBinary;

};

#endif      // __CXMLREADER_H   
            
// End of File
