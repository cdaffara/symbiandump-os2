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
* Description: This file contains the header file of the CNullOutput.
*
*/

#ifndef NULLOUTPUT_H
#define NULLOUTPUT_H

//  INCLUDES
#include "Output.h"

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
// CNullOutput is a STIF Test Framework StifLogger class.
// CNullOutput's methods purpose is to look like logging although it don't
// do any logging operations.

class CNullOutput 
            :public COutput
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CNullOutput* NewL();

        /**
        * Destructor.
        */
        ~CNullOutput();

    public:     // New functions

        /**
        * C++ default constructor.
        */
        CNullOutput();

        /**
        * Write log information or data to the file.
        * 16 bit.
        */
        TInt Write( TBool aWithTimeStamp,
                    TBool aWithLineBreak,
                    TBool aWithEventRanking,
                    const TDesC& aData );

        /**
        * Write log information or data to the file.
        * 8 bit.
        */
        TInt Write( TBool aWithTimeStamp,
                    TBool aWithLineBreak,
                    TBool aWithEventRanking,
                    const TDesC8& aData );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // NULLOUTPUT_H

// End of File
