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

#ifndef HANDLERINTERFACE_H
#define HANDLERINTERFACE_H


// USB MS sub-command creator class
// Intend to be used by event handler and some sub-commands.
class MUsbMsSubCommandCreator
    {
public:
    // Interface to create new sub-cmmand method.
    virtual void CreateSubCmdForRetrieveDriveLetterL(
            TInt aLogicalUnitCount) = 0;
    virtual void CreateSubCmdForMountingLogicalUnitL(
            TText aDrive, TInt aLuNumber) = 0;
    virtual void CreateSubCmdForSaveLatestMountInfoL(
            TText aDrive, TInt aLuNumber) = 0;    
    };

// USB MS event handler interface class
// Intend to be used by sub-command objects
class MUsbMsEventHandler
    {
public:
    // Activate the handler
    virtual void Start() = 0;
    // Complete current request. 
    // Intend to be used by sync command to simulate async behavior.
    virtual void Complete(TInt aError = KErrNone) = 0;
    virtual TRequestStatus& Status() const = 0;
    };

#endif /*HANDLERINTERFACE_H*/
