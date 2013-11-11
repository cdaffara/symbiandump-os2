// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// output stream for dom tree
//

#ifndef CTestFileOutputStream_H_INCLUDED
#define CTestFileOutputStream_H_INCLUDED

#include <f32file.h>
#include <xml/dom/xmlengoutputstream.h>



/**
 * TXmlEngFileOutputStream is used by XML Engine to serialize XML data from file.
 *  XmlEngineDOM.lib
 * @since S60 v3.2
 */
class CTestFileOutputStream : public MXmlEngOutputStream
	{
public:
    /**
     * Default constructor
     *
     * @since S60 v3.2
     * @param aFile Instance of class RFile
     * @param aRFs File server handle   
     */
    CTestFileOutputStream(RFile& aFile,RFs& aRFs);
    
    /**
     * Callback for writing a buffer with data to output stream
     *
     * @since S60 v3.2
     * @return number of bytes written or -1 in case of error
     */	
    TInt Write(const TDesC8 &aBuffer);
    
    /**
     * Callback for closing output stream
     *
     * @since S60 v3.2
     * @return 
     *       0 is succeeded, 
     *      -1 in case of error
     */		
    TInt Close();
    
    /**
     * Checks error flag
     *
     * @since S60 v3.2
     * @return error code
     */				
    TInt CheckError();

private:

    void WriteL(const TDesC8 &aBuffer);
			
private:
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
}; 



#endif /* CTestFileOutputStream_H_INCLUDED */  
