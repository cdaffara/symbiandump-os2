/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   certain osa platfrom specific implementations
*
*/

/*
* %version: 4 %
*/

#include "osa_includeme.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void* GpAlloc( TInt aSize, TBool /*aZeroStamp*/ )
    {
    // the method below always zero stamps the memory upon alloc
    return Kern::Alloc( aSize );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void GpFree( void* aPtr )
    {
    Kern::Free( aPtr );
    }
