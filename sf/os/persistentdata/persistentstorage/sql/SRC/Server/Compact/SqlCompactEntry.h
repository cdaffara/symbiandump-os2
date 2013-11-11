// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SQLCOMPACTENTRY_H
#define SQLCOMPACTENTRY_H

#include "SqlCompact.h"

//Forward declarations
class CSqlCompactTestActive;
class CSqlCompactTimer;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define SQLCOMPACTENTRY_INVARIANT() Invariant()
#else
#define SQLCOMPACTENTRY_INVARIANT() void(0)
#endif

/**
Every CSqlCompactEntry maintains a connection to the database that has to be compacted in background mode.
When the number of the free pages reaches or is above the threshold, the background compaction will be activated
and performed in steps.

@see MSqlCompactConn
@see CSqlCompactTimer

@internalComponent
*/
NONSHARABLE_CLASS(CSqlCompactEntry) : public CBase
	{
	friend class CSqlCompactTestActive;
	
public:
	static CSqlCompactEntry* NewLC(const TDesC& aFullName, TSqlCompactConnFactoryL aConnFactoryL, 
								   const TSqlCompactSettings& aSettings, CSqlCompactTimer& aTimer);
	TInt AddRef();
	TInt Release();
	TInt Compact();
	const TDesC& FullName() const;
	void Invariant() const;
	
private:
	CSqlCompactEntry(const TSqlCompactSettings& aSettings, CSqlCompactTimer& aTimer);
	virtual ~CSqlCompactEntry();
	void ConstructL(const TDesC& aFullName, TSqlCompactConnFactoryL aConnFactoryL);
	void ResetState();
	MSqlCompactConn& Connection();
	static void FreePageCallback(void* aThis, TInt aFreePageCount);

public:
    TSglQueLink			iLink;       
    
private:
	RBuf				iFullName;
	TSqlCompactSettings	iSettings;
	CSqlCompactTimer& 	iTimer;
	MSqlCompactConn*	iConnection;
	TInt				iRefCounter;
	enum TState 
		{
		EInactive,
		EInProgress
		};
	TState				iState;
	TInt				iPageCount;
	TBool				iFreePageCallbackDisabled;

	};

#endif//SQLCOMPACTENTRY_H
