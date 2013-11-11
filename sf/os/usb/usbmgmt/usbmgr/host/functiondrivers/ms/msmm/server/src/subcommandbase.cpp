/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "subcommandbase.h"
#include "msmm_internal_def.h"
#include "msmmserver.h"
#include "eventhandler.h"

#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "subcommandbaseTraces.h"
#endif



THostMsSubCommandParam::THostMsSubCommandParam(MMsmmSrvProxy& aServer, 
        MUsbMsEventHandler& aHandler, 
        MUsbMsSubCommandCreator& aCreator, 
        TDeviceEvent& aEvent) :
    iServer(aServer),
    iHandler(aHandler),
    iCreator(aCreator),
    iEvent(aEvent)
    {
    OstTraceFunctionEntry0( THOSTMSSUBCOMMANDPARAM_THOSTMSSUBCOMMANDPARAM_CONS_ENTRY );
    }

TSubCommandBase::TSubCommandBase(THostMsSubCommandParam& aParameter):
iServer(aParameter.iServer),
iHandler(aParameter.iHandler),
iCreator(aParameter.iCreator),
iEvent(aParameter.iEvent),
iIsExecuted(EFalse),
iIsKeyCommand(ETrue)
    {
    OstTraceFunctionEntry0( TSUBCOMMANDBASE_TSUBCOMMANDBASE_CONS_ENTRY );
    }

void TSubCommandBase::ExecuteL()
    {
    iIsExecuted = ETrue;
    DoExecuteL();
    }

void TSubCommandBase::AsyncCmdCompleteL()
    {
    OstTraceFunctionEntry0( TSUBCOMMANDBASE_ASYNCCMDCOMPLETEL_ENTRY );
    
    DoAsyncCmdCompleteL();
    OstTraceFunctionExit0( TSUBCOMMANDBASE_ASYNCCMDCOMPLETEL_EXIT );
    }

void TSubCommandBase::CancelAsyncCmd()
    {
    OstTraceFunctionEntry0( TSUBCOMMANDBASE_CANCELASYNCCMD_ENTRY );
    
    DoCancelAsyncCmd();
    OstTraceFunctionExit0( TSUBCOMMANDBASE_CANCELASYNCCMD_EXIT );
    }

void TSubCommandBase::DoAsyncCmdCompleteL()
    {
    OstTraceFunctionEntry0( TSUBCOMMANDBASE_DOASYNCCMDCOMPLETEL_ENTRY );
    
    // Empty implementation
    OstTraceFunctionExit0( TSUBCOMMANDBASE_DOASYNCCMDCOMPLETEL_EXIT );
    }

void TSubCommandBase::DoCancelAsyncCmd()
    {
    OstTraceFunctionEntry0( TSUBCOMMANDBASE_DOCANCELASYNCCMD_ENTRY );
    
    // Empty implementation
    OstTraceFunctionExit0( TSUBCOMMANDBASE_DOCANCELASYNCCMD_EXIT );
    }

// End of file
