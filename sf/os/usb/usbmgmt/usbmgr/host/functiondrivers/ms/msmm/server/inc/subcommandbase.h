/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SUBCOMMANDBASE_H
#define SUBCOMMANDBASE_H

#include <e32const.h>
#include <e32cmn.h>
#include <usb/hostms/srverr.h>

class TDeviceEvent;
class MMsmmSrvProxy;
class MUsbMsEventHandler;
class MUsbMsSubCommandCreator;

NONSHARABLE_CLASS(THostMsSubCommandParam)
    {
public:
    MMsmmSrvProxy& iServer;
    MUsbMsEventHandler& iHandler;
    MUsbMsSubCommandCreator& iCreator;
    TDeviceEvent& iEvent;
    
public:
    THostMsSubCommandParam(MMsmmSrvProxy& aServer, 
            MUsbMsEventHandler& aHandler, 
            MUsbMsSubCommandCreator& aCreator, TDeviceEvent& aEvent);
    };

class TSubCommandBase
    {    
public:
	TSubCommandBase(THostMsSubCommandParam& aParameter);

	// Called to execute current sub-command.
    void ExecuteL();
    // Called to do the further operation when current sub-command 
    // is done.
    void AsyncCmdCompleteL();
    // Called to cancel current pending sub-command
    void CancelAsyncCmd();
    // Called to generate command realted error notification data
    virtual void HandleError(THostMsErrData& aData, TInt aError) = 0;
    
    inline TBool IsExecuted() const;
    inline TBool IsKeyCommand() const;
    
protected:
    virtual void DoExecuteL() = 0;
    virtual void DoAsyncCmdCompleteL();
    virtual void DoCancelAsyncCmd();
    
protected:
    MMsmmSrvProxy& iServer;
    MUsbMsEventHandler& iHandler;
    MUsbMsSubCommandCreator& iCreator;
    const TDeviceEvent& iEvent;
    TBool iIsExecuted;
    TBool iIsKeyCommand;
    };

inline TBool TSubCommandBase::IsExecuted() const {return iIsExecuted;};
inline TBool TSubCommandBase::IsKeyCommand() const {return iIsKeyCommand;};

#endif /*SUBCOMMANDBASE_H*/
