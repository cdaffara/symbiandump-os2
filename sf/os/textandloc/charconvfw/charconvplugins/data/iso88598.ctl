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
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm						Parameters
	3				3				0x00							0x9f							Direct							{}		# ASCII
	2				2				0xe0							0xfa							Offset							{}
	1				1				0xa0							0xfe							KeyedTable16OfIndexedTables16	{}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	5				5				0x0000							0x009f							Direct				1									{}		# ASCII
	4				4				0x00a2							0x00a9							Direct				1									{}
	3				3				0x00ab							0x00b9							Direct				1									{}
	2				2				0x05d0							0x05ea							Offset				1									{}
	1				1				0x00a0							0x2017							KeyedTable1616		1									{}
EndUnicodeToForeignData

