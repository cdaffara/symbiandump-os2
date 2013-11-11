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
	4				4				0x00							0x9f							Direct				{}		# ASCII
	3				3				0xc1							0xda							Offset				{}
	2				2				0xe0							0xf2							Offset				{}
	1				1				0xa0							0xbf							KeyedTable1616		{}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	4				4				0x0000							0x009f							Direct				1									{}		# ASCII
	3				3				0x0621							0x063a							Offset				1									{}
	2				2				0x0640							0x0652							Offset				1									{}
	1				1				0x00a0							0x061f							KeyedTable1616		1									{}
EndUnicodeToForeignData

