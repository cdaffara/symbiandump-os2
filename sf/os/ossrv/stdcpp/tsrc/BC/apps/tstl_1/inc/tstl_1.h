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
#define TEST_MODULE_VERSION_MAJOR 50
#define TEST_MODULE_VERSION_MINOR 9
#define TEST_MODULE_VERSION_BUILD 06
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







         virtual TInt Advance( CStifItemParser& aItem );
				 virtual TInt Backinsert_iterator( CStifItemParser& aItem );
				 virtual TInt Frontinsert_iterator( CStifItemParser& aItem );
				 virtual TInt Insert_iterator( CStifItemParser& aItem );
				 virtual TInt Reverse_iterator( CStifItemParser& aItem );
				 virtual TInt Distance( CStifItemParser& aItem );
				//  virtual TInt Distancetype( CStifItemParser& aItem );
				 virtual TInt Bit_set( CStifItemParser& aItem );
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
				 virtual TInt complexL(CStifItemParser& aItem);
				 virtual TInt limitsL(CStifItemParser& aItem);
				 virtual TInt nextperm(CStifItemParser& aItem);
				 virtual TInt prevperm(CStifItemParser& aItem);
				 virtual TInt ptrfun(CStifItemParser& aItem);
				 virtual TInt itertraits(CStifItemParser& aItem);
				 virtual TInt map1(CStifItemParser& aItem);
				 virtual TInt map2(CStifItemParser& aItem);
				 virtual TInt map3(CStifItemParser& aItem);
				 virtual TInt map4(CStifItemParser& aItem);
				 virtual TInt map5(CStifItemParser& aItem);
				 virtual TInt map6(CStifItemParser& aItem);
				 virtual TInt map7(CStifItemParser& aItem);
				 virtual TInt map8(CStifItemParser& aItem);
				 virtual TInt map9(CStifItemParser& aItem);
				 virtual TInt map10(CStifItemParser& aItem);
				 virtual TInt map11(CStifItemParser& aItem);
				 virtual TInt map12(CStifItemParser& aItem);
				 virtual TInt map13(CStifItemParser& aItem);
				 virtual TInt multimap1(CStifItemParser& aItem);
				 virtual TInt multimap2(CStifItemParser& aItem);
				 virtual TInt multimap3(CStifItemParser& aItem);
				 virtual TInt multimap4(CStifItemParser& aItem);
				 virtual TInt multimap5(CStifItemParser& aItem);
				 virtual TInt multimap6(CStifItemParser& aItem);
				 virtual TInt multimap7(CStifItemParser& aItem);
				 virtual TInt multimap8(CStifItemParser& aItem);
				 virtual TInt multimap9(CStifItemParser& aItem);
				 virtual TInt multimap10(CStifItemParser& aItem);
				 virtual TInt multimap11(CStifItemParser& aItem);
				 virtual TInt multimap12(CStifItemParser& aItem);
				 virtual TInt multimap13(CStifItemParser& aItem);
				 virtual TInt multi_set(CStifItemParser& aItem);
				 virtual TInt multi_set2(CStifItemParser& aItem);
				 virtual TInt multi_set3(CStifItemParser& aItem);




        	virtual TInt Includes( CStifItemParser& aItem );
          virtual TInt innerproduct( CStifItemParser& aItem );
	        virtual TInt inplacemerge( CStifItemParser& aItem );
	        virtual TInt Queue( CStifItemParser& aItem );
	        virtual TInt Pqueue( CStifItemParser& aItem );
	        virtual TInt stablesortL( CStifItemParser& aItem );
	        virtual TInt Stack( CStifItemParser& aItem );
	        virtual TInt string1_testL( CStifItemParser& aItem );
#if 0
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
	         
#endif        

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
