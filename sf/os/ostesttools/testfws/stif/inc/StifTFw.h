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
* Description: This file contains the header file of the CStifTFw.
*
*/

#ifndef STIF_TFW_H
#define STIF_TFW_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifLogger.h>
#include <StifTestInterface.h>
#include <stifinternal/UIEngine.h>

// CONSTANTS
// None

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CUIEngineContainer;
class CUIEngineEvent;
class CStifTFwIf;

// CLASS DECLARATION

// CStifTFw is the sub class of STIF Test Framework Interface
class CStifTFw
    :public CUIIf
    {
    public: // Enumerations

    protected: // Enumerations
        
    private: // Enumerations

    public:  // Constructors and destructor
         /**
        * NewL is first phase of two-phased constructor.
        */
        static CStifTFw* NewL( CStifTFwIf* aStifTFwIf );

        /**
        * Destructor.
        */
        virtual ~CStifTFw();
        
    public: 

    protected: // Functions that can be called from derived class

    protected: // Functions that the derived class may implement.
 
    private:

        /**
        * Constructor.
        */
        CStifTFw( CStifTFwIf* aStifTFwIf );

        /**
        * Second phase constructor.
        */
        void ConstructL();

        /**
        * Test case executed.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C void TestExecuted( CUIEngineContainer* aContainer, 
                                    TFullTestResult& aFullTestResult );
        /**
        * Progress information from Test case execution.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt PrintProg ( CUIEngineContainer* aContainer,
                                  TTestProgress& aProgress );

        /**
        * Forward message to ATS.
        */                            
        IMPORT_C TInt RemoteMsg( CUIEngineContainer* aContainer,
                                 const TDesC& /*aMessage*/ );
                                 
        /**
        * Forward reboot indication to ATS.
        */                            
        IMPORT_C TInt GoingToReboot( CUIEngineContainer* aContainer,
                                     TRequestStatus& aStatus );
 
        /**
        * Initialize logger from UI engine.
        */
        void InitializeLogger( CStifLogger* aLogger ){ iLogger = aLogger; }

        /**
        * Remote ATS call
        */
        TInt AtsRemote( CStifTFwIfProt& aReq, CStifTFwIfProt& aResp );

        /**
        * Remote ATS run call
        */
        TInt AtsRemoteRun( CStifTFwIfProt& aReq, CStifTFwIfProt& aResp );

        /**
        * Remote ATS test control call
        */
        TInt AtsRemoteTestCtl( CStifTFwIfProt& aReq, CStifTFwIfProt& aResp );

        /**
        * Remote ATS event system control call
        */
        TInt AtsRemoteEventCtl( CStifTFwIfProt& aReq, CStifTFwIfProt& aResp );
        
        /**
        * Delete data in class
        */
        void DeleteData();

        /**
        * Enumerates test cases from given module and finds test case index by title
        */
        TInt GetCaseNumByTitle(TDesC& aModule, TDesC& aTestCaseFile, TDesC& aTitle, TInt& aCaseNum);

    public: // Data
        /**
        * Logger created by UI engine.
        */ 
        CStifLogger* iLogger;
        
    protected: // Data
        
    private: // Data
        /**
        * Pointer to UI engine.
        */ 
        CUIEngine* iUIEngine;
        
        /**
        * Pointer to CStifTfwIf
        */ 
        CStifTFwIf* iStifTFwIf;

        // Execute test case table
        RPointerArray<CUIEngineContainer>      iTestExecutionTable;

        // Remote execute test case table
        RPointerArray<CUIEngineContainer>      iTestRemoteExecutionTable;

        // Remote execute test case table
        RPointerArray<CUIEngineEvent>          iEventArray;
        
        // Array of master identifiers that have reserved the slave
        RArray<TUint32>                        iMasterArray;

    public: // Friend classes
        // None
        
    protected: // Friend classes
        // None

    private: // Friend classes
        friend class CStifTFwIf;

    };

#endif      // STIF_TFW_H 

// End of File
