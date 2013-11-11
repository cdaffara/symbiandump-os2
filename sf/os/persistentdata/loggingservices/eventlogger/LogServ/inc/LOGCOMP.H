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

#ifndef __LOGCOMP_H__
#define __LOGCOMP_H__

#include <d32dbms.h>
#include <logwrap.h>

// Classes referenced
class MLogServDatabaseTransactionInterface;


/**
Performs incremental compaction on the LogEng database.
CLogMaintenance owns a CLogCompact instance.  

@see CLogMaintenance 
@see MLogServDatabaseTransactionInterface
@internalComponent
*/
class CLogCompact : public CLogActive
	{
public:
	CLogCompact(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	void Start(TRequestStatus& aStatus);
	//
private:
	void DoRunL();
	void DoComplete(TInt& aStatus);
	TBool CompactRequired() const;
	//
private:
	MLogServDatabaseTransactionInterface& iDatabase;
	TPckgBuf<TInt> iStep;
	TBool iCompact;
	//
	RDbIncremental iOperation;
	};

#endif
