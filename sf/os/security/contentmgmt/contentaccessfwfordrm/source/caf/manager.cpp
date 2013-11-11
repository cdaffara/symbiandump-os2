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


#include <caf/manager.h>
#include <caf/agentinterface.h>
#include <caf/agent.h>
#include "agentinfo.h"
#include <caf/agentfactory.h>
#include <caf/attributeset.h>
#include <caf/caftypes.h>
#include <caf/caferr.h>
#include <caf/virtualpath.h>
#include <caf/dirstreamable.h>
#include <caf/rightsmanager.h>
#include <caf/cafpanic.h>
#include "resolver.h"

using namespace ContentAccess;

EXPORT_C CManager* CManager::NewL()
	{
	CManager* self = CManager::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CManager* CManager::NewLC()
	{
	CManager* self = new (ELeave) CManager;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CManager::CManager()
	{
	}

CManager::~CManager()
	{
	delete iResolver;
	}

void CManager::ConstructL()
	{
	iResolver = CAgentResolver::NewL(EFalse);
	}

EXPORT_C TInt CManager::DeleteFile(const TDesC &aFileName) const
	{
	TRAPD(err, DoDeleteFileL(aFileName));
	return err;
	}
	
void CManager::DoDeleteFileL(const TDesC &aFileName) const
	{
	TFileName actualFileName;
	
	// initalise a pointer to the relevant CAgentManager object
	// iResolver retains ownership of the pointer
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aFileName, actualFileName, EContentShareReadOnly);

	User::LeaveIfError(agentInfo.AgentManagerL().DeleteFile(actualFileName));
	}

EXPORT_C TInt CManager::CopyFile(RFile& aSourceFile, const TDesC &aDestination) const
	{
	TRAPD(err, DoCopyFileL(aSourceFile, aDestination));
	return err;
	}

void CManager::DoCopyFileL(RFile& aSourceFile, const TDesC &aDestination) const
	{
	TFileName actualDestination;
	TBool thePrivateDir = EFalse;
	TUid agentUid = KNullUid;

	// Find out which agent owns the directory where the destination file lives
	TUid destinationAgentUid = iResolver->ResolveDirectory(aDestination, actualDestination, thePrivateDir);
	
	if(destinationAgentUid != iResolver->DefaultAgentUid())
		{
		// Destination file is in an agent private directory
		// Use destination agent to copy
		agentUid = destinationAgentUid;
		}
	else
		{
		// Use RFile version of ResolveFileL to find the Agent Uid
		agentUid = iResolver->ResolveFileL(aSourceFile).Agent().ImplementationUid();
		}
		
	// If creating the appropriate CAgentManager succeeded ask the agent to copy the file
	User::LeaveIfError(iResolver->AgentInfoL(agentUid).AgentManagerL().CopyFile(aSourceFile, actualDestination));
	}

EXPORT_C TInt CManager::CopyFile(const TDesC &aSource, const TDesC &aDestination) const
	{
	TRAPD(err, DoCopyFileL(aSource, aDestination));
	return err;
	}

void CManager::DoCopyFileL(const TDesC &aSource, const TDesC &aDestination) const
	{
	TFileName actualSource;
	TFileName actualDestination;
	TBool thePrivateDir = EFalse;
	TUid agentUid = KNullUid;
	
	// Find out which agent owns the directory where the source file lives
	TUid sourceAgentUid = iResolver->ResolveDirectory(aSource, actualSource, thePrivateDir);

	// Find out which agent owns the directory where the destination file lives
	TUid destinationAgentUid = iResolver->ResolveDirectory(aDestination, actualDestination, thePrivateDir);
	
	if(sourceAgentUid != iResolver->DefaultAgentUid())
		{
		// Source file is in an agent private directory
		// Use source agent to copy
		agentUid = sourceAgentUid;
		}
	else if(destinationAgentUid != iResolver->DefaultAgentUid())
		{
		// Destination file is in an agent private directory
		// Use destination agent to copy
		agentUid = destinationAgentUid;
		}
	else
		{
		// Source and destination are in publicly accessable directories
		agentUid = iResolver->ResolveFileL(aSource, actualSource, EContentShareReadOnly).Agent().ImplementationUid();
		}
		
	// If creating the appropriate CAgentManager succeeded ask the agent to copy the file
	User::LeaveIfError(iResolver->AgentInfoL(agentUid).AgentManagerL().CopyFile(actualSource, actualDestination));
	}

EXPORT_C TInt CManager::RenameFile(const TDesC& aSource, const TDesC& aDestination) const
	{
	TRAPD(err, DoRenameFileL(aSource, aDestination));
	return err;
	}

void CManager::DoRenameFileL(const TDesC& aSource, const TDesC& aDestination) const
	{
	TFileName actualSource;
	TFileName actualDestination;
	TBool thePrivateDir = EFalse;
	TUid agentUid = KNullUid;

	// Find out which agent owns the directory where the source file lives
	TUid sourceAgentUid = iResolver->ResolveDirectory(aSource, actualSource, thePrivateDir);

	// Find out which agent owns the directory where the destination file lives
	TUid destinationAgentUid = iResolver->ResolveDirectory(aDestination, actualDestination, thePrivateDir);
	
	if(sourceAgentUid != iResolver->DefaultAgentUid())
		{
		// Source file is in an agent private directory
		// Use source agent to copy
		agentUid = sourceAgentUid;
		}
	else if(destinationAgentUid != iResolver->DefaultAgentUid())
		{
		// Destination file is in an agent private directory
		// Use destination agent to copy
		agentUid = destinationAgentUid;
		}
	else
		{
		// Source and destination are in publicly accessable directories
		agentUid = iResolver->ResolveFileL(aSource, actualSource, EContentShareReadOnly).Agent().ImplementationUid();
		}
		
	// Ask the agent to move or rename the file
	User::LeaveIfError(iResolver->AgentInfoL(agentUid).AgentManagerL().RenameFile(actualSource, actualDestination));
	}

EXPORT_C TInt CManager::MkDir(const TDesC &aPath) const
	{
	TRAPD(err, DoMkDirL(aPath));
		
	return err;	
	}

void CManager::DoMkDirL(const TDesC &aPath) const
	{
	TBool isThePrivateDir = EFalse;
	TFileName actualPath;
	
	// Figure out which agent handles the directory
	TUid uid = iResolver->ResolveDirectory(aPath, actualPath, isThePrivateDir);

	User::LeaveIfError(iResolver->AgentInfoL(uid).AgentManagerL().MkDir(actualPath));
	}

EXPORT_C TInt CManager::MkDirAll (const TDesC &aPath) const
	{
	TRAPD(err, DoMkDirAllL(aPath));
	return err;	
	}

void CManager::DoMkDirAllL(const TDesC &aPath) const
	{
	TBool isThePrivateDir = EFalse;
	TFileName actualPath;
	
	// Figure out which agent handles the directory
	TUid uid = iResolver->ResolveDirectory(aPath, actualPath, isThePrivateDir);
	
	User::LeaveIfError(iResolver->AgentInfoL(uid).AgentManagerL().MkDirAll(actualPath));
	}

EXPORT_C TInt CManager::RmDir(const TDesC &aPath) const
	{
	TRAPD(err, DoRmDirL(aPath));
	return err;	
	}

void CManager::DoRmDirL(const TDesC &aPath) const
	{
	TBool isThePrivateDir = EFalse;
	TFileName actualPath;
	
	// Figure out which agent handles the directory
	TUid uid = iResolver->ResolveDirectory(aPath, actualPath, isThePrivateDir);

	if(isThePrivateDir)
		{
		User::Leave(KErrAccessDenied);	
		}
	else
		{
		User::LeaveIfError(iResolver->AgentInfoL(uid).AgentManagerL().RmDir(actualPath));
		}
	}

EXPORT_C TInt CManager::RenameDir(const TDesC& aOldName, const TDesC& aNewName) const
		{
		TRAPD(err, DoRenameDirL(aOldName, aNewName));
		return err;
		}

void CManager::DoRenameDirL(const TDesC &aOldName, const TDesC& aNewName) const
	{
	TPath actualSource;
	TPath actualDestination;
	TBool thePrivateDir = EFalse;
	TUid agentUid = iResolver->DefaultAgentUid();

	// Find out which agent owns the directory where the source file lives
	TUid sourceAgentUid = iResolver->ResolveDirectory(aOldName, actualSource, thePrivateDir);

	// Find out which agent owns the directory where the destination file lives
	TUid destinationAgentUid = iResolver->ResolveDirectory(aNewName, actualDestination, thePrivateDir);
	
	if(sourceAgentUid != iResolver->DefaultAgentUid())
		{
		// Source file is in an agent private directory
		// Use source agent to copy
		agentUid = sourceAgentUid;
		}
	else if(destinationAgentUid != iResolver->DefaultAgentUid())
		{
		// Destination file is in an agent private directory
		// Use destination agent to copy
		agentUid = destinationAgentUid;
		}
		
	// Ask the agent to move or rename the file
	User::LeaveIfError(iResolver->AgentInfoL(agentUid).AgentManagerL().RenameDir(actualSource, actualDestination));
	}


EXPORT_C TInt CManager::GetDir(const TDesC &aName, TUint aEntryAttMask, TUint aEntrySortKey, CDir *&aEntryList) const 
	{
	TRAPD(err, DoGetDirL(aName, aEntryAttMask, aEntrySortKey, aEntryList));
	return err;
	}
	
void CManager::DoGetDirL(const TDesC &aName, TUint aEntryAttMask, TUint aEntrySortKey, CDir *&aEntryList) const 	
	{
	TBool isThePrivateDir = EFalse;
	TFileName actualPath;
	
	// Figure out which agent handles the directory
	TUid uid = iResolver->ResolveDirectory(aName, actualPath, isThePrivateDir);

	if(isThePrivateDir)
		{
		// If we are doing GetDir of C:\\private\\ just create an empty CDirStreamable
		aEntryList = CDirStreamable::NewL();
		}
	else
		{
		User::LeaveIfError(iResolver->AgentInfoL(uid).AgentManagerL().GetDir(actualPath, aEntryAttMask, aEntrySortKey, aEntryList));
		}
	}

EXPORT_C TInt CManager::GetDir(const TDesC &aName, TUint aEntryAttMask, TUint aEntrySortKey, CDir *&aEntryList, CDir *&aDirList) const 
	{
	TRAPD(err, DoGetDirL(aName, aEntryAttMask, aEntrySortKey, aEntryList, aDirList));
	return err;
	}

void CManager::DoGetDirL(const TDesC &aName, TUint aEntryAttMask, TUint aEntrySortKey, CDir *&aEntryList, CDir *&aDirList) const 
	{
	TBool isThePrivateDir = EFalse;
	TFileName actualPath;
	CDir* emptyDirList;
	
	// Figure out which agent handles the directory
	TUid uid = iResolver->ResolveDirectory(aName, actualPath, isThePrivateDir);

	if(isThePrivateDir)
		{
		// If we are doing GetDir of C:\\private\\ just create an empty entryList
		emptyDirList = CDirStreamable::NewL();
		CleanupStack::PushL(emptyDirList);

		GetListOfAgentPrivateDirectoriesL(aDirList);

		CleanupStack::Pop(emptyDirList);
		aEntryList = emptyDirList;		
		}
	else
		{
		User::LeaveIfError(iResolver->AgentInfoL(uid).AgentManagerL().GetDir(actualPath, aEntryAttMask, aEntrySortKey, aEntryList, aDirList));
		}
	}

EXPORT_C TInt CManager::GetDir(const TDesC &aName, const TUidType &aEntryUid, TUint aEntrySortKey, CDir *&aFileList) const 
	{
	TRAPD(err, DoGetDirL(aName, aEntryUid, aEntrySortKey, aFileList));
	return err;
	}

void CManager::DoGetDirL(const TDesC &aName, const TUidType &aEntryUid, TUint aEntrySortKey, CDir *&aFileList) const 
	{
	TBool isThePrivateDir = EFalse;
	TFileName actualPath;
	
	// Figure out which agent handles the directory
	TUid uid = iResolver->ResolveDirectory(aName, actualPath, isThePrivateDir);

	if(isThePrivateDir)
		{
		// We've been asked to look at C:\\private\\ just return an empty CDirStreamable
		aFileList = CDirStreamable::NewL();
		}
	else
		{
		User::LeaveIfError(iResolver->AgentInfoL(uid).AgentManagerL().GetDir(actualPath, aEntryUid, aEntrySortKey, aFileList));
		}
	}

EXPORT_C TInt CManager::GetAttribute(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath) const
	{
	TRAPD(err, DoGetAttributeL(aAttribute, aValue, aVirtualPath));
	return err;
	}

void CManager::DoGetAttributeL(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath) const
	{
	HBufC* uriBuffer = HBufC::NewLC(aVirtualPath.URI().Length() + KMaxSIDLength);
	TPtr uri = uriBuffer->Des();

	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aVirtualPath.URI(),uri, EContentShareReadOnly);
	User::LeaveIfError(agentInfo.AgentManagerL().GetAttribute(aAttribute, aValue, TVirtualPathPtr(uri, aVirtualPath.UniqueId())));
	CleanupStack::PopAndDestroy(uriBuffer);
	}
	
