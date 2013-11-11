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




#ifndef STAT_H
#define STAT_H

// possible connection types supported
typedef enum STATConnectType
{
	SymbianInvalid	 = 0,	
	SymbianSocket    = 1,
	SymbianSerial    = 2,
	SymbianInfrared	 = 3,
	SymbianBluetooth = 4,
	SymbianUsb = 5,
} STATCONNECTTYPE;

// possible logging levels
typedef enum STATLogLevel
{
	ETerse   = 0,
	ENormal  = 1,
	EVerbose = 2
} STATLOGLEVEL;

// bitmap access structures
#include <pshpack2.h>
struct TBitmapFileHeader
{
	unsigned short bfType;
	unsigned long bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long bfOffBits;
};
#include <poppack.h>

struct TBitmapInfoHeader
{
	unsigned long biSize;
	long biWidth;
	long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	long biXPelsPerMeter;
	long biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
};

#endif
