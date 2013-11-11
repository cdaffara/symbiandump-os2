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

#ifndef CONSOLEMENU_H
#define CONSOLEMENU_H

// System includes
#include <e32base.h>
#include <e32cons.h>
#include <e32hashtab.h>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobserver.h>
#include <memspysession.h>
// User includes
#include "ConsoleConstants.h"

// Classes referenced
class CConsoleBase;
class RMemSpySession;

class CMemSpyConsoleMenu : public CActive, public MMemSpyEngineObserver
    {
public:
    static CMemSpyConsoleMenu* NewLC( RMemSpySession& aSession, CConsoleBase& aConsole );
    ~CMemSpyConsoleMenu();

private:
    CMemSpyConsoleMenu( RMemSpySession& aEngine, CConsoleBase& aConsole );
    void ConstructL();

public: // API
    void DrawMenuL();
    void WaitForInput();

private: // From CActive
    void RunL();
    void DoCancel();

private: // From MMemSpyEngineObserver
    void HandleMemSpyEngineEventL( MMemSpyEngineObserver::TEvent aEvent, TAny* aContext );

private: // Command handlers
    void OnCmdSinkTypeToggleL();
    void OnCmdKernelObjectListingL();
    void OnCmdHeapDataKernelL();
    void OnCmdHeapDataUserL();
    void OnCmdCSVListingHeapL();
    void OnCmdCSVListingStackL();
    void OnCmdHeapCellListUserL();

private: // Internal methods
    void ClearCommandBuffer();
    void ProcessCommandBufferL();
    void RedrawInputPrompt();
    void RedrawStatusMessage();
    void RedrawStatusMessage( const TDesC& aMessage );
    void GetProcessName();
    void InitiateMemSpyClientServerOperationL( TInt aOpCode );

private: // Data members
    RMemSpySession& iSession;
    CConsoleBase& iConsole;
    
    TMemSpyOutputType iOutputType;
    //
    TBuf<KMemSpyMaxInputBufferLength> iCommandBuffer;
    TPoint iCommandPromptPos;
    TPoint iStatusMessagePos;
    TBool iRunningDeviceWideOperation;
    };



#endif
