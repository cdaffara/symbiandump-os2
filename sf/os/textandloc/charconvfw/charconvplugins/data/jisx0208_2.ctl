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

Endianness                                                          FixedBigEndian
ReplacementForUnconvertibleUnicodeCharacters                        0x00 # a dummy value - the plug-ins using JISX0208.CTL will not use this

StartForeignVariableByteData
#   FirstInitialByteValueInRange    LastInitialByteValueInRange     NumberOfSubsequentBytes
    0x00                            0xff                            1
EndForeignVariableByteData

StartForeignToUnicodeData
#   IncludePriority SearchPriority  FirstInputCharacterCodeInRange  LastInputCharacterCodeInRange   Algorithm           Parameters
    27              25              0x2330                          0x2339                          Offset                          {}      # fullwidth digits
    26              24              0x2341                          0x235a                          Offset                          {}      # fullwidth Latin capital letters
    25              23              0x2361                          0x237a                          Offset                          {}      # fullwidth Latin small letters
    24              22              0x2421                          0x2473                          Offset                          {}      # hiragana
    23              21              0x2521                          0x255f                          Offset                          {}      # katakana [1]
    22              20              0x2560                          0x2576                          Offset                          {}      # katakana [2]
    21              19              0x2621                          0x2631                          Offset                          {}      # Greek capital letters [1]
    20              18              0x2632                          0x2638                          Offset                          {}      # Greek capital letters [2]
    19              17              0x2641                          0x2651                          Offset                          {}      # Greek small letters [1]
    18              16              0x2652                          0x2658                          Offset                          {}      # Greek small letters [2]
    17              15              0x2721                          0x2726                          Offset                          {}      # Cyrillic capital letters [1]
    16              14              0x2728                          0x2741                          Offset                          {}      # Cyrillic capital letters [2]
    15              13              0x2751                          0x2756                          Offset                          {}      # Cyrillic small letters [1]
    14              12              0x2758                          0x2771                          Offset                          {}      # Cyrillic small letters [2]
    13              11              0x2D21                          0x2D34                          Offset                          {}      # NEC DIGIT NUMBER WITH CIRCLE          #K3H
    12              10              0x2D35                          0x2D3E                          Offset                          {}      # NEC ROMAN DIGIT                       #K3H
    11              9               0x7c71                          0x7c7a                          Offset                          {}      # NEC SELECT ROMAN SMALL LETTER DIGIT   #K3H
    10              8               0x2D40                          0x2D56                          IndexedTable16                  {}      # NEC KATAKANA UNIT SYMBOL and UNIT SYMBOL  #K3H
    9               7               0x2D5F                          0x2D7C                          IndexedTable16                  {}      # NEC SYMBOL , ERA NAME , and so on         #K3H
    8               6               0x2821                          0x2840                          IndexedTable16                  {}      # box drawings                          #K3H
    7               5               0x9321                          0x937E                          IndexedTable16                  {}      # IBM ext.      #K3H
    6               4               0x9421                          0x947E                          IndexedTable16                  {}      # IBM ext.      #K3H
    5               3               0x9521                          0x957E                          IndexedTable16                  {}      # IBM ext.      #K3H
    4               2               0x9621                          0x967E                          IndexedTable16                  {}      # IBM ext.      #K3H
    3               1               0x9721                          0x972c                          IndexedTable16                  {}      # IBM ext.      #K3H
    2               200             0x2121                          0x7C7E                          KeyedTable16OfIndexedTables16   {6}
    1               100             0x227e                          0x2757                          KeyedTable1616                  {}
ConflictResolution                                                      #K3H
#   ForeignCharacterCodeToResolve   UnicodeCharacterCodePreferred       #K3H    SJIS
    0x2124                          0xFF0C                              #K3H    0x8143
    0x2126                          0x30FB                              #K3H    0x8145
    0x2131                          0xFFE3                              #K3H    0x8150
    0x2142                          0x2016                              #K3H    0x8161
    0x2171                          0xFFE0                              #K3H    0x8191
    0x2172                          0xFFE1                              #K3H    0x8192
    0x224C                          0x00AC                              #K3H    0x81CA
    0x2263                          0x226A                              #K3H    0x81E1
    0x2264                          0x226B                              #K3H    0x81E2
    0x2574                          0x30F4                              #K3H    0x8394
    0x264C                          0x03BC                              #K3H    0x83CA
