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
# WIN1256.CTL (Arabic)
#
Endianness															FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters						0x1a


StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0xff							0
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	4				4				0x00							0x7f							Direct				{}		# ASCII
	3				3				0xab							0xb9							Direct				{}		# ASCII
	2				2				0xc1							0xd6							Offset				{}
	1				1				0x80							0xff							KeyedTable1616		{}		# various stuff
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	5				5				0x0000							0x007f							Direct				1									{}		# ASCII
	4				4				0x00a2							0x00a9							Direct				1									{}		# ASCII
	3				3				0x00ab							0x00b9							Direct				1									{}		# ASCII
	2				2				0x0621							0x0636							Offset				1									{}		# ASCII
    1				1				0x00a0							0x2122							KeyedTable1616      1                                   {}				
EndUnicodeToForeignData

