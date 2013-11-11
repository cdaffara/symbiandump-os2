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
// output stream for dom tree
//



/**
 @file
 @internalComponent
 @prototype
*/
#ifndef XMLENGCGZIPFILEOUTPUTSTREAM_H
#define XMLENGCGZIPFILEOUTPUTSTREAM_H

#include <xml/dom/xmlengoutputstream.h>
#include <ezbufman.h>   //MEZBufferManager

#include <ezgzip.h>     //TEZGZipHeader
class CEZCompressor;



class CXmlEngGZIPFileOutputStream : public CBase, public MXmlEngOutputStream, 
                                    public MEZBufferManager
{
public:
    static CXmlEngGZIPFileOutputStream* NewLC(RFile& aOutputFile, RFs& aRFs);
    static CXmlEngGZIPFileOutputStream* NewL(RFile& aOutputFile, RFs& aRFs);
    ~CXmlEngGZIPFileOutputStream();
    TInt CheckError();
		
    TInt Write( const TDesC8& aBuffer );
    TInt Close();

    /**
     * Implementation of MEZBufferManager interface.
     */
     void InitializeL(CEZZStream& aZStream);
     void NeedInputL(CEZZStream& aZStream);
     void NeedOutputL(CEZZStream& aZStream);
     void FinalizeL(CEZZStream& aZStream);

protected:
    CXmlEngGZIPFileOutputStream(RFile& aOutputFile, RFs& aRFs);
    void ConstructL();

private:
    void WriteL(const TDesC8& aBuffer);
    void CloseL();
			
private:
    TInt iError;
	    
    /**
     * Default input buffers size.
     */
    static const TInt KDefaultInputBufferSize;
	    
    /**
     * Output buffer size. It determines, how often output data will be written
     * to disk. (it's written to disk, when buffer is full.
     *
     */
    
    
    static const TInt KOutputBufferSize;
	    
    RBuf8 iOutputDescriptor;
    RBuf8 iInputDescriptor;
    RBuf8 iOldInputDescriptor;
	    
    /**
     * Engine of GZiping
     */
    CEZCompressor* iCompressor;
    TInt32 iCrc;
    TEZGZipHeader iHeader;
    TInt iUncompressedDataSize;
    RFile iOutputFile;
    RFs   iRFs;
    
    /**
     * State of the object.
     */
    TBool iNoInputNeeded;
    TBool iKeepGoing;
    TBool iCloseInvoked;
	    
}; 



#endif /* XMLENGCGZIPFILEOUTPUTSTREAM_H */  