EndForeignToUnicodeData

StartUnicodeToForeignData
#   IncludePriority SearchPriority  FirstInputCharacterCodeInRange  LastInputCharacterCodeInRange   Algorithm           SizeOfOutputCharacterCodeInBytes    Parameters
    19              18              0x0391                          0x03a1                          Offset              2                                   {}      # Greek capital letters [1]
    18              17              0x03a3                          0x03a9                          Offset              2                                   {}      # Greek capital letters [2]
    17              16              0x03b1                          0x03c1                          Offset              2                                   {}      # Greek small letters [1]
    16              15              0x03c3                          0x03c9                          Offset              2                                   {}      # Greek small letters [2]
    15              14              0x0410                          0x0415                          Offset              2                                   {}      # Cyrillic capital letters [1]
    14              13              0x0416                          0x042f                          Offset              2                                   {}      # Cyrillic capital letters [2]
    13              12              0x0430                          0x0435                          Offset              2                                   {}      # Cyrillic small letters [1]
    12              11              0x0436                          0x044f                          Offset              2                                   {}      # Cyrillic small letters [2]
    11              10              0x2460                          0x2473                          Offset              2                                   {}      # NEC DIGIT NUMBER WITH CIRCLE
    10              9               0x2160                          0x2169                          Offset              2                                   {}      # NEC ROMAN DIGIT
    9               8               0x3008                          0x3011                          Offset              2                                   {}      # brackets (note: in the ForeignToUnicodeData table above, "brackets" come in the KeyedTable16OfIndexedTables16)
    8               7               0x3041                          0x3093                          Offset              2                                   {}      # hiragana
    7               6               0x30a1                          0x30df                          Offset              2                                   {}      # katakana [1]
    6               5               0x30e0                          0x30f6                          Offset              2                                   {}      # katakana [2]
    5               4               0xff10                          0xff19                          Offset              2                                   {}      # fullwidth digits
    4               3               0xff21                          0xff3a                          Offset              2                                   {}      # fullwidth Latin capital letters
    3               2               0xff41                          0xff5a                          Offset              2                                   {}      # fullwidth Latin small letters
    2               1               0x2170                          0x2179                          Offset              2                                   {}      # IBM SMALL LETTER DIGIT
    1               100             0x00A2                          0xffe5                          KeyedTable1616      2                                   {}
