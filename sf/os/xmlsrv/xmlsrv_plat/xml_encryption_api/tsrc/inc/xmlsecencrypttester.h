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
* Description:       Header file for XmlSecEncryptTester 
*
*/
#ifndef __XMLSECENCRYPTTESTER_H__
#define __XMLSECENCRYPTTESTER_H__

//  EXTERNAL INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

//  INTERNAL INCLUDES
// Logging path
_LIT( KXmlSecTesterLogPath, "e:\\temp\\xmlsec" ); 

// Log file
_LIT( KXmlSecTesterLogFile, "XmlSecEncryptTester.txt" ); 

// Test name
_LIT( KTEST_NAME, "XmlSecEncryptTester" );


_LIT8(KImplementationID, "");

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CXmlSecEncryptTester;

	
//  CLASS DEFINITION
/**
*  CXmlSecEncrypt test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CXmlSecEncryptTester) : public CScriptBase
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static CXmlSecEncryptTester* NewL(CTestModuleIf &);
        /**
         * Destructor
         */
        virtual ~CXmlSecEncryptTester();
        

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
        CXmlSecEncryptTester( CTestModuleIf& aTestModuleIf );
        
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

        void CXmlSecEncrypt_NewLL();
        void CXmlSecEncrypt_NewLCL();
        TInt CXmlSecEncrypt_EncryptDataLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_EncryptXmlDocumentLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_EncryptXmlDocumentKeyFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_EncryptXmlDocumentKeyFromBufferLL();
        TInt CXmlSecEncrypt_EncryptXmlNodeLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_EncryptXmlNodeKeyFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_EncryptXmlNodeKeyFromBufferLL();
        TInt CXmlSecEncrypt_DecryptXmlNodeLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_DecryptXmlNodeKeyFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_DecryptXmlNodeKeyFromBufferLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_DecryptXmlDocumentLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_DecryptXmlDocumentKeyFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_DecryptXmlDocumentKeyFromBufferLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_SetTemplateFromFileL_1L(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_SetTemplateFromFileL_2L(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_SetTemplateFromBufferLL();
        TInt CXmlSecEncrypt_SetTemplateLL();
        TInt CXmlSecEncrypt_CreateTemplateLL();
        TInt CXmlSecEncrypt_GetCurrentTemplateLL();
        TInt CXmlSecEncrypt_SetKeyFromFileLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_SetKeyFromBufferLL(CStifItemParser& aItem);
        TInt CXmlSecEncrypt_SetKeyInfoL_1L();
        TInt CXmlSecEncrypt_SetKeyInfoL_2L();
        
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
		
    };

#endif      //  __XMLSECSIGNTESTER_H__

// End of file
