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
* Contains the declaration of  CMemSpyProcess class 
*/

#ifndef MEMSPYAPIPROCESS_H
#define MEMSPYAPIPROCESS_H

// System includes
#include <e32base.h>

class TMemSpyProcessData;

NONSHARABLE_CLASS( CMemSpyApiProcess ) 
	{
public:
	IMPORT_C ~CMemSpyApiProcess();
	
	static CMemSpyApiProcess* NewL(const TMemSpyProcessData& aData);

	static CMemSpyApiProcess* NewLC(const TMemSpyProcessData& aData);
	
public:
	IMPORT_C TProcessId Id() const;
	
	IMPORT_C const TDesC& Name() const;	
	
	IMPORT_C TExitCategoryName ExitCategory() const;
	
	IMPORT_C TInt ExitReason() const;
	
	IMPORT_C TExitType ExitType() const;
	
	IMPORT_C TInt ThreadCount() const;
	
	IMPORT_C TUint32 SID() const;
	
	IMPORT_C TUint32 VID() const;
	
	IMPORT_C TProcessPriority Priority() const;
	
	IMPORT_C TBool IsDead() const;
	
	
private:
	CMemSpyApiProcess();
	
	void ConstructL(const TMemSpyProcessData& aData);
		
private:
	TMemSpyProcessData *iProcessData;
};

	
#endif // MEMSPYAPIPROCESS_H
