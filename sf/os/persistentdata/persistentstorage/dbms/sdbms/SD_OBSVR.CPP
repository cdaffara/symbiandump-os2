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
// DBMS server proxy observer object
// 
//

#include "SD_STD.H"

// Class CDbsNotifier

CDbsNotifier* CDbsNotifier::NewL(const RDbsObject& aDatabase)
	{
	CDbsNotifier* self=new(ELeave) CDbsNotifier;
	CleanupStack::PushL(self);
	self->iObject.OpenL(aDatabase,EDbsDatabaseOpenObserver);
	CleanupStack::Pop();
	return self;
	}

CDbsNotifier::~CDbsNotifier()
	{
	iObject.Close();
	}

void CDbsNotifier::Notify(CDbNotifier::TType aType,TRequestStatus& aStatus)
	{
	iObject.SendReceive(EDbsObserverNotify,TIpcArgs(aType),aStatus);
	}

void CDbsNotifier::Cancel()
	{
	iObject.SendReceive(EDbsObserverCancel);
	}

