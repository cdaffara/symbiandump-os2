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



// INCLUDE FILES
#include <xml/rxmlreader.h>
#include "cxmlreader.h"



// requirement for E32 DLLs

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RXMLReader::CreateL()
// -----------------------------------------------------------------------------
//
EXPORT_C void RXMLReader::CreateL()
	{
		iImplementation = CXMLReader::NewL();
	}



// -----------------------------------------------------------------------------
// RXMLReader::Destroy()
// -----------------------------------------------------------------------------
//  
EXPORT_C void RXMLReader::Destroy()
    {
		if(iImplementation)
		{
			delete iImplementation;
			iImplementation=NULL;
		}
    }

// -----------------------------------------------------------------------------
// RXMLReader::Destroy()
// -----------------------------------------------------------------------------
//  
EXPORT_C void RXMLReader::Close()
    {
		Destroy();
    }
    
// -----------------------------------------------------------------------------
// CXMLReader::SetFeature
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RXMLReader::SetFeature(TXMLFeature aFeature, TBool aStatus)
	{
		return iImplementation->SetFeature(aFeature, aStatus);
	}

// -----------------------------------------------------------------------------
// CXMLReader::GetFeature
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RXMLReader::GetFeature(TXMLFeature aFeature, TBool& aStatus)
	{
		return iImplementation->GetFeature(aFeature, aStatus);
	}

// -----------------------------------------------------------------------------
// CXMLReader::SetContentHandler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RXMLReader::SetContentHandler(MXMLContentHandler* aParserObserver)
	{
		return iImplementation->SetContentHandler(aParserObserver);
	}

// -----------------------------------------------------------------------------
// CXMLReader::SetContentHandler
// Set binary XML content handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RXMLReader::SetContentHandler(MWBXMLContentHandler* aParserObserver)
	{
		return iImplementation->SetContentHandler(aParserObserver);
	}

// -----------------------------------------------------------------------------
// CXMLReader::Parse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RXMLReader::ParseL(RFs &aRFs, const TDesC& aFileToParse)
   {
		iImplementation->ParseL(aRFs, aFileToParse);
   }

// -----------------------------------------------------------------------------
// CXMLReader::Parse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RXMLReader::ParseL(const TDesC8& aBuff) 
	{
		iImplementation->ParseL(aBuff);
	}

// -----------------------------------------------------------------------------
// CXMLReader::Parse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RXMLReader::ParseL(RFile& aOpenedFile) 
	{
		iImplementation->ParseL(aOpenedFile);
	}

//  End of File  
