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




#ifndef TSTL_3_H
#define TSTL_3_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( Ktstl_3LogPath, "\\logs\\testframework\\tstl_3\\" ); 
// Log file
_LIT( Ktstl_3LogFile, "tstl_3.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctstl_3;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctstl_3 test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctstl_3) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctstl_3* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctstl_3();

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
        Ctstl_3( CTestModuleIf& aTestModuleIf );

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
        virtual TInt Includes( CStifItemParser& aItem );
                virtual TInt innerproduct( CStifItemParser& aItem );

        virtual TInt inplacemerge( CStifItemParser& aItem );

        virtual TInt Queue( CStifItemParser& aItem );

        virtual TInt Pqueue( CStifItemParser& aItem );

        virtual TInt stablesortL( CStifItemParser& aItem );

        virtual TInt Stack( CStifItemParser& aItem );
        virtual TInt string1_testL( CStifItemParser& aItem );
        
        virtual TInt Setintersection( CStifItemParser& aItem );
        virtual TInt Setsymdifference( CStifItemParser& aItem );
        virtual TInt Setunion( CStifItemParser& aItem );
        virtual TInt HeapL( CStifItemParser& aItem );
        virtual TInt Swapranges( CStifItemParser& aItem );
        virtual TInt Partialsums( CStifItemParser& aItem );
        virtual TInt Unarynegate(CStifItemParser& aItem);
         virtual TInt Not2(CStifItemParser& aItem);
          virtual TInt GettemporarybufferL(CStifItemParser& aItem);
       virtual TInt Pair(CStifItemParser& aItem);
       virtual TInt Lexicographicalcompare(CStifItemParser& aItem);
       virtual TInt Mismatch(CStifItemParser& aItem);
       virtual TInt Compare_fun(CStifItemParser& aItem);
       virtual TInt Copy_fun(CStifItemParser& aItem);
       virtual TInt Copybackward_fun(CStifItemParser& aItem);
       virtual TInt mergeL(CStifItemParser& aItem);
       virtual TInt allocatoradd(CStifItemParser& aItem);
       virtual TInt alldeall(CStifItemParser& aItem);
        virtual TInt allcons(CStifItemParser& aItem);
        virtual TInt allrebind(CStifItemParser& aItem);
         virtual TInt allmaxsize(CStifItemParser& aItem);


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

#endif      // TSTL_3_H

// End of File
