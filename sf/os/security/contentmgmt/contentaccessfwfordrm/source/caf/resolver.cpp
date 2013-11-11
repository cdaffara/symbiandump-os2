/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <e32debug.h>
#include <ecom/ecom.h>

#include "resolver.h"
#include <caf/agentfactory.h>
#include <caf/agentinterface.h>
#include "agentinfo.h"
#include <caf/agent.h>
#include <caf/cafpanic.h>
#include <caf/caferr.h>
#include <caf/patchdata.h>


using namespace ContentAccess;

// Constants for the F32 agent
_LIT(KF32Agent,"F32 CA Agent");


_LIT(KParentDir, "..\\");
_LIT(KPrivateDir, "\\private\\");
const TInt KPrivateDirLength = 9;   // "\\private\\"
const TInt KPrivateDirAndDriveLength = 11;   // "x:\\private\\"
const TInt KPrivateDirOffset = 2; // "x:\\"


EXPORT_C CAgentResolver* CAgentResolver::NewLC(const TBool aDynamicAgentUpdate)
	{
	CAgentResolver* self = new (ELeave) CAgentResolver(aDynamicAgentUpdate);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CAgentResolver* CAgentResolver::NewL(const TBool aDynamicAgentUpdate)
	{
	CAgentResolver* self = CAgentResolver::NewLC(aDynamicAgentUpdate);
	CleanupStack::Pop(self);
	return self;
	}

CAgentResolver::CAgentResolver(const TBool aDynamicAgentUpdate) : CActive(EPriorityStandard), iDynamicAgentUpdate(aDynamicAgentUpdate)
	{
	}

CAgentResolver::~CAgentResolver()
	{
	// remove ourselves from the ActiveScheduler
	if(IsAdded())
		{
		Deque();
		}
	
	// Unload all the agents
	DestroyListOfAgents();
	
	// Close our ECOM session
	if(iEcomSession)
		{
		if(iDynamicAgentUpdate)
			{
			iEcomSession->CancelNotifyOnChange(iStatus);
			}
		iEcomSession->Close();
		REComSession::FinalClose();
		}

	iSupplierMimeTypes.Close();
	iConsumerMimeTypes.Close();
	iAgentInfos.Close();
	}

void CAgentResolver::ConstructL()
	{
	if(iDynamicAgentUpdate)
		{
		// Add ourselves to the current active scheduler so we can get dynamic 
		// updates when agents are removed or new agents are added
		CActiveScheduler::Add(this);
		}

	iEcomSession = &REComSession::OpenL();
	
	// find all the agents
	BuildListOfAgentsL();

	if(iDynamicAgentUpdate)
		{
		// register for ECOM update notifications in case a new agent appears
		SetActive();
		iEcomSession->NotifyOnChange(iStatus);
		}
	}

void CAgentResolver::BuildListOfAgentsL()
	{
	TInt err = KErrNone;

	// Get all plugins which implement the agent interface
	RImplInfoPtrArray implArray;
	CleanupStack::PushL(TCleanupItem(CleanImplArray, &implArray));
	REComSession::ListImplementationsL(KCAAgentInterfaceUid, implArray);

	for (TInt i = 0; i < implArray.Count(); ++i)
		{
#ifdef __EPOC32__
		// On hardware - to load agents from sources other than ROM the patch 
		// data KCafLoadPostProductionAgents must be set to True (non-zero).
		// Default SymbianOS behavior is to only load agents from ROM
		if ((KCafLoadPostProductionAgents == 0) &&
            !implArray[i]->RomBased())
			{
			// If the agent is not in ROM, don't load it because it might
			// be a security risk.
			continue;
			}
#endif

		// Construct all the agent infos from these implementations
		TRAP(err, AddAgentL(*implArray[i]));

		// If we ran out of memory proagate the leave to the caller
		// otherwise don't let a dodgy agent affect the construction of the other
		// agents
		if(err == KErrNoMemory)
			{
			User::Leave(KErrNoMemory);
			}
		}
	CleanupStack::PopAndDestroy(&implArray);  


	if (!iDefaultAgent)
		{
		// If we didn't find a default agent, we have a big problem so panic
		User::Panic(KCafPanicString, ECafPanicNoF32Agent);
		}
	}

void CAgentResolver::AddAgentL(const CImplementationInformation& aImplInfo)
	{
	// Create a CAgentInfo instance
	CAgentInfo* agentInfo = CAgentInfo::NewLC(aImplInfo);


	if(IsF32Agent(*agentInfo))
		{
		// It's the F32 Agent
		if(iDefaultAgent)
			{
			// If we already have a default agent something is seriously wrong
			User::Panic(KCafPanicString, ECafPanicDuplicateF32Agent);
			}

		// Note that the default agent is NOT stored in the agents list, it is a special case
		iDefaultAgent = agentInfo;
		CleanupStack::Pop(agentInfo);
		}
	else
		{
		// All other agents go in the agent list
		User::LeaveIfError(iAgentInfos.Append(agentInfo));
		CleanupStack::Pop(agentInfo);

		TInt mimeIndex=0;

		// Update our list of all supplier mime types supported by CAF
		for(mimeIndex=0;mimeIndex < agentInfo->SupplierMimeTypes().Count(); mimeIndex++) 
			{
			User::LeaveIfError(iSupplierMimeTypes.Append(*agentInfo->SupplierMimeTypes()[mimeIndex]));
			}

		// Update our list of all consumer mime types supported by CAF
		for(mimeIndex=0;mimeIndex < agentInfo->ConsumerMimeTypes().Count(); mimeIndex++) 
			{
			User::LeaveIfError(iConsumerMimeTypes.Append(*agentInfo->ConsumerMimeTypes()[mimeIndex]));
			}
		}
	}

void CAgentResolver::DestroyListOfAgents()
	{
	iSupplierMimeTypes.Reset();
	iConsumerMimeTypes.Reset();

	// cant forget to delete the default agent
	delete iDefaultAgent;
	iDefaultAgent = NULL;

	// Free memory assocated with the iAgentInfos array 
	iAgentInfos.ResetAndDestroy();
	}

void CAgentResolver::DoCancel()
	{
	// Abort any update notification 
	iEcomSession->CancelNotifyOnChange(iStatus);
	}

void CAgentResolver::RunL()
	{
	// Called by the ECOM framework if a new agent appears

	// remove the existing list of agents and build a new one
	DestroyListOfAgents();
	BuildListOfAgentsL();

	// request notification of any further changes
	iEcomSession->NotifyOnChange(iStatus);
	SetActive();
	}	

TBool CAgentResolver::IsF32Agent(CAgentInfo& aAgentInfo)
	{
	// Check if the agent has no consumer or supplier mime types
	// and that it has the correct name and Uid
	if (aAgentInfo.Agent().ImplementationUid() == KF32AgentImplUid
		&& aAgentInfo.Agent().Name().Compare(KF32Agent()) == 0 
		&& aAgentInfo.ConsumerMimeTypes().Count() == 0 
		&& aAgentInfo.SupplierMimeTypes().Count() == 0)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

CAgentInfo& CAgentResolver::ResolveSupplierMimeL(const TDesC8& aMimeType) const
	{
	// Go through all the agents and return the one which supports the
	// required supplier mime type
	CAgentInfo* retVal=NULL;

	for (TInt i = 0; i < iAgentInfos.Count(); ++i)
		{
		if (iAgentInfos[i]->IsSupportedSupplier(aMimeType))
			{
			retVal = iAgentInfos[i];
			break;
			}
		}
	
	if (!retVal)
		{
		User::Leave(KErrCANoAgent);
		}
	return *retVal;
	}

CAgentInfo& CAgentResolver::ResolveConsumerMime(const TDesC8& aMimeType) const
	{
	// By default, set the return value to be the default agent. If we find
	// anything better, then we change it
	CAgentInfo* retVal = iDefaultAgent;

	for (TInt i = 0; i < iAgentInfos.Count(); ++i)
		{
		if (iAgentInfos[i]->IsSupportedConsumer(aMimeType))
			{
			retVal = iAgentInfos[i];
			break;
			}
		}

	ASSERT(retVal);
	return *retVal;
	}

CAgentInfo& CAgentResolver::ResolveFileL(const TDesC& aURI, TDes& aActualUri, TContentShareMode aShareMode) const
	{
	// Go through all the agents and return the one which supports the file at the given URI	
	TBool thePrivateDir = EFalse;	
	TUid agentUid = ResolveDirectory(aURI, aActualUri, thePrivateDir);
	
	if(agentUid != iDefaultAgent->Agent().ImplementationUid())
		{
		// this file must be living in a private server directory
		// return the agent who owns the directory
		return AgentInfoL(agentUid);
		}
	else
		{
		TInt agentsCount(iAgentInfos.Count());
		CAgentManager* agentManager = NULL;
		for (TInt i = 0; i < agentsCount; ++i)
			{
			TRAPD(result, agentManager = &iAgentInfos[i]->AgentManagerL());
			if(result != KErrNone)
				{
				if(KErrNoMemory == result)
					{
					User::Leave(result);
					}
				else
					{
					continue;	
					}
				}
			if (agentManager->IsRecognizedL(aURI, aShareMode))
				{
				return *iAgentInfos[i];
				}
			}
		}
	return *iDefaultAgent;
	}

CAgentInfo& CAgentResolver::ResolveFileL(RFile& aFile) const
	{
	// Go through all the agents and return the one which supports the file

	TInt agentsCount(iAgentInfos.Count());
	CAgentManager* agentManager = NULL;
	for (TInt i = 0; i < agentsCount; ++i)
		{
		TRAPD(result, agentManager = &iAgentInfos[i]->AgentManagerL());
		if(result != KErrNone)
			{
			if(KErrNoMemory == result)
				{
				User::Leave(result);
				}
			else
				{
				continue;	
				}
			}
		if (agentManager->IsRecognizedL(aFile))
			{
			return *iAgentInfos[i];
			}
		}
	return *iDefaultAgent;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

CAgentInfo& CAgentResolver::ResolveFileL(const TDesC8& aHeaderData)
	{
	// Go through all the agents and return the one which supports the given WMDRM content.
	
	TInt agentsCount(iAgentInfos.Count());
	CAgentManager* agentManager = NULL;
	
	for (TInt i = 0; i < agentsCount; ++i)
		{
		TRAPD(result, agentManager = &iAgentInfos[i]->AgentManagerL());
		if(result != KErrNone)
			{
			if(KErrNoMemory == result)
				{
				User::Leave(result);
				}
			else
				{
				continue;	
				}
			}
			
		if (agentManager->IsRecognizedL(aHeaderData))
			{
			return *iAgentInfos[i];
			}
		}
		
    // will never reach here	
	return *iDefaultAgent;
	}
	
	
EXPORT_C TBool CAgentResolver::DoRecognizeL(const TDesC8& aHeader, TDes8& aFileMimeType, TDes8& aContentMimeType)
	{
	// Go through all the agents and return the one which supports the given WMDRM content.
	TInt agentsCount(iAgentInfos.Count());
	CAgentManager* agentManager = NULL;
	for (TInt i = 0; i < agentsCount; ++i)
		{
		TRAPD(result, agentManager = &iAgentInfos[i]->AgentManagerL());
		if(result != KErrNone)
			{
			if(KErrNoMemory == result)
				{
				User::Leave(result);
				}
			else
				{
				continue;	
				}
			}
			
		if (agentManager->RecognizeContentL(aHeader, aFileMimeType, aContentMimeType))
			{
			// force to lower case to ensure that chosen lower case scheme for mime types is maintained
			aFileMimeType.LowerCase();
			aContentMimeType.LowerCase();			
			return ETrue;
			}
		}
		
	return EFalse;
	}
	
#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	
TUid CAgentResolver::ResolveDirectory(const TDesC& aPath, TDes& aActualPath, TBool& aThePrivateDir) const
	{
	TInt i = 0;
	TInt pathLength = 0;
	TBuf <KPrivateDirAndDriveLength> pathLowerCase;
	
	// Assume it's a publicly accessable path
	aThePrivateDir = EFalse;
	
	// Find the length of the path and private directory
	pathLength = aPath.Length();

	// Check that the path is long enough to be within a private directory
	// and does not include "..\\".The  "..\\" sequence could be a security risk
	if(aPath.Find(KParentDir()) == KErrNotFound && pathLength >= KPrivateDirAndDriveLength)
		{
		// Create a lower case copy of the left hand side of the path
		TPtrC lowerCasePtr = aPath.Mid(KPrivateDirOffset, KPrivateDirLength);
		pathLowerCase.Copy(lowerCasePtr);
		pathLowerCase.LowerCase();
	
		// Compare the first directory in the path to \\private\\ 
		if(KPrivateDir() == pathLowerCase)
			{
			// It is a private directory of some sort
			if(pathLength > KPrivateDirAndDriveLength)
				{
				// It must be a server private directory data cage
				TPtrC serverDirectoryPath = aPath.Right(pathLength - KPrivateDirAndDriveLength);
				for(i = 0; i < AgentInfoCount(); i++)
					{
					// See if the part after \\private\\ matches the agent name
					TPtrC privateDirectoryName = AgentInfo(i).PrivateDirectoryName();
					TPtrC agentName = AgentInfo(i).Agent().Name();
					if(privateDirectoryName.Length() && agentName.Length() && agentName == serverDirectoryPath.Left(agentName.Length()))
						{
						// It must be this agent's private directory
						// Convert \\private\\agentName\\... to \\private\\SID\\...
						aActualPath.Copy(aPath.Left(KPrivateDirAndDriveLength));
						aActualPath.Append(privateDirectoryName);
						aActualPath.Append(aPath.Right(pathLength - KPrivateDirAndDriveLength - agentName.Length()));
						return AgentInfo(i).Agent().ImplementationUid();
						}
					}
				}
			else
				{
				// It's just the c:\\private\\ directory
				// Use the default agent, any calls will just fail
				aThePrivateDir = ETrue;
				}
			}
		}
	
	// Not an agent private directory so just return the default agent
	aActualPath.Copy(aPath);
	return iDefaultAgent->Agent().ImplementationUid();
	}

HBufC* CAgentResolver::ConvertAgentFileNameL(const TDesC& aFileName) const
	{
	TInt i = 0;
	TInt fileNameLength = 0;
	TBuf <KPrivateDirAndDriveLength> pathLowerCase;
	
	fileNameLength = aFileName.Length();
	
	// If the path is shorter than the x:\\private\\ it must be a F32 file
	if(fileNameLength  > KPrivateDirAndDriveLength)
		{
		// Create a lower case copy of the left hand side of the path
		TPtrC lowerCasePtr = aFileName.Mid(KPrivateDirOffset, KPrivateDirLength);
		pathLowerCase.Copy(lowerCasePtr);
		pathLowerCase.LowerCase();
	
		// Compare the first directory in the path to \\private\\ 
		if(KPrivateDir() == pathLowerCase)
			{
			// It is a private directory of some sort
			if(fileNameLength > KPrivateDirAndDriveLength)
				{
				// It must be a server private directory data cage
				TPtrC serverDirectoryPath = aFileName.Right(fileNameLength - KPrivateDirAndDriveLength);
				for(i = 0; i < AgentInfoCount(); i++)
					{
					// See if the part after \\private\\ matches the agent name
					TPtrC privateDirectoryName = AgentInfo(i).PrivateDirectoryName();
					TPtrC agentName = AgentInfo(i).Agent().Name();
					if(privateDirectoryName.Length() && agentName.Length() && privateDirectoryName == serverDirectoryPath.Left(privateDirectoryName.Length()))
						{
						// It is this agent's private directory
						// Convert \\private\\SID\\... \\private\\agentName\\... 
						HBufC* buffer = HBufC::NewL(fileNameLength - privateDirectoryName.Length() + agentName.Length());
						TPtr ptr = buffer->Des();
						ptr.Copy(aFileName.Left(KPrivateDirAndDriveLength));
						ptr.Append(agentName);
						ptr.Append(aFileName.Right(fileNameLength - KPrivateDirAndDriveLength - privateDirectoryName.Length()));
						return buffer;
						}
					}
				}
			}
		}
	return aFileName.AllocL();
	}

EXPORT_C TBool CAgentResolver::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer, TDes8& aFileMimeType, TDes8& aContentMimeType)
	{

	// Given the filename and buffer from apparc, ask the agents in turn if they recognize the file
	// Note this will not call the DefaultAgent (F32) because it won't be able to recognize anything

	TInt agentsCount(iAgentInfos.Count());
	CAgentManager* agentManager = NULL;
	for (TInt i = 0; i < agentsCount; ++i)
		{
		TRAPD(result, agentManager = &iAgentInfos[i]->AgentManagerL());
		if(result != KErrNone)
			{
			if(KErrNoMemory == result)
				{
				User::Leave(result);
				}
			else
				{
				continue;	
				}
			}
		if (agentManager->RecognizeFileL(aName, aBuffer, aFileMimeType, aContentMimeType))
			{
			// force to lower case to ensure that chosen lower case scheme for mime types is maintained
			aFileMimeType.LowerCase();
			aContentMimeType.LowerCase();			
			return ETrue;
			}
		}
	return EFalse;
	}
	

void CAgentResolver::CleanImplArray(TAny* aArray)
	{
	static_cast<RImplInfoPtrArray*>(aArray)->ResetAndDestroy();
	}

EXPORT_C TInt CAgentResolver::PreferredBufferSize()
	{
	TInt size=0;

	if(iDefaultAgent != NULL)
		{
		size = iDefaultAgent->PreferredBufferSize();
		}

	// Find out the maximum buffer requested by any agent
	for (TInt i = 0; i < iAgentInfos.Count(); ++i)
			{
			if(iAgentInfos[i]->PreferredBufferSize() > size)
				{
				size = iAgentInfos[i]->PreferredBufferSize();
				}
			}
	return size;
	}


EXPORT_C const RArray<TPtrC8>& CAgentResolver::ConsumerMimeTypes() const
	{
	return iConsumerMimeTypes;
	}


EXPORT_C const RArray<TPtrC8>& CAgentResolver::SupplierMimeTypes() const
	{
	return iSupplierMimeTypes;
	}


CAgentInfo& CAgentResolver::AgentInfoL(const TDesC& aAgentName) const
	{
	TBool found = EFalse;
	TInt i = 0;
	for(i = 0; i < iAgentInfos.Count(); i++)
		{
		if(iAgentInfos[i]->Agent().Name() == aAgentName)
			{
			found = ETrue;
			break;
			}
		}
	
	if(!found)
		{
		// Can't find the agent so leave
		User::Leave(KErrNotFound);
		}
	
	return *iAgentInfos[i];
	}

CAgentInfo& CAgentResolver::AgentInfoL(const TUid& aUid) const
	{
	TInt i = 0;
	TBool found = EFalse;
	
	// See if it's the F32 agent
	if(aUid == DefaultAgentUid())
		{
		return *iDefaultAgent;
		}
	
	for(i = 0; i < iAgentInfos.Count(); i++)
		{
		if(iAgentInfos[i]->Agent().ImplementationUid() == aUid)
			{
			found = ETrue;
			break;			
			}
		}
	
	if(!found)
		{
		// couldn't find the agent so leave
		User::Leave(KErrNotFound);
		}
	
	return *iAgentInfos[i];
	}

CAgentInfo& CAgentResolver::AgentInfo(TInt aIndex) const
		{
		return *iAgentInfos[aIndex];		
		}

TInt CAgentResolver::AgentInfoCount() const
	{
	return iAgentInfos.Count();	
	}

TUid CAgentResolver::DefaultAgentUid() const
	{
	return iDefaultAgent->Agent().ImplementationUid();
	}
