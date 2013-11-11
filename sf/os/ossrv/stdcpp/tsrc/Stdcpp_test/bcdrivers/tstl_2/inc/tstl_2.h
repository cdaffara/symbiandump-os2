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




#ifndef TSTL_2_H
#define TSTL_2_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( Ktstl_2LogPath, "\\logs\\testframework\\tstl_2\\" ); 
// Log file
_LIT( Ktstl_2LogFile, "tstl_2.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Ctstl_2;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Ctstl_2 test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ctstl_2) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ctstl_2* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ctstl_2();

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
        Ctstl_2( CTestModuleIf& aTestModuleIf );

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

#endif      // TSTL_2_H

// End of File
