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
* Description: This file contains the header file of the STIF test
* interference module.
*
*/

#ifndef STIFTESTINTERFERENCE_H
#define STIFTESTINTERFERENCE_H

// INCLUDES
#include "StifTestModule.h"
#include <StifLogger.h>
#include <e32std.h>
#include <e32base.h>
#include <e32svr.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// CLASS DECLARATION

/**
*  This a MSTIFTestInterference class.
*  MSTIFTestInterference is an interface class. Purpose is to 
*  avoid multi heritance in subclasses.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( MSTIFTestInterference )
    {
    public:     // Enumerations 

    // Way of using test interference
    enum TStifTestInterferenceCategory
        {
        EActiveObject,      // Test interference using active object
        EThread,            // Test interference using thread
        };

    // Type of the test interference
    enum TStifTestInterferenceType
        {
        ENone,                    // No test interference
        ECpuLoad,                 // with CPU load
        EFileSystemReadC,         // with file system load, read from C-drive
        EFileSystemReadD,         // with file system load, read from D-drive
        EFileSystemReadE,         // with file system load, read from E-drive
        EFileSystemReadZ,         // with file system load, read from Z-drive
        EFileSystemWriteC,        // with file system load, write from C-drive
        EFileSystemWriteD,        // with file system load, write from D-drive
        EFileSystemWriteE,        // with file system load, write from E-drive
        EFileSystemFillAndEmptyC, // with file system load, fill and empty from
                                  // C-drive
        EFileSystemFillAndEmptyD, // with file system load, fill and empty from
                                  // D-drive
        EFileSystemFillAndEmptyE, // with file system load, fill and empty from
                                  // E-drive

        ENoneMicroSeconds,                    // No test interference
        ECpuLoadMicroSeconds,                 // with CPU load
        EFileSystemReadCMicroSeconds,         // with file system load, read from C-drive
        EFileSystemReadDMicroSeconds,         // with file system load, read from D-drive
        EFileSystemReadEMicroSeconds,         // with file system load, read from E-drive
        EFileSystemReadZMicroSeconds,         // with file system load, read from Z-drive
        EFileSystemWriteCMicroSeconds,        // with file system load, write from C-drive
        EFileSystemWriteDMicroSeconds,        // with file system load, write from D-drive
        EFileSystemWriteEMicroSeconds,        // with file system load, write from E-drive
        EFileSystemFillAndEmptyCMicroSeconds, // with file system load, fill and empty from
                                  			  // C-drive
        EFileSystemFillAndEmptyDMicroSeconds, // with file system load, fill and empty from
                                  			  // D-drive
        EFileSystemFillAndEmptyEMicroSeconds  // with file system load, fill and empty from
                                  			  // E-drive
        };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param: aTestModuleBase: CTestModuleBase object for get test
                  interference handle to STIF's side(Used if test case panic so
                  test interference thread can be kill by STIF). 
        * @param: aCategory: Test interference category
        * @return: MSTIFTestInterference object
        */
        IMPORT_C static MSTIFTestInterference* NewL( 
                                CTestModuleBase* aTestModuleBase,
                                TStifTestInterferenceCategory aCategory );

        /**
        * Destructor.
        */
        virtual ~MSTIFTestInterference();

    public: // New functions

        /**
        * StartL method starts test interference.
        * @param: aType: Test interference type. 
        * @param: aIdleTime: Test interference idle time.
        * @param: aActiveTime: Test interference active time.
        * @return: Symbian error code.
        */
        virtual TInt StartL( TStifTestInterferenceType aType,
                            TInt aIdleTime,
                            TInt aActiveTime ) = 0;

        /**
        * Stop method stops test interference.
        */
        virtual TInt Stop() = 0;

        /**
        * Sets thread or active object priority. This should use before
        * test interference is started otherwise error code will return.
        *
        * Thread priority can set during interference executions time. User
        * should be sure that given value is acceptable, otherwise SetPriority
        * panics.
        *
        * @param: aPriority: New priority for active object or thread
        * @return: Symbian error code.
        */
        virtual TInt SetPriority( TInt aPriority ) = 0;

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // STIFTESTINTERFERENCE_H

// End of File
