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
	1	 			100				0x00							0x7f							KeyedTable1616		{}
	2				200				0x1b0a							0x1b65							KeyedTable1616		{}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm				SizeOfOutputCharacterCodeInBytes	Parameters
	90				40				0x000c							0x000c							Offset					2										{}
	60				50				0x005b							0x005b							Offset					2										{}
	50				60				0x005d							0x005d							Offset					2										{}
	40				70				0x007b							0x007b							Offset					2										{}
	20				80				0x007d							0x007d							Offset					2										{}
	1				100				0x000a							0x221e							KeyedTable1616			1										{}

ConflictResolution
#	UnicodeCharacterCodeToResolve	ForeignCharacterCodePreferred
0x005C	0x17	#	REVERSE SOLIDUS	\
0x005E	0x16	#	CIRCUMFLEX ACCENT	^
0x007C	0x1A	#	VERTICAL LINE	|
0x007E	0x60	#	TILDE	~	
0x20AC	0x18	#	EURO SIGN	€	
	
EndUnicodeToForeignData