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
#ifndef XMLENGXOPFILEOUTPUTSTREAM_H
#define XMLENGXOPFILEOUTPUTSTREAM_H

#include <f32file.h>
#include "xmlengxopoutputstream.h"

/**
 * TXmlEngSerializerXOPOS is used by XML Engine Serializer to 
 * serialize XML data to file.
 *
 */
class TXmlEngSXOPFileOutputStream : public MXmlEngSXOPOutputStream
	{
public:
    /**
     * Default constructor
     *
     * @param aFile Instance of class RFile
     * @param aRFs File server handle   
     */
    TXmlEngSXOPFileOutputStream(RFile& aFile,RFs& aRFs);
    
    /**
     * Callback for writing a buffer with data to output stream
     *
     * @return number of bytes written or -1 in case of error
     */	
    TInt Write(const TDesC8 &aBuffer);
    
    /**
     * Callback for closing output stream
     *
     * @return 
     *       0 is succeeded, 
     *      -1 in case of error
     */		
    TInt Close();

    /**
     * Callback for closing file
     *
     * @return KErrNone or other error code
     */		
    virtual TInt CloseAll(); 
    
    /**
     * Checks error flag
     *
     * @return error code
     */				
    virtual TInt CheckError();

private: // functions

    /**
     * Callback for writing a buffer with data to output stream
     */	
    void WriteL(const TDesC8 &aBuffer);
			
private: // data
    /** 
     *Instance of class RFile 
     */
    RFile iFile;
    
    /**
     * File server handle   
     */
    RFs iRFs;
    
    /** Error flag **/
    TInt iError;
    
    /** Drive No. **/
    TInt iDrive;    
}; 



#endif /* XMLENGXOPFILEOUTPUTSTREAM_H */  
