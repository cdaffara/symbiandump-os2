# Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

Endianness															FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters						0x1a # ASCII "substitute" character - must be a single character, and must be little-endian if the "Endianness" above is "Unspecified", otherwise in the same endianness as specified

StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0xff							0
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	6				6				0x00							0x9f							Direct				{}		# ASCII + other stuff
	5				5				0xa0							0xcf							Direct				{}		# ASCII + other stuff
	4				4				0xd1							0xdc							Direct				{}
	3				3				0xdf							0xef							Direct				{}
	2				2				0xf1							0xfc							Direct				{}
	1				1				0xd0							0xff							KeyedTable1616		{}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	6				6				0x0000							0x009f							Direct				1									{}		# ASCII
	5				5				0x00a0							0x00cf							Direct				1									{}
	4				4				0x00d1							0x00dc							Direct				1									{}
	3				3				0x00df							0x00ef							Direct				1									{}
	2				2				0x00f1							0x00fc							Direct				1									{}
	1				1				0x00ff							0x015f							KeyedTable1616		1									{}
EndUnicodeToForeignData

