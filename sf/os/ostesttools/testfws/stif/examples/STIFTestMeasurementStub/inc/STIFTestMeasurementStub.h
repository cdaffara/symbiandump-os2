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
* Description: STIF measurement stub module declaration
*
*/

#ifndef STIFTESTMEASUREMENTSTUB_H
#define STIFTESTMEASUREMENTSTUB_H

// INCLUDES
#include <f32file.h>
#include <StifTestInterface.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  This a CSTIFTestMeasurementStub stub class for Test Measurement Module
*  Pluging.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CSTIFTestMeasurementStub ) : 
                                public CSTIFTestMeasurementImplementation
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSTIFTestMeasurementStub* NewL( 
                const TDesC& aConfigurationInfo,
                CSTIFTestMeasurement::TSTIFMeasurementType aMeasurementType );

        /**
        * Destructor.
        */
        virtual ~CSTIFTestMeasurementStub();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CSTIFTestMeasurementStub Start
        * @since ?Series60_version
        * @return Symbian OS error code
        */
        virtual TInt Start();

        /**
        * From CSTIFTestMeasurementStub Start
        * @since ?Series60_version
        * @return Symbian OS error code
        */
        virtual TInt Stop();

        /**
        * From CSTIFTestMeasurementStub Start
        * @since ?Series60_version
        * @return CSTIFTestMeasurement::TSTIFMeasurementType
        */
        virtual CSTIFTestMeasurement::TSTIFMeasurementType MeasurementType();

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

        /**
        * C++ default constructor.
        */
        CSTIFTestMeasurementStub(
            CSTIFTestMeasurement::TSTIFMeasurementType aMeasurementType );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aConfigurationInfo );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data

        // Test Measurement module type
        CSTIFTestMeasurement::TSTIFMeasurementType iMeasurementType;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // STIFTESTMEASUREMENTSTUB_H

// End of File
