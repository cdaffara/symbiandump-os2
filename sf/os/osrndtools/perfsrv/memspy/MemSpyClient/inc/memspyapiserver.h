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

#ifndef MEMSPYAPISERVER_H
#define MEMSPYAPISERVER_H

// System includes
#include <e32base.h>

class TMemSpyServerData;

NONSHARABLE_CLASS( CMemSpyApiServer ) 
	{
public:
	IMPORT_C ~CMemSpyApiServer();
	
	static CMemSpyApiServer* NewL(const TMemSpyServerData& aData);

	static CMemSpyApiServer* NewLC(const TMemSpyServerData& aData);
	
public:
	IMPORT_C TProcessId ProcessId() const;
	
	IMPORT_C TThreadId ThreadId() const;
	
	IMPORT_C const TDesC& Name() const;	
	
	IMPORT_C TInt SessionCount() const;
	
private:
	CMemSpyApiServer();
	
	void ConstructL(const TMemSpyServerData& aData);
		
private:
	TMemSpyServerData *iData;
};

	
#endif // MEMSPYAPISERVER_H
