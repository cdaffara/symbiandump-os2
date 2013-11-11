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


#include <caf/embeddedobject.h>
#include <apmstd.h>
#include "f32agentcontent.h"
#include "f32defaultattributes.h"
#include <caf/f32agentui.h>

using namespace ContentAccess;

CF32AgentContent* CF32AgentContent::NewL(const TDesC& aURI, TContentShareMode aShareMode)
	{
	CF32AgentContent* self = new (ELeave) CF32AgentContent;
	CleanupStack::PushL(self);
	self->ConstructL(aURI, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

CF32AgentContent* CF32AgentContent::NewL(RFile& aFile)
	{
	CF32AgentContent* self = new (ELeave) CF32AgentContent;
	CleanupStack::PushL(self);
	self->ConstructL(aFile);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
CF32AgentContent* CF32AgentContent::NewL(const TDesC8& aHeaderData)
	{
	CF32AgentContent* self = new (ELeave) CF32AgentContent;
	CleanupStack::PushL(self);
	self->ConstructL(aHeaderData);
	CleanupStack::Pop(self);
	return self;
	}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	
CF32AgentContent::CF32AgentContent()
	{
	}

CF32AgentContent::~CF32AgentContent()
	{
	iFile.Close();
	if(iURI)
		{
		// file session only created when file is opened by name
		iFs.Close();
		}
	delete iURI;
	
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT	
	delete iHeaderData;
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	}
  
void CF32AgentContent::ConstructL(const TDesC& aURI, TContentShareMode aShareMode)
	{
	iURI = aURI.AllocL();
	iShareMode = aShareMode;
	
	// Test that the file exists and hold it open so nobody deletes it etc
	TUint mode = TF32DefaultAttributes::GetFileMode(aShareMode);
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFile.Open(iFs, *iURI, mode));
	}

void CF32AgentContent::ConstructL(RFile& aFile)
	{
	User::LeaveIfError(iFile.Duplicate(aFile));
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT	
void CF32AgentContent::ConstructL(const TDesC8& aHeaderData)
	{
	if(aHeaderData.Length() > 0)
		iHeaderData = aHeaderData.AllocL();
	else
		User::Leave(KErrMissingWmdrmHeaderData);
	}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

TInt CF32AgentContent::OpenContainer(const TDesC&)
	{
	return KErrNotFound;
	}

TInt CF32AgentContent::CloseContainer()
	{
	return KErrNotFound;
	}

void CF32AgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray)
	{
	TBuf8 <KMaxDataTypeLength> mimeType;
	CEmbeddedObject *embeddedObject = NULL;
	
	// the only embedded object is the file itself	
	// Try to get the mime type
	mimeType.SetLength(0);
	if(iURI)
		{
		TF32DefaultAttributes::GetMimeTypeL(*iURI, mimeType);
		}
	else
		{
		TF32DefaultAttributes::GetMimeTypeL(iFile, mimeType);
		}	
	embeddedObject = CEmbeddedObject::NewL(KDefaultContentObject(), mimeType, EContentObject);
	CleanupStack::PushL(embeddedObject);
	aArray.AppendL(embeddedObject);
	
	// Now owned by the array so do not destroy
	CleanupStack::Pop(embeddedObject);
	}

void CF32AgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType)
	{
	// the only embedded object is the file itself
	if(aType == EContentObject)
		{
		// just get the default object since there is only one content object
		GetEmbeddedObjectsL(aArray);
		}
	}

void CF32AgentContent::SearchL(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool /* aRecurse */)
	{
	TBuf8 <KMaxDataTypeLength> mimeType;
	
	CEmbeddedObject *embeddedObject = NULL;
	
	// the only embedded object is the file itself	
	if(iURI)
		{
		TF32DefaultAttributes::GetMimeTypeL(*iURI, mimeType);
		}
	else
		{
		TF32DefaultAttributes::GetMimeTypeL(iFile, mimeType);
		}
	
	// Check the file has the correct mime type
	if(aMimeType == mimeType)
		{
		embeddedObject = CEmbeddedObject::NewL(KDefaultContentObject(), mimeType, EContentObject);
		CleanupStack::PushL(embeddedObject);		
		aArray.AppendL(embeddedObject);
	
		// Now owned by the array so do not destroy
		CleanupStack::Pop(embeddedObject);
		}
	else
		{
		// Mime type was incorrect. Set error to indicate no content object
		// of the desired type was found in the file.
		User::Leave(KErrNotFound);
		}
	}

TInt CF32AgentContent::Search(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool aRecurse)
	{
	TRAPD( err, SearchL(aArray, aMimeType, aRecurse) );
	return err;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
TInt CF32AgentContent::GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId)
	{
	
	// check that the unique Id exists
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
	
	TInt err = KErrNone;
	if(iURI)
		{
		err = TF32DefaultAttributes::GetAttribute(aAttribute, aValue, *iURI);
		}
	else if(iHeaderData)
		{
		err = TF32DefaultAttributes::GetAttribute(*iHeaderData, aAttribute, aValue);
		}
	else
		{
		err = TF32DefaultAttributes::GetAttribute(aAttribute, aValue, iFile);
		}
	return err;
	}

#else
TInt CF32AgentContent::GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
	
	TInt err = KErrNone;
	if(iURI)
		{
		err = TF32DefaultAttributes::GetAttribute(aAttribute, aValue, *iURI);
		}
	else
		{
		err = TF32DefaultAttributes::GetAttribute(aAttribute, aValue, iFile);
		}
	return err;
	}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

