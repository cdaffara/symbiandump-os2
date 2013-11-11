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
* Description: This module contains implementation of CNullOutput 
* class member functions.
*
*/

// INCLUDE FILES
#include "Output.h"
#include "NullOutput.h"

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
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CNullOutput

    Method: CNullOutput

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CNullOutput::CNullOutput()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CNullOutput

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CNullOutput::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CNullOutput

    Method: NewL

    Description: Two-phased constructor.

    Parameters: None

    Return Values: CNullOutput*: pointer to CNullOutput object

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CNullOutput* CNullOutput::NewL()
    {
    // Create CNullOutput object nullOutput
    CNullOutput* nullOutput = new (ELeave) CNullOutput();

    CleanupStack::PushL( nullOutput );
    nullOutput->ConstructL();
    CleanupStack::Pop( nullOutput );

    return nullOutput;

    }

/*
-------------------------------------------------------------------------------

    Class: CNullOutput

    Method: ~CNullOutput

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CNullOutput::~CNullOutput()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CNullOutput

    Method: Write

    Description: Write log information or data to the file. 16 bit.

    CNullOutput's Write() method purpose is to look like logging although it
    don't do any logging operations.

    Parameters: TBool aWithTimeStamp: in: Is time stamp flag used
                TBool aWithLineBreak: in: Is line break flag used
                TBool aWithEventRanking: in: Is event ranking flag used
                const TDesC& aData: in: Logged or saved data

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CNullOutput::Write( TBool /* aWithTimeStamp */, 
                            TBool /* aWithLineBreak */,
                            TBool /* aWithEventRanking */,
                            const TDesC& /* aData */ )
    {
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CNullOutput

    Method: Write

    Description: Write log information or data to the file. 8 bit.

    CNullOutput's Write() method purpose is to look like logging although it
    don't do any logging operations.

    Parameters: TBool aWithTimeStamp: in: Is time stamp flag used
                TBool aWithLineBreak: in: Is line break flag used
                TBool aWithEventRanking: in: Is event ranking flag used
                const TDesC8& aData: in: Logged or saved data

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CNullOutput::Write(  TBool /* aWithTimeStamp */, 
                            TBool /* aWithLineBreak */,
                            TBool /* aWithEventRanking */,
                            const TDesC8& /* aData */ )
    {
    return KErrNone;

    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
