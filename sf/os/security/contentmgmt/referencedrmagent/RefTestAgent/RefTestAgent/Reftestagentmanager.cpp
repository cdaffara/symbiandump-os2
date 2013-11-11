/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <caf/caf.h>
#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY
#include <http.h>
#include <http/rhttpheaders.h>
#endif
#include <stringpool.h>

#include "Reftestagentmanager.h"
#include "rtaArchive.h"
#include "drmfilecontent.h"
#include "drmfiles.h"
#include "reftestagentconstants.h"
#include "rtarights.h"

using namespace ContentAccess;
using namespace ReferenceTestAgent;


CRefTestAgentManager* CRefTestAgentManager::NewL()
	{
	CRefTestAgentManager* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CRefTestAgentManager* CRefTestAgentManager::NewLC()
	{
	CRefTestAgentManager* self=new(ELeave) CRefTestAgentManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRefTestAgentManager::CRefTestAgentManager()
	{
	}

CRefTestAgentManager::~CRefTestAgentManager()
	{
	iManagerSession.Close();
	}
  
void CRefTestAgentManager::ConstructL()
	{
	User::LeaveIfError(iManagerSession.Connect());
	}

// DeleteFile API is for deleting files in RTA's private data cage.
// Willl return KErrArgument if call upon to delete a public file.

// A client uses this API as hint for RTA to delete rights
// objects. Parameter "aFileName" is the suggested output file
// given by the client at rights import time.
//
// Hence return KErrNone if:
// 1. File is a RTA private file and delete is successful.
// 2. There are rights associated with the filename.
TInt CRefTestAgentManager::DeleteFile(const TDesC& aFileName)	
	{
	TRAPD(deleteRightsRet, DeleteRightsByKeyL(aFileName));

	// Delete file.
	TInt ret = iManagerSession.DeleteFile(aFileName);

	// Merge return codes
	if ( (ret == KErrNotFound || ret == KErrArgument) && (deleteRightsRet == KErrNone) )
		{
		ret = KErrNone;
		}
	return ret;
	}

void CRefTestAgentManager::DeleteRightsByKeyL(const TDesC& aFileName)	
	{
	// Delete rights.
    HBufC8* hbuf8 = HBufC8::NewLC( aFileName.Length() );
	hbuf8->Des().Copy(aFileName);

	RRtaRights rightsServer;
	User::LeaveIfError(rightsServer.Connect());
	CleanupClosePushL(rightsServer);

	rightsServer.DeleteRightsByKeyL(*hbuf8);

	CleanupStack::PopAndDestroy(&rightsServer);
	CleanupStack::PopAndDestroy(hbuf8);
	}

TInt CRefTestAgentManager::CopyFile(const TDesC& aSource, const TDesC& aDestination)
	{
	return iManagerSession.CopyFile(aSource, aDestination);
	}

TInt CRefTestAgentManager::CopyFile(RFile& aSource, const TDesC& aDestination)
	{
	return iManagerSession.CopyFile(aSource, aDestination);
	}

TInt CRefTestAgentManager::RenameFile(const TDesC& aSource, const TDesC& aDestination)
	{
	return iManagerSession.RenameFile(aSource, aDestination);
	}

TInt CRefTestAgentManager::MkDir(const TDesC& aPath)
	{
	return iManagerSession.MkDir(aPath);
	}

TInt CRefTestAgentManager::MkDirAll(const TDesC& aPath)
	{
	return iManagerSession.MkDirAll(aPath);
	}	

TInt CRefTestAgentManager::RmDir(const TDesC& aPath)
	{
	return iManagerSession.RmDir(aPath);
	}	

TInt CRefTestAgentManager::GetDir(const TDesC& aName,TUint anEntryAttMask,TUint anEntrySortKey, CDir*& anEntryList) const 
	{
	return iManagerSession.GetDir(aName, anEntryAttMask, anEntrySortKey, anEntryList);
	}

TInt CRefTestAgentManager::GetDir(const TDesC& aName,TUint anEntryAttMask,TUint anEntrySortKey, CDir*& anEntryList,CDir*& aDirList) const
	{
return iManagerSession.GetDir(aName, anEntryAttMask, anEntrySortKey, anEntryList, aDirList);
	}

TInt CRefTestAgentManager::GetDir(const TDesC& aName, const TUidType& anEntryUid,TUint anEntrySortKey, CDir*& aFileList) const
	{
	return iManagerSession.GetDir(aName, anEntryUid, anEntrySortKey, aFileList);
	}

TInt CRefTestAgentManager::GetAttribute(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath)
	{
	return iManagerSession.GetAttribute(aVirtualPath, aAttribute, aValue);
	}

TInt CRefTestAgentManager::GetAttribute(TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId) 
	{
	return iManagerSession.GetAttribute(aAttribute, aValue, aFile, aUniqueId);
	}

TInt CRefTestAgentManager::GetAttributeSet(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
	{
	return iManagerSession.GetAttributeSet(aVirtualPath, aAttributeSet);
	}
	
TInt CRefTestAgentManager::GetAttributeSet(RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId) 
	{
	return iManagerSession.GetAttributeSet(aAttributeSet, aFile, aUniqueId);
	}

TInt CRefTestAgentManager::GetStringAttribute(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath)
	{
	return iManagerSession.GetStringAttribute(aVirtualPath, aAttribute, aValue);
	}

TInt CRefTestAgentManager::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId) 
	{
	return iManagerSession.GetStringAttributeSet(aStringAttributeSet, aFile, aUniqueId);
	}

TInt CRefTestAgentManager::GetStringAttributeSet(RStringAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
	{
	return iManagerSession.GetStringAttributeSet(aVirtualPath, aAttributeSet);
	}

TInt CRefTestAgentManager::GetStringAttribute(TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId) 
	{
	return iManagerSession.GetStringAttribute(aAttribute, aValue, aFile, aUniqueId);
	}

void CRefTestAgentManager::NotifyStatusChange(const TDesC& aURI, TEventMask aMask, TRequestStatus& aStatus)
	{	
	TRAPD(err, iManagerSession.NotifyStatusChangeL(aURI, aMask));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err); 
	}

TInt CRefTestAgentManager::CancelNotifyStatusChange(const TDesC& aURI, TRequestStatus& aStatus)
	{
	TInt err = iManagerSession.CancelNotifyStatusChange(aURI);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);	
	return err;
	}

TInt CRefTestAgentManager::SetProperty(TAgentProperty aProperty, TInt aValue)
	{
	return iManagerSession.SetProperty(aProperty, aValue);
	}

void CRefTestAgentManager::DisplayInfoL(TDisplayInfo aInfo, const TVirtualPathPtr& aVirtualPath)
	{
	iManagerSession.DisplayInfoL(aInfo, aVirtualPath);
	}	
	
void CRefTestAgentManager::DisplayInfoL(TDisplayInfo aInfo, RFile& aFile, const TDesC& aUniqueId)
	{
	iManagerSession.DisplayInfoL(aInfo, aFile, aUniqueId);
	}

TBool CRefTestAgentManager::IsRecognizedL(const TDesC& aUri, TContentShareMode /*aShareMode*/) const
	{
	TBool result = EFalse;
	// Check that the file has content only, otherwise it should
	// be put through the supplier API before it can be used
	TPtrC extension(aUri.Right(KRtaExtensionContent().Length()));
	if(extension.CompareF(KRtaExtensionContent) == 0)
		{
		result = ETrue;
		}
	return result;
	}

TBool CRefTestAgentManager::IsRecognizedL(RFile& aFile) const
	{
	TBool result = EFalse;
	
	// Get the name of the file
	TFileName fileName;
	aFile.Name(fileName);
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    TPtrC extension(fileName.Right(KRtaExtensionContent().Length()));     
    TPtrC wmdrmExtn(fileName.Right(KRtaWmdrmFileExtension().Length()));     
    if((extension.CompareF(KRtaExtensionContent) == 0) || (wmdrmExtn.CompareF(KRtaWmdrmFileExtension) == 0))     
        {     
        result = ETrue;     
        }     
      
#else 
	// Check that the file has content only, otherwise it should
	// be put through the supplier API before it can be used
	TPtrC extension(fileName.Right(KRtaExtensionContent().Length()));
	if(extension.CompareF(KRtaExtensionContent) == 0)
		{
		result = ETrue;
		}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	return result;
	}

TBool CRefTestAgentManager::RecognizeFileL(const TDesC& aFileName, const TDesC8& aBuffer, TDes8& aFileMimeType, TDes8& aContentMimeType) const
	{
	TBool result = EFalse;
	
	// Check filename extension
	TPtrC extension(aFileName.Right(KRtaExtensionContent().Length()));
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    TPtrC wmdrmExtn(aFileName.Right(KRtaWmdrmFileExtension().Length()));     
    if(wmdrmExtn.CompareF(KRtaWmdrmFileExtension) == 0)     
        {     
        aFileMimeType.Copy(KRtaWmaContentType);     
        aContentMimeType.Copy(KRtaWmaContentType);     
        result = ETrue;     
        }     
         
    else if(extension.CompareF(KRtaExtensionContent) == 0)     
        {     
        aFileMimeType.Copy(KRtaMimeContent());     
        CRefTestAgentArchive::GetDefaultMimeTypeFromHeaderL(aBuffer, aContentMimeType);     
        result = ETrue;      
        }     
#else    
	if(extension.CompareF(KRtaExtensionContent) == 0)
		{
		// It's a content file ready for applications to read
		aFileMimeType.Copy(KRtaMimeContent());
		CRefTestAgentArchive::GetDefaultMimeTypeFromHeaderL(aBuffer, aContentMimeType);
		result = ETrue;	
		}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	else
		{
		extension.Set(aFileName.Right(KRtaExtensionContentRights().Length()));
		if(extension.CompareF(KRtaExtensionContentRights) == 0)
			{
			// It's a content and rights file ready for the supplier API
			aFileMimeType.Copy(KRtaMimeContentRights());
			result = ETrue;
			}
		else
			{
			extension.Set(aFileName.Right(KRtaExtensionRights().Length()));
			if(extension.CompareF(KRtaExtensionRights) == 0)
				{
				// It's a content and rights file ready for the supplier API
				aFileMimeType.Copy(KRtaMimeRights());
				result = ETrue;
				}
			}
		}
	return result;
	}

TInt CRefTestAgentManager::AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer)
	{
	return iManagerSession.AgentSpecificCommand(aCommand, aInputBuffer, aOutputBuffer);
	}