TInt CF32AgentContent::GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
		
	TInt err = KErrNone;
	if(iURI)
		{
		err = TF32DefaultAttributes::GetAttributeSet(aAttributeSet, *iURI);
		}
	else if(iHeaderData)
		{
		err = TF32DefaultAttributes::GetAttributeSet(*iHeaderData, aAttributeSet);
		}
	else
		{
		err = TF32DefaultAttributes::GetAttributeSet(aAttributeSet, iFile);
		}
	return err;
	}

#else

TInt CF32AgentContent::GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
		
	TInt err = KErrNone;
	if(iURI)
		{
		err = TF32DefaultAttributes::GetAttributeSet(aAttributeSet, *iURI);
		}
	else
		{
		err = TF32DefaultAttributes::GetAttributeSet(aAttributeSet, iFile);
		}
	return err;
	}

#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

TInt CF32AgentContent::GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
		
	TInt err = KErrNone;
	if(iURI)
		{
		err = TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, *iURI);
		}
	else if(iHeaderData)
		{
		err = TF32DefaultAttributes::GetStringAttribute(*iHeaderData, aAttribute, aValue);
		}
	else
		{
		err = TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, iFile);
		}
	return err;
	}

#else
TInt CF32AgentContent::GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
		
	TInt err = KErrNone;
	if(iURI)
		{
		err = TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, *iURI);
		}
	else
		{
		err = TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, iFile);
		}
	return err;
	}
#endif

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

TInt CF32AgentContent::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
	
	TInt err = KErrNone;
	if(iURI)
		{
		err = TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, *iURI);
		}
	else if(iHeaderData)
		{
		err = TF32DefaultAttributes::GetStringAttributeSet(*iHeaderData, aStringAttributeSet);
		}
	else
		{
		err = TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, iFile);
		}
	return err;
	}

#else

TInt CF32AgentContent::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId)
	{
	// check that the unique Id exists
	if(TF32DefaultAttributes::CheckUniqueId(aUniqueId) != KErrNone)
		{
		return KErrNotFound;	
		}
	
	TInt err = KErrNone;
	if(iURI)
		{
		err = TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, *iURI);
		}
	else
		{
		err = TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, iFile);
		}
	return err;
	}

#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

TInt CF32AgentContent::AgentSpecificCommand(TInt , const TDesC8& , TDes8& )
	{
	return KErrCANotSupported;
	}

void CF32AgentContent::AgentSpecificCommand(TInt , const TDesC8& , TDes8& , TRequestStatus& aStatus)
	{
	TRequestStatus* ptr = &aStatus;
	User::RequestComplete(ptr, KErrCANotSupported);
	}

void CF32AgentContent::NotifyStatusChange(TEventMask , TRequestStatus& aStatus, const TDesC& )
	{
	TRequestStatus* ptr = &aStatus;
	User::RequestComplete(ptr, KErrCANotSupported);
	}

TInt CF32AgentContent::CancelNotifyStatusChange(TRequestStatus& , const TDesC& )
	{
	return KErrCANotSupported;
	}

void CF32AgentContent::RequestRights(TRequestStatus& aStatus, const TDesC& )
	{
	TRequestStatus* ptr = &aStatus;
	User::RequestComplete(ptr, KErrCANotSupported);
	}

TInt CF32AgentContent::CancelRequestRights(TRequestStatus& , const TDesC& )
	{
	return KErrCANotSupported;
	}

void CF32AgentContent::DisplayInfoL(TDisplayInfo aInfo, const TDesC& aUniqueId)
	{
	// Check that the client hasn't specified some incorrect UniqueId
	User::LeaveIfError(TF32DefaultAttributes::CheckUniqueId(aUniqueId));


	if(iURI)
		{
		// Open the file handle in order to pass it to the Agent UI
		RFs fs;
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		RFile64 file;
#else
		RFile file;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		
		// default share mode of EFileShareReadersOnly
		TUint mode = EFileShareReadersOnly | EFileStream | EFileRead;

		if(iShareMode == EContentShareReadWrite)
			{
			mode = EFileShareAny | EFileStream | EFileRead;
			}
		else if(iShareMode == EContentShareExclusive)
			{
			mode = EFileShareExclusive | EFileStream | EFileRead;
			}

		
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		User::LeaveIfError(file.Open(fs, *iURI, mode));
		CleanupClosePushL(file);
		AgentUiL().DisplayInfoL(aInfo, file);	
		CleanupStack::PopAndDestroy(2, &fs); // file, fs
		}
	else
		{
		// just pass existing file handle
		AgentUiL().DisplayInfoL(aInfo, iFile);
		}
	}

TInt CF32AgentContent::SetProperty(TAgentProperty aProperty, TInt aValue)
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


CF32AgentUi& CF32AgentContent::AgentUiL()
	{
	if(!iAgentUi)
		{
		// load agent UI from f32AgentUi.dll
		iAgentUi = TF32AgentUiFactory::CreateF32AgentUiL();
		}
	return *iAgentUi;
	}
