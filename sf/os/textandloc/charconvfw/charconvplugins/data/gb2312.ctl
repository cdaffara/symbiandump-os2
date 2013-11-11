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
	21				19				0x00							0x7f							Direct							{}		# ASCII
	20				18				0xa2b1							0xa2c4							Offset							{}		# digits and numbers with full stop
	19				17				0xa2c5							0xa2d8							Offset							{}		# parenthesized digits and numbers
	18				16				0xa2d9							0xa2e2							Offset							{}		# circled digits
	17				15				0xa2e5							0xa2ee							Offset							{}		# parenthesized ideographic digits
	16				14				0xa2f1							0xa2fc							Offset							{}		# Roman numerals
	15				13				0xa3a5							0xa3fd							Offset							{}		# fullwidth stuff
	14				12				0xa4a1							0xa4f3							Offset							{}		# hiragana
	13				11				0xa5a1							0xa5f6							Offset							{}		# katakana
	12				10				0xa6a1							0xa6b1							Offset							{}		# Greek capital letters [1]
	11				9				0xa6b2							0xa6b8							Offset							{}		# Greek capital letters [2]
	10				8				0xa6c1							0xa6d1							Offset							{}		# Greek small letters [1]
	9				7				0xa6d2							0xa6d8							Offset							{}		# Greek small letters [2]
	8				6				0xa7a1							0xa7a6							Offset							{}		# Cyrillic capital letters [1]
	7				5				0xa7a8							0xa7c1							Offset							{}		# Cyrillic capital letters [2]
	6				4				0xa7d1							0xa7d6							Offset							{}		# Cyrillic small letters [1]
	5				3				0xa7d8							0xa7f1							Offset							{}		# Cyrillic small letters [2]
	4				2				0xa8c5							0xa8e9							Offset							{}		# bopomofo
	3				1				0xa9a4							0xa9ef							Offset							{}		# box drawings
	2				200				0xa1a1							0xf7fe							KeyedTable16OfIndexedTables16	{6}
	1				100				0xa3a1							0xa7d7							KeyedTable1616					{}
ConflictResolution
#	ForeignCharacterCodeToResolve	UnicodeCharacterCodePreferred
	0xa1a4							0x00b7
	0xa1aa							0x2015
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	21				20				0x0000							0x007f							Direct				1									{}		# ASCII
	20				19				0x0391							0x03a1							Offset				2									{}		# Greek capital letters [1]
	19				18				0x03a3							0x03a9							Offset				2									{}		# Greek capital letters [2]
	18				17				0x03b1							0x03c1							Offset				2									{}		# Greek small letters [1]
	17				16				0x03c3							0x03c9							Offset				2									{}		# Greek small letters [2]
	16				15				0x0410							0x0415							Offset				2									{}		# Cyrillic capital letters [1]
	15				14				0x0416							0x042f							Offset				2									{}		# Cyrillic capital letters [2]
	14				13				0x0430							0x0435							Offset				2									{}		# Cyrillic small letters [1]
	13				12				0x0436							0x044f							Offset				2									{}		# Cyrillic small letters [2]
	12				11				0x2160							0x216b							Offset				2									{}		# Roman numerals
	11				10				0x2460							0x2469							Offset				2									{}		# circled digits
	10				9				0x2474							0x2487							Offset				2									{}		# parenthesized digits and numbers
	9				8				0x2488							0x249b							Offset				2									{}		# digits and numbers with full stop
	8				7				0x2500							0x254b							Offset				2									{}		# box drawings
	7				6				0x3008							0x300f							Offset				2									{}		# angle and corner brackets
	6				5				0x3041							0x3093							Offset				2									{}		# hiragana
	5				4				0x30a1							0x30f6							Offset				2									{}		# katakana
	4				3				0x3105							0x3129							Offset				2									{}		# bopomofo
	3				2				0x3220							0x3229							Offset				2									{}		# parenthesized ideographic digits
	2				1				0xff05							0xff5d							Offset				2									{}		# fullwidth stuff
	1				100				0x00a4							0xffe5							KeyedTable1616		2									{}
EndUnicodeToForeignData

