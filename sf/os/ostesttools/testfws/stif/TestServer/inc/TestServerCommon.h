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
* Description: This file contains common data for Test Server.
*
*/


#ifndef TEST_SERVER_COMMON_H
#define TEST_SERVER_COMMON_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


// DESCRIPTION

// Server name
_LIT( KTestServerName,"TestFrameworkServer" );

// The server version.
// A version must be specified when creating a session with the server.
const TUint KTestServerMajorVersionNumber=0;
const TUint KTestServerMinorVersionNumber=1;
const TUint KTestServerBuildVersionNumber=1;


#endif // TEST_SERVER_COMMON_H

// End of File
