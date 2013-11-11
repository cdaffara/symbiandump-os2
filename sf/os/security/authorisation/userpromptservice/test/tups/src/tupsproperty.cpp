/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include <e32base.h>
#include <bacline.h>
#include <f32file.h>
#include <e32property.h>
#include <numberconversion.h>
#include "tupsproperty.h"


CUpsProperty* CUpsProperty::NewL(const TUid& aCategory)
	{
	CUpsProperty* self = new(ELeave) CUpsProperty(aCategory);
	return self;
	}

CUpsProperty::CUpsProperty(const TUid& aCategory)
	: iCategory(aCategory)
	{
	
	}

void CUpsProperty::GetL(TInt aInstanceNumber, TInt aPropertyKey, TType aType, TInt& aValue)
	{
	TInt key;
	if (aType == EPolicyEvaluator)
		{
		key = GetPropertyKey(aPropertyKey, KPe_Start, KInstanceInterval, aInstanceNumber);
		}
	else
		{
		key = GetPropertyKey(aPropertyKey, KDc_Start, KInstanceInterval, aInstanceNumber);
		}
	User::LeaveIfError(RProperty::Get(iCategory, key, aValue));
	}

void CUpsProperty::GetL(TInt aInstanceNumber, TInt aPropertyKey, TType aType, TDes& aValue)
	{
	TInt key;
	if (aType == EPolicyEvaluator)
		{
		key = GetPropertyKey(aPropertyKey, KPe_Start, KInstanceInterval, aInstanceNumber);
		}
	else
		{
		key = GetPropertyKey(aPropertyKey, KDc_Start, KInstanceInterval, aInstanceNumber);
		}
	User::LeaveIfError(RProperty::Get(iCategory, key, aValue));
	}

void CUpsProperty::SetL(TInt aInstanceNumber, TInt aPropertyKey, TType aType, TInt aValue)
	{
	TInt key;
	if (aType == EPolicyEvaluator)
		{
		key = GetPropertyKey(aPropertyKey, KPe_Start, KInstanceInterval, aInstanceNumber);
		}
	else
		{
		key = GetPropertyKey(aPropertyKey, KDc_Start, KInstanceInterval, aInstanceNumber);
		}
	User::LeaveIfError(RProperty::Set(iCategory, key, aValue));
	}

void CUpsProperty::SetL(TInt aInstanceNumber, TInt aPropertyKey, TType aType, TDes& aValue)
	{
	TInt key;
	if (aType == EPolicyEvaluator)
		{
		key = GetPropertyKey(aPropertyKey, KPe_Start, KInstanceInterval, aInstanceNumber);
		}
	else
		{
		key = GetPropertyKey(aPropertyKey, KDc_Start, KInstanceInterval, aInstanceNumber);
		}
	User::LeaveIfError(RProperty::Set(iCategory, key, aValue));
	}

TInt CUpsProperty::GetPropertyKey(TInt aPropertyKey, TInt aStart, TInt aInterval, TInt aInstanceNumber)
	{
	TInt actualKey = ((aInstanceNumber - 1) * aInterval) + aPropertyKey + aStart;
	return actualKey;
	}
