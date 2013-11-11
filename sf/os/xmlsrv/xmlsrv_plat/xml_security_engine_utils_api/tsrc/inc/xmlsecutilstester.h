/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:       Header file for XmlSecUtilsTester 
*
*/







#ifndef __XMLSECUTILSTESTER_H__
#define __XMLSECUTILSTESTER_H__

//  EXTERNAL INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

//  INTERNAL INCLUDES
// Logging path
_LIT( KXmlSecTesterLogPath, "e:\\temp\\xmlsec" ); 

// Log file
_LIT( KXmlSecTesterLogFile, "XmlSecUtilsTester.txt" ); 

// Test name
_LIT( KTEST_NAME, "XmlSecUtilsTester" );


_LIT8(KImplementationID, "");

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CXmlSecUtilsTester;

	
//  CLASS DEFINITION
/**
*  CXmlSecTester test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CXmlSecUtilsTester) : public CScriptBase
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static CXmlSecUtilsTester* NewL(CTestModuleIf &);
        /**
         * Destructor
         */
        virtual ~CXmlSecUtilsTester();

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
        //type ?member_function( ?type ?arg1 );
             
      

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();
        
    private:

        /**
        * C++ default constructor.
        */
        CXmlSecUtilsTester( CTestModuleIf& aTestModuleIf );

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
      

        
        void XmlSecInitializeL_1L();
        void XmlSecInitializeL_2L();
        void XmlSecInitializeL_3L();
        void XmlSecPushL_1L();
        void XmlSecPushL_2L();
        
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      //  __XMLSECUTILSTESTER_H__

// End of file
