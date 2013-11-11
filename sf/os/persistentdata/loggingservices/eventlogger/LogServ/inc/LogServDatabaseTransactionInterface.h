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

#ifndef __LOGSERVDATABASETRANSACTIONINTERFACE_H__
#define __LOGSERVDATABASETRANSACTIONINTERFACE_H__

#include <d32dbms.h>
#include "LogServShared.h"
#include "logservsecurity.h"

// Classes referenced
class CLogServCacheTypes;
class CLogServCacheConfig;
class CLogServCacheStrings;
class CLogServResourceInterpreter;
class MLogServDatabaseChangeInterface;

/**
Interface class offering methods for:
 - beginning, committing and rolling back a transaction;
 - accessors to config, types and strings caches;
 - accessors to the resource interpreter and and the change interface;
 
@see CLogServResourceInterpreter  
@see MLogServDatabaseChangeInterface
@see CLogServCacheStrings
@see CLogServCacheTypes
@see CLogServCacheConfig
@internalComponent
*/
class MLogServDatabaseTransactionInterface
	{
public:
	virtual TInt DTIBegin() = 0;//Start a new transaction
	virtual TInt DTICommitAndEnd() = 0;//End an existing transaction
	virtual void DTIRollBack() = 0;//Roll back the current transaction
	virtual TInt DTIExecuteSql(const TDesC& aStatement, TDbTextComparison aComparison = EDbCompareNormal) = 0;//Run some arbitrary SQL
	virtual TBool DTIInTransaction() const = 0;//Is the database in an existing transaction
	virtual TBool DTIDatabaseIsDamaged() const = 0;//Is the database in an existing transaction

	virtual CLogServResourceInterpreter& DTIResourceInterface() const = 0;//Access the resource interface
	virtual MLogServDatabaseChangeInterface& DTIChangeInterface() const = 0;//Acess the change submission interface
	virtual CLogServCacheStrings& DTICacheStrings() const = 0;//Access the string table cache
	virtual CLogServCacheTypes& DTICacheTypes() const = 0;//Access the type table cache
	virtual CLogServCacheConfig& DTICacheConfig() const = 0;//Access the config table cache
	virtual RDbDatabase& DTIDatabase() = 0;//Access the database
	
	virtual TBool DTIIsAllowed(TEventOp aEventOp, const RMessage2& aMessage, TUid aEventType, const char* aDiagnostic = 0) const = 0;//Check client capability
	virtual const RArray<TUid>& DTIUidsOfStandardTypes() = 0;//Returns a collection of built-in TUids of built-types - those defined in Logwrap.rss

	void DTIBeginWithRollBackProtectionLC();//Start a transaction with roll-back protection (cleanup)
	void DTICommitAndCancelRollbackProtectionL();//End a transaction previously started with roll-back protection

private:
	static void RollBackDatabase(TAny* aObject);

	};

#endif
