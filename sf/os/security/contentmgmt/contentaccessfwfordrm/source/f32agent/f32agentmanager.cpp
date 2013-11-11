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


#include <caf/cafpanic.h>
#include <caf/virtualpathptr.h>
#include <caf/dirstreamable.h>
#include "f32agentmanager.h"
#include "f32defaultattributes.h"
#include <caf/f32agentui.h>

using namespace ContentAccess;

CF32AgentManager* CF32AgentManager::NewL()
	{
	CF32AgentManager* self=new(ELeave) CF32AgentManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CF32AgentManager::CF32AgentManager()
	{
	}

CF32AgentManager::~CF32AgentManager()
	{
	delete iFileMan;
	iFs.Close();
	}
  
void CF32AgentManager::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iFileMan = CFileMan::NewL(iFs);
	}

TBool CF32AgentManager::IsRecognizedL(const TDesC&, TContentShareMode) const
	{
	// F32 should be the default agent and should never be called here
	return EFalse;
	}

TBool CF32AgentManager::IsRecognizedL(RFile& ) const
	{
	return EFalse;
	}	

TBool CF32AgentManager::RecognizeFileL(const TDesC&, const TDesC8&, TDes8&, TDes8&) const
	{
	// F32 should be the default agent and should never be called here
	return EFalse;
	}

TInt CF32AgentManager::DeleteFile(const TDesC &aFileName)
	{
	TInt err = KErrNone;
	CF32AgentUi* ui = NULL;
	TBool enableDelete = ETrue;
	
	// get a pointer to the UI
	TRAP(err, ui = &AgentUiL());
	if(err == KErrNone)
		{
		// UI confirmation supported, see if user wants to delete
		TRAP(err, enableDelete = ui->ConfirmDeleteL(aFileName));
		if(err == KErrNone && enableDelete)
			{
			// User agrees to delete
			err = iFs.Delete(aFileName);
			}
		}
	else if(err == KErrCANotSupported)
		{
		// UI is not supported, just delete it
		err = iFs.Delete(aFileName);
		}
	return err;
	}

TInt CF32AgentManager::CopyFile(const TDesC& aSource, const TDesC& aDestination)
	{
	return iFileMan->Copy(aSource, aDestination);
	}

TInt CF32AgentManager::CopyFile(RFile& aSource, const TDesC& aDestination)
	{
	return iFileMan->Copy(aSource, aDestination);
	}

TInt CF32AgentManager::RenameFile(const TDesC& aSource, const TDesC& aDestination)
	{
	TInt result = iFileMan->Rename(aSource, aDestination); 
	// If the files are on a different drive, Rename will fail 
	// Therefore we simulate the Move by doing a Copy, followed by Delete 
	if (result != KErrNone) 
		{ 
		result = iFileMan->Copy(aSource,aDestination); 
		if (result == KErrNone) 
			{ 
			// If the copy was successful try and delete the original 
			result = iFileMan->Delete(aSource); 
			if (result != KErrNone) 
				{
				 // Delete failed so try to cleanup the destination file as we're going to exit with an error 
				// We can safely ignore any error from this as the previous error is more important to propagate, since this is just cleanup
				iFileMan->Delete(aDestination); 
				} 
			} 
		} 
	return result; 
	}	

TInt CF32AgentManager::MkDir(const TDesC& aPath)
	{
	return iFs.MkDir(aPath);
	}

TInt CF32AgentManager::MkDirAll(const TDesC& aPath)
	{
	return iFs.MkDirAll(aPath);
	}

TInt CF32AgentManager::RmDir(const TDesC& aPath)
	{
	return iFs.RmDir(aPath);
	}

TInt CF32AgentManager::RenameDir(const TDesC& aOldName, const TDesC& aNewName)
	{
	return iFs.Rename(aOldName, aNewName);
	}

TInt CF32AgentManager::GetDir(const TDesC& aName,TUint aEntryAttMask,TUint aEntrySortKey, CDir*& aEntryList) const
	{
	return iFs.GetDir(aName, aEntryAttMask, aEntrySortKey, aEntryList);
	}

TInt CF32AgentManager::GetDir(const TDesC& aName,TUint aEntryAttMask,TUint aEntrySortKey, CDir*& aEntryList,CDir*& aDirList) const 
	{
	return iFs.GetDir(aName, aEntryAttMask, aEntrySortKey, aEntryList, aDirList);
	}

TInt CF32AgentManager::GetDir(const TDesC& aName,const TUidType& aEntryUid,TUint aEntrySortKey, CDir*& aFileList) const 
	{
	return iFs.GetDir( aName, aEntryUid, aEntrySortKey, aFileList);
	}

TInt CF32AgentManager::GetAttribute(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath)
	{
	// check that the virtual path exists
	if(TF32DefaultAttributes::CheckVirtualPath(aVirtualPath) != KErrNone)
		{
		return KErrNotFound;	
		}
		
	return TF32DefaultAttributes::GetAttribute(aAttribute, aValue, aVirtualPath.URI());
	}

TInt CF32AgentManager::GetAttributeSet(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
	{
	// check that the virtual path exists
	if(TF32DefaultAttributes::CheckVirtualPath(aVirtualPath) != KErrNone)
		{
		return KErrNotFound;	
		}
		
	return TF32DefaultAttributes::GetAttributeSet(aAttributeSet, aVirtualPath.URI());
	}

TInt CF32AgentManager::GetStringAttribute(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath) 
	{
	// check that the virtual path exists
	if(TF32DefaultAttributes::CheckVirtualPath(aVirtualPath) != KErrNone)
		{
		return KErrNotFound;	
		}
		
	return TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, aVirtualPath.URI());
	}

