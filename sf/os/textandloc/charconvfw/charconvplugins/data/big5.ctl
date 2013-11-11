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
ReplacementForUnconvertibleUnicodeCharacters						0xa1 0x48 # fullwidth question mark - must be a single character, and must be little-endian if the "Endianness" above is "Unspecified", otherwise in the same endianness as specified

StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0x7f							0
	0x80							0xff							1
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm						Parameters
	23				21				0x00							0x7f							Direct							{}		# ASCII
	22				20				0xa2af							0xa2b8							Offset							{}		# fullwidth digits
	21				19				0xa2b9							0xa2c2							Offset							{}		# Roman numerals
	20				18				0xa2c3							0xa2cb							Offset							{}		# hangzhou numerals
	19				17				0xa2cf							0xa2e8							Offset							{}		# fullwidth Latin capital letters
	18				16				0xa2e9							0xa2fe							Offset							{}		# fullwidth Latin small letters [1] (note: fullwidth Latin small letters [2] come in the KeyedTable1616)
	17				15				0xa344							0xa354							Offset							{}		# Greek capital letters [1]
	16				14				0xa355							0xa35b							Offset							{}		# Greek capital letters [2]
	15				13				0xa35c							0xa36c							Offset							{}		# Greek small letters [1]
	14				12				0xa36d							0xa373							Offset							{}		# Greek small letters [2]
	13				11				0xa374							0xa37e							Offset							{}		# bopomofo [1]
	12				10				0xa3a1							0xa3ba							Offset							{}		# bopomofo [2]
	11				9				0xc6a5							0xc6f7							Offset							{}		# hiragana
	10				8				0xc6f8							0xc6fe							Offset							{}		# katakana [1]
	9				7				0xc740							0xc77e							Offset							{}		# katakana [2]
	8				6				0xc7a1							0xc7b0							Offset							{}		# katakana [3]
	7				5				0xc7b4							0xc7ba							Offset							{}		# Cyrillic capital letters [1]
	6				4				0xc7bb							0xc7cd							Offset							{}		# Cyrillic capital letters [2] and Cyrillic small letters [1]
	5				3				0xc7cf							0xc7e8							Offset							{}		# Cyrillic small letters [2]
	4				2				0xc7e9							0xc7f2							Offset							{}		# circled digits
	3				1				0xc7f3							0xc7fc							Offset							{}		# parenthesized digits and numbers
	2				200				0xa140							0xf9d5							KeyedTable16OfIndexedTables16	{6}
	1				100				0xa1c4							0xc7ce							KeyedTable1616					{}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	23				22				0x0000							0x007f							Direct				1									{}		# ASCII
	22				21				0x0391							0x03a1							Offset				2									{}		# Greek capital letters [1]
	21				20				0x03a3							0x03a9							Offset				2									{}		# Greek capital letters [2]
	20				19				0x03b1							0x03c1							Offset				2									{}		# Greek small letters [1]
	19				18				0x03c3							0x03c9							Offset				2									{}		# Greek small letters [2]
	18				17				0x0416							0x041c							Offset				2									{}		# Cyrillic capital letters [1]
	17				16				0x0423							0x0435							Offset				2									{}		# Cyrillic capital letters [2] and Cyrillic small letters [1]
	16				15				0x0436							0x044f							Offset				2									{}		# Cyrillic small letters [2]
	15				14				0x2160							0x2169							Offset				2									{}		# Roman numerals
	14				13				0x2460							0x2469							Offset				2									{}		# circled digits
	13				12				0x2474							0x247d							Offset				2									{}		# parenthesized digits and numbers
	12				11				0x2581							0x2588							Offset				2									{}		# blocks (note: in the ForeignToUnicodeData table above, "blocks" come in the KeyedTable16OfIndexedTables16)
	11				10				0x3021							0x3029							Offset				2									{}		# hangzhou numerals
	10				9				0x3041							0x3093							Offset				2									{}		# hiragana
	9				8				0x30a1							0x30a7							Offset				2									{}		# katakana [1]
	8				7				0x30a8							0x30e6							Offset				2									{}		# katakana [2]
	7				6				0x30e7							0x30f6							Offset				2									{}		# katakana [3]
	6				5				0x3105							0x310f							Offset				2									{}		# bopomofo [1]
	5				4				0x3110							0x3129							Offset				2									{}		# bopomofo [2]
	4				3				0xff10							0xff19							Offset				2									{}		# fullwidth digits
	3				2				0xff21							0xff3a							Offset				2									{}		# fullwidth Latin capital letters
	2				1				0xff41							0xff56							Offset				2									{}		# fullwidth Latin small letters [1] (note: fullwidth Latin small letters [2] come in the KeyedTable1616)
	1				100				0x00a2							0xff64							KeyedTable1616		2									{}
ConflictResolution
#	UnicodeCharacterCodeToResolve	ForeignCharacterCodePreferred
	0x5341							0xa451
	0x5345							0xa4ca
	0xff3f							0xa1c4
	0x2571							0xa2ac
	0x2572							0xa2ad
EndUnicodeToForeignData

