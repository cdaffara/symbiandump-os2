/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Implements LANx Brearer
*
*/

/*
* %version: 6 %
*/

#include <in_sock.h> // Header is retained, but in_sock.h is modified for ipv6
#include <in_iface.h> // Gone.
#include "CLanxBearer.h"
#include "WlanProto.h"

//For Wlan logging
#include "am_debug.h"

using namespace ESock;

// -----------------------------------------------------------------------------
// CLanxBearer::CLanxBearer
// -----------------------------------------------------------------------------
//
CLanxBearer::CLanxBearer(CLANLinkCommon* aLink):iLink(aLink), iSoIfConnectionInfoCached(false)
{
}

// -----------------------------------------------------------------------------
// CLanxBearer::ConstructL
// -----------------------------------------------------------------------------
//
void  CLanxBearer::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CLanxBearer::StartSending
// -----------------------------------------------------------------------------
//
void CLanxBearer::StartSending(CProtocolBase* /*aProtocol*/)
	{
    DEBUG("CLanxBearer::StartSending()");
	
	iUpperControl->StartSending();
	}

// -----------------------------------------------------------------------------
// CLanxBearer::UpdateMACAddr
// -----------------------------------------------------------------------------
//
void CLanxBearer::UpdateMACAddr()
	{	
	}


// MLowerControl methods
// -----------------------------------------------------------------------------
// CLanxBearer::GetName
// -----------------------------------------------------------------------------
//

TInt CLanxBearer::GetName(TDes& aName)
	{
    DEBUG("CLanxBearer::GetName()");
	
	aName.Copy(iIfName);
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLanxBearer::BlockFlow
// -----------------------------------------------------------------------------
//
TInt CLanxBearer::BlockFlow(MLowerControl::TBlockOption /*aOption*/)
	{
    DEBUG("CLanxBearer::BlockFlow()");
	
	return KErrNotSupported;
	}

//
// Utilities
// 
// -----------------------------------------------------------------------------
// CLanxBearer::SetUpperPointers
// -----------------------------------------------------------------------------
//
void CLanxBearer::SetUpperPointers(MUpperDataReceiver* aReceiver, MUpperControl* aControl)
	{
    DEBUG("CLanxBearer::SetUpperPointers()");
	
	ASSERT(iUpperReceiver == NULL && iUpperControl == NULL);
	iUpperReceiver = aReceiver;
	iUpperControl = aControl;
	}

// -----------------------------------------------------------------------------
// CLanxBearer::MatchesUpperControl
// -----------------------------------------------------------------------------
//
TBool CLanxBearer::MatchesUpperControl(const ESock::MUpperControl* aUpperControl) const
/**
Check whether the passed MUpperControl matches that associated with the current instance
*/
	{
    DEBUG("CLanxBearer::MatchesUpperControl()");
	
	ASSERT(iUpperControl);
	return iUpperControl == aUpperControl;
	}
 
