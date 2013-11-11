/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   The first file which every UMAC cpp file needs to include.
*
*/

/*
* %version: 16 %
*/

#ifndef CONFIG_H
#define CONFIG_H

#include "umac_types.h"

extern TAny* os_alloc( const TUint32 aLengthInBytes );
extern void os_free( const TAny* aPtr );

// Returns the current time as the number of microseconds since midnight, 
// January 1st, 0 AD nominal Gregorian
extern TInt64 os_systemTime( void );

#include "802dot11.h"

#include "UmacDebug.h"

#endif // CONFIG_H
