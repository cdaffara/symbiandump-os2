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




#ifndef TSTL_1_H
#define TSTL_1_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( Ktstl_1LogPath, "\\logs\\testframework\\tstl_1\\" ); 
// Log file
_LIT( Ktstl_1LogFile, "tstl_1.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctstl_1;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctstl_1 test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctstl_1) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctstl_1* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctstl_1();

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
        Ctstl_1( CTestModuleIf& aTestModuleIf );

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
       virtual TInt ToUpper( CStifItemParser& aItem );
        virtual TInt ToLower( CStifItemParser& aItem );
        virtual TInt Islower( CStifItemParser& aItem );
        virtual TInt Isupper( CStifItemParser& aItem );
        virtual TInt Maxof( CStifItemParser& aItem );
        virtual TInt Minof( CStifItemParser& aItem );
        virtual TInt Maxelement( CStifItemParser& aItem );
        virtual TInt Minelement( CStifItemParser& aItem );
        virtual TInt Lowerbound( CStifItemParser& aItem );
        virtual TInt Upperbound( CStifItemParser& aItem );
        virtual TInt Swapof( CStifItemParser& aItem );
        virtual TInt Accumulate( CStifItemParser& aItem );
        virtual TInt Adjacent_difference(CStifItemParser& aItem);
        virtual TInt Adjacent_find(CStifItemParser& aItem);
        virtual TInt Logical_or( CStifItemParser& aItem );
        virtual TInt Logical_and( CStifItemParser& aItem );
        virtual TInt Logical_not( CStifItemParser& aItem );
        virtual TInt count_fun( CStifItemParser& aItem );
        virtual TInt countif_fun( CStifItemParser& aItem );
        virtual TInt equal_fun( CStifItemParser& aItem );
        virtual TInt equalto( CStifItemParser& aItem );
        virtual TInt notequalto( CStifItemParser& aItem );
        virtual TInt sort_fun( CStifItemParser& aItem );
        virtual TInt greater_equal_fun( CStifItemParser& aItem );
        virtual TInt less_equal_fun( CStifItemParser& aItem );
        virtual TInt greater_fun( CStifItemParser& aItem );
        virtual TInt less_fun( CStifItemParser& aItem );
        virtual TInt find_fun( CStifItemParser& aItem );
        virtual TInt findif_fun( CStifItemParser& aItem );
        virtual TInt findend_fun( CStifItemParser& aItem );
        virtual TInt findfirstof_fun( CStifItemParser& aItem );
        virtual TInt Minus( CStifItemParser& aItem );
        virtual TInt Plus( CStifItemParser& aItem );
        virtual TInt Divides( CStifItemParser& aItem );
        virtual TInt Modulus( CStifItemParser& aItem );
 		virtual TInt Negate( CStifItemParser& aItem );
 		virtual TInt Logicalnot( CStifItemParser& aItem );
		 virtual TInt Logicaland( CStifItemParser& aItem );
 		virtual TInt Logicalor( CStifItemParser& aItem );
		 virtual TInt Remove( CStifItemParser& aItem );
		 virtual TInt Removeif( CStifItemParser& aItem );
		 virtual TInt Removecopy( CStifItemParser& aItem );
 		virtual TInt Removecopy_if( CStifItemParser& aItem );
		 virtual TInt Replace_fun( CStifItemParser& aItem );
		 virtual TInt Replacecopy_fun( CStifItemParser& aItem );
		 virtual TInt Replacecopyif_fun( CStifItemParser& aItem );
		 virtual TInt Replaceif_fun( CStifItemParser& aItem );
		 virtual TInt Reverse_fun( CStifItemParser& aItem );
		 virtual TInt Reversecopy_fun( CStifItemParser& aItem );
 		virtual TInt Rotate_fun( CStifItemParser& aItem );
 		virtual TInt Rotatecopy_fun( CStifItemParser& aItem );
 		virtual TInt equalrange( CStifItemParser& aItem );
		 virtual TInt exceptionL( CStifItemParser& aItem );
		 virtual TInt foreach( CStifItemParser& aItem );
		 virtual TInt gsliceL( CStifItemParser& aItem );
 		virtual TInt sliceL( CStifItemParser& aItem );
		 virtual TInt unaryfunc( CStifItemParser& aItem );
		 virtual TInt uni_fill( CStifItemParser& aItem );
		 virtual TInt uni_fill_n( CStifItemParser& aItem );
		 virtual TInt uni_copy( CStifItemParser& aItem );
 		virtual TInt maskarray( CStifItemParser& aItem );
 		virtual TInt autoptr( CStifItemParser& aItem );
 		virtual TInt randshuf( CStifItemParser& aItem );
		 virtual TInt randstoriter( CStifItemParser& aItem );
		 virtual TInt iterswap( CStifItemParser& aItem );

		 virtual TInt ios1( CStifItemParser& aItem );
		 virtual TInt ios2( CStifItemParser& aItem );
		 virtual TInt ios3( CStifItemParser& aItem );
		 		 virtual TInt nthelement( CStifItemParser& aItem );
		 		 virtual TInt hasfacet( CStifItemParser& aItem );

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

#endif      // TSTL_1_H

// End of File
