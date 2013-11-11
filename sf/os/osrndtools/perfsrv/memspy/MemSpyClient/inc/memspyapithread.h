/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Contains the declaration of  CMemSpyProcessWrapper class and TMemSpyProcess data class
*/

#ifndef MEMSPYAPITHREAD_H
#define MEMSPYAPITHREAD_H

// System includes
#include <e32base.h>

class TMemSpyThreadData;

NONSHARABLE_CLASS( CMemSpyApiThread ) {

public:
	IMPORT_C ~CMemSpyApiThread();
	
	static CMemSpyApiThread* NewL(const TMemSpyThreadData& aData);

	static CMemSpyApiThread* NewLC(const TMemSpyThreadData& aData);

public:
	IMPORT_C TThreadId Id() const;
	IMPORT_C TProcessId ProcessId() const;
	
	IMPORT_C const TDesC& Name() const;
		
	IMPORT_C TInt SID() const;
	
	IMPORT_C TInt VID() const;
	
	IMPORT_C TThreadPriority ThreadPriority() const;
	
	IMPORT_C TProcessPriority ProcessPriority() const;
	
	IMPORT_C TInt RequestCount() const;
	
	IMPORT_C TInt ThreadHandles() const;
	
	IMPORT_C TInt ProcessHandles() const;
	
	IMPORT_C TInt ThreadNumberUsing() const;
	
	IMPORT_C TInt ProcessNumberUsing() const;
	
	IMPORT_C TInt Attributes() const;
	
	IMPORT_C TInt CpuUse() const;
	
	IMPORT_C TExitType ExitType() const;
	
private:
	CMemSpyApiThread();	
	
	void ConstructL(const TMemSpyThreadData& aData);
private:
	TMemSpyThreadData* iThreadData;
};

#endif
