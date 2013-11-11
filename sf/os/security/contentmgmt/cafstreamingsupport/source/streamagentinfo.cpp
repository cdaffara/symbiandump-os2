// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <ecom/ecom.h>
#include "streamagentinfo.h"
#include <caf/streaming/streamagentfactory.h> 

using namespace StreamAccess;

CStreamAgentInfo* CStreamAgentInfo::NewLC(const CImplementationInformation& aImplementationInfo)
	{
	CStreamAgentInfo* self = new(ELeave) CStreamAgentInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aImplementationInfo);
	return self;
	}

CStreamAgentInfo::CStreamAgentInfo()
	{
	}

CStreamAgentInfo::~CStreamAgentInfo()
	{
	delete iName;
	delete iStreamAgentFactory;
	
	REComSession::DestroyedImplementation(iEcomKey);
	REComSession::FinalClose();	
	}

void CStreamAgentInfo::ConstructL(const CImplementationInformation& aImplementationInfo)
	{
	/* Retrieve the stream agent's Name and Implementation Uid from the CImplementationInformation
	 * object constructed by ECOM
	 */
	iName = aImplementationInfo.DisplayName().AllocL();
	iImplementationUid = aImplementationInfo.ImplementationUid();
	
	// Instantiate an instance of the stream agents factory using the Implementation Uid
	iStreamAgentFactory = static_cast<CStreamAgentFactory *>(REComSession::CreateImplementationL(iImplementationUid, iEcomKey));
	}

const TDesC& CStreamAgentInfo::Name() const
	{
	return *iName;
	}

TUid CStreamAgentInfo::ImplementationUid() const
	{
	return iImplementationUid;
	}

TBool CStreamAgentInfo::IsKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream) const
	{
	/* Query the stream agent factory to determine whether the SDP key management description 
	 * is supported
	 */
	return iStreamAgentFactory->IsKeyStreamSupportedL(aSdpKeyStream);
	}

CStreamAgentFactory& CStreamAgentInfo::StreamAgentFactory() const
	{
	return *iStreamAgentFactory;
	}