EXPORT_C TInt CManager::GetAttribute(TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId) 
	{
	TRAPD(err, DoGetAttributeL(aAttribute, aValue, aFile, aUniqueId));
	return err;
	}

void CManager::DoGetAttributeL(TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId) const
	{
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aFile);
	User::LeaveIfError(agentInfo.AgentManagerL().GetAttribute(aAttribute, aValue, aFile, aUniqueId));
	}
	

EXPORT_C TInt CManager::GetStringAttribute(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath) const
	{
	TRAPD(err, DoGetStringAttributeL(aAttribute, aValue, aVirtualPath));
	return err;
	}
	
void CManager::DoGetStringAttributeL(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath) const
	{
	HBufC* uriBuffer = HBufC::NewLC(aVirtualPath.URI().Length() + KMaxSIDLength);
	TPtr uri = uriBuffer->Des();


	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aVirtualPath.URI(), uri, EContentShareReadOnly);
	
	// find out the attribute
	User::LeaveIfError(agentInfo.AgentManagerL().GetStringAttribute(aAttribute, aValue, TVirtualPathPtr(uri,aVirtualPath.UniqueId())));
	CleanupStack::PopAndDestroy(uriBuffer);
	}
	
EXPORT_C TInt CManager::GetStringAttribute (TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId)	
	{
	TRAPD(err, DoGetStringAttributeL(aAttribute, aValue, aFile, aUniqueId));
	return err;
	}
	
