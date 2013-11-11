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
* Description:  Implementation of WLAN Agent
*
*/

/*
* %version: 8 %
*/

#include "am_debug.h"
#include "wlanagt.h"
#include "wlanagtsm.h"

// WLAN bearer type
const TUint32 KWlanAgentBearerType = 0x3C;

//
// First ordinal export
//
extern "C" EXPORT_C CNifAgentFactory* NewAgentFactoryL()
    {       
    return new(ELeave) CWlanAgentFactory;
    }

// ======== WLAN agent factory MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWlanAgentFactory::InstallL
// ---------------------------------------------------------------------------
//
void CWlanAgentFactory::InstallL()
    {
    }

// ---------------------------------------------------------------------------
// CWlanAgentFactory::NewAgentL
// ---------------------------------------------------------------------------
//
CNifAgentBase* CWlanAgentFactory::NewAgentL(const TDesC& /*aName*/)
    {
    DEBUG( "CWlanAgentFactory::NewAgentL()" );
    return CWlanAgent::NewL();
    }

// ---------------------------------------------------------------------------
// CWlanAgentFactory::Info
// ---------------------------------------------------------------------------
//
TInt CWlanAgentFactory::Info(
    TNifAgentInfo& aInfo, 
    TInt /*aIndex*/) const
    {
    aInfo.iName = KWlanAgentName;
    aInfo.iName.AppendFormat(_L("-AgentFactory[0x%08x]"), this);
    aInfo.iVersion = TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
    return KErrNone;
    }

// ======== WLAN agent MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CWlanAgent::CWlanAgent
// ---------------------------------------------------------------------------
//
CWlanAgent::CWlanAgent()
    {
    }

// ---------------------------------------------------------------------------
// CWlanAgent::CWlanAgent
// ---------------------------------------------------------------------------
//
CWlanAgent::~CWlanAgent()
    {
    }

// ---------------------------------------------------------------------------
// CWlanAgent::CWlanAgent
// ---------------------------------------------------------------------------
//
CWlanAgent* CWlanAgent::NewL()
    {
    CWlanAgent* self = new (ELeave) CWlanAgent();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CWlanAgent::CWlanAgent
// ---------------------------------------------------------------------------
//
void CWlanAgent::ConstructL()
    {
    CStateMachineAgentBase::ConstructL();
    }

// ---------------------------------------------------------------------------
// CWlanAgent::CWlanAgent
// ---------------------------------------------------------------------------
//
CAgentSMBase* CWlanAgent::CreateAgentSML(
    MAgentNotify& aObserver,
    CDialogProcessor* aDlgPrc, 
    CCommsDbAccess& aDbAccess, 
    TCommDbConnectionDirection aDirection )
    {
    if( aDirection == ECommDbConnectionDirectionIncoming )
        {
        User::Leave( KErrNotSupported );
        }
    
    return CWlanSM::NewL( aObserver, aDlgPrc, aDbAccess );
    }

// ---------------------------------------------------------------------------
// CWlanAgent::CWlanAgent
// ---------------------------------------------------------------------------
//
void CWlanAgent::Info( TNifAgentInfo& aInfo ) const
    {
    aInfo.iName = KWlanAgentName;
    aInfo.iName.AppendFormat(_L("-Agent[0x%08x]"), this);
    aInfo.iVersion = TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
    }

// ---------------------------------------------------------------------------
// CWlanAgent::GetBearerInfo
// ---------------------------------------------------------------------------
//
TUint32 CWlanAgent::GetBearerInfo() const
    {
    return KWlanAgentBearerType;
    }
