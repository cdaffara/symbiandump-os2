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
#include "Reftestagentcontent.h"
#include "rtacontent.h"

using namespace ContentAccess;

CRefTestAgentContent* CRefTestAgentContent::NewL(const TDesC& aURI, TContentShareMode aShareMode)
	{
	CRefTestAgentContent* self = NewLC(aURI, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

CRefTestAgentContent* CRefTestAgentContent::NewLC(const TDesC& aURI, TContentShareMode aShareMode)
	{
	CRefTestAgentContent* self=new(ELeave) CRefTestAgentContent();
	CleanupStack::PushL(self);
	self->ConstructL(aURI, aShareMode);
	return self;
	}

CRefTestAgentContent* CRefTestAgentContent::NewL(RFile& aFile)
	{
	CRefTestAgentContent* self = NewLC(aFile);
	CleanupStack::Pop(self);
	return self;
	}

CRefTestAgentContent* CRefTestAgentContent::NewLC(RFile& aFile)
	{
	CRefTestAgentContent* self=new(ELeave) CRefTestAgentContent();
	CleanupStack::PushL(self);
	self->ConstructL(aFile);
	return self;
	}
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
CRefTestAgentContent* CRefTestAgentContent::NewL(const TDesC8& aHeaderData)  
	{
	CRefTestAgentContent* self = NewLC(aHeaderData);    
    CleanupStack::Pop(self);
    return self;
	}
          
CRefTestAgentContent* CRefTestAgentContent::NewLC(const TDesC8& aHeaderData)  
	{
    CRefTestAgentContent* self = new (ELeave) CRefTestAgentContent;
	CleanupStack::PushL(self);  
	self->ConstructL(aHeaderData);
	return self;
	}
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT


CRefTestAgentContent::CRefTestAgentContent() 
	{
	}

CRefTestAgentContent::~CRefTestAgentContent()
	{
	iContentSession.Close();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    delete iHeaderData;
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	}
  
void CRefTestAgentContent::ConstructL(const TDesC& aURI, TContentShareMode aShareMode)
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

	// open server session
	User::LeaveIfError(iContentSession.Open(aURI, mode));
	}

void CRefTestAgentContent::ConstructL(RFile& aFile)
	{
	RFile file;
	User::LeaveIfError(file.Duplicate(aFile));	
	CleanupClosePushL(file);
	// open server session
	User::LeaveIfError(iContentSession.Open(file));
	
	// can now close our file handle, server owns it now
	CleanupStack::PopAndDestroy(&file);
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
void CRefTestAgentContent::ConstructL(const TDesC8& aHeaderData)     
    {     
    iHeaderData = aHeaderData.AllocL();     
    User::LeaveIfError(iContentSession.Open(aHeaderData));     
    }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

TInt CRefTestAgentContent::OpenContainer(const TDesC& aUniqueId)
	{
	return iContentSession.OpenContainer(aUniqueId);
	}

TInt CRefTestAgentContent::CloseContainer()
	{
	return iContentSession.CloseContainer();
	}

void CRefTestAgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray)
	{
	iContentSession.GetEmbeddedObjectsL(aArray);
	}

void CRefTestAgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType)
	{
	iContentSession.GetEmbeddedObjectsL(aArray, aType);  
	}

TInt CRefTestAgentContent::Search(RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8& aMimeType, TBool )
	{
	return iContentSession.Search(aArray, aMimeType, ETrue); 
	}

TInt CRefTestAgentContent::GetAttribute(TInt aAttribute, TInt& aValue, const TDesC& aUniqueId)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    if(iHeaderData != NULL)  
        return iContentSession.GetWmdrmAttribute(aUniqueId, aAttribute, aValue); 
    else
        return iContentSession.GetAttribute(aUniqueId, aAttribute, aValue);
#else
	return iContentSession.GetAttribute(aUniqueId, aAttribute, aValue);
#endif
	}

TInt CRefTestAgentContent::GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    if(iHeaderData != NULL)
        return iContentSession.GetWmdrmAttributeSet(aUniqueId, aAttributeSet);
    else
        return iContentSession.GetAttributeSet(aUniqueId, aAttributeSet);
#else
	return iContentSession.GetAttributeSet(aUniqueId, aAttributeSet);
#endif
	}

TInt CRefTestAgentContent::GetStringAttribute(TInt aAttribute, TDes& aValue, const TDesC& aUniqueId)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    if(iHeaderData != NULL)
        return iContentSession.GetWmdrmStringAttribute(aUniqueId, aAttribute, aValue);
    else
        return iContentSession.GetStringAttribute(aUniqueId, aAttribute, aValue);
#else
	return iContentSession.GetStringAttribute(aUniqueId, aAttribute, aValue);
#endif
	}

TInt CRefTestAgentContent::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId)
	{
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    if(iHeaderData != NULL)
        return iContentSession.GetWmdrmStringAttributeSet(aUniqueId, aStringAttributeSet);
    else
        return iContentSession.GetStringAttributeSet(aUniqueId, aStringAttributeSet);
#else
	return iContentSession.GetStringAttributeSet(aUniqueId, aStringAttributeSet);
#endif
	}

TInt CRefTestAgentContent::AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer)
	{
	return iContentSession.AgentSpecificCommand(aCommand, aInputBuffer, aOutputBuffer);
	}

void CRefTestAgentContent::AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer, TRequestStatus& aStatus)
	{
	TInt err = AgentSpecificCommand(aCommand, aInputBuffer, aOutputBuffer);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

void CRefTestAgentContent::NotifyStatusChange(TEventMask aEventMask, TRequestStatus& aStatus, const TDesC& aUniqueId)
	{
	TRAPD(err, iContentSession.NotifyStatusChangeL(aEventMask, aUniqueId));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

TInt CRefTestAgentContent::CancelNotifyStatusChange(TRequestStatus& aStatus, const TDesC& aUniqueId)
	{
	TInt err = iContentSession.CancelNotifyStatusChange(aUniqueId);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	return err;
	}

void CRefTestAgentContent::RequestRights(TRequestStatus& aStatus , const TDesC& aUniqueId)
	{
	TRAPD(err, iContentSession.RequestRightsL(aUniqueId));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	}

TInt CRefTestAgentContent::CancelRequestRights(TRequestStatus& aStatus, const TDesC& aUniqueId)
	{
	TInt err = iContentSession.CancelRequestRights(aUniqueId);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, err);
	return err;
	}

void CRefTestAgentContent::DisplayInfoL(TDisplayInfo aInfo, const TDesC& aUniqueId)
	{
	iContentSession.DisplayInfoL(aInfo, aUniqueId);
	}

TInt CRefTestAgentContent::SetProperty(TAgentProperty aProperty, TInt aValue)
	{
	return iContentSession.SetProperty(aProperty, aValue); 
	}