void CManager::DoGetStringAttributeL(TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId) const
	{
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aFile);	
	// find out the attribute
	User::LeaveIfError(agentInfo.AgentManagerL().GetStringAttribute(aAttribute, aValue, aFile, aUniqueId));
	}

EXPORT_C TInt CManager::GetAttributeSet(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath) const
	{
	TRAPD(err, DoGetAttributeSetL(aAttributeSet, aVirtualPath));
	return err;
	}

void CManager::DoGetAttributeSetL(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath) const	
	{
	HBufC* uriBuffer = HBufC::NewLC(aVirtualPath.URI().Length() + KMaxSIDLength);
	TPtr uri = uriBuffer->Des();
	
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aVirtualPath.URI(), uri, EContentShareReadOnly);
	
	User::LeaveIfError(agentInfo.AgentManagerL().GetAttributeSet(aAttributeSet, TVirtualPathPtr(uri, aVirtualPath.UniqueId())));
	CleanupStack::PopAndDestroy(uriBuffer);
	}

EXPORT_C TInt CManager::GetAttributeSet (RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId)
	{
	TRAPD(err, DoGetAttributeSetL(aAttributeSet, aFile, aUniqueId));
	return err;
	}

void CManager::DoGetAttributeSetL(RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId) const	
	{
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aFile);
	User::LeaveIfError(agentInfo.AgentManagerL().GetAttributeSet(aAttributeSet, aFile, aUniqueId));
	}	

