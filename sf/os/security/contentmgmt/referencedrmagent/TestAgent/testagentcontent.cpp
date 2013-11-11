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
#include "testagentcontent.h"
#include "testagentattributes.h"

using namespace ContentAccess;

_LIT(KDummyContainer,"DummyContainer");

CTestAgentContent* CTestAgentContent::NewL(const TDesC& aURI, TContentShareMode aShareMode)
	{
	CTestAgentContent* self = NewLC(aURI, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentContent* CTestAgentContent::NewLC(const TDesC& aURI, TContentShareMode aShareMode)
	{
	CTestAgentContent* self=new(ELeave) CTestAgentContent();
	CleanupStack::PushL(self);
	self->ConstructL(aURI, aShareMode);
	return self;
	}

CTestAgentContent* CTestAgentContent::NewL(RFile& aFile)
	{
	CTestAgentContent* self = NewLC(aFile);
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentContent* CTestAgentContent::NewLC(RFile& aFile)
	{
	CTestAgentContent* self=new(ELeave) CTestAgentContent();
	CleanupStack::PushL(self);
	self->ConstructL(aFile);
	return self;
	}


CTestAgentContent::CTestAgentContent() 
	{
	iDummyContainerOpen=EFalse;
	}

CTestAgentContent::~CTestAgentContent()
	{
	delete iTestFile;
	delete iTestDrmContent;
	delete iURI;

	iFile.Close();
	iFs.Close();
	}
  
void CTestAgentContent::ConstructL(const TDesC& aURI, TContentShareMode aShareMode)
	{
	TFileName dummyFileName;
	TTestAgentAttributes::GetDummyFileName(aURI, dummyFileName);
	
	iURI = dummyFileName.AllocL();

	TUint mode = EFileStream | EFileRead;

	if(aShareMode == EContentShareReadWrite)
		{
		mode |= EFileShareReadersOrWriters;
		}
	else if (aShareMode == EContentShareExclusive)
		{
		mode |= EFileShareExclusive;
		}
	else
		{
		mode |= EFileShareReadersOnly;
		}

	// open underlying file handle
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFile.Open(iFs, *iURI, mode));

	// Create DRM file objects
	iTestFile = CTestAgentFile::NewL(iFile);
	iTestDrmContent = CTestAgentDrmContent::NewL(iFile);
	
	// reset file pointer back to the beginning of the file
	TInt pos = 0;
	iFile.Seek(ESeekStart,pos);
	iShareMode = aShareMode;
	}

void CTestAgentContent::ConstructL(RFile& aFile)
	{
	User::LeaveIfError(iFile.Duplicate(aFile));

	// Create file objects
	iTestFile = CTestAgentFile::NewL(iFile);
	iTestDrmContent = CTestAgentDrmContent::NewL(iFile);
	
	// reset file pointer back to the beginning of the file
	TInt pos = 0;
	iFile.Seek(ESeekStart, pos);
	}

TInt CTestAgentContent::OpenContainer(const TDesC& aUniqueId)
	{
	if(!iDummyContainerOpen && aUniqueId == KDummyContainer())
		{
		iDummyContainerOpen = ETrue;
		return KErrNone;
		}
	return KErrNotFound;
	}

TInt CTestAgentContent::CloseContainer()
	{
	if(iDummyContainerOpen)
		{
		iDummyContainerOpen = EFalse;
		return KErrNone;
		}
	return KErrNotFound;
	}

void CTestAgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray)
	{
	if(!iDummyContainerOpen)
		{
		// The content object
		CEmbeddedObject *embeddedObject = CEmbeddedObject::NewL(KDefaultContentObject(), *iURI, iTestDrmContent->ContentMimeType(), EContentObject);
		CleanupStack::PushL(embeddedObject);
		aArray.AppendL(embeddedObject);
		// Now owned by the array so do not destroy
		CleanupStack::Pop(embeddedObject);
		embeddedObject = NULL;

		// the file itself
		embeddedObject = CEmbeddedObject::NewL(KNullDesC(), *iURI, iTestFile->ContentMimeType(), EContentObject);
		CleanupStack::PushL(embeddedObject);
		aArray.AppendL(embeddedObject);
		// Now owned by the array so do not destroy
		CleanupStack::Pop(embeddedObject);
		embeddedObject = NULL;
		
		// The dummy container
		embeddedObject = CEmbeddedObject::NewL(KDummyContainer(), KDummyContainer(), KNullDesC8(), EContainerObject);
		CleanupStack::PushL(embeddedObject);
		aArray.AppendL(embeddedObject);
		// Now owned by the array so do not destroy
		CleanupStack::Pop(embeddedObject);
		embeddedObject = NULL;
		}
	}

void CTestAgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType)
	{
	CEmbeddedObject *embeddedObject = NULL;
	if(!iDummyContainerOpen)
		{
		// the only embedded object is the file itself
		if(aType == EContentObject)
			{
			// The content object
			embeddedObject = CEmbeddedObject::NewL(KDefaultContentObject(), *iURI, iTestDrmContent->ContentMimeType(), EContentObject);
			CleanupStack::PushL(embeddedObject);
			aArray.AppendL(embeddedObject);
			// Now owned by the array so do not destroy
			CleanupStack::Pop(embeddedObject);
			embeddedObject = NULL;

			// the file itself
			embeddedObject = CEmbeddedObject::NewL(KNullDesC(), *iURI, iTestFile->ContentMimeType(), EContentObject);
			CleanupStack::PushL(embeddedObject);
			aArray.AppendL(embeddedObject);
			// Now owned by the array so do not destroy
			CleanupStack::Pop(embeddedObject);
			embeddedObject = NULL;
			}
		else
			{
			// The dummy container
			embeddedObject = CEmbeddedObject::NewL(KDummyContainer(), KDummyContainer(), KNullDesC8(), EContainerObject);
			CleanupStack::PushL(embeddedObject);
			aArray.AppendL(embeddedObject);
			// Now owned by the array so do not destroy
			CleanupStack::Pop(embeddedObject);
			embeddedObject = NULL;
			}
		}
	}

