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
#ifndef XMLENGDATACONTAINER_H
#define XMLENGDATACONTAINER_H

#include <xml/dom/xmlengnode.h>

/**
This class represents different types of binary data in DOM tree.

It can be custom-serialized using the MXmlEngDataSerializer interface which can 
be set in the TXmlEngSerializationOptions used for saving the tree.
*/
class TXmlEngDataContainer : public TXmlEngNode
{
public:

    /** Get CID of data container.
    @return Cid of data container
    */
    IMPORT_C TPtrC8 Cid() const;
        
    /** Get data size in bytes
    @return Number of bytes occupied by data
    */
    IMPORT_C TUint Size() const;

protected:
    /** Default constructor */
	inline TXmlEngDataContainer(); 

    /**
    Constructor
    @param aInternal node pointer
    */
	inline TXmlEngDataContainer(void* aInternal);
};
 
#include <xml/dom/xmlengdatacontainer.inl>

#endif /* XMLENGDATACONTAINER_H */