EXPORT_C TInt CManager::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TVirtualPathPtr& aVirtualPath) const
	{
	TRAPD(err, DoGetStringAttributeSetL(aStringAttributeSet, aVirtualPath));
	return err;
	}

void CManager::DoGetStringAttributeSetL(RStringAttributeSet& aStringAttributeSet, const TVirtualPathPtr& aVirtualPath) const
	{
	HBufC* uriBuffer = HBufC::NewLC(aVirtualPath.URI().Length() + KMaxSIDLength);
	TPtr uri = uriBuffer->Des();
	
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aVirtualPath.URI(),uri, EContentShareReadOnly);

	// find out the array of attributes
	User::LeaveIfError(agentInfo.AgentManagerL().GetStringAttributeSet(aStringAttributeSet, TVirtualPathPtr(uri, aVirtualPath.UniqueId())));
	CleanupStack::PopAndDestroy(uriBuffer);
	}

EXPORT_C TInt CManager::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId)
	{
	TRAPD(err, DoGetStringAttributeSetL(aStringAttributeSet, aFile, aUniqueId));
	return err;
	}

void CManager::DoGetStringAttributeSetL(RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId) const
	{
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aFile);
	// find out the array of attributes
	User::LeaveIfError(agentInfo.AgentManagerL().GetStringAttributeSet(aStringAttributeSet, aFile, aUniqueId));
	}

