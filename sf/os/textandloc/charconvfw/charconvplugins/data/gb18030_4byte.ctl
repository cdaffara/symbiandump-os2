# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
	0x81							0x84							3
EndForeignVariableByteData

#######################################################################################################################
# Note: gbk_shared.cpp knows about the number of foreign-to-Unicode ranges and Unicode-to-foreign ranges in this file #
#######################################################################################################################

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm						Parameters
	# Note: ASCII is in GB2312, so will be extracted from GBK's own conversion data
	1				1				0x81308130						0x8431a439						KeyedTable32OfIndexedTables32	{10}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	# Note: ASCII is in GB2312, so will be extracted from GBK's own conversion data
	1				1				0x0080							0xffff							KeyedTable32OfIndexedTables32		4	{5}
	2				2				0x00a5							0xfe67							KeyedTable3232				4	{}
EndUnicodeToForeignData

