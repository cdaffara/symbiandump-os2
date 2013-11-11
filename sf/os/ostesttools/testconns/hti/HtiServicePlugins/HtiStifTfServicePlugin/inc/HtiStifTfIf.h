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
* Description:  Defines the class that controls STIF Test Framework through
*                the STIF TF interface.
*
*/



#ifndef HTISTIFTFIF_H
#define HTISTIFTFIF_H

//  INCLUDES
#include <StifTFwIf.h>
#include <HtiServicePluginInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDesC8ArrayFlat;
class CHtiStifTfServicePlugin;
class CConsoleBase;

// CLASS DECLARATION
/**
*  Container class for test modules' test infos.
*  One instance of this class contains the test infos of one test module in
*  an RPointerArray and the name of the test module as HBufC.
*
*/
class CHtiTestModuleCaseInfos : public CBase
    {
    public: // Constuctors and destructor

        /**
        * Two-phased constructor.
        */
        static CHtiTestModuleCaseInfos* NewL( const TDesC& aModuleName );


        /**
        * Two-phased constructor.
        */
        static CHtiTestModuleCaseInfos* NewLC( const TDesC& aModuleName );


        /**
        * Destructor.
        */
        ~CHtiTestModuleCaseInfos();

    public:
        /**
        * Compares the order of two CHtiTestMoudleCaseInfos objects.
        * Order is based on the alphabetical order of the module name.
        * @param aFirst reference to the first instance to compare
        * @param aSecond reference to the second instance to compare
        * @return Positive if the first module's name is greater than the
        *         second module's name. Negative if the first module's name is
        *         less than the second module's name. Zero if names are equal.
        */
        static TInt Compare( const CHtiTestModuleCaseInfos& aFirst,
                             const CHtiTestModuleCaseInfos& aSecond );

        /**
        * Compares the equality of two CHtiTestMoudleCaseInfos objects.
        * Equality is based on the name of the module.
        * @param aFirst reference to the first instance to compare
        * @param aSecond reference to the second instance to compare
        * @return ETrue if the module names are equal. EFalse otherwise.
        */
        static TBool Match(  const CHtiTestModuleCaseInfos& aFirst,
                             const CHtiTestModuleCaseInfos& aSecond );

    private:

        CHtiTestModuleCaseInfos();

        void ConstructL( const TDesC& aModuleName );

    public: // Data

        // The name of the test module whose test infos this instance contains
        HBufC* iModuleName;

        // An array of CTestInfo objects (one per test case)
        RPointerArray<CTestInfo> iTestInfos;

    };


// CLASS DECLARATION

/**
*  Implements the STIF Test Framework interface functions.
*/
class CHtiStifTfIf : public CStifTFwIf
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CHtiStifTfIf* NewL( CHtiStifTfServicePlugin* aPlugin );

        /**
        * Destructor.
        */
        virtual ~CHtiStifTfIf();

    public: // New functions

        /**
        * Called when there is a message to be processed by this service.
        * @param aMessage message body destinated to the servive
        * @param aPriority message priority
        */
        void ProcessMessageL( const TDesC8& aMessage,
            THtiMessagePriority aPriority );

        /**
        * Called by the plugin to tell the service how much memory is
        * available for messages in the message queue.
        * @param aAvailableMemory amount of currently available memory
        *        in the message queue
        */
        void NotifyMemoryChange( TInt aAvailableMemory );

        /**
        * Sets the dispatcher to send outgoing messages to.
        * @param aDispatcher pointer to dispatcher instance
        */
        void SetDispatcher( MHtiDispatcher* aDispatcher );

    public: // Functions from base classes

    protected:  // New functions


    protected:  // Functions from base classes

        // virtual functions from CStifTFwIf
        /**
        * Function that is called when test execution is finished.
        * @param aTestId id of the test case
        * @param aTestExecutionResult result of test execution
        * @param aTestResult actual result of the test case
        */
        void TestCompleted( TInt aTestId,
                            TInt aTestExecutionResult,
                            TTestResult& aTestResult );

        /**
        * Function for sending message to ATS.
        * @param aMessage message to be sent
        */
        void AtsSend( const TDesC& aMessage );

        /**
        * Get device identifier.
        * Should be overridden to make them unique among slaves.
        * Returns default value for every slave otherwise.
        * @return the device ID
        */
        TUint16 DevId(){ return iDevId; };

        /**
        * Function is called when test case prints.
        */
        void Print( TInt aTestId, TTestProgress& aProgress );

    private:

        /**
        * C++ default constructor.
        */
        CHtiStifTfIf();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CHtiStifTfServicePlugin* aPlugin );

        // Private helper methods.
        void HandleOpenCmdL();

        void HandleCloseCmdL();

        void HandleLoadModuleCmdL();

        void HandleUnloadModuleCmdL();

        void HandleListCasesCmdL();

        void HandleStartCaseCmdL();

        void HandleCancelCaseCmdL();

        void HandlePauseCaseCmdL();

        void HandleResumeCaseCmdL();

        void HandleAddCaseFileCmdL();

        void HandleRemoveCaseFileCmdL();

        void HandleCaseMsgCmdL();

        void HandleSetDevIdCmdL();

        void HandleSetAttributeCmdL();

        void LoadTestCaseInfosL();

        void LoadTestCaseInfosL( TDesC& aModuleName );

        void ParseParametersL( const TDesC8& aCommand );

        TInt SendResponseMsg( const TDesC8& aMsg,
                              const TUint8 aCommandId = 0 );

        TInt SendErrorResponseMsg( TInt aErrorCode,
                                   const TDesC8& aErrorDescription,
                                   const TUint8 aCommandId = 0 );

        TInt IndexByModuleName( const TDesC& aModuleName );

        CTestInfo* GetCaseInfoL( TInt aCaseIndex );


    public:     // Data

        // Flag telling if the service is busy processing a message
        TBool iIsBusy;

    protected:  // Data

    private:    // Data

        // ID of the last received command
        TUint8 iCommandId;

        // Array container for parameters of the last received message
        CDesC8ArrayFlat* iParameters;

        // The message dispatcher interface
        MHtiDispatcher* iDispatcher;

        // Container for the response message to be dispatched out
        HBufC8* iMessage;

        // Error code in the outgoing error message
        TInt iErrorCode;

        // Flag telling if STIF TF has been opened
        TBool iStifTfOpen;

        // The device ID
        TUint16 iDevId;

        // An array holding the test case infos of all loaded test modules
        RPointerArray<CHtiTestModuleCaseInfos> iLoadedInfos;

        // Pointer to the service plugin instance - this is not owned
        CHtiStifTfServicePlugin* iPlugin;

        // Pointer to the HTI console
        CConsoleBase* iConsole;
    };

#endif      // HTISTIFTFIF_H

// End of File
