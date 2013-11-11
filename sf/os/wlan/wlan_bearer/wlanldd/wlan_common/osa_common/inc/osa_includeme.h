/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   1st file to be included by every source file in the project
*
*/

/*
* %version: 3 %
*/

#ifndef OSA_INCLUDEME_H
#define OSA_INCLUDEME_H

#include "am_platform_libraries.h"
#include "gendebug.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void Trace( TInt aLevel, const TInt8* aFile, TInt aLine )
    {
    TraceDump( aLevel, (reinterpret_cast<const char*>(aFile)) );
    TraceDump( aLevel, (("Line: %d"), aLine));
    }

// the one and only initial hw chunk size for DMA capable memory
const TInt KInitialHwChunkSize = 4 * 4096; // 16 KBits

#endif // OSA_INCLUDEME_H
