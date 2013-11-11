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
#The Control File For ISCII Character Set Conversion Plug-In


Endianness															FixedBigEndian

ReplacementForUnconvertibleUnicodeCharacters		0x1a 	# ASCII substitute character

StartForeignVariableByteData
0x00			0xff			0 															
EndForeignVariableByteData

StartForeignToUnicodeData
	
1	1	0x00	0x7f	Direct		{}
2 2 0x80  0x85	KeyedTable1616	{}	
2	2	0xa1	0xa4	KeyedTable1616	{}	
3	3	0xa5	0xaa	offset		{}	
4	4	0xab	0xb2	IndexedTable16	{}	
5	5	0xb3	0xcd	offset		{}
6	6	0xce	0xcf	KeyedTable1616	{}
7	7	0xd0	0xd8	offset		{}
8	8	0xd9	0xda	KeyedTable1616	{}
9	9	0xdb	0xdf	Offset		{}
10	10	0xe0	0xea	IndexedTable16	{}
11	11	0xf1	0xfa	Offset		{}
12	12	0xfc	0xff	KeyedTable1616	{}
# To support the unusual cases where one Unicode character corresponds to two ISCII characters.
13	13	0xa1e9	0xeae9	KeyedTable1616	{}

ConflictResolution
#ForeignCharacterCodeToResolve	UnicodeCharacterCodePreferred
0xe8							0x094d
0xe9							0x093c


EndForeignToUnicodeData


StartUnicodeToForeignData
1	1	0x0000	0x007f	Direct		1	{}
2	2	0x0901	0x0905	KeyedTable1616	1	{}		
3	3	0x0906	0x090b	Offset		1	{}
4	4	0x090d	0x0914	IndexedTable16	1	{}
5	5	0x0915	0x092f	Offset		1	{}
6	6	0x0930	0x093c	KeyedTable1616	1	{}
7	7	0x093e	0x0943	Offset		1	{}	
8	8	0x0945	0x094d	IndexedTable16	1	{}
9	9	0x095f	0x095f	KeyedTable1616	1	{} 
10	10	0x0964	0x0964	KeyedTable1616	1	{} 
11	11	0x0966	0x096f	Offset		1	{}
12	12	0x200c	0x200d	KeyedTable1616	1	{}
13	13	0x00A0	0x00A0	KeyedTable1616	1	{}
# To support the unusual cases where one Unicode character corresponds to two ISCII characters.
13	13	0x0960	0x0963	KeyedTable1616	2	{}
14	14	0x0950	0x095e	KeyedTable1616	2	{}
15	15	0x093d	0x093d	KeyedTable1616	2	{}
16	16	0x0944	0x0944	KeyedTable1616	2	{}
17	17	0x090C	0x090C	KeyedTable1616	2	{}
ConflictResolution
#ForeignCharacterCodeToResolve	UnicodeCharacterCodePreferred
0x200c							0xe8
0x200d							0xe9
0x0950							0xa1e9
0x093D							0xeae9	
0x0960							0xAAE9
0x0944							0xDFE9
0x0961							0xA7E9
0x090C							0xA6E9
0x0962							0xDBE9
0x0963							0xDCE9

EndUnicodeToForeignData