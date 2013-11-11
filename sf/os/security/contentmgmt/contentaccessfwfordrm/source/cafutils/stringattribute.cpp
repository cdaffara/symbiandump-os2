/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "stringAttribute.h"


using namespace ContentAccess;

CStringAttribute* CStringAttribute::NewL(TInt aAttribute, const TDesC& aValue, TInt aError)
	{
	CStringAttribute* self = new (ELeave) CStringAttribute(aAttribute, aError);
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	CleanupStack::Pop(self);
	return self;
	}

CStringAttribute::CStringAttribute(TInt aAttribute, TInt aError) : iAttribute(aAttribute), iError(aError)
	{
	}

CStringAttribute::~CStringAttribute()
	{
	delete iValue;
	}

TInt CStringAttribute::Attribute() const
	{
	return iAttribute;
	}

const TDesC& CStringAttribute::Value() const
	{
	return *iValue;
	}

TInt CStringAttribute::Error() const
	{
	return iError;
	}

void CStringAttribute::ConstructL(const TDesC& aValue)
	{
	iValue = aValue.AllocL();
	}
