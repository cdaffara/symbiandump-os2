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
* Description:  Client side interface to HtiIPCommServer
*
*/


#ifndef __HTIIPCOMMSERVERCLIENT_H__
#define __HTIIPCOMMSERVERCLIENT_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

class RHtiIPCommServer : public RSessionBase
{
public: // Constructor and destructor
    IMPORT_C RHtiIPCommServer();

public:
    IMPORT_C TInt Connect();
    IMPORT_C void Close();
    IMPORT_C TVersion Version() const;

    IMPORT_C void Receive( TDes8& aData, TRequestStatus& aStatus );
    IMPORT_C void Send( const TDesC8& aData,TRequestStatus& aStatus );

    IMPORT_C void CancelReceive();
    IMPORT_C void CancelSend();

    IMPORT_C TInt GetSendBufferSize() const;
    IMPORT_C TInt GetReceiveBufferSize() const;
};

#endif

// End of File
