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
* Description:       Header file for XmlSecSignTester 
*
*/
#ifndef __XMLSECSIGNTESTER_H__
#define __XMLSECSIGNTESTER_H__

//  EXTERNAL INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

//  INTERNAL INCLUDES
// Logging path
_LIT( KXmlSecTesterLogPath, "e:\\temp\\xmlsec" ); 

// Log file
_LIT( KXmlSecTesterLogFile, "XmlSecSignTester.txt" ); 

// Test name
_LIT( KTEST_NAME, "XmlSecSignTester" );


_LIT8(KImplementationID, "");

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CXmlSecSignTester;

	
//  CLASS DEFINITION
/**
*  CXmlSecSign test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CXmlSecSignTester) : public CScriptBase
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static CXmlSecSignTester* NewL(CTestModuleIf &);
        /**
         * Destructor
         */
        virtual ~CXmlSecSignTester();
        

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
		
	private:    // Constructors and destructors

       /**
        * C++ default constructor.
        */
        CXmlSecSignTester( CTestModuleIf& aTestModuleIf );
        
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
	public: 	// From observer interface

	HBufC8* ReadFileL(const TDesC& aFileName);	

    private:    // New methods

        void CXmlSecSign_NewLL();
        void CXmlSecSign_NewLCL();
        TInt CXmlSecSign_SignXmlDocumentLL(CStifItemParser& aItem);
        TInt CXmlSecSign_SignXmlDocumentKeyFromFileL_1L(CStifItemParser& aItem);
        TInt CXmlSecSign_SignXmlDocumentKeyFromFileL_2L(CStifItemParser& aItem);
        TInt CXmlSecSign_SignXmlDocumentKeyFromBufferL_1L(CStifItemParser& aItem);
        TInt CXmlSecSign_SignXmlDocumentKeyFromBufferL_2L(CStifItemParser& aItem);
        TInt CXmlSecSign_SignXmlNodesLL(CStifItemParser& aItem);
        TInt CXmlSecSign_VerifyXmlNodeLL(CStifItemParser& aItem);
        TInt CXmlSecSign_VerifyXmlNodeKeyFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecSign_VerifyXmlNodeKeyFromBufferLL(CStifItemParser& aItem);
        TInt CXmlSecSign_VerifyXmlDocumentLL(CStifItemParser& aItem);
        TInt CXmlSecSign_VerifyXmlDocumentKeyFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecSign_VerifyXmlDocumentKeyFromBufferLL(CStifItemParser& aItem);
        TInt CXmlSecSign_SetTemplateFromFileL_1L(CStifItemParser& aItem);
        TInt CXmlSecSign_SetTemplateFromFileL_2L(CStifItemParser& aItem);
        TInt CXmlSecSign_SetTemplateFromBufferLL();
        TInt CXmlSecSign_SetTemplateLL();
        TInt CXmlSecSign_CreateTemplateL_1L();
        TInt CXmlSecSign_CreateTemplateL_2L();
        TInt CXmlSecSign_GetCurrentTemplateLL();
        TInt CXmlSecSign_SetKeyInfoL_1L(  );
        TInt CXmlSecSign_SetKeyInfoL_2L(  );
        TInt CXmlSecSign_SetKeyFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecSign_SetKeyFromBufferLL(CStifItemParser& aItem);
        TInt CXmlSecSign_SetCertFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecSign_SetCertFromBufferLL(CStifItemParser& aItem);
        TInt CXmlSecSign_AddTrustedCertFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecSign_AddTrustedCertFromBufferLL(CStifItemParser& aItem);
        TInt CXmlSecSign_SignXmlDocumentL_2L(CStifItemParser& aItem);
        TInt CXmlSecSign_SignXmlDocumentL_3L();
        TInt CXmlSecSign_VerifyXmlNodeL_2L(CStifItemParser& aItem);
        
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
		
    };

#endif      //  __XMLSECSIGNTESTER_H__

// End of file
