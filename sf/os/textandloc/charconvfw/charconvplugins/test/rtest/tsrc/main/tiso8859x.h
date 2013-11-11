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


#if !defined(__TISO8859X_H__)
#define __TISO8859X_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

struct STstIso8859TestDataItem
	{
	TInt iX;
	TInt iCharacterSetIdentifier;
	TInt iTextLength;
	const TUint16* iUnicode;
	const TUint8* iIso8859;
	};

struct STstIso8859TestData
	{
	TInt iNumberOfItems;
	const STstIso8859TestDataItem* iItems;
	};

GLREF_D const STstIso8859TestData iso8859TestData;

#endif

