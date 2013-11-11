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

#include "t_videoframesupplier.h"


//
//
//
CVideoFrameSupplier::CVideoFrameSupplier()
 :  iLargestFrameSize(KErrNotFound)
    {
    }

//
//
//
EXPORT_C CVideoFrameSupplier::~CVideoFrameSupplier()
    {
    iFrames.Close();
    iFile.Close();
    iFs.Close();
    }


//
//
//
EXPORT_C CVideoFrameSupplier* CVideoFrameSupplier::NewL(const TDesC& aFileName, TVideoFileType aFileType) 
    {
    CVideoFrameSupplier* self = new(ELeave) CVideoFrameSupplier();
    CleanupStack::PushL(self);
    self->ConstructL(aFileName, aFileType);
    CleanupStack::Pop(self);
    return self;
    }


//
//
//
void CVideoFrameSupplier::ConstructL(const TDesC& aFileName, TVideoFileType aFileType)
    {
    User::LeaveIfError(iFs.Connect());
    TInt err = iFile.Open(iFs, aFileName, EFileRead);
    if (err != KErrNone)
        {
        RDebug::Print(_L("*** CVideoFrameSupplier: Error %d opening file %S"), err, &aFileName);
        User::Leave(err);
        }
    
    switch (aFileType)
        {
        case EVideoFileTypeMPEG4:
            ParseMPEG4FileL();
            break;
            
        default:
            User::Leave(KErrNotSupported);
        }
    }


//
//
//
EXPORT_C TInt CVideoFrameSupplier::LargestFrameSize() const
    {
    return iLargestFrameSize;
    }


//
//
//
EXPORT_C TInt CVideoFrameSupplier::CurrentFrame() const
    {
    return iCurrentFrame;
    }


//
//
//
EXPORT_C TBool CVideoFrameSupplier::IsLastFrame() const
    {
    return iCurrentFrame >= (iFrames.Count() - 1);
    }


//
//
//
EXPORT_C TInt CVideoFrameSupplier::NextFrameData(TUint8* aPtr, TInt& aFrameLength)
    {
    aFrameLength = KErrNotFound;
    
    if (!aPtr)
        {
        return KErrArgument;
        }
    
    if (iCurrentFrame >= iFrames.Count())
        {
        return KErrEof;
        }
    
    TFrameData data = iFrames[iCurrentFrame++];
    TInt pos = data.iPosition;
    TInt err = iFile.Seek(ESeekStart, pos);
    if (err == KErrNone)
        {
        TPtr8 des(aPtr, data.iLength);
        err = iFile.Read(des);
        if (err == KErrNone)
            {
            aFrameLength = data.iLength;
            }
        }
    
    return err;
    }


//
//
//
TInt CVideoFrameSupplier::AppendFrameData(TInt aPosition, TInt aLength)
    {
    ASSERT(aPosition >= 0);
    ASSERT(aLength > 0);
    
    TFrameData data;
    data.iPosition = aPosition;
    data.iLength = aLength;
    
    if (aLength > iLargestFrameSize)
        {
        iLargestFrameSize = aLength;
        }
    
    return iFrames.Append(data);
    }


//
//
//
void CVideoFrameSupplier::ParseMPEG4FileL()
    {
    iFrames.Reset();
    
    const TInt KBufferSize = 1024;
    
    iState = ESearchForZero;
    iLargestFrameSize = KErrNotFound;
    iFrameStartPosition = KErrNotFound;
    
    TBuf8<KBufferSize> data;
    TInt filePosition = 0;
    TBool lastBuffer = EFalse;
    TInt err = KErrNone;
    
    do
        {
        data.Zero();
        err = iFile.Read(data);
        lastBuffer = (data.Length() != KBufferSize);
        
        if (err == KErrNone)
            {
            for (TInt i = 0; i < data.Length(); i++)
                {
                err = ProcessMPEG4Byte(data[i], filePosition + i);
                }
            
            filePosition += KBufferSize;
            }
        }
    while (!lastBuffer);
    }


//
//
//
TInt CVideoFrameSupplier::ProcessMPEG4Byte(TUint8 aByte, TInt aBytePosition)
    {
    ASSERT(aBytePosition >= 0);
    
    TInt err = KErrNone;
    
    switch (iState)
        {
        case ESearchForZero:
            iState = (aByte == 0x00 ? ESearchForZeroZero : iState);
            break;
            
        case ESearchForZeroZero:
            iState = (aByte == 0x00 ? ESearchForZeroZeroOne : ESearchForZero);
            break;
            
        case ESearchForZeroZeroOne:
            if (aByte == 0x00)
                {
                // Do nothing. It is allowed to have more than two leading zero bytes.
                break;
                }
            
            iState = (aByte == 0x01 ? ESearchForStartCode : ESearchForZero);
            break;
            
        case ESearchForStartCode:
            if (iFrameStartPosition != KErrNotFound)
                {
                // If we are currently in a frame then it ends at this start code.
                TInt length = (aBytePosition - 3) - iFrameStartPosition;     
                err = AppendFrameData(iFrameStartPosition, length);
                iFrameStartPosition = KErrNotFound;
                }
            
            iState = (aByte == 0xB6 ? EBeginningOfFrame : ESearchForZero);
            break;
            
        case EBeginningOfFrame:
            iFrameStartPosition = aBytePosition - 4; // The 0x000001B6 bytes count too.
            if (iFrames.Count() == 0)
                {
                if (iFrameStartPosition != 0)
                    {
                    // There was a header before the frame. The ST-E decoder
                    // requires that this data is prepended to the frame data (first frame only).
                    // We assume that all bytes before this frame are valid header bytes!
                    iFrameStartPosition = 0;
                    }
                }
            
            iState = ESearchForZero;
            break;
            
        default:
            ASSERT(EFalse);
        }
    
    return err;
    }
