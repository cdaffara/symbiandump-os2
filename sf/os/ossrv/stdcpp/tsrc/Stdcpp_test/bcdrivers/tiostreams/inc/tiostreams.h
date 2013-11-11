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
* Description: 
*
*/




#ifndef TIOSTREAMS_H
#define TIOSTREAMS_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KtiostreamsLogPath, "\\logs\\testframework\\tiostreams\\" ); 
// Log file
_LIT( KtiostreamsLogFile, "tiostreams.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctiostreams;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctiostreams test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctiostreams) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctiostreams* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctiostreams();

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
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

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
        Ctiostreams( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
               
 virtual TInt iofstreamL( CStifItemParser& aItem );
 virtual TInt stringbufL( CStifItemParser& aItem );
 virtual TInt stringstreamL( CStifItemParser& aItem );
 virtual TInt streambufL( CStifItemParser& aItem );
 virtual TInt ostreamL( CStifItemParser& aItem );
 virtual TInt istreamL( CStifItemParser& aItem );
 virtual TInt istringstreamL( CStifItemParser& aItem );
 virtual TInt ostringstreamL( CStifItemParser& aItem );
 virtual TInt ostreamiterators( CStifItemParser& aItem );
 virtual TInt fstreamL( CStifItemParser& aItem );
 virtual TInt istrstreamL( CStifItemParser& aItem );
 virtual TInt strstreamL( CStifItemParser& aItem );
 virtual TInt ostrstreamL( CStifItemParser& aItem );
 virtual TInt istreamiterators( CStifItemParser& aItem );
 virtual TInt istreambufiterators( CStifItemParser& aItem );
 virtual TInt strstreambufL( CStifItemParser& aItem );
 virtual TInt freezeL( CStifItemParser& aItem );
  virtual TInt fposL( CStifItemParser& aItem );
  virtual TInt filebufL( CStifItemParser& aItem );
  virtual TInt seekpL( CStifItemParser& aItem );

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

#endif      // TIOSTREAMS_H

// End of File
