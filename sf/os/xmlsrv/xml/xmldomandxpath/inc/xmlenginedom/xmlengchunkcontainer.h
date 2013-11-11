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
// All memory chunk data functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGCHUNKCONTAINER_H
#define XMLENGCHUNKCONTAINER_H

#include <xml/dom/xmlengdatacontainer.h>


/**
This class stores a reference to a RChunk handle within the DOM tree.  No
ownership is tranferred and the client is expected to close the RChunk when it
is no longer required.  Multiple chunk containers can refer to the same RChunk,
regardless of whether the offsets overlap.

This class is most often instantiated by calling
RXmlEngDocument::CreateChunkContainerL().

Sample code for creating chunk container:
@code  
     RXmlEngDOMImplementation domImpl;
     domImpl.OpenL();        // opening DOM implementation object 
     RXmlEngDocument iDoc; 
     // create document element
     TXmlEngElement elem = iDoc.CreateDocumentElementL(_L8("doc"));
     // create RChunk object with chunkName name and size
     RChunk chunk;
     chunk.CreateGlobal(chunkName, size, maxSize);
     CleanupClosePushL(chunk); 
     // create chunk container from Rchunk object with offset to the 
     // binary data in chunk and binary data size in chunk (binarySize) 
     TXmlEngChunkContainer binData = iDoc.CreateChunkContainerL(
								cid,chunk,offset,binarySize); 
     // append chunkcontainer to the dom tree          
     TXmlEngNode ref = iDoc.DocumentElement().AppendChildL(binData);
     // closing all opened objects
     CleanupStack::PopAndDestroy(); //chunk
     iDoc.Close();              
     domImpl.Close();
@endcode 
*/
class TXmlEngChunkContainer : public TXmlEngDataContainer
{
public:
    /** Get memory chunk reference
    @return Memory chunk reference
    */
    IMPORT_C RChunk& Chunk() const;

    /** Get offset of binary data in memory chunk
    @return Offset of binary data in memory chunk
    */
    IMPORT_C TUint ChunkOffset() const;   
	
protected:
    /** Default constructor */
	inline TXmlEngChunkContainer(); 
	
    /**
    Constructor
    @param aInternal node pointer
    */
	inline TXmlEngChunkContainer(void* aInternal);
};



#include <xml/dom/xmlengchunkcontainer.inl>

#endif /* XMLENGCHUNKCONTAINER_H */

