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
// Node container implementation
//

#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengerrors.h>
#include "xmlengserializergzip.h"
#include "xmlenggzipoutputstream.h"
#include "xmlenggzipfileoutputstream.h"
#include "xmlenggzipbufferoutputstream.h"
#include "ezgzip.h"
#include <xml/dom/xmlengserializeerrors.h>



TInt CXmlEngSerializerGZIP::SerializeToFileL(RFs& aRFs,
                          const TDesC& aFileName,
                          const TXmlEngNode aNode,
                          const TXmlEngSerializationOptions& aOpt)
    {
    RFile outFile;
    
    User::LeaveIfError(outFile.Replace(aRFs,aFileName,EFileStream | EFileWrite | EFileShareExclusive));
    CleanupClosePushL(outFile);

    CXmlEngGZIPFileOutputStream* gzfos = CXmlEngGZIPFileOutputStream::NewLC(outFile,aRFs);
    TRAPD(err,aNode.OwnerDocument().SaveL(*gzfos,aNode,aOpt));
    TInt sErr = gzfos->CheckError();
    if(sErr)
        {
        User::Leave(sErr);
        }
    if(err)
        {
        User::Leave(err);
        }
    
    TInt fSize;
    outFile.Size(fSize);
    CleanupStack::PopAndDestroy(gzfos);
    CleanupStack::PopAndDestroy(&outFile);
    
    return fSize;
    }

TInt CXmlEngSerializerGZIP::SerializeToBufferL(RBuf8& aBuf,
                                            const TXmlEngNode aNode,
                                            const TXmlEngSerializationOptions& aOpt)
    {
    CXmlEngGZIPBufferOutputStream* gzbos = CXmlEngGZIPBufferOutputStream::NewLC(aBuf);
    TRAPD(err,aNode.OwnerDocument().SaveL(*gzbos,aNode,aOpt));
    TInt sErr = gzbos->CheckError();
    if(sErr)
        {
        User::Leave(sErr);
        }
    if(err)
        {
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(gzbos);
    return aBuf.Size();
    }
    
TInt CXmlEngSerializerGZIP::SerializeToStreamL(MXmlEngOutputStream& aStream,
                                            const TXmlEngNode aNode,
                                            const TXmlEngSerializationOptions& aOpt)
    {
    CXmlEngGZIPOutputStream* gzbos = CXmlEngGZIPOutputStream::NewLC(aStream);
    TInt size = 0;
    TRAPD(err,size = aNode.OwnerDocument().SaveL(*gzbos,aNode,aOpt));
    TInt sErr = gzbos->CheckError();
    if(sErr)
        {
        User::Leave(sErr);
        }
    if(err)
        {
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(gzbos);
    return size;
    }

// --------------------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------------------
//
CXmlEngSerializerGZIP* CXmlEngSerializerGZIP::NewL()
   {
   CXmlEngSerializerGZIP* self = new (ELeave) CXmlEngSerializerGZIP();
   return self;
   }

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
TInt CXmlEngSerializerGZIP::SerializeL(const TXmlEngNode aRoot)
    {
    TXmlEngSerializationOptions* defPtr = NULL;
    TXmlEngSerializationOptions def = TXmlEngSerializationOptions();
    if(iSerializationOptions )
    	{
    	defPtr = iSerializationOptions;
    	}
    else
        {
        defPtr = &def;
        }
    
	RXmlEngDocument doc = aRoot.OwnerDocument(); 
	
	switch(iSerializationOutput)
		{
		case ESerializeToFile:
			{
            if (!iOutFileName)
	            {
	            User::Leave(KXmlEngErrNoParameters);
	            } 	
	        return SerializeL(iOutFileName->Des(), aRoot, *defPtr);
			}
		case ESerializeToBuffer:
			{
			return SerializeL(*iBuffer, aRoot, *defPtr);
			}
		case ESerializeToStream:
			{
			if(!iOutputStream)
			    {
			    User::Leave(KXmlEngErrNoParameters);
			    }
			return SerializeToStreamL(*iOutputStream, aRoot, *defPtr);
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}						
		}
	return KErrGeneral; //should never happen
    }

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
TInt CXmlEngSerializerGZIP::SerializeL( RFs& aRFs, 
									 const TDesC& aFileName, 
									 const TXmlEngNode aRoot,
									 const TXmlEngSerializationOptions& aOptions )
    {
    return SerializeToFileL(aRFs, aFileName, aRoot, aOptions);
    }

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to file
// --------------------------------------------------------------------------------------
//
TInt CXmlEngSerializerGZIP::SerializeL( const TDesC& aFileName, 
                                     const TXmlEngNode aRoot,
									 const TXmlEngSerializationOptions& aOptions )
    {
    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    TInt outSize = SerializeToFileL(rfs, aFileName, aRoot, aOptions);
    CleanupStack::PopAndDestroy(&rfs);
    return outSize;
    }

// --------------------------------------------------------------------------------------
// Serializes TXmlEngNode to buffer
// --------------------------------------------------------------------------------------
//
TInt CXmlEngSerializerGZIP::SerializeL( RBuf8& aBuffer, 
                                     const TXmlEngNode aRoot, 										  
									 const TXmlEngSerializationOptions& aOptions)
    {
    return SerializeToBufferL(aBuffer,aRoot,aOptions);
    } 
  
// --------------------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------------------
//
CXmlEngSerializerGZIP::CXmlEngSerializerGZIP()
    {
    }
    
// --------------------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------------------
//
CXmlEngSerializerGZIP::~CXmlEngSerializerGZIP()
    {
    }  


