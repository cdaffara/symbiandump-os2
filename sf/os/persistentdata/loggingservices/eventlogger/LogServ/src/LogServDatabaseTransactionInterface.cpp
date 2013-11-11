// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LogServDatabaseTransactionInterface.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> MLogServDatabaseTransactionInterface (source)
/////////////////////////////////////////////////////////////////////////////////////////

void MLogServDatabaseTransactionInterface::DTIBeginWithRollBackProtectionLC()
	{
	const TInt error = DTIBegin();
	User::LeaveIfError(error);
	CleanupStack::PushL(TCleanupItem(RollBackDatabase, this));
	}
	
void MLogServDatabaseTransactionInterface::DTICommitAndCancelRollbackProtectionL()
	{
	const TInt error = DTICommitAndEnd();
	User::LeaveIfError(error);
	CleanupStack::Pop(); // Roll back item
	}

void MLogServDatabaseTransactionInterface::RollBackDatabase(TAny* aObject)
	{
	MLogServDatabaseTransactionInterface* self = reinterpret_cast<MLogServDatabaseTransactionInterface*>(aObject);
	self->DTIRollBack();
	}
