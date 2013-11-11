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

#ifndef __T_VIDEOFRAMESUPPLIER_H__
#define __T_VIDEOFRAMESUPPLIER_H__


#include <e32base.h>
#include <e32std.h>
#include <f32file.h>

typedef struct
    {
    TInt iPosition; // From start of the file.
    TInt iLength;
    }
TFrameData;

typedef enum
    {
    ESearchForZero = 0,
    ESearchForZeroZero,
    ESearchForZeroZeroOne,
    ESearchForStartCode,
    EBeginningOfFrame
    }
TMPEG4ParseState;

//
//
//
class CVideoFrameSupplier : public CBase
    {
public:
    
    typedef enum
        {
        EVideoFileTypeMPEG4
        }
    TVideoFileType;
    
    IMPORT_C static CVideoFrameSupplier* NewL(const TDesC& aFileName, TVideoFileType aFileType);
    IMPORT_C ~CVideoFrameSupplier();

public:
    IMPORT_C TInt LargestFrameSize() const;
    IMPORT_C TInt CurrentFrame() const;
    IMPORT_C TInt NextFrameData(TUint8* aPtr, TInt& aFrameLength);
    IMPORT_C TBool IsLastFrame() const;
    
private:
    CVideoFrameSupplier();
    void ConstructL(const TDesC& aFileName, TVideoFileType aFileType);
    void ParseMPEG4FileL();
    TInt AppendFrameData(TInt aPosition, TInt aLength);
    TInt ProcessMPEG4Byte(TUint8 aByte, TInt aBytePosition);

private:
    RFs iFs;
    RFile iFile;
    RArray<TFrameData> iFrames;
    TInt iLargestFrameSize;
    TInt iCurrentFrame;
    TMPEG4ParseState iState;
    TInt iFrameStartPosition;
    };

#endif  // __T_VIDEOFRAMESUPPLIER_H__

