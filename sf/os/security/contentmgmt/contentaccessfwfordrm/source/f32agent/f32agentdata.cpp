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
#include "f32agentdata.h"
#include "f32defaultattributes.h"
#include <caf/virtualpath.h>
#include <caf/f32agentui.h>
#include <e32def.h>

using namespace ContentAccess;

CF32AgentData* CF32AgentData::NewL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	CF32AgentData* self = new (ELeave) CF32AgentData;
	CleanupStack::PushL(self);
	self->ConstructL(aVirtualPath, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

CF32AgentData* CF32AgentData::NewL(RFile& aFile, const TDesC& aUniqueId)
	{
	CF32AgentData* self = new (ELeave) CF32AgentData;
	CleanupStack::PushL(self);
	self->ConstructL(aFile, aUniqueId);
	CleanupStack::Pop(self);
	return self;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

CF32AgentData* CF32AgentData::NewL(const TDesC8& aHeaderData)
	{
	CF32AgentData* self = new (ELeave) CF32AgentData;
	CleanupStack::PushL(self);
	self->ConstructL(aHeaderData);
	CleanupStack::Pop(self);
	return self;
	}
	
void CF32AgentData::ConstructL(const TDesC8& aHeaderData)
	{
	if(aHeaderData.Length() > 0)
		iHeaderData = aHeaderData.AllocL();
	else
		User::Leave(KErrMissingWmdrmHeaderData);
	}
	
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

CF32AgentData::CF32AgentData()
	{
	}

CF32AgentData::~CF32AgentData()
	{
	// Tidy up RFile and RFs
	iFile.Close();
	if(iVirtualPath) // opened by name
		{
		iFs.Close();
		}

	delete iVirtualPath;
	
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	delete iHeaderData;			
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	}
  
void CF32AgentData::ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	iVirtualPath = CVirtualPath::NewL(aVirtualPath);
	
	// Check that the client hasn't specified some incorrect UniqueId
	User::LeaveIfError(TF32DefaultAttributes::CheckUniqueId(aVirtualPath.UniqueId()));

	TUint mode = TF32DefaultAttributes::GetFileMode(aShareMode);
	User::LeaveIfError(iFs.Connect());

	// Make the file session shareable
	User::LeaveIfError(iFs.ShareAuto());

	User::LeaveIfError(iFile.Open(iFs, aVirtualPath.URI(), mode));
	}

void CF32AgentData::ConstructL(RFile& aFile, const TDesC& aUniqueId)
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API		
	TInt64 pos = 0;
#else
	TInt pos = 0;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	// Check that the client hasn't specified some incorrect UniqueId
	User::LeaveIfError(TF32DefaultAttributes::CheckUniqueId(aUniqueId));	
	
	// When creating a CData from a file handle we must duplicate the file handle
	// before doing anything
	User::LeaveIfError(iFile.Duplicate(aFile));
	User::LeaveIfError(iFile.Seek(ESeekStart, pos));  // reset to start of file
	}

void CF32AgentData::DataSizeL(TInt &aSize)
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TInt64 size;
	User::LeaveIfError(iFile.Size(size));
	aSize=size;
#else
	User::LeaveIfError(iFile.Size(aSize));
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
void CF32AgentData::DataSize64L(TInt64 &aSize)
	{
	User::LeaveIfError(iFile.Size(aSize));
	}
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

TInt CF32AgentData::EvaluateIntent(TIntent /*aIntent*/)
	{
	return KErrNone;
	}

TInt CF32AgentData::ExecuteIntent(TIntent /*aIntent*/)
	{
	return KErrNone;
	}

TInt CF32AgentData::Read(TDes8& aDes) 
	{
	return iFile.Read(aDes);
	}

TInt CF32AgentData::Read(TDes8& aDes,TInt aLength) 
	{
	return iFile.Read(aDes,aLength);
	}

void CF32AgentData::Read(TDes8& aDes,TRequestStatus& aStatus) 
	{
	iFile.Read(aDes, aStatus);
	}

void CF32AgentData::Read(TDes8& aDes,
							 TInt aLength, 
							 TRequestStatus& aStatus) 
	{
	iFile.Read(aDes, aLength, aStatus);
	}
	
