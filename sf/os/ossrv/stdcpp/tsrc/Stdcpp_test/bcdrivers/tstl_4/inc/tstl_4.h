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




#ifndef TSTL_4_H
#define TSTL_4_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( Ktstl_4LogPath, "\\logs\\testframework\\tstl_4\\" ); 
// Log file
_LIT( Ktstl_4LogFile, "tstl_4.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctstl_4;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctstl_4 test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctstl_4) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctstl_4* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctstl_4();

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
        Ctstl_4( CTestModuleIf& aTestModuleIf );

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
        
        virtual TInt Minof( CStifItemParser& aItem );
        virtual TInt Maxelement( CStifItemParser& aItem );
        virtual TInt Minelement( CStifItemParser& aItem );
      
        
        
        virtual TInt ToUpper( CStifItemParser& aItem );
        virtual TInt ToLower( CStifItemParser& aItem );
        virtual TInt Islower( CStifItemParser& aItem );
        virtual TInt Isupper( CStifItemParser& aItem );
        virtual TInt Maxof( CStifItemParser& aItem );
         
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
             virtual TInt Advance( CStifItemParser& aItem );
        virtual TInt Backinsert_iterator( CStifItemParser& aItem );
        virtual TInt Frontinsert_iterator( CStifItemParser& aItem );
        virtual TInt Insert_iterator( CStifItemParser& aItem );
        virtual TInt Reverse_iterator( CStifItemParser& aItem );
        virtual TInt Distance( CStifItemParser& aItem );
     
   
        virtual TInt Isdigit( CStifItemParser& aItem );
        virtual TInt Isgraph( CStifItemParser& aItem );
        virtual TInt Isalnum( CStifItemParser& aItem );
        virtual TInt Isalpha( CStifItemParser& aItem );
        virtual TInt Iscntrl( CStifItemParser& aItem );
        virtual TInt Isprint( CStifItemParser& aItem );
        virtual TInt Ispunct( CStifItemParser& aItem );
        virtual TInt Isspace( CStifItemParser& aItem );
        virtual TInt Isxdigit( CStifItemParser& aItem );
        virtual TInt Fill( CStifItemParser& aItem );
        virtual TInt Generate( CStifItemParser& aItem );
        virtual TInt Search( CStifItemParser& aItem );
        virtual TInt Search_n( CStifItemParser& aItem );
        virtual TInt Times( CStifItemParser& aItem );
        virtual TInt Unique( CStifItemParser& aItem );
        virtual TInt Uniquecopy( CStifItemParser& aItem );
        virtual TInt Partialsort( CStifItemParser& aItem );
        virtual TInt Partialsortcopy( CStifItemParser& aItem );
        virtual TInt Partition( CStifItemParser& aItem );
        virtual TInt Stablepartition( CStifItemParser& aItem );
        virtual TInt Setdifference( CStifItemParser& aItem );
                
        

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

#endif      // TSTL_4_H

// End of File
