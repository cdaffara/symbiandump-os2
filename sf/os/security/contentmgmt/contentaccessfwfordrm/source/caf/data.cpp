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


#include <apmstd.h>

#include <caf/data.h>
#include <caf/agentinterface.h>
#include "agentinfo.h"
#include <caf/agentfactory.h>
#include <caf/attributeset.h>
#include <caf/virtualpath.h>
#include "resolver.h"
#include "agentinfo.h"
#include <e32def.h>

using namespace ContentAccess;

EXPORT_C CData* CData::NewL(const TVirtualPathPtr& aVirtualPath, TIntent aIntent, TContentShareMode aShareMode)
	{
	CData *self = CData::NewLC(aVirtualPath, aIntent, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CData* CData::NewLC(const TVirtualPathPtr& aVirtualPath, TIntent aIntent, TContentShareMode aShareMode)
	{
	CData *self = new (ELeave) CData;
	CleanupStack::PushL(self);
	self->ConstructL(aVirtualPath, aIntent, aShareMode);
	return self;
	}

EXPORT_C CData* CData::NewL(RFile& aFile, const TDesC& aUniqueId, TIntent aIntent)
	{
	CData *self = CData::NewLC(aFile, aUniqueId, aIntent);
	CleanupStack::Pop(self);
	return self;
	}
		
EXPORT_C CData* CData::NewLC(RFile& aFile, const TDesC& aUniqueId, TIntent aIntent)
	{
	CData *self = new (ELeave) CData;
	CleanupStack::PushL(self);
	self->ConstructL(aFile, aUniqueId, aIntent);
	return self;
	}


EXPORT_C CData* CData::NewL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	CData *self = CData::NewLC(aVirtualPath, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CData* CData::NewLC(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	CData *self = new (ELeave) CData;
	CleanupStack::PushL(self);
	self->ConstructL(aVirtualPath, aShareMode);
	return self;
	}

EXPORT_C CData* CData::NewL(RFile& aFile, const TDesC& aUniqueId)
	{
	CData *self = CData::NewLC(aFile, aUniqueId);
	CleanupStack::Pop(self);
	return self;
	}
		
EXPORT_C CData* CData::NewLC(RFile& aFile, const TDesC& aUniqueId)
	{
	CData *self = new (ELeave) CData;
	CleanupStack::PushL(self);
	self->ConstructL(aFile, aUniqueId);
	return self;
	}


CData* CData::NewLC(TUid aUid,
					const TVirtualPathPtr& aVirtualPath,
					TIntent aIntent,
					TContentShareMode aShareMode)
	{
	CData* self = new(ELeave) CData();
	CleanupStack::PushL(self);
	self->ConstructL(aUid, aVirtualPath, aIntent, aShareMode);
	return self;
	}

CData* CData::NewLC(TUid aUid, RFile& aFile, const TDesC& aUniqueId, TIntent aIntent)
	{
	CData* self = new (ELeave) CData();
	CleanupStack::PushL(self);
	self->ConstructL(aUid, aFile, aUniqueId, aIntent);
	return self;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

EXPORT_C CData* CData::NewL(const TDesC8& aHeaderData)
	{
	CData* self = CData::NewLC(aHeaderData);
	CleanupStack::Pop(self);
	return self;
	}
		
EXPORT_C CData* CData::NewL(const TDesC8& aHeaderData, TIntent aIntent)
	{
	CData* self = CData::NewLC(aHeaderData, aIntent);
	CleanupStack::Pop(self);
	return self;
	}
		
EXPORT_C CData* CData::NewLC(const TDesC8& aHeaderData)
	{
	CData* self = new (ELeave) CData();
	CleanupStack::PushL(self);
	self->ConstructL(aHeaderData);
	return self;
	}
		
EXPORT_C CData* CData::NewLC(const TDesC8& aHeaderData, TIntent aIntent)
	{
	CData* self = new (ELeave) CData();
	CleanupStack::PushL(self);
	self->ConstructL(aHeaderData, aIntent);
	return self;
	}
		
CData* CData::NewLC(TUid aAgentUid, const TDesC8& aHeaderData, TIntent aIntent)
	{
	CData* self = new (ELeave) CData();
	CleanupStack::PushL(self);
	self->ConstructL(aAgentUid, aHeaderData, aIntent);
	return self;
	}

#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

CData::CData()
	{ 
	}

CData::~CData() 
	{ 
	// Destroy the agent implementation
	delete iAgentData;
	
	// close ECOM session, may result in the unloading of the DLL
	delete iAgentFactory;
	REComSession::FinalClose();
	}


void CData::ConstructL(const TVirtualPathPtr& aVirtualPath, TIntent aIntent, TContentShareMode aShareMode)
	{
	// Create the agent resolver which will contains a reference to
	// the agent responsible for this piece of content
	CAgentResolver* resolver = CAgentResolver::NewLC(EFalse);
	
	// Create a temporary buffer used to store the translated version of the URI
	HBufC* actualUri = HBufC::NewLC(aVirtualPath.URI().Length() + KMaxSIDLength);
	TPtr uri = actualUri->Des();
	
	// Find the agent who handles the file and translate the URI if it is pointing to a private directory 
	CAgentInfo& agentInfo = resolver->ResolveFileL(aVirtualPath.URI(), uri, aShareMode);

	// Figure out the Uid of the agent
	TUid agentUid = agentInfo.Agent().ImplementationUid();
	
	// call internal CData::NewL()
	CData::ConstructL(agentUid, TVirtualPathPtr(*actualUri, aVirtualPath.UniqueId()), aIntent, aShareMode);
	
	CleanupStack::PopAndDestroy(2, resolver); // actualUri, resolver
	}

void CData::ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	// Create the agent resolver which will contains a reference to
	// the agent responsible for this piece of content
	CAgentResolver* resolver = CAgentResolver::NewLC(EFalse);
	
	// Create a temporary buffer used to store the translated version of the URI
	HBufC* actualUri = HBufC::NewLC(aVirtualPath.URI().Length() + KMaxSIDLength);
	TPtr uri = actualUri->Des();
	
	// Find the agent who handles the file and translate the URI if it is pointing to a private directory 
	CAgentInfo& agentInfo = resolver->ResolveFileL(aVirtualPath.URI(), uri, aShareMode);

	// Figure out the Uid of the agent
	TUid agentUid = agentInfo.Agent().ImplementationUid();
	
	// call internal CData::NewL()
	CData::ConstructL(agentUid, TVirtualPathPtr(*actualUri, aVirtualPath.UniqueId()), aShareMode);
	
	CleanupStack::PopAndDestroy(2, resolver); // actualUri, resolver
	}

void CData::ConstructL(RFile& aFile, const TDesC& aUniqueId, TIntent aIntent)
	{
	// Find the agent who handles the file
	CAgentResolver* resolver = CAgentResolver::NewLC(EFalse);
	CAgentInfo& agentInfo = resolver->ResolveFileL(aFile);

	// Figure out the Uid of the agent
	TUid agentUid = agentInfo.Agent().ImplementationUid();

	// call other constructL now that we have the Uid
	CData::ConstructL(agentUid, aFile, aUniqueId, aIntent);

	// Cleanup resolver after creating CData to unnecessary 
	// unloading and loading of agent plug-ins
	CleanupStack::PopAndDestroy(resolver); 
	}

void CData::ConstructL(RFile& aFile, const TDesC& aUniqueId)
	{
	// Find the agent who handles the file
	CAgentResolver* resolver = CAgentResolver::NewLC(EFalse);
	CAgentInfo& agentInfo = resolver->ResolveFileL(aFile);

	// Figure out the Uid of the agent
	TUid agentUid = agentInfo.Agent().ImplementationUid();

	// call other constructL now that we have the Uid
	CData::ConstructL(agentUid, aFile, aUniqueId);

	// Cleanup resolver after creating CData to unnecessary 
	// unloading and loading of agent plug-ins
	CleanupStack::PopAndDestroy(resolver); 
	}

void CData::ConstructL(TUid aUid,
					   const TVirtualPathPtr& aVirtualPath,
					   TIntent aIntent,
					   TContentShareMode aShareMode)
	{
	CData::ConstructL(aUid, aVirtualPath, aShareMode);

	// Ensure that client's intent is possible
	User::LeaveIfError(iAgentData->EvaluateIntent(aIntent));
	}

void CData::ConstructL(TUid aUid,
					   const TVirtualPathPtr& aVirtualPath,
					   TContentShareMode aShareMode)
	{
	iAgentUid = aUid;

	// create our own agent factory (de-facto session handle for ECOM)
	iAgentFactory = CAgentFactory::NewL(iAgentUid);

	// Create an instance of the Agent's CAgentData
	iAgentData = iAgentFactory->CreateDataConsumerL(aVirtualPath, aShareMode);

	}

void CData::ConstructL(TUid aUid, RFile& aFile, const TDesC& aUniqueId, TIntent aIntent)
	{
	CData::ConstructL(aUid, aFile, aUniqueId);

	// Ensure that client's intent is possible
	User::LeaveIfError(iAgentData->EvaluateIntent(aIntent));
	}

void CData::ConstructL(TUid aUid, RFile& aFile, const TDesC& aUniqueId)
	{
	iAgentUid = aUid;

	// create our own agent factory (de-facto session handle for ECOM)
	iAgentFactory = CAgentFactory::NewL(iAgentUid);

	// Create an instance of the agent's CAgentData
	// The agent must make a duplicate of the file handle
	iAgentData = iAgentFactory->CreateDataConsumerL(aFile, aUniqueId);
  	}
  
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
  	
void CData::ConstructL(const TDesC8& aHeaderData)
	{
	// Find the agent who handles the file
	CAgentResolver* resolver = CAgentResolver::NewLC(EFalse);
	CAgentInfo& agentInfo = resolver->ResolveFileL(aHeaderData);
	
	// Figure out the Uid of the agent
	TUid agentUid = agentInfo.Agent().ImplementationUid();
	CData::ConstructL(agentUid, aHeaderData);
	
	// Cleanup resolver after creating CData to unnecessary 
	// unloading and loading of agent plug-ins
	CleanupStack::PopAndDestroy(resolver); 
	}

void CData::ConstructL(const TDesC8& aHeaderData, TIntent aIntent)
	{
	CData::ConstructL(aHeaderData);
	
	// Ensure that client's intent is possible
	User::LeaveIfError(iAgentData->EvaluateIntent(aIntent));
	}
	
void CData::ConstructL(TUid aAgentUid, const TDesC8& aHeaderData)
	{
	iAgentUid = aAgentUid;
	
	iAgentFactory = CAgentFactory::NewL(iAgentUid);
	// create an instance of agent's CAgentStreamData.
	iAgentData = iAgentFactory->CreateDataConsumerL(aHeaderData);
	}

void CData::ConstructL(TUid aUid, const TDesC8& aHeaderData, TIntent aIntent)
	{
	CData::ConstructL(aUid, aHeaderData);
	
	// Ensure that client's intent is possible
	User::LeaveIfError(iAgentData->EvaluateIntent(aIntent));
	}
	
#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	
EXPORT_C void CData::DataSizeL(TInt& aSize)
	{
	// ask the agent for the data size of the current content object
	iAgentData->DataSizeL(aSize);
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C void CData::DataSize64L(TInt64& aSize)
	{
	// ask the agent for the data size of the current content object
	TRAPD(err, iAgentData->DataSize64L(aSize));
	if(err == KErrCANotSupported)
		{
		//fallback to 32bit API
		TInt size32;
		iAgentData->DataSizeL(size32);
		aSize = size32;
		}
	else
		User::LeaveIfError(err);
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

EXPORT_C TInt CData::EvaluateIntent(TIntent aIntent)
	{
	// ask the agent to re-evaluate the intent on the current content object
	return iAgentData->EvaluateIntent(aIntent);
	}

EXPORT_C TInt CData::ExecuteIntent(TIntent aIntent)
	{
	// ask the agent to re-evaluate the intent on the current content object
	return iAgentData->ExecuteIntent(aIntent);
	}

EXPORT_C TInt CData::Read(TDes8& aDes) const
	{
	// ask the agent to read plaintext from the content object 
	return iAgentData->Read(aDes);
	}

EXPORT_C TInt CData::Read(TDes8& aDes, TInt aLength) const
	{
	// ask the agent to read plaintext from the content object 
	return iAgentData->Read(aDes, aLength);
	}

EXPORT_C void CData::Read(TDes8& aDes, 
						  TRequestStatus& aStatus) const
	{
	// ask the agent to read plaintext from the content object 
	iAgentData->Read(aDes, aStatus);
	}

EXPORT_C void CData::Read(TDes8& aDes, 
						  TInt aLength, 
						  TRequestStatus& aStatus) const
	{
	// ask the agent to read plaintext from the content object 
	iAgentData->Read(aDes, aLength, aStatus);
	}
	
EXPORT_C void CData::ReadCancel(TRequestStatus &aStatus) const
	{
	iAgentData->ReadCancel(aStatus);
	}
	
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

EXPORT_C TInt CData::Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputPacket) const
	{
	return iAgentData->Read(aEncryptedInputDataPacket, aDecryptedOutputPacket);
	}
		
EXPORT_C void CData::Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputPacket, TRequestStatus& aStatus) const
	{
	iAgentData->Read(aEncryptedInputDataPacket, aDecryptedOutputPacket, aStatus);
	}

#endif //#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	EXPORT_C TInt CData::Read_Unused(TInt aPos, TDes8& aDes, 
								TInt aLength, TRequestStatus& aStatus) const
#else
	EXPORT_C TInt CData::Read(TInt aPos, TDes8& aDes, 
								TInt aLength, TRequestStatus& aStatus) const
#endif
	{
	// ask the agent to read plaintext from the content object 
	if(aPos<0)
		return KErrArgument;
	return iAgentData->Read(aPos, aDes, aLength, aStatus);
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt CData::Read(TInt64 aPos, TDes8& aDes,
						  TInt aLength, TRequestStatus& aStatus) const
	{
	// If agent does not support 64bit Read, a fallback to 32bit Read is provided automatically
	// The above API method signature allows error codes to be returned
	// via two different routes (return code & aStatus). Should the async request 
	// dispatch fail then an error code is returned immediately via the 
	// aStatus parameter rather than via the return code of the function.	

	// NOTE: it is not generally not recommended for functions to be able to return
	// error codes in two different ways as this places a bigger error-checking
	// burden on clients.
	if(aPos<0)
		return KErrArgument;
	
	TInt rval = iAgentData->Read64(aPos, aDes, aLength, aStatus);
	return (rval == KErrCANotSupported? iAgentData->Read((TInt)aPos, aDes, aLength, aStatus) : rval );
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

EXPORT_C TInt CData::Seek(TSeek aMode,TInt& aPos) const
	{
	// ask the agent to seek witin the plaintext 
	return iAgentData->Seek(aMode, aPos);
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt CData::Seek64(TSeek aMode,TInt64& aPos) const
	{
	// ask the agent to seek witin the plaintext 
	TInt rval64 = iAgentData->Seek64(aMode, aPos);
	if(rval64 == KErrCANotSupported)
		{
		//fallback to 32bit API
		TInt pos32 = I64INT(aPos);
		TInt rval32 = iAgentData->Seek(aMode, pos32);
		aPos = pos32;
		return rval32;
		}
	return rval64;
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

EXPORT_C TInt CData::SetProperty(TAgentProperty aProperty, TInt aValue)
	{
	// Set a property within the agent
	return iAgentData->SetProperty(aProperty, aValue);
	}

EXPORT_C TInt CData::GetAttribute(TInt aAttribute, TInt& aValue) const
	{
	// Get an attribute related to this content object
	return iAgentData->GetAttribute(aAttribute, aValue);
	}

EXPORT_C TInt CData::GetAttributeSet(RAttributeSet& aAttributeSet) const
	{
	// Get a set of attributes related to this content object
	return iAgentData->GetAttributeSet(aAttributeSet);
	}

EXPORT_C TInt CData::GetStringAttribute(TInt aAttribute, TDes& aValue) const
	{
	// Get a string attribute related to this content object
	return iAgentData->GetStringAttribute(aAttribute, aValue);
	}

EXPORT_C TInt CData::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet) const
	{
	// Get a set of string attributes related to this content object
	return iAgentData->GetStringAttributeSet(aStringAttributeSet);
	}

#ifndef REMOVE_CAF1
EXPORT_C void CData::SetQosL(TQosAttribute aQosAttr, TInt aValue)
	{
	if(aQosAttr == EQosBufferSize)
		{
		User::LeaveIfError(iAgentData->SetProperty(EAgentPropertyBufferSize, aValue));
		}
	}

EXPORT_C TBool CData::GetMimeTypeL(TDes8& aMimeType) const
	{
	TBuf16 <KMaxDataTypeLength> mimeType;
	TInt err = KErrNone;
	if(iAgentUid != KF32AgentImplUid)
		{
		err = iAgentData->GetStringAttribute(EMimeType, mimeType);
		if(err == KErrNone && mimeType.Length() != 0)
			{
			aMimeType.Copy(mimeType);
			return ETrue;
			}
		}
	return EFalse;
	}
#endif // REMOVE_CAF1
