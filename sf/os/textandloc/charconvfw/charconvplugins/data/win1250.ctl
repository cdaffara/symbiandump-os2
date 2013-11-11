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
# CP1250.CTL
# Eastern European Character table (Code Page 1250)
Endianness															FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters						0x1a # This is code which replaces unknown chars

StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
# foreign codes are 8bit. (0=8bit, 1=16bit)
	0x00							0xff							0
EndForeignVariableByteData


StartForeignToUnicodeData
# following table is made based on the output of: perl -w analyse.pl cp1250.txt test.out 1
# Codes 00-7f are directly converted, 80-ff are using table.
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	2				2				0x00							0x7f							Direct				{}		# ASCII
	1				1				0x80							0xff							KeyedTable16OfIndexedTables16      {}		# various Latin characters with diacritics

EndForeignToUnicodeData


# following table is made based on the output of: perl -w analyse.pl cp1250.txt test.out 2
# direct conversion is fastest. KeydTable (slowest) is used to fill the holes. Ranges CAN overlap :) 
# Notice that blocks under 5 bytes shouldn't be added here.

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	7				7				0x0000							0x007f							Direct				1									{}		# ASCII
	6				6				0x0102							0x0107							IndexedTable16      1									{}
	5				5				0x010c							0x0111							IndexedTable16      1									{}
	4				4				0x015e							0x0165							IndexedTable16      1									{}
	3				3				0x0179							0x017e							IndexedTable16      1									{}
    	2				2				0x0081							0x00fd							KeyedTable1616		1									{}
    	1				1				0x0118							0x2122							KeyedTable1616		1									{}
# ConflictResolution
#	UnicodeCharacterCodeToResolve	ForeignCharacterCodePreferred
# No confliced chars
EndUnicodeToForeignData