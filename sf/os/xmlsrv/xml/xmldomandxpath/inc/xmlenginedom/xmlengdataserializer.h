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
// Interface class describing class that may be used as
// serializer for binary data
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGMDATASERIALIZER_H
#define XMLENGMDATASERIALIZER_H

#include <xml/dom/xmlengnode.h>

/**
This interface is used to serialize binary data stored in a node, external
memory chunk (RChunk) or file.  This interface may be implemented by client
applications.
@see TXmlEngSerializationOptions
*/
class MXmlEngDataSerializer
    {
public:
    /** 
	Serializes the node.  The implementor of this class must allocate and own
	the memory returned by this method until serialization is complete.
    @param aNode A node to serialize
    @return The serialized node
	@leave - One of the system-wide error codes
    */
    virtual TPtrC8 SerializeDataL(TXmlEngNode aNode) = 0;
    };


#endif /* XMLENGMDATASERIALIZER_H */  
