# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
ReplacementForUnconvertibleUnicodeCharacters						0xa3 0xbf # fullwidth question mark - must be a single character, and must be little-endian if the "Endianness" above is "Unspecified", otherwise in the same endianness as specified

StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0x7f							0
	0x80							0xff							1
EndForeignVariableByteData

#######################################################################################################################
# Note: gbk_shared.cpp knows about the number of foreign-to-Unicode ranges and Unicode-to-foreign ranges in this file #
#######################################################################################################################

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm						Parameters
	# Note: ASCII is in GB2312, so will be extracted from GBK's own conversion data
	2				2				0x8140							0xfe4f							KeyedTable16OfIndexedTables16	{6}
	1				1				0xa6ee							0xa996							KeyedTable1616					{}
ConflictResolution
#	ForeignCharacterCodeToResolve	UnicodeCharacterCodePreferred
	0xa1a4							0x00b7
	0xa1aa							0x2015
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	# Note: ASCII is in GB2312, so will be extracted from GBK's own conversion data
	1				1				0x0144							0xffe4							KeyedTable1616		2									{}
ConflictResolution
#	UnicodeCharacterCodeToResolve	UnicodeCharacterCodePreferred
	0x2015							0xa1aa
EndUnicodeToForeignData

