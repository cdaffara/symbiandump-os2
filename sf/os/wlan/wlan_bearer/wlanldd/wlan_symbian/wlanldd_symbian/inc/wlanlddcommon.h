/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   WLAN LDD general declarations & definitions.
*
*/

/*
* %version: 15 %
*/

#ifndef WLANLDDCOMMON_H
#define WLANLDDCOMMON_H

#define LDD_NAME _L("wlan")
#define LDD_FILE_NAME _L("wlanldd")

#include <e32def.h>
#include <e32cmn.h>
#include "umac_types.h"
#include "wllddoidmsgstorage.h"

const TInt KWlanDriverMajorVersion = 1;
const TInt KWlanDriverMinorVersion = 0;
const TInt KWlanDriverBuildVersion = 0;

// Values for the Unit parameter used in the LDD
enum TWlanUnit 
    {
    KUnitEthernet, 
    KUnitWlan,
    KUnitMax    // not a real unit just defined for upperbound
    };

const TUint KWlanUnitsAllowedMask = 0x0000000F;

class TCapsWlanDriverV01
	{
public:
	TVersion version;
	};

struct SOutputBuffer
    {
    TUint8* iData;
    TUint32 iLen;
    };

/** 
* Structure to hold information about the shared memory chunk between 
* user mode and kernel mode
*/
class TSharedChunkInfo
    {
public:
    TInt iChunkHandle;
    TInt iSize;
    };

typedef TBuf<KMaxExitCategoryName> TExitCategory;

inline void WlanPanic( 
    const TExitCategory&
#ifdef __KERNEL_MODE__
    aCategory
#endif
    , TInt
#ifdef __KERNEL_MODE__
    aCode
#endif
    )
    {
#ifdef __KERNEL_MODE__
    Kern::PanicCurrentThread( aCategory, aCode );
#endif
    }

#endif // WLANLDDCOMMON
