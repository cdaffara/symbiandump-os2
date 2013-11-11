// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "streamagentresolver.h"
#include "streamagentinfo.h"
#include <caf/streaming/streamagentfactory.h>
#include <caf/patchdata.h>
#include "scaflog.h"

#include <caf/caferr.h>

using namespace StreamAccess;

CStreamAgentResolver* CStreamAgentResolver::NewLC()
	{
	CStreamAgentResolver* self = new(ELeave) CStreamAgentResolver();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CStreamAgentResolver::CStreamAgentResolver()
	{
	}

CStreamAgentResolver::~CStreamAgentResolver()
	{
	// Reset and Destroy the stream agent information array 
	iStreamAgentInfoArray.ResetAndDestroy();
	}

void CStreamAgentResolver::ConstructL()
	{
	// Construct a list of the available stream agent interface implementations
	BuildStreamAgentListL();
	}

void CStreamAgentResolver::BuildStreamAgentListL()
	{
	/* Construct a new implementation information array that is used in conjunction with the ECOM
	 * framework in order to retrieve resource data about the available stream agent plug-ins
	 */
	RImplInfoPtrArray agentImplArray;
	CleanupStack::PushL(TCleanupItem(CleanAgentImplArray, &agentImplArray));
	REComSession::ListImplementationsL(KUidStreamAgentFactoryInterface, agentImplArray);
	
	TInt streamAgentCount = agentImplArray.Count();
	
	DEBUG_PRINTF2(_L("Number of streaming agents found on the device: %d"),streamAgentCount);
	
	for (TInt i = 0; i < streamAgentCount; ++i)
		{
		/* In order to load stream agents from sources other than ROM, the patch 
		 * data KCafLoadPostProductionAgents must be set to True (non-zero).
		 * Default SymbianOS behavior is to only load file and streaming agents from ROM
		 */
		if ((KCafLoadPostProductionAgents == 0) &&
            !agentImplArray[i]->RomBased())
			{
			/* If the stream agent is not in ROM, don't load it because it might
			 * be a security risk.
			 */
			continue;
			}
		
		/* Construct a seperate element in the CStreamAgentInfo array for each of the 
		 * identified stream agent implementations 
		 */
		AddStreamAgentL(*agentImplArray[i]);
		}
	
	CleanupStack::PopAndDestroy(&agentImplArray);
	}

void CStreamAgentResolver::CleanAgentImplArray(TAny* aArray)
	{
	// Performs the cleanup on the implementation information array
	static_cast<RImplInfoPtrArray*>(aArray)->ResetAndDestroy();
	}

void CStreamAgentResolver::AddStreamAgentL(const CImplementationInformation& aAgentInfo)
	{
	// Create a new CStreamAgentInfo instance to add to the stream agent list
	CStreamAgentInfo* agentInformation = CStreamAgentInfo::NewLC(aAgentInfo);
	
	TUid agentUid(agentInformation->ImplementationUid());
	TPtrC agentName(agentInformation->Name());
	
	DEBUG_PRINTF3(_L("Stream Agent Identified: 0x%08x (%S)"),agentUid,&agentName);
	
	// Append the CStreamAgentInfo object to the stream agent information array
	User::LeaveIfError(iStreamAgentInfoArray.Append(agentInformation));
	CleanupStack::Pop(agentInformation);
	}

CStreamAgentInfo& CStreamAgentResolver::ResolveSdpKeyStreamL(const CSdpMediaField& aSdpKeyStream) const
	{
	TInt streamAgentInfoCount = iStreamAgentInfoArray.Count();
	
	/* Loop through each of the identified stream agents to determine which (if any) supports 
	 * the SDP key management description
	 */
	for(TInt i=0; i < streamAgentInfoCount; ++i)
		{
		if(iStreamAgentInfoArray[i]->IsKeyStreamSupportedL(aSdpKeyStream))
			{
			TUid agentUid(iStreamAgentInfoArray[i]->ImplementationUid());
			TPtrC agentName (iStreamAgentInfoArray[i]->Name());
			DEBUG_PRINTF3(_L("Key stream is supported by the agent: 0x%08x (%S)"),agentUid,&agentName);
			return *iStreamAgentInfoArray[i];
			}
		}
	
	// If a stream agent capable of decoding the key stream can not be found, leave with KErrCANoAgent
	DEBUG_PRINTF(_L("A stream agent capable of decoding the key stream can not be found!"));
	User::Leave(KErrCANoAgent);
	
	// Return NULL reference to avoid warnings (should never get here)
	CStreamAgentInfo* retVal=NULL;
	return *retVal;
	}
