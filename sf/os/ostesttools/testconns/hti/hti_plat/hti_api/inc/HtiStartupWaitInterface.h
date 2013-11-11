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
* Description:  Definition of MHtiStartupWaitInterface that is used to make
*                HtiFramework wait for OS startup before continuing own startup.
*
*/


#ifndef HTISTARTUPWAITINTERFACE_H__
#define HTISTARTUPWAITINTERFACE_H__

#include <e32std.h>

const TUid KHtiStartupWaitInterfaceUid = { 0x1020DEB9 };

class MHtiStartupWaitInterface
    {
    public:
        virtual TInt WaitForStartup( TInt aMaxWaitTime ) = 0;
    };

#endif

// End of file
