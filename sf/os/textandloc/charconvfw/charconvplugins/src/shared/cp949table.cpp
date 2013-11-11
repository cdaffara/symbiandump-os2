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
* Description:   
*
*/



#include <e32std.h>
#include <convgeneratedcpp.h>
#include "cp949table.h"

struct SCnvConversionData;

EXPORT_C const TDesC8& CnvCp949Table::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return ReplacementForUnconvertibleUnicodeCharacters_internal();
	}

EXPORT_C const SCnvConversionData& CnvCp949Table::ConversionData()
	{
	return conversionData;
	}

