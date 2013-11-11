/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanTxRateAdaptation class
*
*/

/*
* %version: 21 %
*/

#include "config.h"
#include "UmacTxRateAdaptation.h"
#include "UmacContextImpl.h"
#include "wha_mibDefaultvalues.h"


// ============================ MEMBER FUNCTIONS ===============================

WlanTxRateAdaptation::WlanTxRateAdaptation()
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::WlanTxRateAdaptation()"));

    // default values for the rate adaptation algorithm parameters
    //
    const TUint8 KDefaultMinStepUpCheckpoint        = 10;
    const TUint8 KDefaultMaxStepUpCheckpoint        = 60;
    const TUint8 KDefaultStepUpCheckpointFactor     = 2;
    const TUint8 KDefaultStepDownCheckpoint         = 3;
    const TUint8 KDefaultMinStepUpThreshold         = 70;
    const TUint8 KDefaultMaxStepUpThreshold         = 90;
    const TUint8 KDefaultStepUpThresholdIncrement   = 10;
    const TUint8 KDefaultStepDownThreshold          = 50;  
    
    iAlgorithmParam.iMinStepUpCheckpoint =
        KDefaultMinStepUpCheckpoint;

    iAlgorithmParam.iMaxStepUpCheckpoint =
        KDefaultMaxStepUpCheckpoint;

    iAlgorithmParam.iStepUpCheckpointFactor =
        KDefaultStepUpCheckpointFactor;

    iAlgorithmParam.iStepDownCheckpoint =
        KDefaultStepDownCheckpoint;

    iAlgorithmParam.iMinStepUpThreshold = 
        KDefaultMinStepUpThreshold;

    iAlgorithmParam.iMaxStepUpThreshold = 
        KDefaultMaxStepUpThreshold;

    iAlgorithmParam.iStepUpThresholdIncrement = 
        KDefaultStepUpThresholdIncrement;

    iAlgorithmParam.iStepDownThreshold = 
        KDefaultStepDownThreshold;
        
    iAlgorithmParam.iDisableProbeHandling = EFalse;

    for ( TUint32 j = 0; j != WHA::EQueueIdMax; ++j )
        {
        // initially every Tx queue is mapped to the same (general) Tx
        // policy
        iQueue2Policy[j] = WHA::KDefaultTxRatePolicyId;
        }

    for ( TUint32 i = 0; i != KMaxRatePolicyCount; ++i )
        {
        iPolicy[i].iCurrentTxRate = NULL;
        iPolicy[i].iHead = NULL;
        iPolicy[i].iTail = NULL;
        iPolicy[i].iNumOfRates = 0;
        
        iPolicy[i].iProbe = EFalse;
        iPolicy[i].iTxCount = 0;
        iPolicy[i].iTxFailCount = 0;
        iPolicy[i].iStepUpCheckpoint = KDefaultMinStepUpCheckpoint;
        iPolicy[i].iStepUpThreshold = KDefaultMinStepUpThreshold;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanTxRateAdaptation::RateStepDown( SPolicy& aPolicy ) const
    {
    OsTracePrint( KTxRateAdapt, 
        (TUint8*)("UMAC: WlanTxRateAdaptation::RateStepDown(): old rate: 0x%08x"), 
        aPolicy.iCurrentTxRate->iBitRate);

    if ( aPolicy.iCurrentTxRate->iPrev )
        {
        // descent rate
        aPolicy.iCurrentTxRate = aPolicy.iCurrentTxRate->iPrev;

        OsTracePrint( KTxRateAdapt, 
            (TUint8*)("UMAC: WlanTxRateAdaptation::RateStepDown(): new rate: 0x%08x"), 
            aPolicy.iCurrentTxRate->iBitRate);
        }
    else
        {
        // we are already using the lowest rate
        // nothing to do
        OsTracePrint( KTxRateAdapt, (TUint8*)
            ("UMAC: WlanTxRateAdaptation::RateStepDown(): already using the lowest rate") );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanTxRateAdaptation::RateStepUp( SPolicy& aPolicy ) const
    {
    OsTracePrint( KTxRateAdapt, 
        (TUint8*)("UMAC: WlanTxRateAdaptation::RateStepUp(): old rate: 0x%08x"), 
        aPolicy.iCurrentTxRate->iBitRate);

    TBool status( ETrue );

    if ( aPolicy.iCurrentTxRate->iNext )
        {
        // ascend rate
        aPolicy.iCurrentTxRate = aPolicy.iCurrentTxRate->iNext;

        OsTracePrint( KTxRateAdapt, 
            (TUint8*)("UMAC: WlanTxRateAdaptation::RateStepUp(): new rate: 0x%08x"), 
            aPolicy.iCurrentTxRate->iBitRate);
        }
    else
        {
        // we are already using the highest rate.
        // Indicate that to the caller
        status = EFalse;

        OsTracePrint( KTxRateAdapt, (TUint8*)
            ("UMAC: WlanTxRateAdaptation::RateStepUp(): already using the highest rate") );
        }
        
    return status;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanTxRateAdaptation::InsertRate( 
    TUint8 aPolicyIndex, SBitRate* aBlock ) const
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC:  WlanTxRateAdaptation::InsertRate(): aPolicyIndex: %d"), 
        aPolicyIndex);
    OsTracePrint( KTxRateAdapt, 
        (TUint8*)("UMAC:  WlanTxRateAdaptation::InsertRate(): rate: 0x%08x"), 
        aBlock->iBitRate);

    // traverse our single linked list of rate entrys
    // and locate the correct place for the new rate
    SBitRate* ptr = iPolicy[aPolicyIndex].iHead;
    SBitRate* prev = ptr;

    do
        {
        if ( aBlock->iBitRate < ptr->iBitRate )
            {
            // rate to be inserted is smaller than 
            // the "current" rate
            break;
            }

        prev = ptr;
        ptr = ptr->iNext;
        } 
        while ( ptr );
    
    if ( ptr )
        {
        // correct slot found
        // link the new rate
        prev->iNext = aBlock;
        aBlock->iPrev = prev;
        aBlock->iNext = ptr;
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanTxRateAdaptation::AppendRate( 
    TUint8 aPolicyIndex,
    WHA::TRate aRate )
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC:  WlanTxRateAdaptation::AppendRate(): aPolicyIndex: %d"), 
        aPolicyIndex);
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC:  WlanTxRateAdaptation::AppendRate(): rate: 0x%08x"), aRate);

    // allocate a new block for the rate
    SBitRate* block 
        = static_cast<SBitRate*>(os_alloc( sizeof(SBitRate) ));

    if ( !block )
        {
        // alloc failure; we cannot continue
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: WlanTxRateAdaptation::AppendRate(): alloc failure"));        

        return EFalse;
        }

    block->iBitRate = aRate;
    block->iNext = NULL;
    block->iPrev = NULL;

    if ( !iPolicy[aPolicyIndex].iHead )
        {
        // this is the first append
        
        iPolicy[aPolicyIndex].iHead = block;
        iPolicy[aPolicyIndex].iTail = iPolicy[aPolicyIndex].iHead;
        iPolicy[aPolicyIndex].iCurrentTxRate = block;
        ++iPolicy[aPolicyIndex].iNumOfRates;
        }
    else
        {
        // not the first rate to append
        
        if ( block->iBitRate > iPolicy[aPolicyIndex].iTail->iBitRate )
            {
            // new rate bigger than the last one
            // append to rear
            iPolicy[aPolicyIndex].iTail->iNext = block;
            block->iPrev = iPolicy[aPolicyIndex].iTail;
            // new tail
            iPolicy[aPolicyIndex].iTail = block;
            ++iPolicy[aPolicyIndex].iNumOfRates;
            }
        else if( block->iBitRate < iPolicy[aPolicyIndex].iTail->iBitRate )
            {
            // new rate smaller than the last one
            // insert the rate to the correct slot
            InsertRate( aPolicyIndex, block );
            ++iPolicy[aPolicyIndex].iNumOfRates;
            }
        else
            {
            // new rate equal to the last one
            // this cannot happen as we are setting the rates only once
            // and from a rate bitmask
            }
        }

    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::AppendRate(): num of existing rates: %d"), 
        iPolicy[aPolicyIndex].iNumOfRates);

    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanTxRateAdaptation::Reset( SPolicy& aPolicy ) const
    {
    OsTracePrint( KTxRateAdapt, 
        (TUint8*)("UMAC: WlanTxRateAdaptation::Reset() a policy"));

    // dealloc all rates
    SBitRate* ptr =  aPolicy.iHead;
    SBitRate* cache =  ptr;

    while ( ptr )
        {
        cache = ptr;
        ptr = ptr->iNext;

        // free the previous one
        os_free( cache );        
        }

    aPolicy.iCurrentTxRate = NULL;
    aPolicy.iHead = NULL;
    aPolicy.iTail = NULL;
    aPolicy.iNumOfRates = 0;
    
    aPolicy.iProbe = EFalse;
    aPolicy.iTxCount = 0;
    aPolicy.iTxFailCount = 0;
    aPolicy.iStepUpCheckpoint = iAlgorithmParam.iMinStepUpCheckpoint;
    aPolicy.iStepUpThreshold = iAlgorithmParam.iMinStepUpThreshold;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanTxRateAdaptation::SetRates( 
    TUint8 aPolicyId,
    WHA::TRate aRateBitmask )
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::SetRates(): aPolicyId: 0x%08x"), aPolicyId);
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::SetRates(): aRateBitmask: 0x%08x"), aRateBitmask);

    // make the list of rates; but 1st clear the list
    Reset( iPolicy[aPolicyId - 1] );

    // make rates from bitmask
    const TUint32 cntr_end( 32 );
    TUint32 cntr( 0 );

    for ( TUint32 bit = 1 ; cntr != cntr_end ; (bit <<= 1) )
        {
        if ( aRateBitmask & bit )
            {
            // rate is to be set
            if ( !AppendRate( aPolicyId - 1, aRateBitmask & bit ) )
                {
                return EFalse;
                }
            }

        ++cntr;
        }

    // if we got this far, the rates are now in the list
    
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanTxRateAdaptation::SetPolicy( 
    WHA::TQueueId aQueueId, 
    TUint8 aPolicyId )
    {
    OsTracePrint( KTxRateAdapt, 
        (TUint8*)("UMAC: WlanTxRateAdaptation::SetPolicy(): aQueueId: %d"), 
        aQueueId);
    OsTracePrint( KTxRateAdapt, 
        (TUint8*)("UMAC: WlanTxRateAdaptation::SetPolicy(): aPolicyId: %d"), 
        aPolicyId);

    iQueue2Policy[aQueueId] = aPolicyId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanTxRateAdaptation::SetCurrentMaxTxRate( 
    TUint8 aPolicyId, 
    WHA::TRate aRate )
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC:  WlanTxRateAdaptation::SetCurrentMaxTxRate(): aPolicyId: %d"), 
        aPolicyId );
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC:  WlanTxRateAdaptation::SetCurrentMaxTxRate(): aRate: 0x%08x"), 
        aRate );

    if ( !iPolicy[aPolicyId - 1].iNumOfRates )
        {
        // catch implementation error
        OsAssert( (TUint8*)("UMAC: panic: no rates defined"),
            (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // traverse our single linked list of rate entrys from rear (i.e. from 
    // the highest) to front (i.e. to the lowest) and try to locate the 
    // provided rate entry, or the next lower one, so that it can be set as 
    // the current max tx rate

    SBitRate* ptr = iPolicy[aPolicyId - 1].iTail;

    do
        {
        if ( ptr->iBitRate <= aRate )
            {
            // found
            break;
            }
        else
            {
            ptr = ptr->iPrev;            
            }
        } 
        while ( ptr );
    
    if ( ptr )
        {
        // usable rate was found. Set it as the current rate
        iPolicy[aPolicyId - 1].iCurrentTxRate = ptr;        
        }
    else
        {
        // the provided max Tx rate is lower than any of the
        // rates in this rate policy. In this case we have to set the 
        // lowest rate in the policy as the current rate (even 
        // though it is higher than what was requested)
        iPolicy[aPolicyId - 1].iCurrentTxRate = iPolicy[aPolicyId - 1].iHead;
        }

    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC:  WlanTxRateAdaptation::SetCurrentMaxTxRate(): current rate now set to: 0x%08x"), 
        iPolicy[aPolicyId - 1].iCurrentTxRate->iBitRate );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanTxRateAdaptation::SetAlgorithmParameters( 
    TUint8 aMinStepUpCheckpoint,
    TUint8 aMaxStepUpCheckpoint,
    TUint8 aStepUpCheckpointFactor,
    TUint8 aStepDownCheckpoint,
    TUint8 aMinStepUpThreshold,
    TUint8 aMaxStepUpThreshold,
    TUint8 aStepUpThresholdIncrement,
    TUint8 aStepDownThreshold,
    TBool aDisableProbeHandling )
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::SetAlgorithmParameters") );

    iAlgorithmParam.iMinStepUpCheckpoint = aMinStepUpCheckpoint;
    iAlgorithmParam.iMaxStepUpCheckpoint = aMaxStepUpCheckpoint;
    iAlgorithmParam.iStepUpCheckpointFactor = aStepUpCheckpointFactor;
    iAlgorithmParam.iStepDownCheckpoint = aStepDownCheckpoint;
    iAlgorithmParam.iMinStepUpThreshold = aMinStepUpThreshold;
    iAlgorithmParam.iMaxStepUpThreshold = aMaxStepUpThreshold;
    iAlgorithmParam.iStepUpThresholdIncrement = aStepUpThresholdIncrement;
    iAlgorithmParam.iStepDownThreshold = aStepDownThreshold;
    iAlgorithmParam.iDisableProbeHandling = aDisableProbeHandling;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanTxRateAdaptation::RatePolicy( 
    const WlanContextImpl& aCtxImpl,
    WHA::TQueueId aQueueId,
    TBool aUseSpecialRatePolicy,
    WHA::TRate& aRate, 
    TUint8& aPolicyId ) const
    {
    aPolicyId = iQueue2Policy[aQueueId];

    if ( aCtxImpl.WHASettings().iCapability & 
         WHA::SSettings::KAutonomousRateAdapt )
        {
        // autonomous rate adaptation is being used, i.e. rate adaptation is
        // handled by lower layers. In this case it's not relevant to specify
        // a Max Tx Rate and the whole value is not applicable. So we return
        // zero        
        const WHA::TRate notRelevant( 0 );
        aRate = notRelevant;
        
        const TUint8 KSpecialTxAutoRatePolicy = 
            aCtxImpl.SpecialTxAutoRatePolicy();
        
        if ( aUseSpecialRatePolicy && KSpecialTxAutoRatePolicy )
            {
            aPolicyId = KSpecialTxAutoRatePolicy;
            }
        }
    else
        {
        // rate adaptation is handled by this object
        
        if ( !iPolicy[aPolicyId - 1].iNumOfRates )
            {
            OsAssert( (TUint8*)
                ("UMAC: WlanTxRateAdaptation::RatePolicy(): panic: no rates defined"),
                (TUint8*)(WLAN_FILE), __LINE__ );
            }

        if ( !iPolicy[aPolicyId - 1].iCurrentTxRate )
            {
            // catch implementation error

            OsTracePrint( KErrorLevel, 
                (TUint8*)("UMAC: no current rate specified for policy id: %d"), 
                aPolicyId );        
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }
            
        // return the current Max Tx Rate
        aRate = iPolicy[aPolicyId - 1].iCurrentTxRate->iBitRate;    
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanTxRateAdaptation::OnTxCompleted( 
    WHA::TRate aRate, 
    TBool aSuccess,
    WHA::TQueueId aQueueId,
    WHA::TRate aRequestedRate )
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): aRate: 0x%08x"), 
        aRate);
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): aSuccess: %d"), 
        aSuccess);
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): aQueueId: %d"), 
        aQueueId);
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): aRequestedRate: 0x%08x"), 
        aRequestedRate);

    TBool success( aSuccess );

    TUint8 policyId = iQueue2Policy[aQueueId];
    OsTracePrint( KTxRateAdapt, 
        (TUint8*)("UMAC: WlanTxRateAdaptation::OnTxCompleted(): policyId: %d"), 
        policyId);    

    if ( iPolicy[policyId - 1].iCurrentTxRate )
        {
        // as Current Tx Rate is defined for this policy, it means that
        // this rate adaptation object is configured and we can handle this 
        // event
           
        if ( aSuccess )
            {
            // the frame was successfully delivered to nw
            
            // from rate adaptation point of view we are interested in the 
            // actual Tx rate of the frame only if the current rate for the Tx
            // policy in question is still the same as the originally requested Tx
            // rate for this frame.
            // (After we have switched the curent rate to something else, we may
            // still receive Tx completions for frames which we have requested to 
            // be sent with the previous rate. We are not interested in their 
            // actual Tx rate any more.)
            if ( aRequestedRate == iPolicy[policyId - 1].iCurrentTxRate->iBitRate )
                {
                if ( aRate != aRequestedRate )
                    {
                    // actual Tx rate was different than the originally requested
                    // rate. This is a Tx "failure" from Tx rate adaptation point
                    // of view
                    OsTracePrint( KTxRateAdapt, (TUint8*)
                        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): succeeded with a different rate than requested"));
                    success = EFalse;
                    }
                }
            else
                {
                // curent rate of the Tx policy in question not the same any more
                // as the originally requested rate for this frame. The frame is
                // ignored from Tx rate adaptation considerations            
                OsTracePrint( KTxRateAdapt, (TUint8*)
                    ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): current rate for this Tx policy not same any more as originally requested rate for this frame"));
                OsTracePrint( KTxRateAdapt, (TUint8*)
                    ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): frame ignored from Tx rate adaptation considerations"));
                return;
                }
            }
                
        TRateStep rateStep = OnTxCompleted( success, iPolicy[policyId - 1] );
        
        switch ( rateStep )
            {
            case EStepUp:
                if ( RateStepUp( iPolicy[policyId - 1] ) )
                    {
                    // step up succeeded (i.e. there was a higher rate to switch to)

                    // if probe frame handling hasn't been disabled, the next 
                    // transmitted frame will be handled as a probe frame
                    if ( !iAlgorithmParam.iDisableProbeHandling )
                        {
                        iPolicy[policyId - 1].iProbe = ETrue;                    
                        }                
                    }                
                break;
            case EStepDown:
                RateStepDown( iPolicy[policyId - 1] );
                break;
            case EKeepCurrent:
                // nothing to do here
                break;
            default:
                OsTracePrint( KErrorLevel, 
                    (TUint8*)("UMAC: unkown rate step: %d"), 
                    rateStep );        
                OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );                        
            }    
        }
    else
        {
        // Current Tx Rate is not defined for this policy, which means that
        // this rate adaptation object is not configured (it has been
        // reset) and cannot handle the event. So we just discard it
        OsTracePrint( KTxRateAdapt, (TUint8*)
            ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): Rate adaptation not configured. Tx complete event discarded"));
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanTxRateAdaptation::Reset()
    {
    OsTracePrint( KTxRateAdapt, 
        (TUint8*)("UMAC: WlanTxRateAdaptation::Reset() the whole object"));

    for ( TUint32 j = 0; j != WHA::EQueueIdMax; ++j )
        {
        // every Tx queue is mapped to the same (general) Tx policy
        iQueue2Policy[j] = WHA::KDefaultTxRatePolicyId;
        }

    for ( TUint32 i = 0; i != KMaxRatePolicyCount; ++i )
        {
        // reset the state of every individual Tx policy
        Reset( iPolicy[i] );
        }        
    }

// ---------------------------------------------------------------------------
// Note: The rate adaptation algorithm requires that:
// iStepDownCheckpoint <= iStepUpCheckpoint and
// iStepDownCheckpoint > 1 (or it can also be == 1 if iDisableProbeHandling
// is ETrue)
// ---------------------------------------------------------------------------
//
WlanTxRateAdaptation::TRateStep WlanTxRateAdaptation::OnTxCompleted( 
    TBool aSuccess,
    SPolicy& aPolicy ) const
    {
    TRateStep rateStep( EKeepCurrent );

    ++aPolicy.iTxCount;

    if ( !aSuccess )
        {
        ++aPolicy.iTxFailCount;
        }

    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): iProbe (0 no/1 yes): %d"),
        aPolicy.iProbe );
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): iTxCount after counting this frame: %d"),
        aPolicy.iTxCount );
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): iTxFailCount after counting this frame: %d"),
        aPolicy.iTxFailCount );
    OsTracePrint( KTxRateAdapt, 
        (TUint8*)("UMAC: WlanTxRateAdaptation::OnTxCompleted(): iCurrentTxRate: 0x%08x"), 
        aPolicy.iCurrentTxRate->iBitRate);
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): iStepUpCheckpoint: %d"),
        aPolicy.iStepUpCheckpoint  );
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): iStepUpThreshold: %d"),
        aPolicy.iStepUpThreshold  );

    if ( aPolicy.iProbe )
        {
        // this was a probe frame; check if a rate change is needed
        
        if ( !aSuccess )
            {
            // either the Tx failed completely or it succeeded at a lower 
            // rate than requested

            OsTracePrint( KTxRateAdapt, (TUint8*)
                ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): probe failure"));
            
            // decrement the rate
            rateStep = EStepDown;

            aPolicy.iStepUpCheckpoint = 
                ( aPolicy.iStepUpCheckpoint * 
                  iAlgorithmParam.iStepUpCheckpointFactor 
                  > iAlgorithmParam.iMaxStepUpCheckpoint ) ? 
                 iAlgorithmParam.iMaxStepUpCheckpoint :
                 aPolicy.iStepUpCheckpoint * 
                 iAlgorithmParam.iStepUpCheckpointFactor;
            
            aPolicy.iStepUpThreshold = 
                ( aPolicy.iStepUpThreshold + 
                  iAlgorithmParam.iStepUpThresholdIncrement
                > iAlgorithmParam.iMaxStepUpThreshold ) ?
                iAlgorithmParam.iMaxStepUpThreshold :
                aPolicy.iStepUpThreshold + 
                iAlgorithmParam.iStepUpThresholdIncrement;
            
            OsTracePrint( KTxRateAdapt, (TUint8*)
                ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): new iStepUpCheckpoint: %d"),
                aPolicy.iStepUpCheckpoint  );
            OsTracePrint( KTxRateAdapt, (TUint8*)
                ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): new iStepUpThreshold: %d"),
                aPolicy.iStepUpThreshold  );

            aPolicy.iTxCount = 0;
            aPolicy.iTxFailCount = 0;                
            }
        else
            {
            // Tx success with requested rate; nothing more to do
            }  
                          
        // in any case clear the probe flag
        aPolicy.iProbe = EFalse;
        }
    else 
        {        
        if ( aPolicy.iTxCount % iAlgorithmParam.iStepDownCheckpoint == 0 )
            {
            // check if the rate should be decremented
            OsTracePrint( KTxRateAdapt, (TUint8*)
                ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): check for decrement need"));

            if ( aPolicy.iTxFailCount * 100 
                 >= iAlgorithmParam.iStepDownThreshold * aPolicy.iTxCount )
                {
                // at least iStepDownThreshold % of iTxCount frames have 
                // "failed" => decrement rate
                //
                rateStep = EStepDown;                  
                aPolicy.iStepUpCheckpoint = 
                    iAlgorithmParam.iMinStepUpCheckpoint;
                aPolicy.iStepUpThreshold = 
                    iAlgorithmParam.iMinStepUpThreshold;

                aPolicy.iTxCount = 0;
                aPolicy.iTxFailCount = 0;
                }            
            }

        if ( rateStep != EStepDown && 
            aPolicy.iTxCount >= aPolicy.iStepUpCheckpoint )
            {
            // check if the rate should be incremented
            OsTracePrint( KTxRateAdapt, (TUint8*)
                ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): check for increment need"));
            
            if ( ( aPolicy.iTxCount - aPolicy.iTxFailCount ) * 100 
                 >= aPolicy.iStepUpThreshold *  aPolicy.iTxCount )
                {
                // at least iStepUpThreshold % of iTxCount frames have 
                // "succeeded" => increment rate
                //
                rateStep = EStepUp;
                }
                
            aPolicy.iTxCount = 0;
            aPolicy.iTxFailCount = 0;
            }
        }
        
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanTxRateAdaptation::OnTxCompleted(): rateStep: %d (keep = 0, down, up)"),
        rateStep );

    return rateStep;
    }
