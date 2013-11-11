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
# Name:             3GPP TS 23.038 V8.1.0 "Spanish GSM le + GSM 7 bits extension table " to Unicode converter control table
# Unicode version:  3.0
# Table version:    1.0
# Date:             March 6th 2009
# Authors:          Wendy Du
# Version history
#         2009/3/6          1.0 version new version.
#


Endianness															FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters						0x3f

StartForeignVariableByteData
#	FirstInitialByteValueInRange	LastInitialByteValueInRange		NumberOfSubsequentBytes
	0x00							0x1a							0
	0x1b							0x1b							1
	0x1c							0xff							0
EndForeignVariableByteData

StartForeignToUnicodeData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm		Parameters
	6	 	100		0x00				0x7f				KeyedTable1616		{}
	2		200		0x1b09				0x1b75				KeyedTable1616		{}

EndForeignToUnicodeData

StartUnicodeToForeignData
#	IncludePriority	SearchPriority	FirstInputCharacterCodeInRange	LastInputCharacterCodeInRange	Algorithm						SizeOfOutputCharacterCodeInBytes	Parameters
	80				30				0x000c							0x000c							Offset							2										{}
	75				40				0x0020							0x0023							Direct							1										{}
	72				42				0x0025							0x003F				            Direct							1										{}
	73				43				0x0041							0x005a							Direct							1										{}
	70				45				0x005b							0x005e							KeyedTable16OfIndexedTables16	2										{4}
	65				50				0x005F							0x005F							Offset							1										{}
	60				55				0x0061							0x007A							Direct							1										{}
	55				60				0x007b							0x007E							KeyedTable16OfIndexedTables16	2										{4}
	50				65				0x00c1							0x00c1							Offset							2										{}
	45				70				0x00cd							0x00cd							Offset							2										{}
	40				75				0x00d3							0x00d3							Offset							2										{}
	35				80				0x00da							0x00da							Offset							2										{}
	30				90				0x00e1							0x00e1							Offset							2										{}
	25				100				0x00e7							0x00e7							Offset							2										{}
	20				120				0x00ed							0x00ed							Offset							2										{}
	15				140				0x00f3							0x00f3							Offset							2										{}
	10				160				0x00fa							0x00fa							Offset							2										{}
	5				180				0x20ac							0x20ac							Offset				  			2										{}
	1				200				0x000a							0x2211							KeyedTable1616					1										{}
   		
ConflictResolution
#	UnicodeCharacterCodeToResolve	ForeignCharacterCodePreferred
0x000D	0x0D	#	CARRIAGE RETURN	
	
EndUnicodeToForeignData
