/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "baseprofiletimestamping.h"

_LIT(KErrDialogTimestampCheckFailed, "A timestamp check failed on Component: %S, Port: %d.  Expected time: %u +/- %u, actual: %Lu");
_LIT(KErrDialogTimestampComparisonFailed, "A timestamp compare with clock media time failed on Component: %S, Port: %d. Media time: %Lu, delay allowed: %u, actual: %Lu");

CBaseProfileTimestampHandling::CBaseProfileTimestampHandling(ROmxScriptTest& aTestOwner, RASBreakEventHandler& aParentEventHandler)
: CBaseProfileHandler(aTestOwner, aParentEventHandler)
    {
    }

CBaseProfileTimestampHandling::~CBaseProfileTimestampHandling()
    {
    iPendingTimestampChecks.Close();
    }
    
void CBaseProfileTimestampHandling::SetClockComponent(OMX_COMPONENTTYPE* aClockComponent)
    {
    iClockComponent = aClockComponent;
    }

void CBaseProfileTimestampHandling::QueueTimestampCheckL(TInt aPortIndex, TUint aTime, TUint aTolerance)
    {
    TTimestampCheckingInfo timestampCheck;
    timestampCheck.iPortIndex = aPortIndex;
    timestampCheck.iExpectedTime = aTime;
    timestampCheck.iTolerance = aTolerance;
    timestampCheck.iCompareWithRefClock = EFalse;
    
    iPendingTimestampChecks.AppendL(timestampCheck);
    }

void CBaseProfileTimestampHandling::QueueCompareWithRefClockL(TInt aPortIndex, TUint aTolerance)
    {
    TTimestampCheckingInfo timestampCheck;
    timestampCheck.iPortIndex = aPortIndex;
    timestampCheck.iExpectedTime = 0;
    timestampCheck.iTolerance = aTolerance;
    timestampCheck.iCompareWithRefClock = ETrue;
    
    iPendingTimestampChecks.AppendL(timestampCheck);   
    } 

void CBaseProfileTimestampHandling::DoFillBufferDone(OMX_BUFFERHEADERTYPE* aFilledBuffer)
    {
    CheckTimestamp(aFilledBuffer);
    }

void CBaseProfileTimestampHandling::DoEmptyBufferDone(OMX_BUFFERHEADERTYPE* aEmptiedBuffer)
    {
    CheckTimestamp(aEmptiedBuffer);
    }

void CBaseProfileTimestampHandling::CheckTimestamp(OMX_BUFFERHEADERTYPE* aBuffer)
    {
    TInt bufferIndex = iBufferHeaders.Find(*aBuffer, CBaseProfileHandler::BufferHeaderMatchComparison);
    
    for (TInt index=0; index < iPendingTimestampChecks.Count(); index++)
        {
        if (iBufferHeaders[bufferIndex]->iPortIndex != iPendingTimestampChecks[index].iPortIndex)
            {
            continue;
            }
        TTimestampCheckingInfo passCriteria = iPendingTimestampChecks[index];
        iPendingTimestampChecks.Remove(index);
        
        if (passCriteria.iCompareWithRefClock)
            {
            OMX_TIME_CONFIG_TIMESTAMPTYPE timeInfo;
            timeInfo.nSize = sizeof(timeInfo);
            timeInfo.nVersion = KOmxVersion;
            timeInfo.nPortIndex = passCriteria.iPortIndex;

            OMX_ERRORTYPE error = iClockComponent->GetConfig(iClockComponent, OMX_IndexConfigTimeCurrentMediaTime, &timeInfo);
            
            if (error != OMX_ErrorNone)
                {
                iErrorCallbacks.FailWithOmxError(_L("OMX_GetConfig(OMX_IndexConfigTimeCurrentMediaTime)"), error);
                }
            else if ((aBuffer->nTimeStamp > timeInfo.nTimestamp) || (aBuffer->nTimeStamp < (timeInfo.nTimestamp - passCriteria.iTolerance)))
                {
                TBuf<200> errorString;
                errorString.Format(KErrDialogTimestampComparisonFailed, iXmlName, passCriteria.iPortIndex, timeInfo.nTimestamp, passCriteria.iTolerance, aBuffer->nTimeStamp);
                iErrorCallbacks.FailTest(errorString);  
                }
            }
        else
            {
            if ((aBuffer->nTimeStamp < (passCriteria.iExpectedTime - passCriteria.iTolerance)) || (aBuffer->nTimeStamp > (passCriteria.iExpectedTime + passCriteria.iTolerance)))
                {
                TBuf<200> errorString;
                errorString.Format(KErrDialogTimestampCheckFailed, iXmlName, passCriteria.iPortIndex, passCriteria.iExpectedTime, passCriteria.iTolerance, aBuffer->nTimeStamp);
                iErrorCallbacks.FailTest(errorString);
                }
            }
        
        break;
        }
    }

