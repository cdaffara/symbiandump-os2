# Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# win874.CTL (Thai)
Endianness															FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters						0x1a


StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0xfb							0
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	4				4				0x00							0x7f							Direct				{}		# ASCII
	3				3				0xa1							0xda							Offset				{}
	2				2				0xdf							0xfb							Offset				{}
    1				1				0x80							0xa0							KeyedTable1616      {}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	4				4				0x0000							0x007f							Direct				1									{}		# ASCII				
	3				3				0x0e01							0x0e3a							Offset				1									{}
	2				2				0x0e3f							0x0e5b							Offset				1									{}
    1				1				0x00a0							0x20ac							KeyedTable1616      1                                   {}
EndUnicodeToForeignData

