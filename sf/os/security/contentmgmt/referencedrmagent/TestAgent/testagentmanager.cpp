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


#include <caf/caf.h>
#include <caf/dirstreamable.h>
#include "testagentmanager.h"
#include "testagentattributes.h"
#include "TestAgentFile.h"
#include "TestAgentDrmContent.h"
#include <stringpool.h>
#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY
#include <http.h>
#include <http/rhttpheaders.h>
#endif
#include <stringpool.h>
using namespace ContentAccess;

_LIT(KDrmContentExtension,".DRM");
// Do not change the case of KDrmMimeType as it is needed for case testing.
_LIT(KDrmMimeType, "APPLICATION/TESTAGENT.DRM");

CTestAgentManager* CTestAgentManager::NewL()
	{
	CTestAgentManager* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentManager* CTestAgentManager::NewLC()
	{
	CTestAgentManager* self=new(ELeave) CTestAgentManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestAgentManager::CTestAgentManager()
	{
	}

CTestAgentManager::~CTestAgentManager()
	{
	delete iFileMan;
	iFs.Close();
	}
  
void CTestAgentManager::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iFileMan = CFileMan::NewL(iFs);
	}


TInt CTestAgentManager::DeleteFile(const TDesC &aFileName)	
	{
	// Most likely would have some kind of rights check here to determine if it can be
	// deleted
	return iFs.Delete(aFileName);
	}

TInt CTestAgentManager::CopyFile(const TDesC& /*aSource*/, const TDesC& /*aDestination*/)
	{
	return KErrCANotSupported;
	}

TInt CTestAgentManager::CopyFile(RFile& /*aSource*/, const TDesC& /*aDestination*/)
	{
	return KErrCANotSupported;
	}

TInt CTestAgentManager::RenameFile(const TDesC& /*aSource*/, const TDesC& /*aDestination*/)
	{
	return KErrCANotSupported;
	}

TInt CTestAgentManager::MkDir(const TDesC& /*aPath*/)
	{
	return KErrCANotSupported;
	}

TInt CTestAgentManager::MkDirAll(const TDesC& /*aPath*/)
	{
	return KErrCANotSupported;
	}	

TInt CTestAgentManager::RmDir(const TDesC& /*aPath*/)
	{
	return KErrCANotSupported;
	}	

TInt CTestAgentManager::GetDir(const TDesC& /*aName*/,TUint /*aEntryAttMask*/,TUint /*aEntrySortKey*/, CDir*& /*aEntryList*/) const 
	{
	return KErrCANotSupported;
	}

TInt CTestAgentManager::GetDir(const TDesC& /*aName*/,TUint /*aEntryAttMask*/,TUint /*aEntrySortKey*/, CDir*& /*aEntryList*/,CDir*& /*aDirList*/) const
	{
	return KErrCANotSupported;
	}

TInt CTestAgentManager::GetDir(const TDesC& /*aName*/,const TUidType& /*aEntryUid*/,TUint /*aEntrySortKey*/, CDir*& /*aFileList*/) const
	{
	return KErrCANotSupported;
	}

TInt CTestAgentManager::GetAttribute(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath)
	{
	TRAPD(err, GetAttributeL(aAttribute, aValue, aVirtualPath));
	return err;
	}

void CTestAgentManager::GetAttributeL(TInt aAttribute, TInt& aValue, const TVirtualPathPtr& aVirtualPath)
	{
	RFile file;
	CTestAgentFileInterface *fileObject = NULL;

	// check virtual path here to see it's ok
	if(TTestAgentAttributes::CheckVirtualPath(aVirtualPath) != KErrNone)
		{
		User::Leave( KErrNotFound );	
		}

	// open file in order to retrieve an attribute from it
	User::LeaveIfError(file.Open(iFs, aVirtualPath.URI(), EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);

	if(aVirtualPath.UniqueId().Length() == 0)
		{
		fileObject = CTestAgentFile::NewL(file);
		}
	else
		{
		fileObject = CTestAgentDrmContent::NewL(file);
		}
	CleanupStack::PushL(fileObject);

	User::LeaveIfError(fileObject->GetAttribute(aAttribute, aValue));
	CleanupStack::PopAndDestroy(2, &file);
	}

TInt CTestAgentManager::GetAttributeSet(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
	{
	TRAPD(err, GetAttributeSetL(aAttributeSet, aVirtualPath));
	return err;
	}

void CTestAgentManager::GetAttributeSetL(RAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
	{
	RFile file;
	CTestAgentFileInterface *fileObject = NULL;	

	// check virtual path here to see it's ok
	if(TTestAgentAttributes::CheckVirtualPath(aVirtualPath) != KErrNone)
		{
		User::Leave( KErrNotFound );	
		}

	// open file in order to retrieve an attribute from it
	User::LeaveIfError(file.Open(iFs, aVirtualPath.URI(), EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);

	if(aVirtualPath.UniqueId().Length() == 0)
		{
		fileObject = CTestAgentFile::NewL(file);
		}
	else
		{
		fileObject = CTestAgentDrmContent::NewL(file);
		}
	CleanupStack::PushL(fileObject);
	User::LeaveIfError(TTestAgentAttributes::GetAttributeSet(*fileObject, aAttributeSet));
	CleanupStack::PopAndDestroy(2, &file);
	}

TInt CTestAgentManager::GetStringAttributeSet(RStringAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
	{
	TRAPD(err, GetStringAttributeSetL(aAttributeSet, aVirtualPath));
	return err;
	}

void CTestAgentManager::GetStringAttributeSetL(RStringAttributeSet& aAttributeSet, const TVirtualPathPtr& aVirtualPath)
	{
	RFile file;
	CTestAgentFileInterface *fileObject = NULL;
	
	// check virtual path here to see it's ok
	if(TTestAgentAttributes::CheckVirtualPath(aVirtualPath) != KErrNone)
		{
		User::Leave(KErrNotFound);	
		}

	// open file in order to retrieve an attribute from it
	User::LeaveIfError(file.Open(iFs, aVirtualPath.URI(), EFileStream | EFileRead | EFileShareReadersOnly));

	CleanupClosePushL(file);

	if(aVirtualPath.UniqueId().Length() == 0)
		{
		fileObject = CTestAgentFile::NewL(file);
		}
	else
		{
		fileObject = CTestAgentDrmContent::NewL(file);
		}

	CleanupStack::PushL(fileObject);
	User::LeaveIfError(TTestAgentAttributes::GetStringAttributeSet(*fileObject, aAttributeSet));
	CleanupStack::PopAndDestroy(2, &file);
	}

TInt CTestAgentManager::GetStringAttribute(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath)
	{
	TRAPD(err, GetStringAttributeL(aAttribute, aValue, aVirtualPath));
	return err;
	}

void CTestAgentManager::GetStringAttributeL(TInt aAttribute, TDes& aValue, const TVirtualPathPtr& aVirtualPath)
	{
	RFile file;
	CTestAgentFileInterface *fileObject = NULL;
	
	// check virtual path here to see it's ok
	if(TTestAgentAttributes::CheckVirtualPath(aVirtualPath) != KErrNone)
		{
		User::Leave( KErrNotFound );	
		}

	// open file in order to retrieve an attribute from it
	User::LeaveIfError(file.Open(iFs, aVirtualPath.URI(), EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(file);

	if(aVirtualPath.UniqueId().Length() == 0)
		{
		fileObject = CTestAgentFile::NewL(file);
		}
	else
		{
		fileObject = CTestAgentDrmContent::NewL(file);
		}
	CleanupStack::PushL(fileObject);

	User::LeaveIfError(fileObject->GetStringAttribute(aAttribute, aValue));
	CleanupStack::PopAndDestroy(2, &file);
	}

void CTestAgentManager::NotifyStatusChange(const TDesC& /*aURI*/, TEventMask /*aMask*/, TRequestStatus& aStatus)
	{
	TRequestStatus *ptr = &aStatus;
	User::RequestComplete(ptr, KErrCANotSupported);
	}

TInt CTestAgentManager::CancelNotifyStatusChange(const TDesC& /*aURI*/, TRequestStatus& /*aStatus*/)
	{
	return KErrCANotSupported;
	}

TInt CTestAgentManager::SetProperty(TAgentProperty /*aProperty*/, TInt /*aValue*/)
	{
	return KErrNone;
	}

void CTestAgentManager::DisplayInfoL(TDisplayInfo /*aInfo*/, const TVirtualPathPtr& /*aVirtualPath*/)
	{
	User::Leave(KErrCANotSupported);
	}	

TBool CTestAgentManager::IsRecognizedL(const TDesC& aUri, TContentShareMode /*aShareMode*/) const
	{
	// This is a test implementation of IsRecognizedL
	// For testing purposes it only recognizes DCF content files

	TInt result;
	
	// Convert URI to uppercase
	HBufC *uriUpper = aUri.AllocLC();
	TPtr uriPtr(uriUpper->Des());
	uriPtr.UpperCase();

	// Check that the file extension is .DRM
	TPtrC extension = uriPtr.Right(KDrmContentExtension().Length());
	result = extension.Compare(KDrmContentExtension);
	CleanupStack::PopAndDestroy(uriUpper);
	
	if(result == 0)
		{
		return ETrue;
		}
	else 
		{
		return EFalse;
		}
	}

TBool CTestAgentManager::IsRecognizedL(RFile& aFile) const
	{
	// This is a test implementation of IsRecognizedL
	// For testing purposes it only recognizes DCF content files

	TInt result;
	
	// Convert URI to uppercase
	HBufC *uriUpper = HBufC::NewLC(KMaxFileName);
	TPtr uriPtr(uriUpper->Des());
	aFile.Name(uriPtr);
	uriPtr.UpperCase();

	// Check that the file extension is .DRM
	TPtrC extension = uriPtr.Right(KDrmContentExtension().Length());
	result = extension.Compare(KDrmContentExtension);
	CleanupStack::PopAndDestroy(uriUpper);
	
	if(result == 0)
		{
		return ETrue;
		}
	else 
		{
		return EFalse;
		}
	}

TBool CTestAgentManager::RecognizeFileL(const TDesC& aFileName, const TDesC8& aBuffer, TDes8& aFileMimeType, TDes8& aContentMimeType) const
	{
	// This is a test implementation of DoRecognizeL
	// For testing purposes it only recognizes DCF content files
	TInt result;

	// Convert filename to uppercase
	HBufC *FileNameUpper = aFileName.AllocLC();
	TPtr FileNamePtr(FileNameUpper->Des());
	FileNamePtr.UpperCase();

	// Check filename extension
	TPtrC extension = FileNamePtr.Right(KDrmContentExtension().Length());
	result = extension.Compare(KDrmContentExtension);
	CleanupStack::PopAndDestroy(); // FileNameUpper

	if(result == 0)
		{
		// It's a .DRM file
		aFileMimeType.Copy(KDrmMimeType);
	
		if(aBuffer.Length() > 3)
			{
			// Find content type 
			// content-type length is a single byte, stored at offset 0 in the DRM file
			TUint8 length = aBuffer[0];

			// find out if the file has been processed through supplier
			// interface yet
			if(aBuffer[length + 1] == (TUint8)'T')
				{
				// content type always starts at offset 1 in the DCF file
				aContentMimeType.Copy(aBuffer.Mid(1,length));
				}
			}
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}


_LIT(KAgentSpecificCommandResponse, "output");

TInt CTestAgentManager::AgentSpecificCommand(TInt aCommand, const TDesC8& /*aInputBuffer*/, TDes8& aOutputBuffer)
	{
	if(aCommand == 1)
		{
		aOutputBuffer.Copy(KAgentSpecificCommandResponse());
		return KErrNone;
		}
	return KErrCANotSupported;
	}

void CTestAgentManager::AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus)
	{
	TRequestStatus *ptr = &aStatus;
	User::RequestComplete(ptr, AgentSpecificCommand(aCommand, aInputBuffer, aOutputBuffer));
	}

void CTestAgentManager::DisplayManagementInfoL()
	{
	User::Leave(KErrCANotSupported);
	}

//
// This API is non-leaving and returns void. If we want to
// make leavescan happy, the options are: 1. TRAP leave calls
// and ignore the errors. 2. Panic. Both options could potentially
// break RTA clients' testcases. Hence just leave it the way it is.
#ifdef INTERNALLY_ENABLE_UPWARD_DEPENDENCY
void CTestAgentManager::PrepareHTTPRequestHeaders(RStringPool& aStringPool, RHTTPHeaders& aRequestHeaders) const
	{
	TBuf8 <KMaxDataTypeLength> mimeType;
	mimeType.Copy(KDrmMimeType());
	RStringF valStr = aStringPool.OpenFStringL(mimeType);
	CleanupClosePushL(valStr);
	THTTPHdrVal val(valStr);
	aRequestHeaders.SetFieldL(aStringPool.StringF(HTTP::EAccept, RHTTPSession::GetTable()), val);
	CleanupStack::PopAndDestroy(&valStr);
	}
#else
void CTestAgentManager::PrepareHTTPRequestHeaders(RStringPool& /*aStringPool*/, RHTTPHeaders& /*aRequestHeaders*/) const
	{
	}
#endif

