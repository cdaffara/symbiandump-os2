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

#ifndef __LOGSERVCACHECONFIG_H__
#define __LOGSERVCACHECONFIG_H__

#include <logcli.h>

// Classes referenced
class MLogServDatabaseTransactionInterface;

/**
Caches a copy of the LogEng configuration data (a TLogConfig instance).
Provides public methods for retrieving/updating the configuration data.
if the configuration data is updated in a transaction, the changes will be committed/rolled back
when the transaction is committed/rolled back.  

Note: the LogEng configuration is kept in a database table with name "Config".

@see TLogConfig
@see MLogServDatabaseTransactionInterface
@internalComponent
*/
class CLogServCacheConfig : public CBase
	{
public:
	static CLogServCacheConfig* NewL(MLogServDatabaseTransactionInterface& aDatabase);
	~CLogServCacheConfig();
	const TLogConfig& Config() const;
	void UpdateL(const TLogConfig& aConfig);
#ifdef _DEBUG    
	inline TBool InTransaction() const;
#endif   
	void Commit();
	inline void Rollback();

private:
	CLogServCacheConfig(MLogServDatabaseTransactionInterface& aDatabase);
	void ConstructL();
	void ReadL();

private:
	MLogServDatabaseTransactionInterface& iDatabase;
	TLogConfig  iConfig;//The last good configuration value. Essentially, the current configuration
	TLogConfig  iRequestedConfiguration;//The requested configuration
	TBool       iInTransaction;//Indicates whether the configuration cache is currently within an update transaction
	
	};

#ifdef _DEBUG    
/**
Indicates whether a configuration transaction has begun
*/
inline TBool CLogServCacheConfig::InTransaction() const
    {
    return iInTransaction;
    }
#endif//_DEBUG

/**
Roll back a configuration transaction
*/
inline void CLogServCacheConfig::Rollback()
    {
    // Nothing really to do here other than discard the transaction status
    // (ready to start over again).
    iInTransaction = EFalse;
    }

#endif