TInt CF32AgentData::Read(TInt aPos, TDes8& aDes,
							 TInt aLength, 
							 TRequestStatus& aStatus) 
	{
	iFile.Read(aPos, aDes, aLength, aStatus);
	return KErrNone;
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CF32AgentData::Read64(TInt64 aPos, TDes8& aDes,
							 TInt aLength, 
							 TRequestStatus& aStatus) 
	{
	iFile.Read(aPos, aDes, aLength, aStatus);
	return KErrNone;
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	
void CF32AgentData::ReadCancel(TRequestStatus& aStatus)
	{
	iFile.ReadCancel(aStatus);
	}

TInt CF32AgentData::Seek(TSeek aMode, TInt& aPos) 
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TInt64 pos = aPos;
	TInt offset = iFile.Seek(aMode, pos);
	aPos = I64INT(pos);
#else
	TInt offset = iFile.Seek(aMode, aPos);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	return offset;
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CF32AgentData::Seek64(TSeek aMode, TInt64& aPos) 
	{
	return iFile.Seek(aMode, aPos);
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API


TInt CF32AgentData::SetProperty(TAgentProperty aProperty, TInt aValue)
	{
	
	if(aProperty==EAgentPropertyAgentUI)
		//	should only pass type EAgentPropertyAgentUI 
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

TInt CF32AgentData::GetAttribute(TInt aAttribute, TInt& aValue)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	if(iHeaderData)
		{
		return TF32DefaultAttributes::GetAttribute(*iHeaderData, aAttribute, aValue);
		}
		
	else if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetAttribute(aAttribute, aValue, iVirtualPath->URI());
		}
		
	else
		{
		return TF32DefaultAttributes::GetAttribute(aAttribute, aValue, iFile);
		}
#else
	if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetAttribute(aAttribute, aValue, iVirtualPath->URI());
		}
	else
		{
		return TF32DefaultAttributes::GetAttribute(aAttribute, aValue, iFile);
		}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	}

TInt CF32AgentData::GetAttributeSet(RAttributeSet& aAttributeSet)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	if(iHeaderData)
		{
		return TF32DefaultAttributes::GetAttributeSet(*iHeaderData, aAttributeSet);
		}
		
	else if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetAttributeSet(aAttributeSet, iVirtualPath->URI());
		}
			
	else
		{
		return TF32DefaultAttributes::GetAttributeSet(aAttributeSet, iFile);
		}
#else
	if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetAttributeSet(aAttributeSet, iVirtualPath->URI());
		}	
	else
		{
		return TF32DefaultAttributes::GetAttributeSet(aAttributeSet, iFile);
		}	
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT		
	}

TInt CF32AgentData::GetStringAttribute(TInt aAttribute, TDes& aValue)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	if(iHeaderData)
		{
		return TF32DefaultAttributes::GetStringAttribute(*iHeaderData, aAttribute, aValue);
		}
	
	else if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, iVirtualPath->URI());
		}
	else
		{
		return TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, iFile);
		}
#else
	if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, iVirtualPath->URI());
		}
	else
		{
		return TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, iFile);
		}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT	
	}

TInt CF32AgentData::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	if(iHeaderData)
		{
		return TF32DefaultAttributes::GetStringAttributeSet(*iHeaderData, aStringAttributeSet);
		}
		
	else if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, iVirtualPath->URI());
		}
	else
		{
		return TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, iFile);
		}
#else
	if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, iVirtualPath->URI());
		}
	else
		{
		return TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, iFile);
		}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	}

CF32AgentUi& CF32AgentData::AgentUiL()
	{
	if(!iAgentUi)
		{
		// load agent UI from f32AgentUi.dll
		iAgentUi = TF32AgentUiFactory::CreateF32AgentUiL();
		}
	return *iAgentUi;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

TInt CF32AgentData::Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket)
	{
	if(aEncryptedInputDataPacket.Length() <= 0)
		{
		return KErrInsufficientDataPacketLength;
		}
	
	aDecryptedOutputDataPacket = aEncryptedInputDataPacket;
	return KErrNone;
	}
			
void CF32AgentData::Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	if(aEncryptedInputDataPacket.Length() <= 0)
		{
		User::RequestComplete(status, KErrInsufficientDataPacketLength);		
		}
	
	aDecryptedOutputDataPacket = aEncryptedInputDataPacket;
	User::RequestComplete(status, KErrNone);
	}

#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
