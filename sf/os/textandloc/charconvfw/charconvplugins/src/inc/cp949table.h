/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   CP949 character conversion table
*
*/


#if !defined(__CP949TABLE_H__)
#define __CP949TABLE_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__CHARCONV_H__)
#include <charconv.h>
#endif

class CnvCp949Table 
    {
public:
    IMPORT_C static const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();
    IMPORT_C static const SCnvConversionData& ConversionData();
    };

#endif

