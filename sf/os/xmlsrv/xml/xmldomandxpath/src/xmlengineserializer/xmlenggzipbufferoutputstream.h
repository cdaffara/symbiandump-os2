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
 @released
*/
#ifndef XMLENGCGZIPBUFFEROUTPUTSTREAM_H
#define XMLENGCGZIPBUFFEROUTPUTSTREAM_H

#include <xml/dom/xmlengoutputstream.h>
#include <ezbufman.h>   //MEZBufferManager
#include <ezgzip.h>     //TEZGZipHeader

class CEZCompressor;



class CXmlEngGZIPBufferOutputStream : public CBase, public MXmlEngOutputStream, public MEZBufferManager
	{
	public:
        static CXmlEngGZIPBufferOutputStream* NewLC(RBuf8& aOutputBuffer);

        static CXmlEngGZIPBufferOutputStream* NewL(RBuf8& aOutputBuffer);

	    ~CXmlEngGZIPBufferOutputStream();
	    
	    /**
	     * Get last error
	     */
		TInt CheckError();

        /**
         * Implementation of MXmlEngOutputStream interface.
         */
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
	    CXmlEngGZIPBufferOutputStream(RBuf8& aOutputBuffer);
	    void ConstructL();

    private:
        void WriteL(const TDesC8& aBuffer);
		void CloseL();
			
	private:
	    /**
	     * Last error
	     */
	    TInt iError;
	    
	    /**
	     * Default input buffers size.
	     */
	    static const TInt KDefaultInputBufferSize = 4096;
	    
	    /**
	     * Output buffer size. It determines, how often output data will be written
	     * to disk. (it's written to disk, when buffer is full.
	     *
	     */   
	    static const TInt KOutputBufferSize = 0x8000;
	    
	    RBuf8 iOutputDescriptor;
	    RBuf8 iInputDescriptor;
	    RBuf8 iOldInputDescriptor;
	    
	    /**
	     * Engine of GZiping
	     */
	    CEZCompressor* iCompressor;

	    RBuf8& iOutputString;
        /**
         * State of the object.
         */	    
	    TBool iNoInputNeeded;
	    TBool iKeepGoing;
	    TBool iCloseInvoked;
	    
	}; 



#endif /* XMLENGCGZIPBUFFEROUTPUTSTREAM_H */  
