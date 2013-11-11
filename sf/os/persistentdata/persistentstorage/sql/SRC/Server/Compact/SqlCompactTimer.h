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

#ifndef SQLCOMPACTTIMER_H
#define SQLCOMPACTTIMER_H

#include <e32base.h>

//Forward declarations
class CSqlCompactTestActive;
class CSqlCompactEntry;

#ifdef _DEBUG
#define SQLCOMPACTTIMER_INVARIANT() Invariant()
#else
#define SQLCOMPACTTIMER_INVARIANT() void(0)
#endif

/**
A CTimer derived class that performs the background compaction.
The CSqlCompactTimer class maintains a queue of CSqlCompactEntry objects waiting to be compacted
(the databases).
The class offers methods for adding/removing CSqlCompactEntry objects to/from the queue and a Restart()
method that can be used to delay the next compaction step, improving this way the SQL server responsiveness to 
client requests.

The CSqlCompactEntry objects needing compaction will be added at the front of the queue.
Every time when timer's RunL() method gets executed, the last element from the queue will be picked-up and one
compaction step will be performed. When the CSqlCompactEntry object completes the compaction, it will remove
itself from the queue.

@see CSqlCompactEntry

@internalComponent
*/
NONSHARABLE_CLASS(CSqlCompactTimer) : protected CTimer
	{
	friend class CSqlCompactTestActive;
	
public:
	static CSqlCompactTimer* NewL(TInt aIntervalMs);
	virtual ~CSqlCompactTimer();
	void Restart();
	void Queue(CSqlCompactEntry& aEntry);
	void DeQueue(CSqlCompactEntry& aEntry);
	void Invariant() const;
	
private:
	CSqlCompactTimer(TInt aIntervalMs);
	void ConstructL();
	virtual void RunL();

private:
	TInt						iIntervalMicroSec;
	TSglQue<CSqlCompactEntry>	iQueue;
	
	};

#endif//SQLCOMPACTTIMER_H
