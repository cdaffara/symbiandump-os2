/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

#include "TestContentHandler.h"
#include "xmlengtester.h"

#include <e32svr.h>
#include <e32std.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <xml/dom/xmlengbinarycontainer.h>
#include <xml/dom/xmlengchunkcontainer.h>
#include "xmlengtesterdef.h"

#include <xml/dom/xmlengserializer.h>
#include <xml/dom/xmlengdeserializer.h>

#include <InetProtTextUtils.h>
#include <libc\string.h>
#include <charconv.h>
#include <UTF.H>
#include <xml/dom/xmlengdomparser.h>

/************************************** SerXOP ****************************************/



// -----------------------------------------------------------------------------
// CLibxml2Tester::SerializeDefaultFileL
// serialization to file test function 
// specification
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	

TInt CLibxml2Tester::SerializeDefaultFileL(CStifItemParser& aItem)
	{
	TPtrC pData;
	aItem.GetNextString(pData);
	
	TPtrC pDoc;
	aItem.GetNextString(pDoc);
	
	TPtrC pOut;
	aItem.GetNextString(pOut);
	   
    iDoc = parser.ParseFileL(pDoc);
        
  	HBufC8* buffer = ReadFileToBufferL(pData);
	CleanupStack::PushL( buffer );
    
    //create binary container  
	TXmlEngBinaryContainer binaryData = iDoc.CreateBinaryContainerL(CID_1(),buffer->Des());
	iDoc.DocumentElement().AppendChildL(binaryData);
	
	
	
	// procesing
	
	
	
	// create serializer 
	CXmlEngSerializer* serializer = CXmlEngSerializer::NewL();
	CleanupStack::PushL(serializer);
	
	// set serialization options
	TXmlEngSerializationOptions options ( TXmlEngSerializationOptions::KOptionStandalone );
	serializer->SetSerializationOptions( options );
	
	// set output
	TFileName srcFileName( pOut );
	serializer->SetOutputL( srcFileName );	
	
	// serialize
	TInt serializedBits = serializer->SerializeL( iDoc );
	
	// end processing
	
	CleanupStack::PopAndDestroy( 2 ); 
	return KErrNone;
	}
		

// End of file
