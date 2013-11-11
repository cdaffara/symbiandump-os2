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
// Purpose: The DProcessTracker object tracks which processes are being
// debugged. The DProcessTracker class uses a DTargetProcess object for
// each process being debugged.
// Note: Although TheDProcessTracker object is a global, it should be unique
// as only the Debug Security Server should load and use this driver.
//
//

#include <e32def.h>
#include <e32def_private.h>
#include <e32cmn.h>
#include <e32cmn_private.h>
#include <kernel/kernel.h>
#include <kernel/kern_priv.h>
#include "nk_priv.h"
#include <rm_debug_api.h>

#include "d_target_process.h"
#include "debug_logging.h"
#include "debug_utils.h"

// ctor
DTargetProcess::DTargetProcess()
	:iProcessName(NULL)
	{
	}

// dtor
DTargetProcess::~DTargetProcess()
	{
	delete iProcessName;
	iAgentList.ResetAndDestroy();
	}

// Compare two DTargetProcess items. They are the same if they have the same name.
TInt DTargetProcess::Compare(const DTargetProcess& aFirst, const DTargetProcess& aSecond)
	{
	const TDesC& left = aFirst.iProcessName ? *aFirst.iProcessName : KNullDesC();
	const TDesC& right = aSecond.iProcessName ? *aSecond.iProcessName : KNullDesC();
	return left.Compare(right);
	}

// Set the name of the process we are tracking
TInt DTargetProcess::SetProcessName(const TDesC8& aProcessName)
	{
	// Argument checking
	if (aProcessName.Length() < 1)
		{
		return KErrArgument;
		}

	if (iProcessName) 
		return KErrNotReady; // You can only set the processname once
	iProcessName = HBuf8::New(aProcessName);
	if (!iProcessName) 
		return KErrNoMemory;
	return KErrNone;
	}

// Obtain the name of the process being tracked
const TDesC& DTargetProcess::ProcessName() const
	{
	return iProcessName ? *iProcessName : KNullDesC();
	}

// Returns a pointer to the DDebugAgent with aAgentId.
// If the agent is not in the list, it returns NULL.
DDebugAgent* DTargetProcess::Agent(TUint64 aAgentId)
	{
	for(TInt i = 0; i < iAgentList.Count(); i++)
		{
		if (iAgentList[i]->Id() == aAgentId)
			{
			return iAgentList[i];
			}
		}

	// what do we return if we don't have any agents?
	return NULL;
	}

// Adds aAgentId as a tracking agent for this process.
TInt DTargetProcess::AddAgent(TUint64 aAgentId)
	{
	DDebugAgent* agent = DDebugAgent::New(aAgentId);
	LOG_MSG4("DTargetProcess::AddAgent(), agentId=%d, curr iAgentList.Count=%d, new agent=0x%08x",
		I64LOW(aAgentId), iAgentList.Count(), agent );

	if(agent == NULL)
		{
		LOG_MSG("DTargetProcess::AddAgent() couldn't allocate memory for DDebugAgent");
		return KErrNoMemory;
		}
	return iAgentList.Insert(agent,0);
	}

// Stops tracking the process with this agent
TInt DTargetProcess::RemoveAgent(TUint64 aAgentId)
	{
	// We need to find and then remove the agent
	for(TUint i = 0; i < iAgentList.Count(); i++)
		{
		if (iAgentList[i]->Id() == aAgentId)
			{
			LOG_MSG4("DTargetProcess::RemoveAgent(), deleting agent[%d], id 0x%x, address=0x%x",
					i, I64LOW(aAgentId), iAgentList[i]); 
			delete iAgentList[i];
			iAgentList.Remove(i);
			return KErrNone;
			}
		}

	return KErrNotFound;
	}

// Index through the agents by position
DDebugAgent* DTargetProcess::operator[](TInt aIndex)
	{
	return iAgentList[aIndex];
	}

// returns the number of agents tracking this process.
TInt DTargetProcess::AgentCount() const
	{
	return iAgentList.Count();
	}

void DTargetProcess::NotifyEvent(const TDriverEventInfo& aEventInfo)
	{
	// Stuff the event info into all the tracking agents event queues
	LOG_MSG4("DTargetProcess::NotifyEvent(): num attached agents: %d, iEventType=%d, this=0x%08x", 
		AgentCount(), aEventInfo.iEventType, this);

	for(TInt i = 0; i < AgentCount(); i++)
		{
		// Index through all the relevant debug agents
		DDebugAgent* debugAgent = iAgentList[i];
		if(debugAgent != NULL)
			{
			debugAgent->NotifyEvent(aEventInfo);
			}
		}
	}

