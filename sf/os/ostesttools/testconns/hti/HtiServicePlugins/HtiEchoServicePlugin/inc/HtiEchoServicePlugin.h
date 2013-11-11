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
* Description:  Implementation of ECOM plug-in service interface providing
*                the echo service.
*
*/


#ifndef CHTIECHOSERVICEPLUGIN_H__
#define CHTIECHOSERVICEPLUGIN_H__

// INCLUDES
#include <HtiServicePluginInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Implementation of ECOM plug-in service interface.
*  Provides the echo service.
*
*/
class CHtiEchoServicePlugin : public CHTIServicePluginInterface
    {
public:

    static CHtiEchoServicePlugin* NewL();

    // Interface implementation
    void ProcessMessageL(const TDesC8& aMessage,
                        THtiMessagePriority aPriority);

    void NotifyMemoryChange(TInt aAvailableMemory);

protected:

    CHtiEchoServicePlugin();
    void ConstructL();

    virtual ~CHtiEchoServicePlugin();

protected:
    HBufC8* iReply;
    };

#endif // CHTIECHOSERVICEPLUGIN_H__


// End of File
