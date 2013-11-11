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

#ifndef NCMSESSION_H
#define NCMSESSION_H

#include <e32base.h>


class CNcmEngine;

NONSHARABLE_CLASS(CNcmSession) : public CSession2
    {
public:
    static CNcmSession* NewL(CNcmEngine& aEngine);
    ~CNcmSession();

private:
    CNcmSession(CNcmEngine& aEngine);

private: // from CSession2
    /**
    Called when a message is received from the client.
    @param aMessage Message received from the client.
    */
    void ServiceL(const RMessage2& aMessage);

private:
    // unowned
    CNcmEngine& iNcmEngine;
    };

#endif // NCMSESSION_H
