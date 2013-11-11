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
* Description: This file contains common data for Test Engine.
*
*/

#ifndef TEST_ENGINE_COMMON_H
#define TEST_ENGINE_COMMON_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
// None

// DESCRIPTION

// Server name
_LIT(KTestEngineName,"TestFrameworkEngine");

// The server version.
// A version must be specified when creating a session with the server.
const TUint KTestEngineMajorVersionNumber = 0;
const TUint KTestEngineMinorVersionNumber = 2;
const TUint KTestEngineBuildVersionNumber = 1;

// Function for starting the TestEngineServer
IMPORT_C TInt StartEngine();

#endif // TEST_ENGINE_COMMON_H

// End of File
