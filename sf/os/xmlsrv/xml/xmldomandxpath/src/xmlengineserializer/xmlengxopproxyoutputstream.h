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
#ifndef XMLENGXOPPROXYOUTPUTSTREAM_H
#define XMLENGXOPPROXYOUTPUTSTREAM_H

#include <f32file.h>
#include "xmlengxopoutputstream.h"

/**
 * TXmlEngSXOPOutputStream is used by XML Engine Serializer to 
 * serialize XML data to client stream.
 *
 */
class TXmlEngSXOPProxyOutputStream : public MXmlEngSXOPOutputStream
	{
public:
	/**
     * Default constructor
     *
     * @param aStream Instance of client stream
     */
    TXmlEngSXOPProxyOutputStream(MXmlEngOutputStream& aStream);
    
    /**
     * Callback for writing a buffer with data to output stream
     *
     * @return number of bytes written or -1 in case of error
     */	
    TInt Write(const TDesC8& aBuffer);
    
    /**
     * Callback for closing output stream
     *
     * @return KErrNone or other error code
     */		
    TInt Close();

    /**
     * Callback for closing client output stream
     *
     * @return KErrNone or other error code
     */		
    TInt CloseAll();    
    
    /**
     * Checks error flag
     *
     * @return error code
     */				
    TInt CheckError();
			
private:
    /** 
     * Client output stream
     */
    MXmlEngOutputStream* iStream;
    
    /** Error flag **/
    TInt iError;
}; 



#endif /* XMLENGXOPPROXYOUTPUTSTREAM_H */  
