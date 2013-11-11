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
* Description:  Header file to help determine if a specific packet size
* will result in the packet being padded.  This is to avoid a packet that*
*/


#ifndef PADPACKET_H_2B31B9BD_01D6_4f35_B525_4E07C03D7762
#define PADPACKET_H_2B31B9BD_01D6_4f35_B525_4E07C03D7762

#if ! defined __E32DEF_H__
	// Win32 code.
	typedef signed int TInt;
	typedef int TBool;
#endif // ! defined __E32DEF_H__

static const int packetMultiple = 64;
static const unsigned char packetPad = 'S';

inline TBool PadPacket( TInt packetSize )
{
	return ( 0 == ( packetSize % packetMultiple ) );
}

#endif // defined PADPACKET_H_2B31B9BD_01D6_4f35_B525_4E07C03D7762
