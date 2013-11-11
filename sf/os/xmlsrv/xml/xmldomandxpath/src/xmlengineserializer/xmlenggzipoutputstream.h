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
// Class used to serialize DOM tree to compressed stream
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGCGZIPOUTPUTSTREAM_H
#define XMLENGCGZIPOUTPUTSTREAM_H

#include <xml/dom/xmlengoutputstream.h>
#include <ezbufman.h>   //MEZBufferManager
#include <ezgzip.h>     //TEZGZipHeader

class CEZCompressor;

class CXmlEngGZIPOutputStream : public CBase, public MXmlEngOutputStream, public MEZBufferManager
	{
	public:
        static CXmlEngGZIPOutputStream* NewLC(MXmlEngOutputStream& aOutputStream);

        static CXmlEngGZIPOutputStream* NewL(MXmlEngOutputStream& aOutputStream);

	    ~CXmlEngGZIPOutputStream();
	    
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
	    CXmlEngGZIPOutputStream(MXmlEngOutputStream& aOutputBuffer);
	    void ConstructL();

    private:
        void WriteL(const TDesC8& aBuffer);
		void CloseL();
			
	private:
	    /**
	     * Last error
	     */
	    TInt iError;
	    
	    RBuf8 iOutputDescriptor;
	    RBuf8 iInputDescriptor;
	    RBuf8 iOldInputDescriptor;
	    
	    /**
	     * Engine of GZiping
	     */
	    CEZCompressor* iCompressor;

	    MXmlEngOutputStream* iOutputString;
        /**
         * State of the object.
         */	    
	    TBool iNoInputNeeded;
	    TBool iKeepGoing;
	    TBool iCloseInvoked;
	    
	}; 



#endif /* XMLENGCGZIPOUTPUTSTREAM_H */  
