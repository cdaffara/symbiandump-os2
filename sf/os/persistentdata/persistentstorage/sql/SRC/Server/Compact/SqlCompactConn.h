// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SQLCOMPACTCONN_H
#define SQLCOMPACTCONN_H

#include "SqlCompact.h"

//Forward declarations
struct sqlite3;
struct TSqlFreePageCallback;

#ifdef _DEBUG
#define SQLCOMPACTCONN_INVARIANT() Invariant()
#else
#define SQLCOMPACTCONN_INVARIANT() void(0)
#endif

/**
An implementation class for the MSqlCompactConn interface.
Establishes a connection with the database.
The established connection can be used for compacting the database.

@see MSqlCompactConn

@internalComponent
*/
NONSHARABLE_CLASS(CSqlCompactConn) : public CBase, public MSqlCompactConn
	{
	friend class CSqlCompactTestActive;
	
public:
	static CSqlCompactConn* NewL(const TDesC& aFullName, TSqlFreePageCallback& aFreePageCallback);
	virtual ~CSqlCompactConn();
	virtual void Release();
	virtual TInt Compact(TInt aPageCount, TInt& aProcessedPageCount, TInt aLength);
	void Invariant() const;

private:
	void ConstructL(const TDesC& aFullName, TSqlFreePageCallback& aFreePageCallback);
	TInt FreePageCountL();
	TInt PageSizeL();

private:
	sqlite3* iHandle;

	};

/**
A factory function for the MSqlCompactConn interface.

@see MSqlCompactConn

@internalComponent
*/
MSqlCompactConn* SqlCreateCompactConnL(const TDesC& aFullName, TSqlFreePageCallback& aFreePageCallback);

#endif//SQLCOMPACTCONN_H
