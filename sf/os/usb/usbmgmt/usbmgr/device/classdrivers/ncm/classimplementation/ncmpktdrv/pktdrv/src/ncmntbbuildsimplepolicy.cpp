/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* implementation for simple NTB build policy
*
*/


/**
@file
@internalComponent
*/


#include "ncmntbbuildsimplepolicy.h"
#include "ncmntbbuilder.h"
#include "ncmpktdrvcommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmntbbuildsimplepolicyTraces.h"
#endif



const TInt KMaxPacketsOfNtb = 10;
const TInt KMaxPacketDelay = 1000;
const TInt KMinFreeBuffers = 5;


_LIT(KSimPolicyPanic, "CNcmNtbBuildSimplePolicy");
const TInt KTimerError = 1;


// ======== MEMBER FUNCTIONS ========
//


CNcmNtbBuildPolicy* CNcmNtbBuildSimplePolicy::NewL(CNcmNtbBuilder& aNtbBuilder)
    {
    OstTraceFunctionEntry1( CNCMNTBBUILDSIMPLEPOLICY_NEWL_ENTRY, ( TUint )&( aNtbBuilder ) );
    CNcmNtbBuildSimplePolicy *self=new (ELeave) CNcmNtbBuildSimplePolicy(aNtbBuilder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    OstTraceFunctionExit0( CNCMNTBBUILDSIMPLEPOLICY_NEWL_EXIT );
    return self;    
    }

CNcmNtbBuildSimplePolicy::CNcmNtbBuildSimplePolicy(CNcmNtbBuilder& aNtbBuilder)
    : CNcmNtbBuildPolicy(aNtbBuilder)
    {
    CActiveScheduler::Add(this);
    }

CNcmNtbBuildSimplePolicy::~CNcmNtbBuildSimplePolicy()
    {
    OstTraceFunctionEntry1( CNCMNTBBUILDSIMPLEPOLICY_CNCMNTBBUILDSIMPLEPOLICY_ENTRY, this );
    Cancel();
    iTimer.Close();
    OstTraceFunctionExit1( CNCMNTBBUILDSIMPLEPOLICY_CNCMNTBBUILDSIMPLEPOLICY_EXIT, this );
    }

void CNcmNtbBuildSimplePolicy::DoCancel()
    {
    OstTraceFunctionEntry1( CNCMNTBBUILDSIMPLEPOLICY_DOCANCEL_ENTRY, this );
    iTimer.Cancel();
    iPacketsCount = 0;    
    OstTraceFunctionExit1( CNCMNTBBUILDSIMPLEPOLICY_DOCANCEL_EXIT, this );
    }


void CNcmNtbBuildSimplePolicy::ConstructL()
    {
    User::LeaveIfError(iTimer.CreateLocal());
    }

void CNcmNtbBuildSimplePolicy::RunL()
    {
    OstTraceFunctionEntry1( CNCMNTBBUILDSIMPLEPOLICY_RUNL_ENTRY, this );
    OstTrace1( TRACE_NORMAL, CNCMNTBBUILDSIMPLEPOLICY_RUNL, "CNcmNtbBuildSimplePolicy::RunL Status=%d", iStatus.Int() );

    if(iStatus.Int() != KErrNone)
        {
        User::Panic(KSimPolicyPanic, KTimerError);
        OstTraceFunctionExit1( CNCMNTBBUILDSIMPLEPOLICY_RUNL_EXIT, this );
        return;
        }

    if (iPacketsCount > 0)
        {
        iNtbBuilder.CompleteNtbBuild();
        }
    OstTraceFunctionExit1( CNCMNTBBUILDSIMPLEPOLICY_RUNL_EXIT_DUP1, this );
    }

void CNcmNtbBuildSimplePolicy::CompleteNtbBuild()
    {
    OstTraceFunctionEntry1( CNCMNTBBUILDSIMPLEPOLICY_COMPLETENTBBUILD_ENTRY, this );
    
    Cancel();
    iPacketsCount = 0;
    OstTraceFunctionExit1( CNCMNTBBUILDSIMPLEPOLICY_COMPLETENTBBUILD_EXIT, this );
    }


void CNcmNtbBuildSimplePolicy::StartNewNtb()
    {
    OstTraceFunctionEntry1( CNCMNTBBUILDSIMPLEPOLICY_STARTNEWNTB_ENTRY, this );    
    OstTraceFunctionExit1( CNCMNTBBUILDSIMPLEPOLICY_STARTNEWNTB_EXIT, this );
    }

void CNcmNtbBuildSimplePolicy::UpdateNtb(TInt aSize)
    {
    OstTraceFunctionEntryExt( CNCMNTBBUILDSIMPLEPOLICY_UPDATENTB_ENTRY, this );
    OstTrace1( TRACE_NORMAL, CNCMNTBBUILDSIMPLEPOLICY_UPDATENTB, "CNcmNtbBuildSimplePolicy::UpdateNtb aSize=%d", aSize );

    if (iPacketsCount == 0)
        {
        iStatus = KRequestPending;
        iTimer.After(iStatus, KMaxPacketDelay);
        SetActive();
        }
    
    if (KMaxPacketsOfNtb <= ++iPacketsCount && aSize >= iBufferSize/2)
        {
        iNtbBuilder.CompleteNtbBuild();
        }
   else if (iFreeBufferCount >= KMinFreeBuffers && aSize >= iBufferSize/8)
        {
        iNtbBuilder.CompleteNtbBuild();
        }
    OstTraceFunctionExit1( CNCMNTBBUILDSIMPLEPOLICY_UPDATENTB_EXIT, this );
    }
    
void CNcmNtbBuildSimplePolicy::UpdateBufferSize(TInt aSize)
    {
    iBufferSize = aSize;
    }

void CNcmNtbBuildSimplePolicy::UpdateFreeBufferCount(TInt aCount)
    {
    iFreeBufferCount = aCount;
	}

void CNcmNtbBuildSimplePolicy::UpdateTotalBufferCount(TInt aCount)
    {
    iTotalBufferCount = aCount;
    }