TInt CTestAgentContent::Search(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecursive)
	{
	TRAPD(err, SearchL(aArray, aMimeType, aRecursive));
	return err;
	}

void CTestAgentContent::SearchL(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool )
	{
	CEmbeddedObject *embeddedObject = NULL;
	if(iTestDrmContent->ContentMimeType() == aMimeType)
		{
		// content object matches required mime type
		embeddedObject = CEmbeddedObject::NewL(KDefaultContentObject(), *iURI, iTestDrmContent->ContentMimeType(), EContentObject);
		}
	else if(aMimeType == iTestFile->ContentMimeType())
		{
		// matches the file itself
		embeddedObject = CEmbeddedObject::NewL(KNullDesC(), *iURI, iTestFile->ContentMimeType(), EContentObject);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	
	CleanupStack::PushL(embeddedObject);
	aArray.AppendL(embeddedObject);
	// Now owned by the array so do not destroy
	CleanupStack::Pop(embeddedObject);
	}

TInt CTestAgentContent::GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TTestAgentAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}

	if(aUniqueId.Length() == 0)
		{
		// If it's the entire file
		return iTestFile->GetAttribute(aAttribute, aValue);
		}
	else if(aUniqueId == KDefaultContentObject())
		{
		// If it's the embedded content
		return iTestDrmContent->GetAttribute(aAttribute, aValue);
		}
	return KErrNotFound;
	}

TInt CTestAgentContent::GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TTestAgentAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
	
	if(aUniqueId.Length() == 0)	
		{
		// The entire file
		return TTestAgentAttributes::GetAttributeSet(*iTestFile, aAttributeSet);
		}
	else if(aUniqueId == KDefaultContentObject())
		{
		// the embedded content
		return TTestAgentAttributes::GetAttributeSet(*iTestDrmContent, aAttributeSet);	
		}
	return KErrNotFound;
	}

TInt CTestAgentContent::GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TTestAgentAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
		
	if(aUniqueId.Length() == 0)
		{
		// If it's the entire file
		return iTestFile->GetStringAttribute(aAttribute, aValue);
		}
	else 
		{
		// If it's the embedded content
		return iTestDrmContent->GetStringAttribute(aAttribute, aValue);
		}
	}

TInt CTestAgentContent::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TTestAgentAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}

	if(aUniqueId.Length() == 0)	
		{
		// The entire file
		return TTestAgentAttributes::GetStringAttributeSet(*iTestFile, aStringAttributeSet);
		}
	else
		{
		// the embedded content
		return TTestAgentAttributes::GetStringAttributeSet(*iTestDrmContent, aStringAttributeSet);	
		}
	}

_LIT(KAgentSpecificCommandResponse, "output");

TInt CTestAgentContent::AgentSpecificCommand(TInt aCommand, const TDesC8&, TDes8& aOutputBuffer)
	{
	if(aCommand == 1)
		{
		aOutputBuffer.Copy(KAgentSpecificCommandResponse());
		return KErrNone;
		}
	return KErrCANotSupported;
	}

void CTestAgentContent::AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus)
	{
	TRequestStatus *ptr = &aStatus;
	User::RequestComplete(ptr, AgentSpecificCommand(aCommand, aInputBuffer, aOutputBuffer));
	}

void CTestAgentContent::NotifyStatusChange(TEventMask , TRequestStatus& aStatus, const TDesC& )
	{
	TRequestStatus *ptr = &aStatus;
	User::RequestComplete(ptr, KErrCANotSupported);
	}

TInt CTestAgentContent::CancelNotifyStatusChange(TRequestStatus& , const TDesC& )
	{
	return KErrCANotSupported;
	}

void CTestAgentContent::RequestRights(TRequestStatus& aStatus , const TDesC& )
	{
	TRequestStatus *ptr = &aStatus;
	User::RequestComplete(ptr, KErrCANotSupported);
	}

TInt CTestAgentContent::CancelRequestRights(TRequestStatus& , const TDesC& )
	{
	return KErrCANotSupported;
	}

void CTestAgentContent::DisplayInfoL(TDisplayInfo , const TDesC& )
	{
	User::Leave(KErrCANotSupported);
	}

TInt CTestAgentContent::SetProperty(TAgentProperty /*aProperty*/, TInt /*aValue*/)
	{
	return KErrNone;
	}