EXPORT_C void CManager::NotifyStatusChange(const TDesC &aURI, TEventMask aMask, TRequestStatus &aStatus) 
	{
	TRAPD(err, DoNotifyStatusChangeL(aURI, aMask, aStatus));
	if(err != KErrNone)
		{
		// Must have failed before asking the agent for status
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	}

void CManager::DoNotifyStatusChangeL(const TDesC &aURI, TEventMask aMask, TRequestStatus &aStatus) 
	{
	HBufC* uriBuffer = HBufC::NewLC(aURI.Length() + KMaxSIDLength);
	TPtr uri = uriBuffer->Des();
	
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aURI, uri, EContentShareReadOnly);
	
	// Ask the agent to notifiy the client when the status of the given file changes
	agentInfo.AgentManagerL().NotifyStatusChange(uri, aMask, aStatus);
	CleanupStack::PopAndDestroy(uriBuffer);
	}

EXPORT_C TInt CManager::CancelNotifyStatusChange (const TDesC &aURI, TRequestStatus &aStatus) 
	{
	TRAPD(err, DoCancelNotifyStatusChangeL(aURI, aStatus));
	return err;
	}

void CManager::DoCancelNotifyStatusChangeL(const TDesC &aURI, TRequestStatus &aStatus) 
	{
	HBufC* uriBuffer = HBufC::NewLC(aURI.Length() + KMaxSIDLength);
	TPtr uri = uriBuffer->Des();
	
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aURI, uri, EContentShareReadOnly);
	
	// cancel the notification request
	User::LeaveIfError(agentInfo.AgentManagerL().CancelNotifyStatusChange(uri, aStatus));
	CleanupStack::PopAndDestroy(uriBuffer);
	}

