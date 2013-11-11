/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   The one and only packet scheduler
*
*/

/*
* %version: 9 %
*/

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanPacketScheduler::UnsentPackets() const
    {
    return ( iNumOfNotCompletedPackets > 0 );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanPacketScheduler::TranmsitQueueFull( 
    WHA::TQueueId aQueueId )
    {
    iQueueStates[aQueueId] = EQueueFull;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanPacketScheduler::StopTxPipeLine()
    {
    iTxPipelineActive = EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanPacketScheduler::StartTxPipeLine()
    {
    iTxPipelineActive = ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanPacketScheduler::Full() const
    {
    return (iNumOfPackets == KNumOfElements);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TUint WlanPacketScheduler::Priority( WHA::TQueueId aQueueId)
    {
    // Defines a priority for every WHA Queue
    const TUint KWhaQueuePriorityTable[] = 
        { 1,    // priority of WHA::ELegacy
          0,    // priority of WHA::EBackGround
          2,    // priority of WHA::EVideo
          3     // priority of WHA::EVoice
        };
          
    return KWhaQueuePriorityTable[aQueueId];
    }
