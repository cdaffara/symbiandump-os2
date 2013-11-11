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
* Description: This file contains the header file of the CUIStore 
* class
*
*/

#ifndef UISTORE_H
#define UISTORE_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <stifinternal/UIEngine.h>
#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStoreContainer.h>

#include <StifLogger.h>

// CONSTANTS
_LIT( KDefaultIni, "\\testframework\\testframework.ini" );

// Timeout for popups
const TInt KDefaultPopupTimeout = 2100; // 35s. Note this is maximun time in
                                        // current implementation(used in
                                        // After()-method, 32bit).
// Timeout for popups
const TInt KRebootPopupTimeout = 1;

// MACROS

// DATA TYPES

// Menu update type

// FUNCTION PROTOTYPES
//
// FORWARD DECLARATIONS
class CUIStoreIf;
class CStartedTestCase;
class CUIEngine;
class CTestInfo;
class CUIEngineContainer;
class CStartedTestSet;
class CUIStorePopup;

// DESCRIPTION
// CUIStore class
class CUIStore
        :public CUIIf
    {
    public: // Enumerations
                
    private: // Enumerations
    
        // Popup priorities
        enum TPopupPriority
            {
            EPopupPriorityHighest,
            EPopupPriorityNormal,
            EPopupPriorityLow
            };
         
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */ 
        static CUIStore* NewL( CUIStoreIf* iUIStoreIf );

        /**
        * Destructor of CUIStore.
        */
        ~CUIStore();


    public: // New functions
        
        /**
        * Used to open TestFramework.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt Open( const TDesC& aTestFrameworkIni = KNullDesC );

        /**
        * Used to close TestFramework.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt Close();

        /**
        * Used to add test module to test framework.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt AddTestModule( const TDesC& aModuleName, 
                                     const TDesC& aIniFile = KNullDesC );

        /**
        * Used to remove test module to test framework.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt RemoveTestModule( const TDesC& aModuleName );

        /**
        * Used to add test case file for test module.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt AddTestCaseFile( const TDesC& aModuleName,
                                       const TDesC& aCaseFile );

        /**
        * Used to remove test case file from test module.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt RemoveTestCaseFile( const TDesC& aModuleName,
                                          const TDesC& aCaseFile );

        /**
        * Start selected test case identified with CTestInfo.
        * anIndex contains index in StartedTestCase array, 
        * which is valid only during execution of calling 
        * function.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt StartTestCase( const CTestInfo& aTestInfo,
                                     TInt& anIndex );
                                        
        /**
        * Return array of existing test cases.
        */
        IMPORT_C TInt TestCases( RRefArray<CTestInfo>& aTestCases,
                                 const TDesC& aTestModule = (TDesC&) KNullDesC(),
                                 const TDesC& aTestCaseFile = (TDesC&) KNullDesC() );

        
        
        /**
        * Return started (running/runned) test case
        */
        IMPORT_C CStartedTestCase& StartedTestCaseL( TInt anIndex );
      
        /**
        * Return started (running/runned) test cases
        */
        IMPORT_C TInt StartedTestCases( RRefArray<CStartedTestCase>& aTestCases,
                               TInt aStatus = CUIStoreIf::EStatusAll,
                               const TDesC& aTestModule = (TDesC&) KNullDesC(),
                               const TDesC& aTestCaseFile = (TDesC&) KNullDesC() );        
        
        /**
        * Return modules array
        */
        IMPORT_C TInt Modules( RRefArray<TDesC>& aTestModules );

        /**
        * Return test case files
        */
        IMPORT_C TInt TestCaseFiles( RRefArray<TDesC>& aTestCaseFiles,
                            const TDesC& aTestModule = (TDesC&) KNullDesC() );


        /**
        * Loads all TestFramework test modules from \sys\bin\ directories 
        * of all drives.
        */
        IMPORT_C TInt LoadAllModules();
        
        /**
        * Lists all TestFramework test modules from \sys\bin\ directories 
        * of all drives.
        */
        IMPORT_C TInt ListAllModules( RPointerArray<TDesC>& aModuleNames );


        /**
        * Test set support.
        */
                                     
        /**
        * Create new test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt CreateTestSet( const TDesC& aSetName );

        /**
        * Remove active test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt RemoveTestSet( const TDesC& aSetName );
        
        /**
        * Query test sets.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt TestSets( RRefArray<CTestSetInfo>& aSetInfos );        

        /**
        * Query test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C const CTestSetInfo& TestSetL( const TDesC& aSetName  );                
        
        /**
        * Add test case to test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt AddToTestSet( const TDesC& aSetName, const CTestInfo& aTestInfo );
 
        /**
        * Insert test case to test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt InsertToTestSet( const TDesC& aSetName, 
                                       const CTestInfo& aTestInfo, 
                                       TInt aPos );
 
        /**
        * Remove test case from test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt RemoveFromTestSet( const TDesC& aSetName, const CTestInfo& aTestInfo );
        
        /**
        * Save test set. Depreceated, SaveTestSet2 to be used instead
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt SaveTestSet( const TDesC& aSetName );

        /**
        * Save test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt CUIStore::SaveTestSet2( TDes& aSetName );
        /**
        * Load test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt LoadTestSet( const TDesC& aSetName );

        /**
        * Unload active test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt UnloadTestSet( const TDesC& aSetName );
        
        
        /**
        * Load saved test cases.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt LoadSavedTestCases();
                
        /**
        * Start selected test set identified with CTestSetInfo.
        * anIndex contains index in StartedTestSet array, 
        * which is valid only during execution of calling 
        * function.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt StartTestSet( const CTestSetInfo& aTestSetInfo,
                                    TInt& anIndex,
                                    CStartedTestSet::TSetType aType = 
                                        CStartedTestSet::ESetSequential );
        IMPORT_C TInt StartTestSet( const CTestSetInfo& aTestSetInfo,
                                    TInt& anIndex,
                                    CStartedTestSet::TSetType aType,
                                    TBool aNotStartedCasesMode);

        /**
        * Abort running test set.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt AbortTestSet( CStartedTestSet& aSetInfo );

        /**
        * Return started (running/runned) test set
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C CStartedTestSet& StartedTestSetL( TInt anIndex );
      
        /**
        * Return started (running/runned) test cases
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt StartedTestSets( RRefArray<CStartedTestSet>& aTestCases,
                                       TInt aStatus = CUIStoreIf::ESetAll );       

        /**
        * Updates information stored about test set in file.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt UpdateTestSet(CTestSetInfo& aSetInfo);
        
        /**
        * Reads filters (if any) from initialization file.
        */
        IMPORT_C void ReadFiltersL(RPointerArray<TDesC>& aFilters);
        
        /**
        * Creates list of available Test Sets.
        *
        * Returns Symbian OS error code.
        */
        
        IMPORT_C TInt GetTestSetsList( RRefArray<TDesC>& aArray );
        

    public: // Functions from base classes


    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CUIStore( CUIStoreIf* iUIStoreIf );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
        /**
        * Test case executed.
        *
        * Returns Symbian OS error code.
        */
        void TestExecuted ( CUIEngineContainer* aContainer,
                            TFullTestResult& aFullTestResult );

        /**
        * Progress information from Test case execution.
        *
        * Returns Symbian OS error code.
        */
        TInt PrintProg ( CUIEngineContainer* aContainer,
                         TTestProgress& aProgress );

        /**
        * Error information from Test framework.
        *
        * Returns Symbian OS error code.
        */
        TInt ErrorPrint( TErrorNotification& aError );
                         
        /**
        * Remote protocol messages handler.
        */
        TInt RemoteMsg( CUIEngineContainer* aContainer,
                        const TDesC& aMessage );
        /**
        * Reboot indication handler.
        */
        TInt GoingToReboot( CUIEngineContainer* aContainer,
                            TRequestStatus& aStatus );
        
        TInt MsgRemote( CUIEngineContainer* aContainer,
                        CStifTFwIfProt& aReq, 
                        CStifTFwIfProt& aResp );
        TInt MsgRemoteRun( CUIEngineContainer* aContainer,
                           CStifTFwIfProt& aReq, 
                           CStifTFwIfProt& aResp );
        TInt MsgRemoteTestCtl( CUIEngineContainer* aContainer,
                               CStifTFwIfProt& aReq, 
                               CStifTFwIfProt& aResp );
        TInt MsgRemoteEventCtl( CUIEngineContainer* aContainer,
                                CStifTFwIfProt& aReq, 
                                CStifTFwIfProt& aResp );

        /**
        * Asynchronous remote sendreceive message.
        */
        TInt MsgRemoteSendReceive( CUIEngineContainer* aContainer,
                           CStifTFwIfProt& aReq, 
                           CStifTFwIfProt& aResp );
        
        /**
        * Initialize logger from UI engine.
        */
        inline void InitializeLogger( CStifLogger* aLogger )
            { iLogger = aLogger; };
        
        /**
        * Set update flag.
        */
        inline void RefreshAllCases()
            { iUpdateNeeded = ETrue; };
            
        /**
        * Update test case list if required.
        */
        TInt UpdateCases();
        
        /**
        * Try to locate default initialization file.
        */
        void CheckIniL( TFileName& aIni );
        
        /**
        * Find testcase by container.
        */
        TInt FindByContainer( CUIEngineContainer* const aContainer,
                              CStartedTestCase*& aTestCase );

        /**
        * Loads all TestFramework test modules from \sys\bin\ directories .
        */
        void LoadAllModulesL();
        
        /**
        * Lists all TestFramework test modules from \sys\bin\ directories.
        */
        void ListAllModulesL( RPointerArray<TDesC>& aModuleNames );
        
        /**
        * Find test set by name.
        */
        TInt FindSetByName( const TDesC& aSetName, CTestSetInfo*& aSetInfo );
        
        /**
        * Find test set by CStartedTestCase.
        */
        TInt FindStartedSetByCase( const CStartedTestCase* aTestCase, 
                                   CStartedTestSet*& aSet  );
   
        /**
        * Load test set.
        */
        void LoadTestSetL( const TDesC& aSetName, const TDesC& aSetFileName );
        
        /**
        * Save test set.
        */ 
        void SaveTestSetL( CTestSetInfo& aSetInfo, const TDesC& aSetFileName );

        /**
        * Updates in file information about test set.
        */ 
        void UpdateTestSetL(CTestSetInfo& aSetInfo, const TDesC& aSetFileName);
        
        /**
        * Read the whole line from the file.
        */ 
        void ReadLineL(RFile &file, TDes& buffer);

        /**
        * Write given line to the file and adds end of line.
        */ 
        void WriteLineL(RFile &file, const TDesC& buffer);

        /**
        * Load saved executed test cases.
        *
        * Returns Symbian OS error code.
        */
        void LoadExecutedTestCasesL();
        
        /**
        * Load test modules and test case files used in 
        * aTestCases if not already done.
        */
        void LoadTestModulesAndTestCaseFilesL( const RRefArray<const CTestInfo>& aTestCases );
        
        /**
        * Parse test set name from test set file name.
        */
        TInt ParseTestSetName( const TDesC& aSetFileName, TPtrC& aSetName, TFileName& aFileName );
        
        /**
        * Do remote popup. Added aPopupPriority parameter for prioritizing popups @js      
        */
        TInt RemotePopup( CUIEngineContainer* aContainer,
                          const TDesC& aReq, 
                          CStifTFwIfProt* aResp,
                          TPopupPriority aPopupPriority = EPopupPriorityLow);        
        
        /**
        * Callback from UIStorePopup.
        */
        TInt RemotePopupComplete( CUIStorePopup* aPopup, 
                                  TInt aError,
                                  CUIEngineContainer* aContainer,
                                  CStifTFwIfProt* aResp,
                                  TKeyCode aKeyCode );
        /**
        * Return slave id.
        */
        inline TInt16 DevId(){ return 1; };
        
        /**
        * Prints the highest priority popup from queue
        */
        void SetRemotePopupFromQueue();
        
        /**
        * Shuffles remote run result popups
        */
        void ShuffleRemoteRunResultPopups( CUIStorePopup* aPopup );
        
    public:   //Data
        // Logger
        CStifLogger*                        iLogger;

    protected:  // Data
        // None

    private:    // Data
    	RPointerArray<HBufC> 				iFileList;
        // Pointer to UI (up)
        CUIStoreIf*                         iUIStoreIf;   
        
        // Pointer to UI engine (down)
        CUIEngine*                          iUIEngine;

    	// Available test cases   
        RPointerArray<CTestInfo>            iTestCases;    
        // Started test cases    
        RPointerArray<CStartedTestCase>     iStartedTestCases; 
 
    	// Available test sets   
        RPointerArray<CTestSetInfo>         iTestSets;    
        // Started test sets    
        RPointerArray<CStartedTestSet>      iStartedTestSets; 
        
        RPointerArray<CUIStorePopup>        iPopups;
               
        // Flag for update for test case list required
        TBool                               iUpdateNeeded;
        
        // Tells if popup is already active
        TBool                               iPopupActive;
        
        // Priority of the currently active popup
        TPopupPriority                      iActivePopupPriority;        
        
        TFileName                           iTestFrameworkIni;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        friend class CUIStorePopup;

    };
    
#endif // UISTORE_H

// End of File
