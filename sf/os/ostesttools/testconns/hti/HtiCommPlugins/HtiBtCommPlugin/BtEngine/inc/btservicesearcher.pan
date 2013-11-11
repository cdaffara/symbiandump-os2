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
* Description:  Bluetooth service searcher panic codes.
*
*/


#ifndef __BTSERVICE_SEARCHER_PAN__
#define __BTSERVICE_SEARCHER_PAN__

#include <e32std.h>

/** BTServiceSearcher application panic codes */
enum TBTServiceSearcherPanics
    {
    EBTServiceSearcherNextRecordRequestComplete = 1,
    EBTServiceSearcherAttributeRequestResult,
    EBTServiceSearcherAttributeRequestComplete,
    EBTServiceSearcherInvalidControlIndex,
    EBTServiceSearcherProtocolRead,
    EBTServiceSearcherAttributeRequest,
    EBTServiceSearcherSdpRecordDelete
    };


/** Panic Category */
_LIT( KPanicServiceSearcher, "ServiceSearcher" );


inline void Panic( TBTServiceSearcherPanics aReason )
    {
    User::Panic( KPanicServiceSearcher, aReason );
    }

#endif // __BTSERVICE_SEARCHER_PAN__

// End of File
