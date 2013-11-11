// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DBMS server proxy incremental object
// 
//

#include "SD_STD.H"

// Class CDbsIncremental
CDbsIncremental* CDbsIncremental::NewL(const RDbsObject& aDbs,TDbsFunction aFunction,TIpcArgs& aArgs,TInt& aStep)
	{
	TPckg<TInt> pckg(aStep);
	aArgs.Set(3,&pckg);
	CDbsIncremental* self=new(ELeave) CDbsIncremental;
	CleanupStack::PushL(self);
	self->iObject.OpenL(aDbs,aFunction,aArgs);
	if (self->iObject.Handle()==0)
		{	// no incremental object was required, so remove this one
		CleanupStack::PopAndDestroy();	// self
		return 0;
		}
	CleanupStack::Pop();	// self
	return self;
	}

CDbsIncremental::~CDbsIncremental()
	{
	iObject.Close();
	}

void CDbsIncremental::Next(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus)
//
// Asyncronous next step
//
	{
	iObject.SendReceive(EDbsIncrementalNext,TIpcArgs(aStep(),&aStep),aStatus);
	}
