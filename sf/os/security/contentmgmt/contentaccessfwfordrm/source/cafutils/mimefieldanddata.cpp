/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef REMOVE_CAF1

#include <caf/caftypes.h>
#include <e32base.h>
#include "mimefieldanddata.h"

using namespace ContentAccess;

CMimeFieldAndData* CMimeFieldAndData::NewL(const TDesC8 &aFieldName, const TDesC8 &aData)
	{
	CMimeFieldAndData* self = new (ELeave) CMimeFieldAndData();
	CleanupStack::PushL(self);
	self->ConstructL(aFieldName, aData);
	CleanupStack::Pop(self);
	return self;
	}

void CMimeFieldAndData::ConstructL(const TDesC8 &aFieldName, const TDesC8 &aData)
	{
	iFieldName = aFieldName.AllocL();
	iData = aData.AllocL();
	}	

CMimeFieldAndData::CMimeFieldAndData()
	{
	}

CMimeFieldAndData::~CMimeFieldAndData()
	{
	delete iFieldName;
	delete iData;
	}

TPtrC8 CMimeFieldAndData::FieldName() const
	{
	return *iFieldName;
	}

TPtrC8 CMimeFieldAndData::Data() const
	{
	return *iData;
	}
#endif // #ifndef REMOVE_CAF1
