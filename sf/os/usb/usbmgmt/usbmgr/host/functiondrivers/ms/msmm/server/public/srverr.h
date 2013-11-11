/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
*/

#ifndef SRVERR_H
#define SRVERR_H

enum THostMsErrCode
    {
    EHostMsErrGeneral = 0x1, // General error 
    EHostMsErrNoDriveLetter = 0x2, // No drive letter available currently
    EHostMsErrUnknownFileSystem = 0x3, // Unknown file system on currently 
    // adding device
    EHostMsErrInvalidParameter = 0x4, // Invalid request parameter
    EHostMsErrOutOfMemory = 0x5, // Out of memory
    EHostMsEjectInProgress = 0x6, // eject request commenced
    EHostMsErrNone = 0x7, // no error == success condition
    EHostMsErrInUse = 0x8, // Drives in use
    EHostMsErrorEndMarker
    };
    
NONSHARABLE_CLASS(THostMsErrData)
    {
public:
    THostMsErrCode  iError; // Error code
    TInt            iE32Error; // Symbian e32err code
    TName           iManufacturerString;
    TName           iProductString;
    TText8          iDriveName; // Drive letter
    };

typedef TPckgBuf<THostMsErrData> THostMsErrorDataPckg;

#endif /*SRVERR_H*/
