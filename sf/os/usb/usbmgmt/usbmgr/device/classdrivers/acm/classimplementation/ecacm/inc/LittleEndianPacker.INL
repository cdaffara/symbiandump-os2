/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** file LittleEndianPacker.INL
 *
 * Implements inline macro functions used to interface between USB
 * (little-endian) byte buffers and standard data types.
 *
 * The implementation uses inline macros (for speed) that are used to 
 * step down a little-endian USB buffer to extract and insert members.
 *
 * The functions update the given buffer index so that they can be 
 * used in sequence to extract or pack successive parts of a message.
 *
 * Although these functions are of more general use than just the CDC 
 * application, they are located here pending possible promotion into 
 * the chapter.9 area, where they would become available for use in 
 * other classes, e.g. HID
 *
 * Copyright (c) 2003 Symbian Ltd.	All rights reserved.
 */

#ifndef __LITTLEENDIANPACKER_INL__
#define __LITTLEENDIANPACKER_INL__

#include <e32def.h>

/* ------------------------------------------------------------------------- */

typedef union _FloatInt
	{
	TUint32 uVal;
	TReal32 fVal;
	}
	UFloatInt;

inline TReal32 CCdcControlInterface::GetF32( TUint8** aPPbuf )
	{
	UFloatInt retval;

	retval.uVal = GetU32(aPPbuf);

	return retval.fVal;
	};

inline void CCdcControlInterface::PutF32( TUint8** aPPbuf, TReal32 aF32 )
	{
	UFloatInt inval;

	inval.fVal = aF32;

	PutU32(aPPbuf,inval.uVal);
	};

/* ------------------------------------------------------------------------- */

inline TUint32 CCdcControlInterface::GetU32( TUint8** aPPbuf )
	{
	TUint8* pbuf = *aPPbuf;
	
	*aPPbuf = pbuf + sizeof(TUint32);

	return (TUint32)( (pbuf[3] << 24 ) | ( pbuf[2] << 16 ) | ( pbuf[1] << 8 ) | ( pbuf[0] ) );
	};

inline void CCdcControlInterface::PutU32( TUint8** aPPbuf, TUint32 aU32 )
	{
	TUint8* pbuf = *aPPbuf;
	
	*aPPbuf = pbuf + sizeof(TUint32);

	pbuf[3] = (TUint8) ((aU32 >> 24)	   ); // mask not needed, only 8 bits left
	pbuf[2] = (TUint8) ((aU32 >> 16) & 0xFF);
	pbuf[1] = (TUint8) ((aU32 >>  8) & 0xFF);
	pbuf[0] = (TUint8) ((aU32	   ) & 0xFF); // shift not needed, mask takes 8 bits
	};

/* ------------------------------------------------------------------------- */

inline TUint16 CCdcControlInterface::GetU16( TUint8** aPPbuf )
	{
	TUint8* pbuf = *aPPbuf;
	
	*aPPbuf = pbuf + sizeof(TUint16);

	return (TUint16) ( ( pbuf[1] << 8) | ( pbuf[0] ) );
	};

inline void CCdcControlInterface::PutU16( TUint8** aPPbuf, TUint16 aU16 )
	{
	TUint8* pbuf = *aPPbuf;

	*aPPbuf = pbuf + sizeof(TUint16);
	
	pbuf[3] = (TUint8) ((aU16 >> 8) 	  ); // mask not needed, only 8 bits left
	pbuf[0] = (TUint8) ((aU16	  ) & 0xFF); // shift not needed, mask takes 8 bits
	};

/* ------------------------------------------------------------------------- */

inline TUint8 CCdcControlInterface::GetU08( TUint8** aPPbuf )
	{
	TUint8* pbuf = *aPPbuf;
	
	*aPPbuf = pbuf + sizeof(TUint8);

	return (TUint8) ( pbuf[0] );
	};

inline void CCdcControlInterface::PutU08( TUint8** aPPbuf, TUint8 aU8 )
	{
	TUint8* pbuf = *aPPbuf;

	*aPPbuf = pbuf + sizeof(TUint8);
	
	pbuf[0] = (aU8);
	};

/* ------------------------------------------------------------------------- */
#endif // __LITTLEENDIANPACKER_INL__
/* ------------------------------------------------------------------------- */

