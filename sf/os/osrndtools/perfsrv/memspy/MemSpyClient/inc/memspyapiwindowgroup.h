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

#ifndef MEMSPYAPIWINDOWGROUP_H
#define MEMSPYAPIWINDOWGROUP_H

// System includes
#include <e32base.h>
#include <f32file.h>

class TMemSpyEngineWindowGroupDetails;

NONSHARABLE_CLASS( CMemSpyApiWindowGroup ) : public CBase
	{
public:
	IMPORT_C ~CMemSpyApiWindowGroup();
	
	static CMemSpyApiWindowGroup* NewL(const TMemSpyEngineWindowGroupDetails& aData);

	static CMemSpyApiWindowGroup* NewLC(const TMemSpyEngineWindowGroupDetails& aData);
	
public:
	IMPORT_C TInt Id() const;
	IMPORT_C const TDesC& Name() const;
	IMPORT_C const TDesC& FullName() const;
	IMPORT_C TThreadId ThreadId() const;
	IMPORT_C TInt OrdinalPosition() const;
	IMPORT_C TInt Priority() const;
	IMPORT_C TInt WindowGroupHandle() const;
	IMPORT_C TUid Uid() const;
	IMPORT_C TBool IsBusy() const;
	IMPORT_C TBool IsSystem() const;
	IMPORT_C TBool IsHidden() const;
    IMPORT_C const TDesC& Caption() const;
    IMPORT_C const TDesC& DocName() const;
    IMPORT_C TBool IsFocused() const;
	
private:
	void ConstructL(const TMemSpyEngineWindowGroupDetails& aData);
		
private:
	TMemSpyEngineWindowGroupDetails *iData;
};

#endif // MEMSPYAPIWINDOWGROUP_H
