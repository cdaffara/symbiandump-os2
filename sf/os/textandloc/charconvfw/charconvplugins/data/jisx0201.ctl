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
ReplacementForUnconvertibleUnicodeCharacters						0x00 # a dummy value - the plug-ins using JISX0201.CTL will not use this

StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0xff							0
EndForeignVariableByteData

StartForeignToUnicodeData
#		IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			Parameters
	StartAdditionalSubsetTable jisRomanConversionData
		6				6				0x00							0x5b							Direct				{}		# ASCII characters [1]
		5				2				0x5c							0x5c							Offset				{}		# yen sign
		4				5				0x5d							0x7d							Direct				{}		# ASCII characters [2]
		3				1				0x7e							0x7e							Offset				{}		# overline
		2				4				0x7f							0x7f							Direct				{}		# ASCII characters [3]
	EndAdditionalSubsetTable jisRomanConversionData
	StartAdditionalSubsetTable halfWidthKatakana8ConversionData
		1				3				0xa1							0xdf							Offset				{}		# half-width katakana
	EndAdditionalSubsetTable halfWidthKatakana8ConversionData
EndForeignToUnicodeData

StartUnicodeToForeignData
#		IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm			SizeOfOutputCharacterCodeInBytes	Parameters
	StartAdditionalSubsetTable jisRomanConversionData
		6				1				0x0000							0x005b							Direct				1									{}		# ASCII characters [1]
		5				2				0x005d							0x007d							Direct				1									{}		# ASCII characters [2]
		4				3				0x007f							0x007f							Direct				1									{}		# ASCII characters [3]
		3				5				0x00a5							0x00a5							Offset				1									{}		# yen sign
		2				6				0x203e							0x203e							Offset				1									{}		# overline
	EndAdditionalSubsetTable jisRomanConversionData
	StartAdditionalSubsetTable halfWidthKatakana8ConversionData
		1				4				0xff61							0xff9f							Offset				1									{}		# half-width katakana
	EndAdditionalSubsetTable halfWidthKatakana8ConversionData
EndUnicodeToForeignData