EXPORT_C TInt CManager::SetProperty(TAgentProperty aProperty, TInt aValue)
	{
	TRAPD(err, DoSetPropertyL(aProperty, aValue));
	return err;
	}

void CManager::DoSetPropertyL(TAgentProperty aProperty, TInt aValue)
	{
	TInt err = KErrNone;
	TInt rVal = KErrNone;
	TInt i = 0;
	TInt count = iResolver->AgentInfoCount();
	
	CAgentInfo &defaultAgentInfo = iResolver->AgentInfoL(iResolver->DefaultAgentUid());
	err = defaultAgentInfo.AgentManagerL().SetProperty(aProperty, aValue);
	if(err == KErrNoMemory)
		{
		User::Leave(KErrNoMemory);
		}
	
	// Ignore F32 agent return code unless it's KErrNoMemory
	err= KErrNone;
	
	// Set the property in all the other agents
	for( i = 0; i < count; i++)
		{
		CAgentInfo& agentInfo = iResolver->AgentInfo(i);
		err = agentInfo.AgentManagerL().SetProperty(aProperty, aValue);
		if(err == KErrNoMemory)
			{
			User::Leave(KErrNoMemory);
			}
			
		// If an error occurred and no previous error has occured
		if(err != KErrNone && rVal == KErrNone)
			{
			rVal = err;
			}
		}
	
	// leave if an error occurred in any agent
	User::LeaveIfError(rVal);
	}

EXPORT_C void CManager::DisplayInfoL(TDisplayInfo aInfo, const TVirtualPathPtr& aVirtualPath)
	{
	HBufC* uriBuffer = HBufC::NewLC(aVirtualPath.URI().Length() + KMaxSIDLength);
	TPtr uri = uriBuffer->Des();

	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aVirtualPath.URI(), uri, EContentShareReadOnly);
	
	// find out the attribute
	agentInfo.AgentManagerL().DisplayInfoL(aInfo, TVirtualPathPtr(uri, aVirtualPath.UniqueId()));
	CleanupStack::PopAndDestroy(uriBuffer);
	}

EXPORT_C void CManager::DisplayInfoL(TDisplayInfo aInfo, RFile& aFile, const TDesC& aUniqueId) 
	{
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aFile);
	// find out the attribute
	agentInfo.AgentManagerL().DisplayInfoL(aInfo, aFile, aUniqueId);
	}

EXPORT_C void CManager::ListAgentsL (RArray <TAgent>& aAgents) 
	{
	TInt i = 0;
	TBuf <KMaxAgentNameLength> agentName;
	TInt count = iResolver->AgentInfoCount();
	
	// build the array of pointers from CAgentResolver
	for(i = 0; i < count; i++)
		{
		TAgent agent = iResolver->AgentInfo(i).Agent();
		User::LeaveIfError(aAgents.Append(agent));
		}
	}

EXPORT_C TInt CManager::AgentSpecificCommand (TAgent &aAgent, TInt aCommand, const TDesC8 &aInputBuffer, TDes8 &aOutputBuffer) 
	{
	CAgentManager* manager = NULL;
	TInt err = KErrNone;
	TRAP(err, manager = &iResolver->AgentInfoL(aAgent.ImplementationUid()).AgentManagerL());
	if(err == KErrNone)
		{
		err = manager->AgentSpecificCommand (aCommand, aInputBuffer, aOutputBuffer);
		}
	return err;
	}

