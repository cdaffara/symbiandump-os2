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
	5				5				0x00							0x9f							Direct				{}		# ASCII
	4				4				0xa1							0xac							Offset				{}
	3				3				0xae							0xef							Offset				{}
	2				2				0xf1							0xfc							Offset				{}
	1				1				0xa0							0xff							KeyedTable1616		{}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	5				5				0x0000							0x009f							Direct				1									{}		# ASCII
	4				4				0x0401							0x040c							Offset				1									{}
	3				3				0x040e							0x044f							Offset				1									{}
	2				2				0x0451							0x045c							Offset				1									{}
	1				1				0x00a0							0x2116							KeyedTable1616		1									{}
EndUnicodeToForeignData

