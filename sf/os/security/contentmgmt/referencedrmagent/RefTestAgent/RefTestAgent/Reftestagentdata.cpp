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
#include "Reftestagentdata.h"
#include "drmfiles.h"
#include <e32def.h>

using namespace ContentAccess;

CRefTestAgentData* CRefTestAgentData::NewL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	CRefTestAgentData* self = NewLC(aVirtualPath, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

CRefTestAgentData* CRefTestAgentData::NewLC(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	CRefTestAgentData* self=new(ELeave) CRefTestAgentData();
	CleanupStack::PushL(self);
	self->ConstructL(aVirtualPath, aShareMode);
	return self;
	}

CRefTestAgentData* CRefTestAgentData::NewL(RFile& aFile, const TDesC& aUniqueId)
	{	
	CRefTestAgentData* self = NewLC(aFile, aUniqueId);
	CleanupStack::Pop(self);
	return self;
	}

CRefTestAgentData* CRefTestAgentData::NewLC(RFile& aFile, const TDesC& aUniqueId)
	{
	CRefTestAgentData* self=new(ELeave) CRefTestAgentData();
	CleanupStack::PushL(self);
	self->ConstructL(aFile, aUniqueId);
	return self;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
      
CRefTestAgentData* CRefTestAgentData::NewL(const TDesC8& aHeaderData)     
    {     
    CRefTestAgentData* self = NewLC(aHeaderData);     
    CleanupStack::Pop(self);     
    return self;     
    }     
      
CRefTestAgentData* CRefTestAgentData::NewLC(const TDesC8& aHeaderData)     
    {     
    CRefTestAgentData* self = new (ELeave) CRefTestAgentData;     
    CleanupStack::PushL(self);     
    self->ConstructL(aHeaderData);     
    return self;     
    }     
         
void CRefTestAgentData::ConstructL(const TDesC8& aHeaderData)     
    {     
    iHeaderData = aHeaderData.AllocL();     
    User::LeaveIfError(iServer.Open(aHeaderData));     
    }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
  

CRefTestAgentData::CRefTestAgentData()
	{
	}

CRefTestAgentData::~CRefTestAgentData()
	{
	iServer.Close();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    delete iHeaderData;     
#endif 
	}
  
void CRefTestAgentData::ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	// default share mode of EFileShareReadersOnly
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

	User::LeaveIfError(iServer.Open(aVirtualPath.URI(), aVirtualPath.UniqueId(), mode));
	}


void CRefTestAgentData::ConstructL(RFile& aFile, const TDesC& aUniqueId)
	{
	User::LeaveIfError(iServer.Open(aFile, aUniqueId));
	}

void CRefTestAgentData::DataSizeL(TInt &aSize)
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TInt64 size64 = 0;
	User::LeaveIfError(iServer.DataSize(size64));
	aSize = I64INT(size64);
#else
	User::LeaveIfError(iServer.DataSize(aSize));
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
void CRefTestAgentData::DataSize64L(TInt64 &aSize)
	{
	User::LeaveIfError(iServer.DataSize(aSize));
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

TInt CRefTestAgentData::EvaluateIntent(TIntent aIntent)
	{
	// Always allow EPeek
	TInt err = KErrNone;
	if(aIntent != EPeek)
		{
		TRAP(err, iServer.EvaluateIntentL(aIntent));
		}
	return err;
	}

TInt CRefTestAgentData::ExecuteIntent(TIntent aIntent)
	{
	// Always allow EPeek
	TInt err = KErrNone;
	if(aIntent != EPeek)
		{
		TRAP(err, iServer.ExecuteIntentL(aIntent));
		}
	return err;
	}

TInt CRefTestAgentData::Read(TDes8& aDes) 
	{
	return iServer.Read(aDes, aDes.MaxLength());
	}

TInt CRefTestAgentData::Read(TDes8& aDes,TInt aLength) 
	{
	return iServer.Read(aDes, aLength);
	}

void CRefTestAgentData::Read(TDes8& aDes,TRequestStatus& aStatus) 
	{
	TInt err = Read(aDes);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CRefTestAgentData::Read(TDes8& aDes,
							 TInt aLength, 
							 TRequestStatus& aStatus) 
	{
	TInt err = Read(aDes, aLength);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

TInt CRefTestAgentData::Seek(TSeek aMode, TInt& aPos) 
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TInt64 pos64 = aPos;
	TInt rval = iServer.Seek(aMode, pos64);
	aPos = I64INT(pos64);
#else
	TInt rval = iServer.Seek(aMode, aPos);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	return rval;
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CRefTestAgentData::Seek64(TSeek aMode, TInt64& aPos) 
	{
	return iServer.Seek(aMode, aPos);	
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

TInt CRefTestAgentData::SetProperty(TAgentProperty aProperty, TInt aValue)
	{
	return iServer.SetProperty(aProperty, aValue);
	}

TInt CRefTestAgentData::GetAttribute(TInt aAttribute, TInt& aValue)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    if(iHeaderData != NULL)     
        return iServer.GetWmdrmAttribute(aAttribute, aValue);     
    else     
        return iServer.GetAttribute(aAttribute, aValue);     
#else     
    return iServer.GetAttribute(aAttribute, aValue);
#endif
	}

TInt CRefTestAgentData::GetAttributeSet(RAttributeSet& aAttributeSet)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    if(iHeaderData != NULL)     
        return iServer.GetWmdrmAttributeSet(aAttributeSet);     
    else     
        return iServer.GetAttributeSet(aAttributeSet);     
#else 
	return iServer.GetAttributeSet(aAttributeSet);
#endif
	}

TInt CRefTestAgentData::GetStringAttribute(TInt aAttribute, TDes& aValue)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    if(iHeaderData != NULL)     
        return iServer.GetWmdrmStringAttribute(aAttribute, aValue);     
    else     
        return iServer.GetStringAttribute(aAttribute, aValue);     
#else 
	return iServer.GetStringAttribute(aAttribute, aValue);
#endif
	}

TInt CRefTestAgentData::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    if(iHeaderData != NULL)     
        return iServer.GetWmdrmStringAttributeSet(aStringAttributeSet);     
    else     
        return iServer.GetStringAttributeSet(aStringAttributeSet);     
#else    
	return iServer.GetStringAttributeSet(aStringAttributeSet);
#endif
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CRefTestAgentData::Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	return Read64(aPos, aDes, aLength, aStatus);
	}
#endif

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CRefTestAgentData::Read64(TInt64 aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
#else
TInt CRefTestAgentData::Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{

	TInt err = iServer.Read(aPos, aDes, aLength);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	return KErrNone;
	}

void CRefTestAgentData::ReadCancel(TRequestStatus&)
	{
	// Reference Agent does not perform true asynchronous read operations, all Read client-server interaction
	// is done synchronously. Thus, it makes no sense to provide an asynchronous cancellation, as there
	// cannot be a situation where the consumer waits for an asynchronous operation to complete
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
      
TInt CRefTestAgentData::Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket)     
    {     
    return iServer.Read(aEncryptedInputDataPacket, aDecryptedOutputDataPacket);     
    }     
                 
void CRefTestAgentData::Read(const TDesC8& aEncryptedInputDataPacket, TDes8& aDecryptedOutputDataPacket, TRequestStatus& aStatus)     
    {     
    TInt err = Read(aEncryptedInputDataPacket, aDecryptedOutputDataPacket);     
    TRequestStatus* status = &aStatus;     
    User::RequestComplete(status, err);     
    }     
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
