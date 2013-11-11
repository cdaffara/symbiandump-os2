/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/********************************************************************************
 *
 * Switches
 *
 *******************************************************************************/
#ifndef __NTOH_H__
#define __NTOH_H__

/********************************************************************************
 *
 * Includes
 *
 *******************************************************************************/
#include <e32std.h>

/********************************************************************************
 *
 * Definitions
 *
 *******************************************************************************/
#define LilEnd 0
#define BigEnd 1

/********************************************************************************
 *
 * Macro Functions
 *
 *******************************************************************************/

/********************************************************************************
 *
 * Prototypes
 *
 *******************************************************************************/
int DetectLocalEndian();
long LittleToBigL( long src );
long BigToLittleL( long src );
short LittleToBigS( short src );
short BigToLittleS( short src );
long ntohl( long src );
long htonl( long src );
short ntohs( short src );
short htons( short src );

#endif // __NTOH_H__
