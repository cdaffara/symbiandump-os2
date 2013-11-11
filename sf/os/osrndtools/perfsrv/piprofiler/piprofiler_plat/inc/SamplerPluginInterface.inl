/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


// LITERALS
_LIT8(KTrue, "true");
_LIT8(KFalse, "false");

inline CSamplerPluginInterface::CSamplerPluginInterface()
    : iOrder( KSamplerPluginNotIndexed )
    {
    iBuffer = 0;
    iStream = 0;
    }

// -----------------------------------------------------------------------------
// CSamplerPluginInterface::~CSamplerPluginInterface()
// Destructor.
// -----------------------------------------------------------------------------
//
inline CSamplerPluginInterface::~CSamplerPluginInterface()
    {
    iBuffer = 0;
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }


inline CSamplerPluginInterface* CSamplerPluginInterface::NewL(const TUid aImplementationUid, TAny* aInitParams)
    {
    // Define options, how the default resolver will find appropriate
    // implementation.
    return REINTERPRET_CAST(CSamplerPluginInterface*, 
                            REComSession::CreateImplementationL(aImplementationUid,
                                                                _FOFF( CSamplerPluginInterface, iDtor_ID_Key ),
                                                                aInitParams)); 
    }

inline void CSamplerPluginInterface::ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray)
    {
    REComSession::ListImplementationsL(KSamplerPluginInterfaceUid, aImplInfoArray);
    }

inline void CSamplerPluginInterface::SetOrder( TInt aOrder )
    {
    iOrder = aOrder;
    }

inline TInt CSamplerPluginInterface::Flush() 
    {
	// complete the header
	TUint32 header;
	header = (iBuffer->iDataSize & 0x00ffffff) - 4;
	header += (iSamplerId << 24);

	// flush the header info
	iBuffer->iBuffer[0] = header;
	iBuffer->iBuffer[1] = header >> 8;
	iBuffer->iBuffer[2] = header >> 16;
	iBuffer->iBuffer[3] = header >> 24;
	
    // write data to filled buffers
    iStream->AddToFilledBuffers(iBuffer);
    // notify selected writer plugin to write data to output
    iStream->NotifyWriter();

    iBuffer = 0;

	return KErrNone;
}


inline TInt CSamplerPluginInterface::AddSample(TUint8* aSample, TUint32 aLength, TInt aLimitSize)
    {
    LOGTEXT(_L("CSamplerPluginInterface::AddSample - entry"));
	if(iBuffer == 0) 
	    {
	    // get next free buffer where to write data
		iBuffer = iStream->GetNextFreeBuffer();
		iBuffer->iBufDes->Zero();
		
		// get space for the header
		TUint32 header = 0;
		iBuffer->iBufDes->Append((TUint8*)&header, 4);	
		iBuffer->iDataSize += 4;
	    }
		
	// add data to the buffer...
	// if all data fit to the current buffer
	if(iBuffer->iBufferSize - iBuffer->iDataSize >= (TInt)aLength)
	    {
		iBuffer->iBufDes->Append(aSample, (TInt)aLength);
		iBuffer->iDataSize += (TInt)aLength;
	    }
	else 
	    {	
		// fill in the buffer
		TUint32 rest = iBuffer->iBufferSize - iBuffer->iDataSize;
		iBuffer->iBufDes->Append(aSample, rest);
		iBuffer->iDataSize += (TInt)rest;
		
		// The buffer is full now, complete the header
		TUint32 header;
		header = (iBuffer->iDataSize & 0x00ffffff) - 4;
		header += (iSamplerId << 24);
		iBuffer->iBuffer[0] = header;
		iBuffer->iBuffer[1] = header >> 8;
		iBuffer->iBuffer[2] = header >> 16;
		iBuffer->iBuffer[3] = header >> 24;
		
		// write data to filled buffers
		iStream->AddToFilledBuffers(iBuffer);
	    // notify selected writer plugin to write data to output
	    iStream->NotifyWriter();
		
		// Fetch an empty buffer and reserve space for the header
		iBuffer = iStream->GetNextFreeBuffer();
		iBuffer->iBufDes->Zero();
		header = 0;
		iBuffer->iBufDes->Append((TUint8*)&header, 4);	
		iBuffer->iDataSize += 4;
			
		// copy the rest of data to the new buffer
		iBuffer->iBufDes->Append(aSample+rest, aLength-rest);
		iBuffer->iDataSize += (TInt)aLength-rest;
	    }
	
	// Once iBuffer->dataSize reaches the limitSize, data from iBuffer is flushed to file/debug port.
	// If limitSize is set to zero, buffer is not changed until iBuffer gets full.
	if(aLimitSize != 0) 
	    {
		if(iBuffer->iDataSize >= aLimitSize) 
		    {
			// The buffer is full now, complete the header
			TUint32 header;
			header = (iBuffer->iDataSize & 0x00ffffff) - 4;
			header += (iSamplerId << 24);
			iBuffer->iBuffer[0] = header;
			iBuffer->iBuffer[1] = header >> 8;
			iBuffer->iBuffer[2] = header >> 16;
			iBuffer->iBuffer[3] = header >> 24;
	

            // write data to filled buffers
            iStream->AddToFilledBuffers(iBuffer);
            // notify selected writer plugin to write data to output
            iStream->NotifyWriter();
		    
			// Fetch an empty buffer and reserve space for the header
			iBuffer = iStream->GetNextFreeBuffer();
			iBuffer->iBufDes->Zero();
			header = 0;
			iBuffer->iBufDes->Append((TUint8*)&header, 4);	
			iBuffer->iDataSize += 4;
		    }
	    }
	return KErrNone;
    }

// ----------------------------------------------------------------------------
// Converts given descriptor into TBool value.
// ----------------------------------------------------------------------------
//
inline void CSamplerPluginInterface::Str2Bool(const TDesC8& aBuf, TBool& aValue)
    {
    if (aBuf.CompareF(KFalse) == 0)
        aValue = EFalse;
    else
        aValue = ETrue;
    }

// ----------------------------------------------------------------------------
// Converts given descriptor into TInt value.
// ----------------------------------------------------------------------------
//
inline void CSamplerPluginInterface::Str2Int(const TDesC8& aBuf, TInt& aValue)
    {
    TLex8 conv;
    conv.Assign(aBuf);
    
    if (conv.Val(aValue) != KErrNone)
        aValue = 0;
    }

// ----------------------------------------------------------------------------
// Converts given descriptor into TInt value.
// ----------------------------------------------------------------------------
//
inline void CSamplerPluginInterface::Str2Int(const TDesC8& aBuf, TUint32& aValue)
    {
    TInt temp(0);
    
    TLex8 conv;
    conv.Assign(aBuf);
    
    if (conv.Val(temp) != KErrNone)
        aValue = 0;
    else
        aValue = (TUint32)temp;
    }

// End of file
