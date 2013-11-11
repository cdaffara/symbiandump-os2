/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
@file
@internalComponent
*/
#ifndef NCMPKTDRVCOMMON_H
#define NCMPKTDRVCOMMON_H

#include <e32base.h>
#include "ncmcommon.h"

const TInt KErrCongestion = -6668;
const TInt KErrNoBuffer = -6669;


NONSHARABLE_CLASS(TNcmNtbInParam)
    {
public:
    TUint32    iNtbInMaxSize;
    TUint16    iNdpInDivisor;
    TUint16    iNdpInPayloadRemainder;
    TUint16    iNdpInAlignment;
    };


NONSHARABLE_CLASS(TNcmBuffer)
    {
public:
    TUint8*    iPtr;
    TUint32    iLen;
    TUint32    iMaxLength;
    };

#endif // NCMPKTDRVCOMMON_H
