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


#ifndef TMATH_OPERATIONS_H
#define TMATH_OPERATIONS_H

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
_LIT( Ktmath_operationsLogPath, "\\logs\\testframework\\tmath_operations\\" ); 
// Log file
_LIT( Ktmath_operationsLogFile, "tmath_operations.txt" ); 
_LIT( Ktmath_operationsLogFileWithTitle, "tmath_operations_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctmath_operations;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctmath_operations test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctmath_operations) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctmath_operations* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctmath_operations();

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
        Ctmath_operations( CTestModuleIf& aTestModuleIf );

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
        virtual TInt Accumulate( CStifItemParser& aItem );
        virtual TInt Inner_Product( CStifItemParser& aItem );
        virtual TInt Partial_Sum( CStifItemParser& aItem );
		virtual TInt Adjacent_Difference( CStifItemParser& aItem );
		virtual TInt Power( CStifItemParser& aItem );
		virtual TInt Iota( CStifItemParser& aItem );
		
		virtual TInt Cos_float( CStifItemParser& aItem );
		virtual TInt Cos_double( CStifItemParser& aItem );
		virtual TInt Cos_longdouble( CStifItemParser& aItem );
		virtual TInt Sin_float( CStifItemParser& aItem );
		virtual TInt Sin_double( CStifItemParser& aItem );
		virtual TInt Sin_longdouble( CStifItemParser& aItem );
		virtual TInt Tan_float( CStifItemParser& aItem );
		virtual TInt Tan_double( CStifItemParser& aItem );
		virtual TInt Tan_longdouble( CStifItemParser& aItem );
		virtual TInt Pow_float( CStifItemParser& aItem );
		virtual TInt Pow_double( CStifItemParser& aItem );
		virtual TInt Pow_longdouble( CStifItemParser& aItem );
		virtual TInt Cosh_float( CStifItemParser& aItem );
		virtual TInt Cosh_double( CStifItemParser& aItem );
		virtual TInt Cosh_longdouble( CStifItemParser& aItem );
		virtual TInt Sinh_float( CStifItemParser& aItem );
		virtual TInt Sinh_double( CStifItemParser& aItem );
		virtual TInt Sinh_longdouble( CStifItemParser& aItem );
		virtual TInt Tanh_float( CStifItemParser& aItem );
		virtual TInt Tanh_double( CStifItemParser& aItem );
		virtual TInt Tanh_longdouble( CStifItemParser& aItem );
		virtual TInt Sqrt_float( CStifItemParser& aItem );
		virtual TInt Sqrt_double( CStifItemParser& aItem );
		virtual TInt Sqrt_longdouble( CStifItemParser& aItem );
		virtual TInt Log_float( CStifItemParser& aItem );
		virtual TInt Log_double( CStifItemParser& aItem );
		virtual TInt Log_longdouble( CStifItemParser& aItem );
		virtual TInt Log10_float( CStifItemParser& aItem );
		virtual TInt Log10_double( CStifItemParser& aItem );
		virtual TInt Log10_longdouble( CStifItemParser& aItem );
		virtual TInt Exp_float( CStifItemParser& aItem );
		virtual TInt Exp2_float( CStifItemParser& aItem );
		virtual TInt Exp2_double( CStifItemParser& aItem );
		virtual TInt Exp_double( CStifItemParser& aItem );
		virtual TInt Exp_longdouble( CStifItemParser& aItem );
		virtual TInt Fabs_float( CStifItemParser& aItem );
		virtual TInt Fabs_double( CStifItemParser& aItem );
		virtual TInt Fabs_longdouble( CStifItemParser& aItem );
		virtual TInt Acos_float( CStifItemParser& aItem );
		virtual TInt Acos_double( CStifItemParser& aItem );
		virtual TInt Acos_longdouble( CStifItemParser& aItem );
		virtual TInt Asin_float( CStifItemParser& aItem );
		virtual TInt Asin_double( CStifItemParser& aItem );
		virtual TInt Asin_longdouble( CStifItemParser& aItem );
		virtual TInt Atan_float( CStifItemParser& aItem );
		virtual TInt Atan_double( CStifItemParser& aItem );
		virtual TInt Atan_longdouble( CStifItemParser& aItem );
		virtual TInt Atan2_float( CStifItemParser& aItem );
		virtual TInt Atan2_double( CStifItemParser& aItem );
		virtual TInt Atan2_longdouble( CStifItemParser& aItem );
		virtual TInt Ceil_float( CStifItemParser& aItem );
		virtual TInt Ceil_double( CStifItemParser& aItem );
		virtual TInt Ceil_longdouble( CStifItemParser& aItem );
		virtual TInt Floor_float( CStifItemParser& aItem );
		virtual TInt Floor_double( CStifItemParser& aItem );
		virtual TInt Floor_longdouble( CStifItemParser& aItem );
		virtual TInt Fmod_float( CStifItemParser& aItem );
		virtual TInt Fmod_double( CStifItemParser& aItem );
		virtual TInt Fmod_longdouble( CStifItemParser& aItem );
		virtual TInt Modf_double( CStifItemParser& aItem );		
		virtual TInt Frexp_float( CStifItemParser& aItem );
		virtual TInt Frexp_double( CStifItemParser& aItem );
		virtual TInt Frexp_longdouble( CStifItemParser& aItem );
		virtual TInt Ldexp_float( CStifItemParser& aItem );
		virtual TInt Ldexp_double( CStifItemParser& aItem );
		virtual TInt Ldexp_longdouble( CStifItemParser& aItem );
	
		virtual TInt Cbrt ( CStifItemParser& aItem );               
		virtual TInt Acosh( CStifItemParser& aItem );
		virtual TInt Asinh( CStifItemParser& aItem );
		virtual TInt Atanh( CStifItemParser& aItem );
		virtual TInt Erf ( CStifItemParser& aItem );
    	virtual TInt Lgamma( CStifItemParser& aItem );		
		virtual TInt Acosf( CStifItemParser& aItem );
		virtual TInt Asinf( CStifItemParser& aItem );
		virtual TInt Atanf( CStifItemParser& aItem );
		virtual TInt Atan2f( CStifItemParser& aItem );
		virtual TInt Cosf( CStifItemParser& aItem );
		virtual TInt Sinf( CStifItemParser& aItem );
		virtual TInt Tanf( CStifItemParser& aItem );
		virtual TInt Coshf( CStifItemParser& aItem );
		virtual TInt Sinhf( CStifItemParser& aItem );
		virtual TInt Tanhf( CStifItemParser& aItem );       
		virtual TInt Ldexpf( CStifItemParser& aItem );
		virtual TInt Log10f( CStifItemParser& aItem );
		virtual TInt Logf( CStifItemParser& aItem );
		virtual TInt Modff( CStifItemParser& aItem );
		virtual TInt Powf( CStifItemParser& aItem );
		virtual TInt Sqrtf( CStifItemParser& aItem );
		virtual TInt Ceilf( CStifItemParser& aItem );
		virtual TInt Fabsf( CStifItemParser& aItem );
		virtual TInt Frexpf( CStifItemParser& aItem );
		virtual TInt Expf( CStifItemParser& aItem );		
		virtual TInt Fmodf( CStifItemParser& aItem );
		virtual TInt Roundf( CStifItemParser& aItem );
		virtual TInt LRound( CStifItemParser& aItem );
		virtual TInt LLRound( CStifItemParser& aItem );
		virtual TInt Round( CStifItemParser& aItem );
		virtual TInt Erff( CStifItemParser& aItem );
		virtual TInt Erfcf( CStifItemParser& aItem );
		virtual TInt Hypotf( CStifItemParser& aItem );
		virtual TInt Hypot( CStifItemParser& aItem );
		virtual TInt Lgammaf( CStifItemParser& aItem );
		virtual TInt Acoshf( CStifItemParser& aItem );
		virtual TInt Asinhf( CStifItemParser& aItem );
		virtual TInt Atanhf( CStifItemParser& aItem );
		virtual TInt Cbrtf( CStifItemParser& aItem );
		virtual TInt Logbf( CStifItemParser& aItem );
		virtual TInt Copysignf( CStifItemParser& aItem );
		virtual TInt Nextafterf( CStifItemParser& aItem );
		virtual TInt Remainderf( CStifItemParser& aItem );
		virtual TInt Rintf( CStifItemParser& aItem );
		virtual TInt Fmaxf( CStifItemParser& aItem );
		virtual TInt Fminf( CStifItemParser& aItem );
		virtual TInt Nearbyintf ( CStifItemParser& aItem );               
		virtual TInt Truncf( CStifItemParser& aItem );
		virtual TInt Fmaf ( CStifItemParser& aItem );
		virtual TInt Fma ( CStifItemParser& aItem );
		virtual TInt Expm1f_float( CStifItemParser& aItem );
		virtual TInt Expm1_double( CStifItemParser& aItem );
		virtual TInt Fdim_double( CStifItemParser& aItem );
		virtual TInt Fdimf_float( CStifItemParser& aItem );
		virtual TInt Fdiml_longdouble( CStifItemParser& aItem );
		virtual TInt Abs_float( CStifItemParser& aItem );
		virtual TInt Abs_double( CStifItemParser& aItem );
		virtual TInt Abs_longdouble( CStifItemParser& aItem );
		virtual TInt Scalbf( CStifItemParser& aItem );
		virtual TInt Significandf ( CStifItemParser& aItem );
		virtual TInt Fegetexceptflag ( CStifItemParser& aItem );
		virtual TInt Feraiseexcept ( CStifItemParser& aItem );
		virtual TInt Feclearexcept( CStifItemParser& aItem);
		virtual TInt Fesetexceptflag( CStifItemParser& aItem);
		
		virtual TInt Signbit( CStifItemParser& aItem );
		virtual TInt Isnormal( CStifItemParser& aItem );
		virtual TInt Isnan( CStifItemParser& aItem );
		virtual TInt Isinf( CStifItemParser& aItem );
		virtual TInt Isfinite ( CStifItemParser& aItem );               
		virtual TInt Fpclassifyd( CStifItemParser& aItem );
		virtual TInt Nanval ( CStifItemParser& aItem );
		virtual TInt Infinite ( CStifItemParser& aItem );

        //ADD NEW METHOD DEC HERE

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

#endif      // TMATH_OPERATIONS_H

// End of File