void CRefTestAgentManager::AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus)
	{
	TInt err = AgentSpecificCommand(aCommand, aInputBuffer, aOutputBuffer);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CRefTestAgentManager::DisplayManagementInfoL()
	{
	iManagerSession.DisplayManagementInfoL();
	}

//
// This API is non-leaving and returns void. If we want to
// make leavescan happy, the options are: 1. TRAP leave calls
// and ignore the errors. 2. Panic. Both options could potentially
// break RTA clients' testcases. Hence just leave it the way it is.
#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY
void CRefTestAgentManager::PrepareHTTPRequestHeaders(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const
	{
	// Add the accept header for the reference test agent
	TBuf8 <KMaxDataTypeLength> mimeType;

	// Add accept header for Content only files
	mimeType.Copy(KRtaMimeContent());
	RStringF valStr = aStringPool.OpenFStringL(mimeType);
	CleanupClosePushL(valStr);
	THTTPHdrVal val1(valStr);
	aRequestHeaders.SetFieldL(aStringPool.StringF(HTTP::EAccept, RHTTPSession::GetTable()), val1);
	CleanupStack::PopAndDestroy(&valStr);
	
	// Add accept header for content and rights files
	mimeType.Copy(KRtaMimeContentRights());
	valStr = aStringPool.OpenFStringL(mimeType);
	CleanupClosePushL(valStr);
	THTTPHdrVal val2(valStr);
	aRequestHeaders.SetFieldL(aStringPool.StringF(HTTP::EAccept, RHTTPSession::GetTable()), val2);
	CleanupStack::PopAndDestroy(&valStr);
	}
#else
void CRefTestAgentManager::PrepareHTTPRequestHeaders(RStringPool& /*aStringPool*/, RHTTPHeaders& /*aRequestHeaders*/) const
	{
	}
#endif


#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
      
TBool CRefTestAgentManager::IsRecognizedL(const TDesC8& aHeaderData) const
    {
    TBool result = EFalse;
    if(aHeaderData.Find(KRtaAsfHeaderObject) != KErrNotFound)     
        {     
        result = ETrue;     
        }     
             
    return result;     
    }
     
TBool CRefTestAgentManager::RecognizeContentL(const TDesC8& aHeaderData, TDes8& aFileMimeType, TDes8& aContentMimeType) const     
    {     
    TBool result = EFalse;     
    if(aHeaderData.Find(KRtaAsfHeaderObject) != KErrNotFound)     
        {     
        aFileMimeType.Copy(KRtaWmaContentType);     
        aContentMimeType.Copy(KRtaWmaContentType);     
        result = ETrue;     
        }     
         
    return result;     
    }     
         
TInt CRefTestAgentManager::GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue)     
    {     
    return iManagerSession.GetAttribute(aHeaderData, aAttribute, aValue);     
    }     
      
TInt CRefTestAgentManager::GetAttributeSet(const TDesC8& aHeaderData, RAttributeSet& aAttributeSet)     
    {     
    return iManagerSession.GetAttributeSet(aHeaderData, aAttributeSet);     
    }     
      
TInt CRefTestAgentManager::GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue)      
    {     
    return iManagerSession.GetStringAttribute(aHeaderData, aAttribute, aValue);     
    }     
      
TInt CRefTestAgentManager::GetStringAttributeSet(const TDesC8& aHeaderData, RStringAttributeSet& aAttributeSet)      
    {     
    return iManagerSession.GetStringAttributeSet(aHeaderData, aAttributeSet);     
    }     
         
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

