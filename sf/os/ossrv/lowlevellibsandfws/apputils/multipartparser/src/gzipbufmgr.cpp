// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Decompression buffer manager for GZip data
//

#include <ezstream.h>
#include "gzipbufmgr.h"
//-----------------------------------------------------------------------------

// NOTE: This code was taken from the DeflateFilter (s60/DeflateFilter/)

// ----------------------------------------------------------------------------
// StreamBufMgr::NewL
// Two-phase constructor
// ----------------------------------------------------------------------------
//
GZipBufMgr* GZipBufMgr::NewL(TPtrC8 in)
    {
    GZipBufMgr* self = new (ELeave)GZipBufMgr(in);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }//lint !e1746 const reference is not necessary here, because TPtrC8 takes only 4-bytes

// ----------------------------------------------------------------------------
// GZipBufMgr::GZipBufMgr
// Constructor
// ----------------------------------------------------------------------------
//
GZipBufMgr::GZipBufMgr(TPtrC8 in)
                    : iInput(in), iOutputDes(0, 0), iOutput(0),
                      iNeedInput(EFalse), iNeedOutput(EFalse), 
                      iFinalized(EFalse), iOffset(0), iID1(31), iID2(139)
    {
    } //lint !e1746

// ----------------------------------------------------------------------------
// GZipBufMgr::ConstructL
// Initialize the output buffer
// ----------------------------------------------------------------------------
//
void GZipBufMgr::ConstructL()
    {
    // Output buffer and descriptor
    iOutput = HBufC8::NewMaxL(1<<15);
    iOutputDes.Set(iOutput->Des());

    // Setup the GZip header, this sets iOffset to after the header and start
    // of the data
	TEZGZipHeader header;
    ReadGZipHeaderL(header);

    // Move pass the header and set the buffer to the data
	TPtrC8 data(REINTERPRET_CAST(const TUint8*, iInput.Ptr() + iOffset), iInput.Length() - iOffset);

    // Setup the data buffer
	SetGZipBuffer(data);

    }

// ----------------------------------------------------------------------------
// GZipBufMgr::~GZipBufMgr
// Destructor
// ----------------------------------------------------------------------------
//
GZipBufMgr::~GZipBufMgr()
    {
    delete iOutput;
    iOutput = NULL;
    }

// ----------------------------------------------------------------------------
// GZipBufMgr::InitializeL
// Overriden function from MEZBufferManager
// ----------------------------------------------------------------------------
//
void GZipBufMgr::InitializeL(CEZZStream& aZStream)
    {
    // read more
    aZStream.SetInput(iInput);
    iOutputDes.SetLength(0);
    aZStream.SetOutput(iOutputDes);
    iNeedInput = EFalse;
    iNeedOutput = EFalse;
    }

// ----------------------------------------------------------------------------
// GZipBufMgr::NeedInputL
// Overriden function from MEZBufferManager
// ----------------------------------------------------------------------------
//
void GZipBufMgr::NeedInputL(CEZZStream& /*aZStream*/)
    {
    //aZStream.SetInput(iInput);
    iNeedInput = ETrue;
    }

// ----------------------------------------------------------------------------
// GZipBufMgr::NeedOutputL
// Overriden function from MEZBufferManager
// ----------------------------------------------------------------------------
//
void GZipBufMgr::NeedOutputL(CEZZStream& /*aZStream*/)
    {
    //aZStream.SetOutput(iOutputDes);
    iNeedOutput = ETrue;
    }

// ----------------------------------------------------------------------------
// GZipBufMgr::NeedOutputL
// Overriden function from MEZBufferManager
// ----------------------------------------------------------------------------
//
void GZipBufMgr::FinalizeL(CEZZStream& /*aZStream*/)
    {
    // do nothing now, should check the checksum value here
    // read the checksum value from the last chunk
    //const TInt32* checksum = REINTERPRET_CAST(const TInt32*, iInput.Ptr() + (iInput.Length() - sizeof(TInt32)*2));
    iFinalized = ETrue;
    }


