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
* Description:  CHtiIPCommServer declaration
*
*/


#ifndef __HTIIPCOMMSERVER_H__
#define __HTIIPCOMMSERVER_H__

//  INCLUDES

#include <e32def.h>
#include <e32base.h>

#include "HtiConnectionManager.h"
#include "HtiIPCommServerSession.h"

//  CONSTANTS

// Reasons for IPCommServer panic
enum TIPCommServerPanic
    {
    EBadDescriptor = 1,
    EUnknownCompletion,
    EUnknownState
    };

//  FUNCTION PROTOTYPES

// Function to panic the server
//GLREF_C void PanicServer(TIPCommServerPanic aPanic);


//  FORWARD DECLARATIONS
class CHtiConnectionManager;

// CLASS DECLARATION


class CHtiIPCommServer : public CServer2
{
public:
    static CHtiIPCommServer* NewLC();

    /**
     * Used to inform the server that session has been created
     */
    void SessionOpened();

    /**
     * Used to inform the server that session has been closed
     */
    void SessionClosed();

    void CloseServer();

private:
    CHtiIPCommServer();

    ~CHtiIPCommServer();

    void ConstructL();

   /**
    * Creates a new client session
    * @param aVersion Version of the server
    * @return Pointer to the clinet session
    */
    CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage) const;

public:
    CHtiConnectionManager* iConnectionManager;

    // Used to prevent stopping active scheduler twice
    TBool iRunning;

private:
    TInt  iNrOfSessions;
};

#endif // __HTIIPCOMMSERVER_H__

// End of file
