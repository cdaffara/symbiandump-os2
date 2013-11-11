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
* Description: This file contains the header file of the 
* CScriptBase.
*
*/

#ifndef SCRIPTRUNNERINTERNAL_H
#define SCRIPTRUNNERINTERNAL_H

// INCLUDES
#include <StifTestModule.h>
#include <StifLogger.h>

// CONSTANTS
// None

// MACROS
#define ENTRY(A,B) \
	{ (TText*) L ##A, (StifTestFunction) &B }

// DATA TYPES

// For function pointer operations. If need a new method that calls
// from testscriptclass the testscripter. Add method's enum definition
// here and implement the new method also TestScripterInternal.h need
// new implementations.
enum TStifTSCallBackType
    {
    EStifTSCallClass,
    EStifTSGetObject,
    };

// FORWARD DECLARATIONS
class CStifItemParser;
class CTestScripter;
class CScriptBase;

// DATA TYPES
typedef TInt (CScriptBase::* StifTestFunction)( CStifItemParser& );    
class TStifFunctionInfo
    {
    public:
        const TText* iFunctionName;
        StifTestFunction iMethod;
    };

// FUNCTION PROTOTYPES
// (Function pointer) Used to forward operations from test script class
// to TestScripter.
typedef TInt (*CallBackFunc)( CTestScripter* aTestScripter,
                              TStifTSCallBackType aType,
                              const TDesC& aLine );
// CLASS DECLARATION
// None

// DESCRIPTION
// This a Test Module interface template 
// that does not really do anything.

class CScriptBase 
        :public CBase
    {
    public: // Enumerations
       
    private: // Enumerations
       
    public: // Constructors and destructor          
    
    public: // New functions
       
        /**
        * Get reference to TestModuleIf API.
        */ 
        IMPORT_C CTestModuleIf& TestModuleIf();
        
        /**
        * Signal TestScripter to continue from waittestclass.
        */
        IMPORT_C void Signal( TInt aError = KErrNone );
        
        // Called from test class. TestScripter forwards
        // operations to other test class object.
        // aLine: in: <object name> <method name> <parameters>
        IMPORT_C TInt CallTestClass( const TDesC& aLine );
        
        // Get TestClass object pointer. Use CallTestClass instead if 
        // you can, use this only with your own risk!!!
        // aObjectName: in: <object name>
        IMPORT_C CScriptBase* GetTestObject( const TDesC& aObjectName );
        
        // Result description can be set from within the test class method
        IMPORT_C void SetResultDescription(const TDesC& aDescription);
        
    public: 
        
        /**
        * Runs a script line, must be implemented in derived class.
        */
        virtual TInt RunMethodL( CStifItemParser& aItem ) = 0;  

        /**
        * Destructor of CScriptBase.
        */
        virtual ~CScriptBase(){ iSignalErrors.Reset(); iSignalErrors.Close();  };
                
    protected: // New functions

        /**
        * Constructor.
        */
        IMPORT_C CScriptBase( CTestModuleIf& aTestModuleIf );

        /**
        * Internal fuction to run specified method.
        */
        IMPORT_C virtual TInt RunInternalL( 
            TStifFunctionInfo const aFunctions[], 
            TInt aCount, 
            CStifItemParser& aItem );

        /**
         * Internal fuction to get const value defined in 
         * [Define]...[Enddefine] section of script file
         */        
        IMPORT_C TInt GetConstantValue( const TDesC& aName, TDes& aValue );


        /**
         * Internal fuction to get const value defined in 
         * [Define]...[Enddefine] section of script file
         */        
        IMPORT_C TInt GetConstantValue( const TDesC& aName, TInt& aValue );


        /**
         * Internal fuction to get const value defined in 
         * [Define]...[Enddefine] section of script file
         */        
        IMPORT_C TInt GetConstantValue( const TDesC& aName, TReal& aValue );
        
        /**
         * Internal fuction to set value of local variable 
         */        
        IMPORT_C TInt SetLocalValue(const TDesC& aName, const TDesC& aValue);

        /**
         * Internal fuction to set value of local variable 
         */        
        IMPORT_C TInt SetLocalValue(const TDesC& aName, const TInt aValue);

        /**
         * Internal fuction to set value of local variable 
         */        
        IMPORT_C TInt SetLocalValue(const TDesC& aName, const TReal aValue);

        /**
         * Internal fuction to get value of local variable 
         */        
        IMPORT_C TInt GetLocalValue(const TDesC& aName, TDes& aValue);

        /**
         * Internal fuction to get value of local variable 
         */        
        IMPORT_C TInt GetLocalValue(const TDesC& aName, TInt& aValue);

        /**
         * Internal fuction to get value of local variable 
         */        
        IMPORT_C TInt GetLocalValue(const TDesC& aName, TReal& aValue);
        
    protected: // Functions from base classes
        
    private:
    
        IMPORT_C void EnableSignal( TRequestStatus& aStatus );           
        
        IMPORT_C void CancelSignal();
        
        // This set from TestScripter(Initializes function pointer to use)
        IMPORT_C void SetScripter( CallBackFunc aFunc, CTestScripter* aTestScripter );

    public: // Data
        
    protected: // Data
    
        /**
        * Logger.
        */
        CStifLogger*    iLog;

        /**
        * Array for handling multiple 'waittestclass' given from testclass's
        * test configure file.
        */
        RArray<TInt> iSignalErrors;

    private: // Data
        // Backpointer
        CTestModuleIf& iTestModuleIf;
        
        // Completed when test class signals continue script execution
        TRequestStatus* iStatus; 

        // Backpointer to TestScripter
        CallBackFunc iFunc;

        // Backpointer
        CTestScripter* iTestScripter;

    public: // Friend classes
        
    protected: // Friend classes
        
    private: // Friend classes
        friend class CTestScripter;
        friend class CTestContinue;
        
    };

#endif      // SCRIPTRUNNER_H

// End of File
