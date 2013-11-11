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

#include <caf/streaming/protectedstreamdesc.h>
#include "ipseckeystreamsink.h"

using namespace StreamAccess;


EXPORT_C CIpSecProtectedStreamDesc* CIpSecProtectedStreamDesc::NewLC(const TInetAddr& aSourceAddr, const TInetAddr& aTargetAddr)
	{
	CIpSecProtectedStreamDesc* self = new (ELeave) CIpSecProtectedStreamDesc(aSourceAddr, aTargetAddr);
	CleanupStack::PushL(self);
	return self;			
	}

EXPORT_C CKeyStreamSink* CIpSecProtectedStreamDesc::CreateKeyStreamSinkLC() const
	{	
	CIpSecKeyStreamSink* keyStreamSink = CIpSecKeyStreamSink::NewLC(iSourceAddr, iTargetAddr);
	return keyStreamSink;	
	}
	
CIpSecProtectedStreamDesc::CIpSecProtectedStreamDesc(const TInetAddr& aSourceAddr, const TInetAddr& aTargetAddr) :
												     iSourceAddr(aSourceAddr), 
													 iTargetAddr(aTargetAddr)							
	{							 	
	}

CIpSecProtectedStreamDesc::~CIpSecProtectedStreamDesc()
	{
	}
