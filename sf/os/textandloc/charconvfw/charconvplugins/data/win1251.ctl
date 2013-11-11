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
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	2				2				0x00							0x7f							Direct				{}		# ASCII
	1				1				0x80							0xff							IndexedTable16		{}		# various stuff
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	6				6				0x0000							0x007f							Direct				1									{}		# ASCII
    	5				5				0x0401							0x040C							IndexedTable16				1									{}				
	4				4				0x040E							0x044F							IndexedTable16				1									{}	
	3				3				0x0451							0x045C							IndexedTable16				1									{}		
    	2				2				0x0098							0x00BB							KeyedTable1616		1									{}
    	1				1				0x045E							0x2122							KeyedTable1616		1									{}

EndUnicodeToForeignData

