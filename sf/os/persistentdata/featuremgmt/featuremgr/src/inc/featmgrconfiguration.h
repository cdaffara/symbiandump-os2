/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef FEATMGRCONFIGURATION_H
#define FEATMGRCONFIGURATION_H

//  INCLUDES
#include <e32base.h>
#include <e32uid.h>

// CONSTANTS
const TInt KServerVersionMajor          = 1; // UPDATE VERSION 
const TInt KServerVersionMinor          = 0;
const TInt KServerVersionBuild          = 0;
const TInt KServerCActivePriority       = CActive::EPriorityUserInput;
const TInt KDefaultFeatMgrTimer         = (10*1000*1000);
#ifdef EXTENDED_FEATURE_MANAGER_TEST
const TInt KShutdownTimeout				= 500000;
const TInt KServerUid2Int               = 0x10205054;
#else
const TInt KServerUid2Int               = 0x10205054;
#endif // EXTENDED_FEATURE_MANAGER_TEST
const TUid KServerUid2                  = { KServerUid2Int }; 

const TInt KDefaultAsyncSlots           = 1; // Async message slots to be 
                                             // used in client server comms. 

#ifdef EXTENDED_FEATURE_MANAGER_TEST
_LIT( KServerProcessName, "!test_featmgrserver" );
_LIT( KServerExeName, "test_featmgrserver.exe" );
#ifdef __WINS__
_LIT( KServerExeDrive, "Z:" );
#else
_LIT( KServerExeDrive, "C:" );
#endif // _WINS
#else
_LIT( KServerProcessName, "!FeatMgrServer" );
_LIT( KServerExeName, "FeatMgrServer.exe" );
_LIT( KServerExeDrive, "Z:" );
#endif // EXTENDED_FEATURE_MANAGER_TEST

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

#endif  // FEATMGRCONFIGURATION_H
            
// End of File
