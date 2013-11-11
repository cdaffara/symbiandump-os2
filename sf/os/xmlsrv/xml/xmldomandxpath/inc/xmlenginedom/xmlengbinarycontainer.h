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
// All binary data functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGBINARYCONTAINER_H
#define XMLENGBINARYCONTAINER_H

#include <xml/dom/xmlengdatacontainer.h>

/**
This class represents binary data in a DOM tree.

Binary data is treated in general as text nodes in DOM tree. 
Data is stored in process's heap memory.

Sample code for creating binary container:
@code  
     RXmlEngDOMImplementation domImpl;
     domImpl.OpenL();        // opening DOM implementation object 
     RXmlEngDocument iDoc; 
     // create document element
     TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
     // create binary container from buffer (str1) and CID equals cid
     TXmlEngBinaryContainer binData=iDoc.CreateBinaryContainerL(cid,*str1);  
     elem.AppendChildL(binData);      // append container to the dom tree       
     iDoc.Close();               // closing all opened objects
     domImpl.Close();
@endcode 
*/
class TXmlEngBinaryContainer : public TXmlEngDataContainer
{
public:

    /** Get content of the container.
    @return The contents of the container
    */
    IMPORT_C TPtrC8 Contents() const;
    
    /** 
	Copies the given string and sets the contents of the binary container
    @param aNewContents  The actual value to store
	@leave KXmlEngErrNullNode Node is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C void SetContentsL( const TDesC8& aNewContents );

    /** 
	Appends contents to binary container
    @param aData  Content to be appended
	@leave KXmlEngErrNullNode Node is NULL
	@leave - One of the system-wide error codes
    */    
    IMPORT_C void AppendContentsL( const TDesC8& aData );

    /** Default constructor */
	inline TXmlEngBinaryContainer(); 

protected:

    /**
    Constructor
    @param aInternal node pointer
    */
	inline TXmlEngBinaryContainer(void* aInternal);
};

#include <xml/dom/xmlengbinarycontainer.inl>

#endif /* XMLENGBINARYCONTAINER_H */

