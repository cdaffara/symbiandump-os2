/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "jisx0201.h"

struct SCnvConversionData;

GLREF_D const SCnvConversionData jisRomanConversionData;
GLREF_D const SCnvConversionData halfWidthKatakana8ConversionData;

EXPORT_C const SCnvConversionData& CnvJisX0201::ConversionData()
	{
	return conversionData;
	}

EXPORT_C const SCnvConversionData& CnvJisRoman::ConversionData()
	{
	return jisRomanConversionData;
	}

EXPORT_C const SCnvConversionData& CnvHalfWidthKatakana8::ConversionData()
	{
	return halfWidthKatakana8ConversionData;
	}