EXPORT_C void CManager::AgentSpecificCommand (TAgent &aAgent, TInt aCommand, const TDesC8 &aInputBuffer, TDes8 &aOutputBuffer, TRequestStatus &aStatus) 
	{
	CAgentManager* manager = NULL;
	
	TRAPD(err, manager = &iResolver->AgentInfoL(aAgent.ImplementationUid()).AgentManagerL());
	if(err == KErrNone)
		{
		manager->AgentSpecificCommand (aCommand, aInputBuffer, aOutputBuffer, aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
	}

EXPORT_C void CManager::DisplayManagementInfoL(TAgent& aAgent)
	{
	CAgentInfo& agentInfo = iResolver->AgentInfoL(aAgent.ImplementationUid());

	// Ask agent to display management info
	agentInfo.AgentManagerL().DisplayManagementInfoL();
	}

EXPORT_C CRightsManager* CManager::CreateRightsManagerL(TAgent& aAgent) const
	{
	// Create a rights manager
	return CRightsManager::NewL(aAgent.ImplementationUid());
	}

void CManager::GetListOfAgentPrivateDirectoriesL(CDir *&aDir) const
	{
	CDirStreamable *ptr = NULL;
	// fill in the agent directories under the \\private\\ directory 
	TInt i = 0;
	TInt count = iResolver->AgentInfoCount();
	
	TBuf <KMaxAgentNameLength> agentName;
	TPath privateDirectory;
	
	// Create CDirStreamable object
	ptr = CDirStreamable::NewL();
	CleanupStack::PushL(ptr);
	
	// set aDir to point to the CDirStreamable we just created
	for(i = 0; i < count; i++)
		{
		// only fill in the agents that have a private directory that is not blank
		if(iResolver->AgentInfo(i).PrivateDirectoryName().Length() != 0)
			{
			TEntry aEntry;
			aEntry.iName = iResolver->AgentInfo(i).Agent().Name();
			aEntry.iAtt = KEntryAttDir;
			aEntry.iType = TUidType();
			aEntry.iSize = 0;
			ptr->AddL(aEntry);
			}
		}
	CleanupStack::Pop(ptr);
	aDir = ptr;
	}

#ifndef REMOVE_CAF1
EXPORT_C void CManager::DeleteFileL(const TDesC &aFileName)
	{
	CManager *m = CManager::NewLC();
	User::LeaveIfError(m->DeleteFile(aFileName));
	CleanupStack::PopAndDestroy(m);
	}
#endif // REMOVE_CAF1

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

EXPORT_C TInt CManager::GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue) const
	{
	TRAPD(err, DoGetAttributeL(aHeaderData, aAttribute, aValue));
	return err;
	}

void CManager::DoGetAttributeL(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue) const
	{
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aHeaderData);
	User::LeaveIfError(agentInfo.AgentManagerL().GetAttribute(aHeaderData, aAttribute, aValue));
	}
	
EXPORT_C TInt CManager::GetAttributeSet(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet) const
	{
	TRAPD(err, DoGetAttributeSetL(aHeaderData, aAttributeSet));
	return err;
	}

void CManager::DoGetAttributeSetL(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet) const	
	{
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aHeaderData);
	
	User::LeaveIfError(agentInfo.AgentManagerL().GetAttributeSet(aHeaderData, aAttributeSet));
	}
	
EXPORT_C TInt CManager::GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue) const
	{
	TRAPD(err, DoGetStringAttributeL(aHeaderData, aAttribute, aValue));
	return err;
	}
	
void CManager::DoGetStringAttributeL(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue) const
	{
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aHeaderData);
	
	// find out the attribute
	User::LeaveIfError(agentInfo.AgentManagerL().GetStringAttribute(aHeaderData, aAttribute, aValue));
	}
	
EXPORT_C TInt CManager::GetStringAttributeSet(const TDesC8& aHeaderData, RStringAttributeSet& aStringAttributeSet) const
	{
	TRAPD(err, DoGetStringAttributeSetL(aHeaderData, aStringAttributeSet));
	return err;
	}

void CManager::DoGetStringAttributeSetL(const TDesC8& aHeaderData, RStringAttributeSet& aStringAttributeSet) const
	{
	// Find the agent who handles the file 
	CAgentInfo& agentInfo = iResolver->ResolveFileL(aHeaderData);

	// find out the array of attributes
	User::LeaveIfError(agentInfo.AgentManagerL().GetStringAttributeSet(aHeaderData, aStringAttributeSet));
	}
	
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