TInt CF32AgentManager::GetStringAttributeSet(RStringAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath) 
	{
	// check that the virtual path exists
	if(TF32DefaultAttributes::CheckVirtualPath(aVirtualPath) != KErrNone)
		{
		return KErrNotFound;	
		}
		
	return TF32DefaultAttributes::GetStringAttributeSet(aAttributeSet, aVirtualPath.URI());
	}

TInt CF32AgentManager::GetAttribute(TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId)
	{
	// Check that the client hasn't specified some incorrect UniqueId
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;
		}
		
	return TF32DefaultAttributes::GetAttribute(aAttribute, aValue, aFile);
	}

TInt CF32AgentManager::GetAttributeSet(RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId)
	{
	// Check that the client hasn't specified some incorrect UniqueId
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;
		}
		
	return TF32DefaultAttributes::GetAttributeSet(aAttributeSet, aFile);
	}
	
TInt CF32AgentManager::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId)
	{
	// Check that the client hasn't specified some incorrect UniqueId
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;
		}
		
	return TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, aFile);
	}
	
TInt CF32AgentManager::GetStringAttribute(TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId)
	{
	// Check that the client hasn't specified some incorrect UniqueId
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;
		}

	return TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, aFile);
	}		


void CF32AgentManager::NotifyStatusChange(const TDesC& , TEventMask , TRequestStatus& aStatus) 
	{
	TRequestStatus* ptr = &aStatus;
	User::RequestComplete(ptr, KErrCANotSupported);
	}

TInt CF32AgentManager::CancelNotifyStatusChange(const TDesC& , TRequestStatus& ) 
	{
	return KErrCANotSupported;
	}

TInt CF32AgentManager::SetProperty(TAgentProperty aProperty, TInt aValue)
	{
	if(aProperty==EAgentPropertyAgentUI)
		// should only pass type EAgentPropertyAgentUI 
		{
		
		CF32AgentUi* ui = NULL;
		// get a pointer to the UI
		TRAPD(err, ui = &AgentUiL());
		if(err)
			{
			return err;
			}
		return ui->SetProperty(aProperty, aValue);
		}
	else
		{
		return KErrCANotSupported;
		}
	}

void CF32AgentManager::DisplayInfoL(TDisplayInfo aInfo, const TVirtualPathPtr& aVirtualPath) 
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	RFile64 file;
#else
	RFile file;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	// Check that the client hasn't specified some incorrect UniqueId
	User::LeaveIfError(TF32DefaultAttributes::CheckUniqueId(aVirtualPath.UniqueId()));

	// Open the file as read only
	User::LeaveIfError(file.Open(iFs, aVirtualPath.URI(), EFileShareReadersOnly | EFileStream | EFileRead));
	CleanupClosePushL(file);
	AgentUiL().DisplayInfoL(aInfo, file);
	CleanupStack::PopAndDestroy(&file);
	}

void CF32AgentManager::DisplayInfoL(TDisplayInfo aInfo, RFile& aFile, const TDesC& aUniqueId) 
	{
	// Check that the client hasn't specified some incorrect UniqueId
	User::LeaveIfError(TF32DefaultAttributes::CheckUniqueId(aUniqueId));

	// Open the file as read only
	AgentUiL().DisplayInfoL(aInfo, aFile);
	}


TInt CF32AgentManager::AgentSpecificCommand(TInt , const TDesC8& , TDes8& )
	{
	return KErrCANotSupported;
	}

void CF32AgentManager::AgentSpecificCommand(TInt , const TDesC8& , TDes8& , TRequestStatus& aStatus) 
	{
	TRequestStatus* ptr = &aStatus;
	User::RequestComplete(ptr, KErrCANotSupported);
	}

void CF32AgentManager::DisplayManagementInfoL() 
	{
	User::Panic(KCafPanicString, ECafPanicF32AgentManagementInfoNotSupported);
	}

CF32AgentUi& CF32AgentManager::AgentUiL()
	{
	if(!iAgentUi)
		{
		iAgentUi = TF32AgentUiFactory::CreateF32AgentUiL();
		}
	return *iAgentUi;
	}

void CF32AgentManager::PrepareHTTPRequestHeaders(RStringPool& /*aStringPool*/, RHTTPHeaders& /*aRequestHeaders*/) const
	{
	User::Panic(KCafPanicString, ECafPanicF32AgentPrepareHTTPHeadersNotSupported);
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

TBool CF32AgentManager::IsRecognizedL(const TDesC8& /*aHeaderData*/) const
	{
	// F32 should be the default agent and should never be called here
	return EFalse;
	}
	
TBool CF32AgentManager::RecognizeContentL(const TDesC8&, TDes8&, TDes8&) const
	{
	// F32 should be the default agent and should never be called here
	return EFalse;
	}
	
TInt CF32AgentManager::GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue)
	{
	return TF32DefaultAttributes::GetAttribute(aHeaderData, aAttribute, aValue);
	}

TInt CF32AgentManager::GetAttributeSet(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet)
	{
	return TF32DefaultAttributes::GetAttributeSet(aHeaderData, aAttributeSet);
	}

TInt CF32AgentManager::GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue) 
	{
	return TF32DefaultAttributes::GetStringAttribute(aHeaderData, aAttribute, aValue);
	}

TInt CF32AgentManager::GetStringAttributeSet(const TDesC8& aHeaderData, RStringAttributeSet& aAttributeSet) 
	{
	return TF32DefaultAttributes::GetStringAttributeSet(aHeaderData, aAttributeSet);
	}
	
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
