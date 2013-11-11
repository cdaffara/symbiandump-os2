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

# KOI8R.CTL
# Russian Character table (KOI8-R)
#
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
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm		    	           Parameters
	2				2				0x00							0x7f							Direct			                   {}	# ASCII
	1				1				0x80							0xff							KeyedTable16OfIndexedTables16      {}	# box drawing and cyrillic chars

EndForeignToUnicodeData


# KeydTable (slowest) is used to fill the holes. Ranges CAN overlap.
# Notice that blocks under 5 bytes shouldn't be added here.

# TODO: This table should have been created using the analyse.pl script that comes with
# charconv sources. Unfortunately i couldn't get it working so the following is made manually
# (mostly just by guessing) and might contain errors...

StartUnicodeToForeignData

#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
    2				2				0x0000							0x007f							Direct				1									{}		# ASCII
    1				1				0x00B0							0x25A0							KeyedTable1616      1									{}



# ConflictResolution
#	UnicodeCharacterCodeToResolve	ForeignCharacterCodePreferred
# No confliced chars
EndUnicodeToForeignData
