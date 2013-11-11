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

Endianness                                                          FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters                        0x81 0x48 # a dummy value - the plug-ins using PICTOGRAPH_SHARED.CTL will not use this

StartForeignVariableByteData
#   FirstInitialByteValueInRange    LastInitialByteValueInRange     NumberOfSubsequentBytes
    0xF0                            0xF9                            1
EndForeignVariableByteData

StartForeignToUnicodeData
#       IncludePriority SearchPriority  FirstInputCharacterCodeInRange  LastInputCharacterCodeInRange   Algorithm           Parameters
        1               1               0xF040                          0xF07E                          Offset              {}
        2               2               0xF080                          0xF0FC                          Offset              {}
        3               3               0xF140                          0xF17E                          Offset              {}
        4               4               0xF180                          0xF1FC                          Offset              {}
        5               5               0xF240                          0xF27E                          Offset              {}
        6               6               0xF280                          0xF2FC                          Offset              {}
        7               7               0xF340                          0xF37E                          Offset              {}
        8               8               0xF380                          0xF3FC                          Offset              {}
        9               9               0xF440                          0xF47E                          Offset              {}
        10              10              0xF480                          0xF4FC                          Offset              {}
        11              11              0xF540                          0xF57E                          Offset              {}
        12              12              0xF580                          0xF5FC                          Offset              {}
        13              13              0xF640                          0xF67E                          Offset              {}
        14              14              0xF680                          0xF6FC                          Offset              {}
        15              15              0xF740                          0xF77E                          Offset              {}
        16              16              0xF780                          0xF7FC                          Offset              {}
        17              17              0xF840                          0xF87E                          Offset              {}
        18              18              0xF880                          0xF89E                          Offset              {}
        19              19              0xF89F                          0xF8FC                          Offset              {}          # DoCoMo Pictograph
        20              20              0xF940                          0xF97E                          Offset              {}          # DoCoMo Pictograph
        21              21              0xF980                          0xF9FC                          Offset              {}          # DoCoMo Pictograph
EndForeignToUnicodeData

StartUnicodeToForeignData
#       IncludePriority SearchPriority  FirstInputCharacterCodeInRange  LastInputCharacterCodeInRange   Algorithm           SizeOfOutputCharacterCodeInBytes    Parameters
        1               1               0xE000                          0xE03E                          Offset              2                                   {}
        2               2               0xE03F                          0xE0BB                          Offset              2                                   {}
        3               3               0xE0BC                          0xE0FA                          Offset              2                                   {}
        4               4               0xE0FB                          0xE177                          Offset              2                                   {}
        5               5               0xE178                          0xE1B6                          Offset              2                                   {}
        6               6               0xE1B7                          0xE233                          Offset              2                                   {}
        7               7               0xE234                          0xE272                          Offset              2                                   {}
        8               8               0xE273                          0xE2EF                          Offset              2                                   {}
        9               9               0xE2F0                          0xE32E                          Offset              2                                   {}
        10              10              0xE32F                          0xE3AB                          Offset              2                                   {}
        11              11              0xE3AC                          0xE3EA                          Offset              2                                   {}
        12              12              0xE3EB                          0xE467                          Offset              2                                   {}
        13              13              0xE468                          0xE4A6                          Offset              2                                   {}
        14              14              0xE4A7                          0xE523                          Offset              2                                   {}
        15              15              0xE524                          0xE562                          Offset              2                                   {}
        16              16              0xE563                          0xE5DF                          Offset              2                                   {}
        17              17              0xE5E0                          0xE61E                          Offset              2                                   {}
        18              18              0xE61F                          0xE63D                          Offset              2                                   {}
        19              19              0xE63E                          0xE69B                          Offset              2                                   {}          # DoCoMo Pictograph
        20              20              0xE69C                          0xE6DA                          Offset              2                                   {}          # DoCoMo Pictograph
        21              21              0xE6DB                          0xE757                          Offset              2                                   {}          # DoCoMo Pictograph
EndUnicodeToForeignData

