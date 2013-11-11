// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "UnitTestInfo.h"


CUnitTestInfo::CUnitTestInfo()
: CBase()
	{
	}


CUnitTestInfo::~CUnitTestInfo()
	{
	delete iUnitTestId;
	}


CUnitTestInfo* CUnitTestInfo::NewLC(const TDesC& aSetId)
	{
	CUnitTestInfo* self = new (ELeave) CUnitTestInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aSetId);
	return self;
	}


CUnitTestInfo* CUnitTestInfo::NewL(const TDesC& aSetId)
	{
	CUnitTestInfo* self = NewLC(aSetId);
	CleanupStack::Pop();
	return self;
	}


void CUnitTestInfo::ConstructL(const TDesC& aUnitTestId)
	{
	iUnitTestId = aUnitTestId.AllocL();
	}

