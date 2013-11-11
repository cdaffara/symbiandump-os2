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
// All file container data functions
// @file
// @publishedAll
// @released
//



#ifndef XMLENGFILECONTAINER_H
#define XMLENGFILECONTAINER_H

#include <xml/dom/xmlengdatacontainer.h>

class RFile;

/**
This class represents data stored as a RFile in the DOM tree.

The RFile container is treated in general as a text node in the DOM tree.  Data
is stored in the file system referenced by the RFile handle.

Sample code for creating a file container:
@code  
     RXmlEngDOMImplementation domImpl;
     domImpl.OpenL();        		// opening DOM implementation object 
     RXmlEngDocument iDoc; 
     // create document element
     TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
     // create file container from file (file1 is an RFile object) and CID equals cid
     TXmlEngFileContainer binData = iDoc.CreateFileContainerL(cid, file1);
     elem.AppendChildL(binData);    // append container to the dom tree       
     iDoc.Close();               	// closing all opened objects
     domImpl.Close();
@endcode 
*/
class TXmlEngFileContainer : public TXmlEngDataContainer
{
public:
    /**
    Gets the RFile reference
    @return RFile reference
    */
    IMPORT_C RFile& File() const;
	
protected:
    /** Default constructor */
	inline TXmlEngFileContainer(); 
	
    /**
    Constructor
    @param aInternal Node pointer
    */
	inline TXmlEngFileContainer(void* aInternal);
};

#include <xml/dom/xmlengfilecontainer.inl>

#endif // XMLENGFILECONTAINER_H 

