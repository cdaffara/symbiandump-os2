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
* Description: Implements common reset module for STIF 
* TestFramework. According to this DLL template can be implemented 
* licensee specific reset module.
*
*/

// INCLUDE FILES
#include "StifHWResetStub.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CStifHWResetStub

    Method: CStifHWResetStub

    Description: Default constructor.
    
    Parameters:    None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CStifHWResetStub::CStifHWResetStub()
    {
    // None

    }

/*
-------------------------------------------------------------------------------

    Class: CStifHWResetStub

    Method: ConstructL

    Description: Symbian OS second phase constructor. Nothing to do.

    Parameters:    None

    Return Values: None

    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CStifHWResetStub::ConstructL()
    {
    // Create Logger
    iLogger = CStifLogger::NewL( _L( "C:\\logs\\testframework\\testengine\\"),
                                    _L( "StifHWResetStub" ),
                                        CStifLogger::ETxt,
                                        CStifLogger::EFile,
                                        EFalse, // Append
                                        ETrue,
                                        ETrue,
                                        EFalse,
                                        EFalse );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifHWResetStub

    Method: NewL

    Description: Two-phased constructor. Constructs new CStifHWResetStub
                 instance and returns pointer to it.
        
    Parameters: None

    Return Values: CStifHWResetStub*: new object.

    Errors/Exceptions: Leaves if memory allocation fails or ConstructL leaves.

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C CStifHWResetStub* CStifHWResetStub::NewL()
    {
    // Construct new CStifHWResetStub instance.
    CStifHWResetStub* self = new ( ELeave ) CStifHWResetStub();    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
  
    }

/*
-------------------------------------------------------------------------------

    Class: CStifHWResetStub

    Method: ~CStifHWResetStub

    Description: Destructor.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/    
CStifHWResetStub::~CStifHWResetStub()
    {
    delete iLogger;
    iLogger = NULL;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifHWResetStub

    Method: DoReset

    Description: Does reset according to reset type.
    
    Parameters: CTestModuleIf::TRebootType aResetType: in: Reset type

    Return Values: TInt: Symbian OS error

    Errors/Exceptions: Error code returned if reset method returns an error.

    Status: Proposal
    
-------------------------------------------------------------------------------
*/    
TInt CStifHWResetStub::DoReset( CTestModuleIf::TRebootType aResetType )
    {
    iLogger->Log( _L( "CStifHWResetStub::DoReset(), reset type: %d" ), aResetType );
    TInt ret( KErrNone );

    switch( aResetType )
        {
        case CTestModuleIf::EDefaultReset:
            {
            ret = KillProcess(); // For an example
            break;
            }
        case CTestModuleIf::EDeviceReset0:
            {
            ret = KillProcess(); // For an example
            break;
            }
        case CTestModuleIf::EDeviceReset1:
            {
            ret = KillProcess(); // For an example
            break;
            }
        case CTestModuleIf::EDeviceReset2:
            {
            ret = GeneralReset(); // Do nothing, for an example
            break;
            }
        case CTestModuleIf::EDeviceReset3:
            {
            ret = GeneralReset(); // Do nothing, for an example
            break;
            }
        case CTestModuleIf::EDeviceReset4:
            {
            ret = GeneralReset(); // Do nothing, for an example
            break;
            }
        case CTestModuleIf::EDeviceReset5:
            {
            ret = GeneralReset(); // Do nothing, for an example
            break;
            }
        default:
            {
            return KErrArgument;
            }
        }

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifHWResetStub

    Method: KillProcess

    Description: Kills process
    
    Parameters: None

    Return Values: TInt: Symbian OS error

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/    
TInt CStifHWResetStub::KillProcess()
    {
    iLogger->Log( _L( "KillProcess()" ) );
    // Do process kill if MTC messages not allowed
    RProcess thisProcess;
    thisProcess.Kill( KErrNone );
    thisProcess.Close();

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifHWResetStub

    Method: GeneralReset

    Description: General reset for example.
    
    Parameters: None

    Return Values: TInt: Symbian OS error

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/    
TInt CStifHWResetStub::GeneralReset()
    {
    // Lisencee specific reset
    iLogger->Log( _L( "GeneralReset()" ) );
    return KErrNone;

    }

// ================= OTHER EXPORTED FUNCTIONS =================================

/*
-------------------------------------------------------------------------------
   
    Function: LibEntryL

    Description: Polymorphic Dll Entry Point
    
    Test framework calls this function to obtain new instance of test module
    class. 

    Parameters:    None
    
    Return Values: CStifHWResetStub* Pointer to CStifHWResetStub instance
    
    Errors/Exceptions: Leaves if CStifHWResetStub::NewL leaves
    
    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CStifHWResetStub* LibEntryL()
    {
    return CStifHWResetStub::NewL();

    }

// End of File
