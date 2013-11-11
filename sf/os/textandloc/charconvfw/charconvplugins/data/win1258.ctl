# Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
ReplacementForUnconvertibleUnicodeCharacters						0x1a


StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0xff							0
#	0x80							0xff							0
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	7				7				0x00							0x81							Direct				{}			#
	6				6				0xa0							0xc2							Direct				{}			#
	5				5				0xc4							0xcb							Direct				{}			#
	4				4				0xd6							0xdc							Direct				{}			#
	3				3				0xe4							0xeb							Direct				{}			#
	2				2				0xf6							0xfc							Direct				{}			#
	1				1				0x82							0xff							KeyedTable16OfIndexedTables16		{}		# various stuff
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	7				7				0x0000						0x0081							Direct								1									{}			
	6				6				0x00a0						0x00c2							Direct								1									{}			
	5				5				0x00c4						0x00cb							Direct								1									{}			
	4				4				0x00d6						0x00dc							Direct								1									{}			
	3				3				0x00e4						0x00eb							Direct								1									{}			
	2				2				0x00f6						0x00fc							Direct								1									{}			
	1				1				0x008a						0x2122							KeyedTable1616				1									{}				
EndUnicodeToForeignData

