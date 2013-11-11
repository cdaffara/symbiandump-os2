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

Endianness															FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters						0x1a


StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0xfe							0
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	4				4				0x00							0x7f							Direct				{}		# ASCII
	3				3				0xab							0xb9							Direct				{}		# ASCII
	2				2				0xe0							0xfa							Offset				{}
	1				1				0x80							0xfe							KeyedTable1616		{}		# various stuff
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	6				6				0x0000							0x007f							Direct				1									{}		# ASCII
	5				5				0x00ab							0x00b9							Direct				1									{}		# ASCII
	4				4				0x05b0							0x05b9							Offset				1									{}	
	3				3				0x05bb							0x05c3							Offset				1                                   {}
	2				2				0x05d0							0x05ea							Offset				1                                   {}
    1				1				0x00a0							0x2122							KeyedTable1616      1                                   {}				
EndUnicodeToForeignData