ConflictResolution                                                      #K3H
#   UnicodeCharacterCodeToResolve   ForeignCharacterCodePreferred       #K3H    Shift JIS
    0x2252                          0x2262                              #K3H    0x81e0  NEC ext.
    0x2261                          0x2261                              #K3H    0x81df  NEC ext.
    0x222b                          0x2269                              #K3H    0x81e7  NEC ext.
    0x221a                          0x2265                              #K3H    0x81e3  NEC ext.
    0x22a5                          0x225d                              #K3H    0x81db  NEC ext.
    0x2220                          0x225c                              #K3H    0x81da  NEC ext.
    0x2235                          0x2268                              #K3H    0x81e6  NEC ext.
    0x2229                          0x2241                              #K3H    0x81bf  NEC ext.
    0x222a                          0x2240                              #K3H    0x81be  NEC ext.

    0x7e8a                          0x7921                              #K3H    IBM ext.
    0x891c                          0x7922                              #K3H    IBM ext.
    0x9348                          0x7923                              #K3H    IBM ext.
    0x9288                          0x7924                              #K3H    IBM ext.
    0x84dc                          0x7925                              #K3H    IBM ext.
    0x4fc9                          0x7926                              #K3H    IBM ext.
    0x70bb                          0x7927                              #K3H    IBM ext.
    0x6631                          0x7928                              #K3H    IBM ext.
    0x68c8                          0x7929                              #K3H    IBM ext.
    0x92f9                          0x792a                              #K3H    IBM ext.
    0x66fb                          0x792b                              #K3H    IBM ext.
    0x5f45                          0x792c                              #K3H    IBM ext.
    0x4e28                          0x792d                              #K3H    IBM ext.
    0x4ee1                          0x792e                              #K3H    IBM ext.
    0x4efc                          0x792f                              #K3H    IBM ext.
    0x4f00                          0x7930                              #K3H    IBM ext.
    0x4f03                          0x7931                              #K3H    IBM ext.
    0x4f39                          0x7932                              #K3H    IBM ext.
    0x4f56                          0x7933                              #K3H    IBM ext.
    0x4f92                          0x7934                              #K3H    IBM ext.
    0x4f8a                          0x7935                              #K3H    IBM ext.
    0x4f9a                          0x7936                              #K3H    IBM ext.
    0x4f94                          0x7937                              #K3H    IBM ext.
    0x4fcd                          0x7938                              #K3H    IBM ext.
    0x5040                          0x7939                              #K3H    IBM ext.
    0x5022                          0x793a                              #K3H    IBM ext.
    0x4fff                          0x793b                              #K3H    IBM ext.
    0x501e                          0x793c                              #K3H    IBM ext.
    0x5046                          0x793d                              #K3H    IBM ext.
    0x5070                          0x793e                              #K3H    IBM ext.
    0x5042                          0x793f                              #K3H    IBM ext.
    0x5094                          0x7940                              #K3H    IBM ext.
    0x50f4                          0x7941                              #K3H    IBM ext.
    0x50d8                          0x7942                              #K3H    IBM ext.
    0x514a                          0x7943                              #K3H    IBM ext.
    0x5164                          0x7944                              #K3H    IBM ext.
    0x519d                          0x7945                              #K3H    IBM ext.
    0x51be                          0x7946                              #K3H    IBM ext.
    0x51ec                          0x7947                              #K3H    IBM ext.
    0x5215                          0x7948                              #K3H    IBM ext.
    0x529c                          0x7949                              #K3H    IBM ext.
    0x52a6                          0x794a                              #K3H    IBM ext.
    0x52c0                          0x794b                              #K3H    IBM ext.
    0x52db                          0x794c                              #K3H    IBM ext.
    0x5300                          0x794d                              #K3H    IBM ext.
    0x5307                          0x794e                              #K3H    IBM ext.
    0x5324                          0x794f                              #K3H    IBM ext.
    0x5372                          0x7950                              #K3H    IBM ext.
    0x5393                          0x7951                              #K3H    IBM ext.
    0x53b2                          0x7952                              #K3H    IBM ext.
    0x53dd                          0x7953                              #K3H    IBM ext.
    0xfa0e                          0x7954                              #K3H    IBM ext.
    0x549c                          0x7955                              #K3H    IBM ext.
    0x548a                          0x7956                              #K3H    IBM ext.
    0x54a9                          0x7957                              #K3H    IBM ext.
    0x54ff                          0x7958                              #K3H    IBM ext.
    0x5586                          0x7959                              #K3H    IBM ext.
    0x5759                          0x795a                              #K3H    IBM ext.
    0x5765                          0x795b                              #K3H    IBM ext.
    0x57ac                          0x795c                              #K3H    IBM ext.
    0x57c8                          0x795d                              #K3H    IBM ext.
    0x57c7                          0x795e                              #K3H    IBM ext.
    0xfa0f                          0x795f                              #K3H    IBM ext.
    0xfa10                          0x7960                              #K3H    IBM ext.
    0x589e                          0x7961                              #K3H    IBM ext.
    0x58b2                          0x7962                              #K3H    IBM ext.
    0x590b                          0x7963                              #K3H    IBM ext.
    0x5953                          0x7964                              #K3H    IBM ext.
    0x595b                          0x7965                              #K3H    IBM ext.
    0x595d                          0x7966                              #K3H    IBM ext.
    0x5963                          0x7967                              #K3H    IBM ext.
    0x59a4                          0x7968                              #K3H    IBM ext.
    0x59ba                          0x7969                              #K3H    IBM ext.
    0x5b56                          0x796a                              #K3H    IBM ext.
    0x5bc0                          0x796b                              #K3H    IBM ext.
    0x752f                          0x796c                              #K3H    IBM ext.
    0x5bd8                          0x796d                              #K3H    IBM ext.
    0x5bec                          0x796e                              #K3H    IBM ext.
    0x5c1e                          0x796f                              #K3H    IBM ext.
    0x5ca6                          0x7970                              #K3H    IBM ext.
    0x5cba                          0x7971                              #K3H    IBM ext.
    0x5cf5                          0x7972                              #K3H    IBM ext.
    0x5d27                          0x7973                              #K3H    IBM ext.
    0x5d53                          0x7974                              #K3H    IBM ext.
    0xfa11                          0x7975                              #K3H    IBM ext.
    0x5d42                          0x7976                              #K3H    IBM ext.
    0x5d6d                          0x7977                              #K3H    IBM ext.
    0x5db8                          0x7978                              #K3H    IBM ext.
    0x5db9                          0x7979                              #K3H    IBM ext.
    0x5dd0                          0x797a                              #K3H    IBM ext.
    0x5f21                          0x797b                              #K3H    IBM ext.
    0x5f34                          0x797c                              #K3H    IBM ext.
    0x5f67                          0x797d                              #K3H    IBM ext.
    0x5fb7                          0x797e                              #K3H    IBM ext.
    0x5fde                          0x7a21                              #K3H    IBM ext.
    0x605d                          0x7a22                              #K3H    IBM ext.
    0x6085                          0x7a23                              #K3H    IBM ext.
    0x608a                          0x7a24                              #K3H    IBM ext.
    0x60de                          0x7a25                              #K3H    IBM ext.
    0x60d5                          0x7a26                              #K3H    IBM ext.
    0x6120                          0x7a27                              #K3H    IBM ext.
    0x60f2                          0x7a28                              #K3H    IBM ext.
    0x6111                          0x7a29                              #K3H    IBM ext.
    0x6137                          0x7a2a                              #K3H    IBM ext.
    0x6130                          0x7a2b                              #K3H    IBM ext.
    0x6198                          0x7a2c                              #K3H    IBM ext.
    0x6213                          0x7a2d                              #K3H    IBM ext.
    0x62a6                          0x7a2e                              #K3H    IBM ext.
    0x63f5                          0x7a2f                              #K3H    IBM ext.
    0x6460                          0x7a30                              #K3H    IBM ext.
    0x649d                          0x7a31                              #K3H    IBM ext.
    0x64ce                          0x7a32                              #K3H    IBM ext.
    0x654e                          0x7a33                              #K3H    IBM ext.
    0x6600                          0x7a34                              #K3H    IBM ext.
    0x6615                          0x7a35                              #K3H    IBM ext.
    0x663b                          0x7a36                              #K3H    IBM ext.
    0x6609                          0x7a37                              #K3H    IBM ext.
    0x662e                          0x7a38                              #K3H    IBM ext.
    0x661e                          0x7a39                              #K3H    IBM ext.
    0x6624                          0x7a3a                              #K3H    IBM ext.
    0x6665                          0x7a3b                              #K3H    IBM ext.
    0x6657                          0x7a3c                              #K3H    IBM ext.
    0x6659                          0x7a3d                              #K3H    IBM ext.
    0xfa12                          0x7a3e                              #K3H    IBM ext.
    0x6673                          0x7a3f                              #K3H    IBM ext.
    0x6699                          0x7a40                              #K3H    IBM ext.
    0x66a0                          0x7a41                              #K3H    IBM ext.
    0x66b2                          0x7a42                              #K3H    IBM ext.
    0x66bf                          0x7a43                              #K3H    IBM ext.
    0x66fa                          0x7a44                              #K3H    IBM ext.
    0x670e                          0x7a45                              #K3H    IBM ext.
    0xf929                          0x7a46                              #K3H    IBM ext.
    0x6766                          0x7a47                              #K3H    IBM ext.
    0x67bb                          0x7a48                              #K3H    IBM ext.
    0x6852                          0x7a49                              #K3H    IBM ext.
    0x67c0                          0x7a4a                              #K3H    IBM ext.
    0x6801                          0x7a4b                              #K3H    IBM ext.
    0x6844                          0x7a4c                              #K3H    IBM ext.
    0x68cf                          0x7a4d                              #K3H    IBM ext.
    0xfa13                          0x7a4e                              #K3H    IBM ext.
    0x6968                          0x7a4f                              #K3H    IBM ext.
    0xfa14                          0x7a50                              #K3H    IBM ext.
    0x6998                          0x7a51                              #K3H    IBM ext.
    0x69e2                          0x7a52                              #K3H    IBM ext.
    0x6a30                          0x7a53                              #K3H    IBM ext.
    0x6a6b                          0x7a54                              #K3H    IBM ext.
    0x6a46                          0x7a55                              #K3H    IBM ext.
    0x6a73                          0x7a56                              #K3H    IBM ext.
    0x6a7e                          0x7a57                              #K3H    IBM ext.
    0x6ae2                          0x7a58                              #K3H    IBM ext.
    0x6ae4                          0x7a59                              #K3H    IBM ext.
    0x6bd6                          0x7a5a                              #K3H    IBM ext.
    0x6c3f                          0x7a5b                              #K3H    IBM ext.
    0x6c5c                          0x7a5c                              #K3H    IBM ext.
    0x6c86                          0x7a5d                              #K3H    IBM ext.
    0x6c6f                          0x7a5e                              #K3H    IBM ext.
    0x6cda                          0x7a5f                              #K3H    IBM ext.
    0x6d04                          0x7a60                              #K3H    IBM ext.
    0x6d87                          0x7a61                              #K3H    IBM ext.
    0x6d6f                          0x7a62                              #K3H    IBM ext.
    0x6d96                          0x7a63                              #K3H    IBM ext.
    0x6dac                          0x7a64                              #K3H    IBM ext.
    0x6dcf                          0x7a65                              #K3H    IBM ext.
    0x6df8                          0x7a66                              #K3H    IBM ext.
    0x6df2                          0x7a67                              #K3H    IBM ext.
    0x6dfc                          0x7a68                              #K3H    IBM ext.
    0x6e39                          0x7a69                              #K3H    IBM ext.
    0x6e5c                          0x7a6a                              #K3H    IBM ext.
    0x6e27                          0x7a6b                              #K3H    IBM ext.
    0x6e3c                          0x7a6c                              #K3H    IBM ext.
    0x6ebf                          0x7a6d                              #K3H    IBM ext.
    0x6f88                          0x7a6e                              #K3H    IBM ext.
    0x6fb5                          0x7a6f                              #K3H    IBM ext.
    0x6ff5                          0x7a70                              #K3H    IBM ext.
    0x7005                          0x7a71                              #K3H    IBM ext.
    0x7007                          0x7a72                              #K3H    IBM ext.
    0x7028                          0x7a73                              #K3H    IBM ext.
    0x7085                          0x7a74                              #K3H    IBM ext.
    0x70ab                          0x7a75                              #K3H    IBM ext.
    0x710f                          0x7a76                              #K3H    IBM ext.
    0x7104                          0x7a77                              #K3H    IBM ext.
    0x715c                          0x7a78                              #K3H    IBM ext.
    0x7146                          0x7a79                              #K3H    IBM ext.
    0x7147                          0x7a7a                              #K3H    IBM ext.
    0xfa15                          0x7a7b                              #K3H    IBM ext.
    0x71c1                          0x7a7c                              #K3H    IBM ext.
    0x71fe                          0x7a7d                              #K3H    IBM ext.
    0x72b1                          0x7a7e                              #K3H    IBM ext.
    0x72be                          0x7b21                              #K3H    IBM ext.
    0x7324                          0x7b22                              #K3H    IBM ext.
    0xfa16                          0x7b23                              #K3H    IBM ext.
    0x7377                          0x7b24                              #K3H    IBM ext.
    0x73bd                          0x7b25                              #K3H    IBM ext.
    0x73c9                          0x7b26                              #K3H    IBM ext.
    0x73d6                          0x7b27                              #K3H    IBM ext.
    0x73e3                          0x7b28                              #K3H    IBM ext.
    0x73d2                          0x7b29                              #K3H    IBM ext.
    0x7407                          0x7b2a                              #K3H    IBM ext.
    0x73f5                          0x7b2b                              #K3H    IBM ext.
    0x7426                          0x7b2c                              #K3H    IBM ext.
    0x742a                          0x7b2d                              #K3H    IBM ext.
    0x7429                          0x7b2e                              #K3H    IBM ext.
    0x742e                          0x7b2f                              #K3H    IBM ext.
    0x7462                          0x7b30                              #K3H    IBM ext.
    0x7489                          0x7b31                              #K3H    IBM ext.
    0x749f                          0x7b32                              #K3H    IBM ext.
    0x7501                          0x7b33                              #K3H    IBM ext.
    0x756f                          0x7b34                              #K3H    IBM ext.
    0x7682                          0x7b35                              #K3H    IBM ext.
    0x769c                          0x7b36                              #K3H    IBM ext.
    0x769e                          0x7b37                              #K3H    IBM ext.
    0x769b                          0x7b38                              #K3H    IBM ext.
    0x76a6                          0x7b39                              #K3H    IBM ext.
    0xfa17                          0x7b3a                              #K3H    IBM ext.
    0x7746                          0x7b3b                              #K3H    IBM ext.
    0x52af                          0x7b3c                              #K3H    IBM ext.
    0x7821                          0x7b3d                              #K3H    IBM ext.
    0x784e                          0x7b3e                              #K3H    IBM ext.
    0x7864                          0x7b3f                              #K3H    IBM ext.
    0x787a                          0x7b40                              #K3H    IBM ext.
    0x7930                          0x7b41                              #K3H    IBM ext.
    0xfa18                          0x7b42                              #K3H    IBM ext.
    0xfa19                          0x7b43                              #K3H    IBM ext.
    0xfa1a                          0x7b44                              #K3H    IBM ext.
    0x7994                          0x7b45                              #K3H    IBM ext.
    0xfa1b                          0x7b46                              #K3H    IBM ext.
    0x799b                          0x7b47                              #K3H    IBM ext.
    0x7ad1                          0x7b48                              #K3H    IBM ext.
    0x7ae7                          0x7b49                              #K3H    IBM ext.
    0xfa1c                          0x7b4a                              #K3H    IBM ext.
    0x7aeb                          0x7b4b                              #K3H    IBM ext.
    0x7b9e                          0x7b4c                              #K3H    IBM ext.
    0xfa1d                          0x7b4d                              #K3H    IBM ext.
    0x7d48                          0x7b4e                              #K3H    IBM ext.
    0x7d5c                          0x7b4f                              #K3H    IBM ext.
    0x7db7                          0x7b50                              #K3H    IBM ext.
    0x7da0                          0x7b51                              #K3H    IBM ext.
    0x7dd6                          0x7b52                              #K3H    IBM ext.
    0x7e52                          0x7b53                              #K3H    IBM ext.
    0x7f47                          0x7b54                              #K3H    IBM ext.
    0x7fa1                          0x7b55                              #K3H    IBM ext.
    0xfa1e                          0x7b56                              #K3H    IBM ext.
    0x8301                          0x7b57                              #K3H    IBM ext.
    0x8362                          0x7b58                              #K3H    IBM ext.
    0x837f                          0x7b59                              #K3H    IBM ext.
    0x83c7                          0x7b5a                              #K3H    IBM ext.
    0x83f6                          0x7b5b                              #K3H    IBM ext.
    0x8448                          0x7b5c                              #K3H    IBM ext.
    0x84b4                          0x7b5d                              #K3H    IBM ext.
    0x8553                          0x7b5e                              #K3H    IBM ext.
    0x8559                          0x7b5f                              #K3H    IBM ext.
    0x856b                          0x7b60                              #K3H    IBM ext.
    0xfa1f                          0x7b61                              #K3H    IBM ext.
    0x85b0                          0x7b62                              #K3H    IBM ext.
    0xfa20                          0x7b63                              #K3H    IBM ext.
    0xfa21                          0x7b64                              #K3H    IBM ext.
    0x8807                          0x7b65                              #K3H    IBM ext.
    0x88f5                          0x7b66                              #K3H    IBM ext.
    0x8a12                          0x7b67                              #K3H    IBM ext.
    0x8a37                          0x7b68                              #K3H    IBM ext.
    0x8a79                          0x7b69                              #K3H    IBM ext.
    0x8aa7                          0x7b6a                              #K3H    IBM ext.
    0x8abe                          0x7b6b                              #K3H    IBM ext.
    0x8adf                          0x7b6c                              #K3H    IBM ext.
    0xfa22                          0x7b6d                              #K3H    IBM ext.
    0x8af6                          0x7b6e                              #K3H    IBM ext.
    0x8b53                          0x7b6f                              #K3H    IBM ext.
    0x8b7f                          0x7b70                              #K3H    IBM ext.
    0x8cf0                          0x7b71                              #K3H    IBM ext.
    0x8cf4                          0x7b72                              #K3H    IBM ext.
    0x8d12                          0x7b73                              #K3H    IBM ext.
    0x8d76                          0x7b74                              #K3H    IBM ext.
    0xfa23                          0x7b75                              #K3H    IBM ext.
    0x8ecf                          0x7b76                              #K3H    IBM ext.
    0xfa24                          0x7b77                              #K3H    IBM ext.
    0xfa25                          0x7b78                              #K3H    IBM ext.
    0x9067                          0x7b79                              #K3H    IBM ext.
    0x90de                          0x7b7a                              #K3H    IBM ext.
    0xfa26                          0x7b7b                              #K3H    IBM ext.
    0x9115                          0x7b7c                              #K3H    IBM ext.
    0x9127                          0x7b7d                              #K3H    IBM ext.
    0x91da                          0x7b7e                              #K3H    IBM ext.
    0x91d7                          0x7c21                              #K3H    IBM ext.
    0x91de                          0x7c22                              #K3H    IBM ext.
    0x91ed                          0x7c23                              #K3H    IBM ext.
    0x91ee                          0x7c24                              #K3H    IBM ext.
    0x91e4                          0x7c25                              #K3H    IBM ext.
    0x91e5                          0x7c26                              #K3H    IBM ext.
    0x9206                          0x7c27                              #K3H    IBM ext.
    0x9210                          0x7c28                              #K3H    IBM ext.
    0x920a                          0x7c29                              #K3H    IBM ext.
    0x923a                          0x7c2a                              #K3H    IBM ext.
    0x9240                          0x7c2b                              #K3H    IBM ext.
    0x923c                          0x7c2c                              #K3H    IBM ext.
    0x924e                          0x7c2d                              #K3H    IBM ext.
    0x9259                          0x7c2e                              #K3H    IBM ext.
    0x9251                          0x7c2f                              #K3H    IBM ext.
    0x9239                          0x7c30                              #K3H    IBM ext.
    0x9267                          0x7c31                              #K3H    IBM ext.
    0x92a7                          0x7c32                              #K3H    IBM ext.
    0x9277                          0x7c33                              #K3H    IBM ext.
    0x9278                          0x7c34                              #K3H    IBM ext.
    0x92e7                          0x7c35                              #K3H    IBM ext.
    0x92d7                          0x7c36                              #K3H    IBM ext.
    0x92d9                          0x7c37                              #K3H    IBM ext.
    0x92d0                          0x7c38                              #K3H    IBM ext.
    0xfa27                          0x7c39                              #K3H    IBM ext.
    0x92d5                          0x7c3a                              #K3H    IBM ext.
    0x92e0                          0x7c3b                              #K3H    IBM ext.
    0x92d3                          0x7c3c                              #K3H    IBM ext.
    0x9325                          0x7c3d                              #K3H    IBM ext.
    0x9321                          0x7c3e                              #K3H    IBM ext.
    0x92fb                          0x7c3f                              #K3H    IBM ext.
    0xfa28                          0x7c40                              #K3H    IBM ext.
    0x931e                          0x7c41                              #K3H    IBM ext.
    0x92ff                          0x7c42                              #K3H    IBM ext.
    0x931d                          0x7c43                              #K3H    IBM ext.
    0x9302                          0x7c44                              #K3H    IBM ext.
    0x9370                          0x7c45                              #K3H    IBM ext.
    0x9357                          0x7c46                              #K3H    IBM ext.
    0x93a4                          0x7c47                              #K3H    IBM ext.
    0x93c6                          0x7c48                              #K3H    IBM ext.
    0x93de                          0x7c49                              #K3H    IBM ext.
    0x93f8                          0x7c4a                              #K3H    IBM ext.
    0x9431                          0x7c4b                              #K3H    IBM ext.
    0x9445                          0x7c4c                              #K3H    IBM ext.
    0x9448                          0x7c4d                              #K3H    IBM ext.
    0x9592                          0x7c4e                              #K3H    IBM ext.
    0xf9dc                          0x7c4f                              #K3H    IBM ext.
    0xfa29                          0x7c50                              #K3H    IBM ext.
    0x969d                          0x7c51                              #K3H    IBM ext.
    0x96af                          0x7c52                              #K3H    IBM ext.
    0x9733                          0x7c53                              #K3H    IBM ext.
    0x973b                          0x7c54                              #K3H    IBM ext.
    0x9743                          0x7c55                              #K3H    IBM ext.
    0x974d                          0x7c56                              #K3H    IBM ext.
    0x974f                          0x7c57                              #K3H    IBM ext.
    0x9751                          0x7c58                              #K3H    IBM ext.
    0x9755                          0x7c59                              #K3H    IBM ext.
    0x9857                          0x7c5a                              #K3H    IBM ext.
    0x9865                          0x7c5b                              #K3H    IBM ext.
    0xfa2a                          0x7c5c                              #K3H    IBM ext.
    0xfa2b                          0x7c5d                              #K3H    IBM ext.
    0x9927                          0x7c5e                              #K3H    IBM ext.
    0xfa2c                          0x7c5f                              #K3H    IBM ext.
    0x999e                          0x7c60                              #K3H    IBM ext.
    0x9a4e                          0x7c61                              #K3H    IBM ext.
    0x9ad9                          0x7c62                              #K3H    IBM ext.
    0x9adc                          0x7c63                              #K3H    IBM ext.
    0x9b75                          0x7c64                              #K3H    IBM ext.
    0x9b72                          0x7c65                              #K3H    IBM ext.
    0x9b8f                          0x7c66                              #K3H    IBM ext.
    0x9bb1                          0x7c67                              #K3H    IBM ext.
    0x9bbb                          0x7c68                              #K3H    IBM ext.
    0x9c00                          0x7c69                              #K3H    IBM ext.
    0x9d70                          0x7c6a                              #K3H    IBM ext.
    0x9d6b                          0x7c6b                              #K3H    IBM ext.
    0xfa2d                          0x7c6c                              #K3H    IBM ext.
    0x9e19                          0x7c6d                              #K3H    IBM ext.
    0x9ed1                          0x7c6e                              #K3H    IBM ext.
    0x2170                          0x7c71                              #K3H    IBM ext.
    0x2171                          0x7c72                              #K3H    IBM ext.
    0x2172                          0x7c73                              #K3H    IBM ext.
    0x2173                          0x7c74                              #K3H    IBM ext.
    0x2174                          0x7c75                              #K3H    IBM ext.
    0x2175                          0x7c76                              #K3H    IBM ext.
    0x2176                          0x7c77                              #K3H    IBM ext.
    0x2177                          0x7c78                              #K3H    IBM ext.
    0x2178                          0x7c79                              #K3H    IBM ext.
    0x2179                          0x7c7a                              #K3H    IBM ext.
    0xffe2                          0x7c7b                              #K3H    IBM ext.
    0xffe4                          0x7c7c                              #K3H    IBM ext.
    0xff07                          0x7c7d                              #K3H    IBM ext.
    0xff02                          0x7c7e                              #K3H    IBM ext.

    0x3231                          0x2D6A                              #K3H    Symbol(KABUSHIKI KAISYA)
    0x2116                          0x2D62                              #K3H    Symbol(NO)
    0x2121                          0x2D64                              #K3H    SYMBOL OF TEL

    0x2160                          0x2D35                              #K3H    ROMAN DIGIT ONE
    0x2161                          0x2D36                              #K3H    ROMAN DIGIT TWO
    0x2162                          0x2D37                              #K3H    ROMAN DIGIT THREE
    0x2163                          0x2D38                              #K3H    ROMAN DIGIT FOUR
    0x2164                          0x2D39                              #K3H    ROMAN DIGIT FIVE
    0x2165                          0x2D3a                              #K3H    ROMAN DIGIT SIX
    0x2166                          0x2D3b                              #K3H    ROMAN DIGIT SEVEN
    0x2167                          0x2D3c                              #K3H    ROMAN DIGIT EIGHT
    0x2168                          0x2D3d                              #K3H    ROMAN DIGIT NINES
    0x2169                          0x2D3e                              #K3H    ROMAN DIGIT TEN
EndUnicodeToForeignData