// ----------------------------------------------------------------------------
// Public methods
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// GZipBufMgr::ReadGZipHeaderL
// Read the header of Gzip-ed stream in Gzip format, RFC1952.
// Sets the iOffset pointer, which marks the end of the header and start of the
// data.  This method needs to be called (or iOffset set) before calling
// SetGZipBuffer method.
// ----------------------------------------------------------------------------
//
void GZipBufMgr::ReadGZipHeaderL(TEZGZipHeader& aHeader)
    {
    // construct a stream for reading
    RMemReadStream memBuf;
    memBuf.Open(iInput.Ptr(), iInput.Length());
    
    TInt obligatoryData = sizeof(aHeader.iId1) + sizeof(aHeader.iId2) +
                          sizeof(aHeader.iCompressionMethod) +
                          sizeof(aHeader.iFlags) + sizeof(aHeader.iTime) +
                          sizeof(aHeader.iExtraFlags) + sizeof(aHeader.iOs);
    
    // copy the header data
    TPtr8 des(&aHeader.iId1, 0, obligatoryData);
    TRAPD(error, memBuf.ReadL(des));
    
    if (error == KErrEof)
        {
        User::Leave(EZGZipFile::EBadGZipHeader);
        }
    
    if (des.Size() < obligatoryData)
        {
        // should consider the situation when not enough data has arrived
        User::Leave(EZGZipFile::EBadGZipHeader);
        }
    
    if (aHeader.iId1 != iID1 || aHeader.iId2 != iID2)
        {
        User::Leave(EZGZipFile::EBadGZipHeader);
        }
    
    if (aHeader.iFlags & (1 << EZGZipFile::EFExtra))
        {
        // The extra bit is set
        des.Set(REINTERPRET_CAST(TUint8 *, &aHeader.iXlen), 0, sizeof(aHeader.iXlen));
        memBuf.ReadL(des);
        if (des.Size() != sizeof(aHeader.iXlen) || aHeader.iXlen < 0) //lint !e737 sign/unsigned doesn't realy matter here
            {
            User::Leave(EZGZipFile::EBadGZipHeader);
            }
        
        aHeader.iExtra = HBufC8::NewMaxL(aHeader.iXlen);
        TPtr8 des1 = aHeader.iExtra->Des();
        memBuf.ReadL(des1);
        
        if (des1.Size() != aHeader.iXlen)
            {
            User::Leave(EZGZipFile::EBadGZipHeader);
            }
        }
    
    if (aHeader.iFlags & (1 << EZGZipFile::EFName))
        {
        // Read in filename
        ReadStringIntoDescriptorL(memBuf, &aHeader.iFname);
        }
    
    if (aHeader.iFlags & (1 << EZGZipFile::EFComment))
        {
        // Read in comment
        ReadStringIntoDescriptorL(memBuf, &aHeader.iComment);
        }
    
    if (aHeader.iFlags & (1 << EZGZipFile::EFHcrc))
        {
        des.Set(REINTERPRET_CAST(TUint8*, &aHeader.iCrc), 0, sizeof(aHeader.iCrc));
        memBuf.ReadL(des);
        if (des.Size() != sizeof(aHeader.iCrc)) //lint !e737 unsigned/signed doesn't matter
            {
            User::Leave(EZGZipFile::EBadGZipHeader);
            }
        }
    
    // Set iOffset to mark end of header and beginning of data
    iOffset = memBuf.Source()->TellL(MStreamBuf::ERead).Offset();
    
    memBuf.Close();
    }

// ----------------------------------------------------------------------------
// GZipBufMgr::ReadStringIntoDescriptorL
// Read null-terminated strings into the buffer
// ----------------------------------------------------------------------------
//
void GZipBufMgr::ReadStringIntoDescriptorL(RMemReadStream& aMem, HBufC8 **aDes) const
    {
    TInt i;
    CArrayFixFlat<TUint8> *bytes = new (ELeave) CArrayFixFlat<TUint8>(16);
    CleanupStack::PushL(bytes);
    TUint8 ch;
    
    while ((ch = aMem.ReadUint8L()) != '\0')
        {
        bytes->AppendL(ch);
        }
    
    *aDes = HBufC8::NewMaxL(bytes->Count());
    TPtr8 desPtr = (*aDes)->Des(); //lint because error #111 - separate desPtr used
    
    for (i = 0; i < bytes->Count(); i++)
        {
        desPtr[i] = (*bytes)[i]; //lint !e1058 force conversion here
        }
    
    CleanupStack::PopAndDestroy(); // delete bytes	
    }

// ----------------------------------------------------------------------------
// GZipBufMgr::SetBuffer
// Set the input buffer
// ----------------------------------------------------------------------------
//
void GZipBufMgr::SetGZipBuffer(TPtrC8 data)
    {
    iInput.Set(data);
    }//lint !e1746
