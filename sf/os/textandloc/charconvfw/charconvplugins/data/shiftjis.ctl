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
ReplacementForUnconvertibleUnicodeCharacters						0x81 0x48 # fullwidth question mark - must be a single character, and must be little-endian if the "Endianness" above is "Unspecified", otherwise in the same endianness as specified

StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0x7e							0
	0xa1							0xdf							0
	0x81							0x9f							1
	0xe0							0xef							1
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	24				21				0x0020							0x005b							Direct				{}		# ASCII characters [1]
	23				100				0x005c							0x005c							Offset				{}		# yen sign
	22				20				0x005d							0x007d							Direct				{}		# ASCII characters [2]
	21				200				0x007e							0x007e							Offset				{}		# overline
	20				19				0x00a1							0x00df							Offset				{}		# halfwidth katakana
	19				18				0x8140							0x817e							IndexedTable16		{}		# various stuff
	18				17				0x8180							0x81ac							IndexedTable16		{}		# various stuff
	17				16				0x824f							0x8258							Offset				{}		# fullwidth digits
	16				15				0x8260							0x8279							Offset				{}		# fullwidth Latin capital letters
	15				14				0x8281							0x829a							Offset				{}		# fullwidth Latin small letters
	14				13				0x829f							0x82f1							Offset				{}		# hiragana
	13				12				0x8340							0x837e							Offset				{}		# katakana [1]
	12				11				0x8380							0x8396							Offset				{}		# katakana [2]
	11				10				0x839f							0x83af							Offset				{}		# Greek capital letters [1]
	10				9				0x83b0							0x83b6							Offset				{}		# Greek capital letters [2]
	9				8				0x83bf							0x83cf							Offset				{}		# Greek small letters [1]
	8				7				0x83d0							0x83d6							Offset				{}		# Greek small letters [2]
	7				6				0x8440							0x8445							Offset				{}		# Cyrillic capital letters [1]
	6				5				0x8447							0x8460							Offset				{}		# Cyrillic capital letters [2]
	5				4				0x8470							0x8475							Offset				{}		# Cyrillic small letters [1]
	4				3				0x8477							0x847e							Offset				{}		# Cyrillic small letters [2]
	3				2				0x8480							0x8491							Offset				{}		# Cyrillic small letters [3]
	2				1				0x849f							0x84be							IndexedTable16		{}		# box drawings
	1				500				0x81b8							0xeaa4							KeyedTable1616		{}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	22				21				0x0020							0x005b							Direct				1									{}		# ASCII characters [1]
	21				20				0x005d							0x007d							Direct				1									{}		# ASCII characters [1]
	20				19				0xff61							0xff9f							Offset				1									{}		# halfwidth katakana
	19				18				0x00a5							0x00a5							Offset				1									{}		# yen sign
	18				17				0x203e							0x203e							Offset				1									{}		# overline
	17				16				0x0391							0x03a1							Offset				2									{}		# Greek capital letters [1]
	16				15				0x03a3							0x03a9							Offset				2									{}		# Greek capital letters [2]
	15				14				0x03b1							0x03c1							Offset				2									{}		# Greek small letters [1]
	14				13				0x03c3							0x03c9							Offset				2									{}		# Greek small letters [2]
	13				12				0x0410							0x0415							Offset				2									{}		# Cyrillic capital letters [1]
	12				11				0x0416							0x042f							Offset				2									{}		# Cyrillic capital letters [2]
	11				10				0x0430							0x0435							Offset				2									{}		# Cyrillic small letters [1]
	10				9				0x0436							0x043d							Offset				2									{}		# Cyrillic small letters [2]
	9				8				0x043e							0x044f							Offset				2									{}		# Cyrillic small letters [3]
	8				7				0x3008							0x3011							Offset				2									{}		# brackets (note: in the ForeignToUnicodeData table above, brackets come in one of the IndexedTable16s)
	7				6				0x3041							0x3093							Offset				2									{}		# hiragana
	6				5				0x30a1							0x30df							Offset				2									{}		# katakana [1]
	5				4				0x30e0							0x30f6							Offset				2									{}		# katakana [2]
	4				3				0xff10							0xff19							Offset				2									{}		# fullwidth digits
	3				2				0xff21							0xff3a							Offset				2									{}		# fullwidth Latin capital letters
	2				1				0xff41							0xff5a							Offset				2									{}		# fullwidth Latin small letters
	1				100				0x005c							0xffe5							KeyedTable1616		2									{}
EndUnicodeToForeignData

