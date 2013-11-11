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
*
*/

/**
 @file
 @internalComponent
*/

#include <e32base.h>

#include "ncmserverconsts.h"
#include "ncmsession.h"
#include "ncmengine.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmsessionTraces.h"
#endif



CNcmSession* CNcmSession::NewL(CNcmEngine& aEngine)
    {
    CNcmSession* self = new(ELeave) CNcmSession(aEngine);
    return self;
    }

CNcmSession::CNcmSession(CNcmEngine& aEngine)
    : iNcmEngine(aEngine)
    {
    }

CNcmSession::~CNcmSession()
    {
    }

void CNcmSession::ServiceL(const RMessage2& aMessage)
    {
    OstTraceFunctionEntry0(CNCMSESSION_SERVICEL);

    OstTraceExt3(TRACE_NORMAL,CNCMSESSION_SERVICEL_DUP01, "CNcmSession::ServiceL %d %d %d", aMessage.Function(),
        aMessage.Int0(), aMessage.Int1());

    switch ( aMessage.Function() )
        {
    case ENcmTransferHandle:
        {
        RMessagePtr2 messagePtr(aMessage);
        TInt ret = iNcmEngine.Start(messagePtr);

        aMessage.Complete(ret);
        break;
        }

    case ENcmSetIapId:
        {
        TInt iap  = aMessage.Int0();
        TInt ret = iNcmEngine.SetStateToWatcher(ENcmSetTypeIapId, iap);

        aMessage.Complete(ret);
        }
        break;

    case ENcmSetDhcpResult:
        {
        TInt result = aMessage.Int0();
        TInt ret =  iNcmEngine.SetStateToWatcher(ENcmSetTypeDhcpReturn, result);

        aMessage.Complete(ret);
        }
        break;

    case ENcmDhcpProvisionNotify:
        {
        TInt ret = iNcmEngine.RegisterNotify(aMessage);

        if (KErrNone != ret)
            {
            aMessage.Complete(ret);
            }
        }
        break;

    case ENcmDhcpProvisionNotifyCancel:
        {
        iNcmEngine.DeRegisterNotify();
        aMessage.Complete(KErrNone);
        }
        break;

    case ENcmTransferBufferSize:
        {
        TInt size  = aMessage.Int0();
        TInt ret = iNcmEngine.SetInEpBufferSize(size);
        aMessage.Complete(ret);
        }
        break;

    default:
        // Unknown function, panic the user
        aMessage.Panic(KNcmSrvPanic, ENcmBadNcmMessage);
        break;
        }
    }
