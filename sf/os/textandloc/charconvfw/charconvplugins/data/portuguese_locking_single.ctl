# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
# Name:             3GPP TS 23.038 V8.1.0 "Portuguese Locking Shift Table + GSM 7 bits extension table " to Unicode converter control table
# Unicode version:  3.0
# Table version:    1.0
# Date:             March 6th 2009
# Authors:          Wendy Du
# Version history
#         2009/3/6          1.0 version new version.
#

Endianness															FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters						0x3f

StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0x1a							0
	0x1b							0x1b							1
	0x1c							0xff							0
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm		Parameters
	1	 	100		0x00				0x7f				KeyedTable1616		{}
	2		200		0x1b00				0x1b7f				KeyedTable1616		{}

EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm				SizeOfOutputCharacterCodeInBytes	Parameters
	60		45		0x000c				0x000c				Offset					2					{}
	55		50		0x005b				0x005b				Offset					2					{}
	50		55		0x005d				0x005d				Offset					2					{}
	45		60		0x007b				0x007b				Offset					2					{}
	43		62		0x007d				0x007d				Offset					2					{}
	40		65		0x0393				0x0393				Offset					2					{}
	35		70		0x0398				0x0398				Offset					2					{}
	30		75		0x03a0				0x03a0				Offset					2					{}
	25		80		0x03a3				0x03a3				Offset					2					{}
	20		85		0x03a6				0x03a6				Offset					2					{}
	15		90		0x03a8				0x03a8				Offset					2					{}
	10		95		0x03a9				0x03a9				Offset					2					{}
	1		100		0x0001				0x227f				KeyedTable1616				1					{}

ConflictResolution
#	UnicodeCharacterCodeToResolve	ForeignCharacterCodePreferred
0x000D	0x0D	#	CR	CARRIAGE RETURN	
0x0025  0x25	#	TBD?
0x0026	0x26	#
0x005C	0x17	#	REVERSE SOLIDUS	\
0x005E	0x16	#	CIRCUMFLEX ACCENT	^
0x007C	0x1A	#	VERTICAL LINE	|
0x007E	0x60	#	TILDE	~	
0x00C0	0x14	#	À	LATIN CAPITAL LETTER A WITH GRAVE	
0x00C1	0x0E	#	Á	LATIN CAPITAL LETTER A WITH ACUTE	
0x00C2	0x1c	#	Â	LATIN CAPITAL LETTER A WITH CIRCUMFLEX	
0x00C3	0x5b	#	Ã	LATIN CAPITAL LETTER A WITH TILDE	
0x00CA	0x1E	#	Ê	LATIN CAPITAL LETTER E WITH CIRCUMFLEX	
0x00CD	0x40	#	Í	LATIN CAPITAL LETTER I WITH ACUTE	
0x00D3	0x19	#	Ó	LATIN CAPITAL LETTER O WITH ACUTE	
0x00D4	0x0b	#	Ô	LATIN CAPITAL LETTER O WITH CIRCUMFLEX	
0x00D5	0x5c	#	Õ	LATIN CAPITAL LETTER O WITH TILDE	
0x00DA	0x5D	#	Ú	LATIN CAPITAL LETTER U WITH ACUTE	
0x00E1	0x0F	#	á	LATIN SMALL LETTER A WITH ACUTE	
0x00E2	0x1D	#	â	LATIN SMALL LETTER A WITH CIRCUMFLEX	
0x00E3	0x7b	#	ã	LATIN SMALL LETTER A WITH TILDE	
0x00E7	0x09	#	ç	LATIN SMALL LETTER C WITH CEDILLA	
0x00EA	0x04	#	ê	LATIN SMALL LETTER E WITH CIRCUMFLEX	
0x00ED	0x07	#	í	LATIN SMALL LETTER I WITH ACUTE	
0x00F3	0x08	#	ó	LATIN SMALL LETTER O WITH ACUTE	
0x00F4	0x0c	#	ô	LATIN SMALL LETTER O WITH CIRCUMFLEX	
0x00F5	0x7c	#	õ	LATIN SMALL LETTER O WITH TILDE	
0x00FA	0x06	#	ú	LATIN SMALL LETTER U WITH ACUTE	
0x20AC	0x18	#	EURO SIGN	€	
	
EndUnicodeToForeignData