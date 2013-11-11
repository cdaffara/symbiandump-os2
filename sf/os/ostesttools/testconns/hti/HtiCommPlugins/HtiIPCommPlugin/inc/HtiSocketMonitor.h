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
* Description:  ECOM plugin to communicate over IP port
*
*/


#ifndef CHTISOCKETS_H
#define CHTISOCKETS_H


// INCLUDES
#include <HtiCommPluginInterface.h> // defined in HtiFramework project


// FORWARD DECLARATIONS

// CLASS DECLARATION

class MHtiSocketObserver
{
public:
    enum TRequestType
    {
    EReadSocket,
    EWriteSocket
    };

    virtual void ReportComplete( MHtiSocketObserver::TRequestType aType, TInt aError ) = 0;
};


class CHtiSocketMonitor : public CActive
{
public:
    static CHtiSocketMonitor* NewL( MHtiSocketObserver::TRequestType aType, MHtiSocketObserver* aObserver );
    void Activate();
    ~CHtiSocketMonitor();

private:
    CHtiSocketMonitor( MHtiSocketObserver::TRequestType aType, MHtiSocketObserver* aObserver );
    void ConstructL();

public: // CActive interface
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    MHtiSocketObserver::TRequestType iType;
    MHtiSocketObserver* iObserver;
};


#endif
