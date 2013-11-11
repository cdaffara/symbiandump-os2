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
// Implementation of XOP serializer Output Stream
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGXOPOUTPUTSTREAM_H
#define XMLENGXOPOUTPUTSTREAM_H

#include <f32file.h>
#include <xml/dom/xmlengoutputstream.h>

/**
 * MXmlEngSXOPOutputStream is used by XML Engine Serializer to 
 * serialize XML data.
 * NOTE: this class should never be exported due to binary compatibility issues
 *
 */
class MXmlEngSXOPOutputStream : public MXmlEngOutputStream
	{
public:
    /**
     * Callback for final closing stream
     *
     * @return Error code
     */		
    virtual TInt CloseAll() = 0;
    
    /**
     * Checks error flag
     *
     * @return error code
     */				
    virtual TInt CheckError() = 0;
}; 



#endif /* XMLENGXOPOUTPUTSTREAM_H */  
