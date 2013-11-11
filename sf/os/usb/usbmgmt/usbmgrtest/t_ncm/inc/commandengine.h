/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
 @internalComponent
 @test
 */


#ifndef COMMANDENGINE_H
#define COMMANDENGINE_H

#include "ncmtestdefines.hrh"

#include <e32base.h>
#include <e32hashtab.h>

#define COMMAND_MAX_COUNT_PER_PAGE NUM_EVENT_ON_SCREEN
#define COMMAND_MAX_PAGE 9

class CUsbNcmConsole;

NONSHARABLE_CLASS(CNcmCommandBase) : public CActive
/**
 The CNcmCommandBase class is the base class of command classes
 */
    {
    friend class CNcmCommandEngine;
public:
    virtual ~CNcmCommandBase();

    //Set the command key
    void SetKey(TUint aKey);
    //Get the command key
    TUint Key() const;

    //The command action when user choose a command. It must be implemneted by command classes.
    virtual void DoCommandL() = 0;

    //Set the command description. which will be displayed in command help.
    void SetDescription(const TDesC& aDescription);
    const virtual TDesC& Description();

protected:
    CNcmCommandBase(TInt aPriority, CUsbNcmConsole& aConsole, TUint aKey);

protected:
    //From CActive
    virtual void DoCancel();
    virtual void RunL();

protected:
    //Main console
    CUsbNcmConsole& iTestConsole;

    //The character which indicates the command
    TUint iKey;
    //The command's description
    TBuf<NUM_CHARACTERS_ON_LINE> iDescription;

    };

NONSHARABLE_CLASS(CNcmCommandEngine) : public CActive
/**
 CNcmCommandEngine manages all command classes.
 */
    {
public:
    static CNcmCommandEngine* NewL(CUsbNcmConsole& aUsb);
    ~CNcmCommandEngine();

    //Add command to command engine.
    void RegisterCommand(CNcmCommandBase* aCommand);
    //Destroy all commands in command engine.
    void RemoveAllCommand();

    //Get command help info.
    void PrintHelp(TInt aPage);

private:
    CNcmCommandEngine(CUsbNcmConsole& aUsb);
    void ConstructL();

    //From CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

private:
    //Main console
    CUsbNcmConsole& iTestConsole;

    //The mapping of key and commands.
    RHashMap<TUint, CNcmCommandBase*> iCommandMap;
    //keep the key infomation, the command will be displayed in which page and which line.
    TUint iCommandKeys[COMMAND_MAX_PAGE][COMMAND_MAX_COUNT_PER_PAGE];
    //Indicate the page of the last command .
    TInt iLastPage;
    //Indicate the place of the last command in the last page.
    TInt iLastItem;
    //Indicate the rest lines in the last used page.
    TInt iRemainLine;
    };

#endif // COMMANDENGINE_H
