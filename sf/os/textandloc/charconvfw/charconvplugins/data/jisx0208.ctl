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
ReplacementForUnconvertibleUnicodeCharacters						0x00 # a dummy value - the plug-ins using JISX0208.CTL will not use this

StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0xff							1
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	16				14				0x2330							0x2339							Offset							{}		# fullwidth digits
	15				13				0x2341							0x235a							Offset							{}		# fullwidth Latin capital letters
	14				12				0x2361							0x237a							Offset							{}		# fullwidth Latin small letters
	13				11				0x2421							0x2473							Offset							{}		# hiragana
	12				10				0x2521							0x255f							Offset							{}		# katakana [1]
	11				9				0x2560							0x2576							Offset							{}		# katakana [2]
	10				8				0x2621							0x2631							Offset							{}		# Greek capital letters [1]
	9				7				0x2632							0x2638							Offset							{}		# Greek capital letters [2]
	8				6				0x2641							0x2651							Offset							{}		# Greek small letters [1]
	7				5				0x2652							0x2658							Offset							{}		# Greek small letters [2]
	6				4				0x2721							0x2726							Offset							{}		# Cyrillic capital letters [1]
	5				3				0x2728							0x2741							Offset							{}		# Cyrillic capital letters [2]
	4				2				0x2751							0x2756							Offset							{}		# Cyrillic small letters [1]
	3				1				0x2758							0x2771							Offset							{}		# Cyrillic small letters [2]
	2				200				0x2121							0x7426							KeyedTable16OfIndexedTables16	{6}
	1				100				0x227e							0x2757							KeyedTable1616					{}
EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	16				15				0x0391							0x03a1							Offset				2									{}		# Greek capital letters [1]
	15				14				0x03a3							0x03a9							Offset				2									{}		# Greek capital letters [2]
	14				13				0x03b1							0x03c1							Offset				2									{}		# Greek small letters [1]
	13				12				0x03c3							0x03c9							Offset				2									{}		# Greek small letters [2]
	12				11				0x0410							0x0415							Offset				2									{}		# Cyrillic capital letters [1]
	11				10				0x0416							0x042f							Offset				2									{}		# Cyrillic capital letters [2]
	10				9				0x0430							0x0435							Offset				2									{}		# Cyrillic small letters [1]
	9				8				0x0436							0x044f							Offset				2									{}		# Cyrillic small letters [2]
	8				7				0x3008							0x3011							Offset				2									{}		# brackets (note: in the ForeignToUnicodeData table above, "brackets" come in the KeyedTable16OfIndexedTables16)
	7				6				0x3041							0x3093							Offset				2									{}		# hiragana
	6				5				0x30a1							0x30df							Offset				2									{}		# katakana [1]
	5				4				0x30e0							0x30f6							Offset				2									{}		# katakana [2]
	4				3				0xff10							0xff19							Offset				2									{}		# fullwidth digits
	3				2				0xff21							0xff3a							Offset				2									{}		# fullwidth Latin capital letters
	2				1				0xff41							0xff5a							Offset				2									{}		# fullwidth Latin small letters
	1				100				0x00a2							0xffe5							KeyedTable1616		2									{}
EndUnicodeToForeignData

