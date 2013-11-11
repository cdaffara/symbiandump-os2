// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Purpose: Kernel-side tracking of process state
// 
//
 
#ifndef D_TARGET_PROCESS_H
#define D_TARGET_PROCESS_H

#include "d_debug_agent.h"

// Debug Process Tracker class
class DTargetProcess : public DBase
{
public:
	DTargetProcess();
	~DTargetProcess();

	static TInt Compare(const DTargetProcess& aFirst, const DTargetProcess& aSecond);

	TInt SetProcessName(const TDesC8& aProcessName);
	const TDesC8& ProcessName() const;

	TInt AddAgent(const TUint64 aAgentId);

	TInt RemoveAgent(TUint64 aAgentId);

	DDebugAgent* operator[](TInt aIndex);

	DDebugAgent* Agent(TUint64 aAgentId);

	TInt AgentCount() const;
	void NotifyEvent(const TDriverEventInfo& aEventInfo);

private:
	HBuf8* iProcessName;
	RPointerArray<DDebugAgent> iAgentList;

	RArray<TUint64> iSuspendedThreads;

	RPointerArray<NFastSemaphore> iFrozenThreadSemaphores;

};

#endif // D_TARGET_PROCESS_H

