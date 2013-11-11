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


#ifndef TLOCALE_H
#define TLOCALE_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_MODULE_VERSION_MAJOR 50
#define TEST_MODULE_VERSION_MINOR 9
#define TEST_MODULE_VERSION_BUILD 06
// Logging path
_LIT( KtlocaleLogPath, "\\logs\\testframework\\tlocale\\" ); 
// Log file
_LIT( KtlocaleLogFile, "tlocale.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctlocale;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctlocale test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctlocale) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctlocale* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctlocale();

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
				/**
         * Method used to log version of test module
         */
        void SendTestModuleVersion();
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
        Ctlocale( CTestModuleIf& aTestModuleIf );

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
       virtual TInt hasfacet( CStifItemParser& aItem );
       virtual TInt usefacet( CStifItemParser& aItem );
			 virtual TInt numget( CStifItemParser& aItem );
       virtual TInt numput( CStifItemParser& aItem );
			 virtual TInt num_punct( CStifItemParser& aItem );
       virtual TInt numpunctbyname( CStifItemParser& aItem );
       virtual TInt moneyget( CStifItemParser& aItem );
       virtual TInt moneyput( CStifItemParser& aItem );
  		 virtual TInt money_punct( CStifItemParser& aItem );
       virtual TInt moneypunctbyname( CStifItemParser& aItem );
       virtual TInt timeget( CStifItemParser& aItem );
       virtual TInt timeput( CStifItemParser& aItem );
       virtual TInt messagesL( CStifItemParser& aItem );
       virtual TInt messagesbyname( CStifItemParser& aItem );
       virtual TInt collateL( CStifItemParser& aItem );
       virtual TInt collatebyname( CStifItemParser& aItem );
       virtual TInt codecvt1( CStifItemParser& aItem );
       virtual TInt codecvt2( CStifItemParser& aItem );

       virtual TInt codecvtbyname1( CStifItemParser& aItem );
       virtual TInt codecvtbyname2( CStifItemParser& aItem );

#if 1
			virtual TInt ctype_byname1L( CStifItemParser& aItem );
			virtual TInt moneypunct_byname1L( CStifItemParser& aItem );

			virtual TInt moneypunct1L( CStifItemParser& aItem );
			virtual TInt numpunct1L( CStifItemParser& aItem );
			virtual TInt numpunct_byname1L( CStifItemParser& aItem );
#endif			
//			virtual TInt num_get1L( CStifItemParser& aItem );




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

#endif      // TLOCALE_H

// End of File
