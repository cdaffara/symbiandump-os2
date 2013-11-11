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

#ifndef __LOGREC_H__
#define __LOGREC_H__

#include <d32dbms.h>
#include <logwrap.h>

// Classes referenced
class MLogServDatabaseTransactionInterface;

/**
Performs LogEng database recovery.  
 
@see MLogServDatabaseTransactionInterface 
@internalComponent
*/
class CLogRecover : public CLogActive
	{
public:
	CLogRecover(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority);
	TBool Start(TRequestStatus& aStatus);
	//
private:
	void DoRunL();
	void DoComplete(TInt& aStatus);
	//
private:
	MLogServDatabaseTransactionInterface& iDatabase;
	TPckgBuf<TInt> iStep;
	//
	RDbIncremental iOperation;
	};

#endif
