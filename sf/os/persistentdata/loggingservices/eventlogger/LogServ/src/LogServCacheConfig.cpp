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

#include "LogServCacheConfig.h"
#include "LOGQUERY.H"
#include "logservpanic.h"
#include "LogServDatabaseTransactionInterface.h"

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServCacheConfig (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServCacheConfig::CLogServCacheConfig(MLogServDatabaseTransactionInterface& aDatabase)
:	iDatabase(aDatabase)
	{
	}

CLogServCacheConfig::~CLogServCacheConfig()
	{
	}

void CLogServCacheConfig::ConstructL()
	{
	ReadL();
	}

CLogServCacheConfig* CLogServCacheConfig::NewL(MLogServDatabaseTransactionInterface& aDatabase)
	{
	CLogServCacheConfig* self = new(ELeave) CLogServCacheConfig(aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Get the current configuration
*/
const TLogConfig& CLogServCacheConfig::Config() const
	{
	return iConfig;
	}

/**
Update the config table
*/
void CLogServCacheConfig::UpdateL(const TLogConfig& aConfig)
	{
    RLogConfigDbTable tbl;
    tbl.OpenLC(iDatabase.DTIDatabase());
    if(!tbl.FirstL())
        {
        User::Leave(KErrNotFound);
        }
    tbl.UpdateL();
    tbl.SetColL(RLogConfigDbTable::iAgeColNo, aConfig.iMaxEventAge);
    tbl.SetColL(RLogConfigDbTable::iSizeColNo, aConfig.iMaxLogSize);
    tbl.SetColL(RLogConfigDbTable::iRecentColNo, aConfig.iMaxRecentLogSize);
    tbl.PutL();
    CleanupStack::PopAndDestroy(&tbl);
    iRequestedConfiguration = aConfig;
    iInTransaction = ETrue;
	}

/**
Commit the requested configuration
*/
void CLogServCacheConfig::Commit()
    {
    __ASSERT_ALWAYS(iInTransaction, Panic(ELogCacheConfigNotInTransaction));
    iConfig = iRequestedConfiguration;
    iInTransaction = EFalse;
    }

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServCacheConfig::ReadL()
	{
	RLogConfigDbTable tbl;
	tbl.OpenLC(iDatabase.DTIDatabase(), RDbRowSet::EReadOnly);
	if(!tbl.FirstL())
		{
		User::Leave(KErrNotFound);
		}
	__ASSERT_ALWAYS(tbl.CountL() == 1, Panic(ELogTooManyRows4));
	tbl.GetL();
	iConfig.iMaxEventAge = tbl.ColUint32(RLogConfigDbTable::iAgeColNo);
	iConfig.iMaxLogSize = tbl.ColUint16(RLogConfigDbTable::iSizeColNo);
	iConfig.iMaxRecentLogSize = tbl.ColUint8(RLogConfigDbTable::iRecentColNo);
	CleanupStack::PopAndDestroy(&tbl);
	}
