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
 * Local Includes
 *
 *******************************************************************************/
#include "ntoh.h"
#include "assert.h"

/********************************************************************************
 *
 * Implementation
 *
 *******************************************************************************/
int DetectLocalEndian()
{
	/* put a long onto the stack */
	long integer_value = 0x11223344;
	char *stack_value = (char*)&integer_value;
	int iType = LilEnd;

	/* see how it's layed out */
	if( stack_value[0] == 0x11 ) 
	{
		asserte( stack_value[1] == 0x22 );
		asserte( stack_value[2] == 0x33 );
		asserte( stack_value[3] == 0x44 );
		iType = BigEnd;
	}
	else if( stack_value[0] == 0x44 )
	{
		asserte( stack_value[1] == 0x33 );
		asserte( stack_value[2] == 0x22 );
		asserte( stack_value[3] == 0x11 );
	}
	else
	{
		;
	}

	return iType;
}

long LittleToBigL( long src )
{
	long dst;
	char *psrc = (char*)&src, *pdst = (char*)&dst;
	pdst[0] = psrc[3];
	pdst[1] = psrc[2];
	pdst[2] = psrc[1];
	pdst[3] = psrc[0];
	return dst;
}

long BigToLittleL( long src )
{
	long dst;
	char *psrc = (char*)&src, *pdst = (char*)&dst;
	pdst[0] = psrc[3];
	pdst[1] = psrc[2];
	pdst[2] = psrc[1];
	pdst[3] = psrc[0];
	return dst;
}

short LittleToBigS( short src )
{
	short dst;
	char *psrc = (char*)&src, *pdst = (char*)&dst;
	pdst[0] = psrc[1];
	pdst[1] = psrc[0];
	return src;
}

short BigToLittleS( short src )
{
	short dst;
	char *psrc = (char*)&src, *pdst = (char*)&dst;
	pdst[0] = psrc[1];
	pdst[1] = psrc[0];
	return src;
}

long ntohl( long src )
{
	// get the endianess of this machine -- if bigendian then it is the
	// same as network byte order and nothing needs 
	int thisMachine = DetectLocalEndian();
	if( thisMachine == BigEnd ) 
		return src;

	// otherwise we have to convert from bigendian to little
	return BigToLittleL( src );
}

long htonl( long src )
{
	// get the endianess of this machine -- if bigendian then it is the
	// same as network byte order and nothing needs 
	int thisMachine = DetectLocalEndian();
	if( thisMachine == BigEnd ) 
		return src;

	// otherwise we have to convert from littleendian to big
	return LittleToBigL( src );
}

short ntohs( short src )
{
	// get the endianess of this machine -- if bigendian then it is the
	// same as network byte order and nothing needs 
	int thisMachine = DetectLocalEndian();
	if( thisMachine == BigEnd ) 
		return src;

	// otherwise we have to convert from bigendian to little
	return BigToLittleS( src );}

short htons( short src )
{
	// get the endianess of this machine -- if bigendian then it is the
	// same as network byte order and nothing needs 
	int thisMachine = DetectLocalEndian();
	if( thisMachine == BigEnd ) 
		return src;

	// otherwise we have to convert from littleendian to big
	return LittleToBigS( src );
}
