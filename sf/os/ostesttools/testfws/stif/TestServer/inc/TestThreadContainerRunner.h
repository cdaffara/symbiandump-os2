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
* TestThreadContainerRunner class.
*
*/

#ifndef TESTTHREADCONTAINERRUNNER_H_
#define TESTTHREADCONTAINERRUNNER_H_

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestThreadContainer;
class CTestModuleContainer;

// CLASS DECLARATION

// DESCRIPTION
// This class is used to emulate CTestThreadContainer::ExecutionThread method
// as an active object run in main thread of uitestserverstarter.
class CTestThreadContainerRunner: public CActive
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
		// Current operation type
		enum TOperationType { ENone,			// None 
							  ESetup, 			// Setup
							  ETearDown,		// TearDown
							  ERunOneIteration,	// Emulate one iteration of while loop placed 
							  					// in CTestThreadContainer::ExecutionThread   
							};
	
	public:  // Constructors and destructor
		/**
		 * Performs first phase of two-phased constructor.
		 */
		IMPORT_C static CTestThreadContainerRunner* NewL( TThreadId aMainThreadId, 
				CActiveScheduler* aMainThreadActiveScheduler );

		/**
		 * C++ destructor.
		 */
		~CTestThreadContainerRunner();
	
	private:  // Constructors and destructor
		/**
		 * C++ constructor.
		 */
		CTestThreadContainerRunner();
		
		/**
		 * Performs second phase of two-phased constructor.
		 */
		void ConstructL( TThreadId aMainThreadId, 
				CActiveScheduler* aMainThreadActiveScheduler );
	
	public: // New functions
		/**
		 * Performs TestThreadContainer setup.
		 */
		void Setup( CTestModuleContainer* aTestModuleContainer );
		
		/**
		 * Runs one iteration of emulated CTestThreadContainer::ExecutionThread while loop.
		 */
		void RunOneIteration();
		
		/**
		 * Performs cleanup of TestThreadContainer.
		 */
		void TeareDown();
		
		/**
		 * Checks if operation change signal was signaled from suspend state. 
		 */
		void CheckSignalFromSuspend();

		/**
		 * Checks if test thread is reusable.
		 */
		TBool IsReusable();


	public: // Functions from base classes
		/**
		 * RunL derived from CActive handles the completed requests.
		 */
		void RunL();

		/**
		 * DoCancel derived from CActive handles the Cancel.
		 */
		void DoCancel();

    protected:	// New functions
        // None
        
    protected:	// Functions from base classes
        // None
        
    private:	// New functions
    	/**
    	 * Test thread exception handler. 
    	 */
		static void ExceptionHandler ( TExcType aType );
	
		/**
		 * Raises panic.
		 */
		void Panic( TInt aReason );
		
		/**
		 * Complets current operation request, what causes execution of RunL method 
		 * in main thread of UITestServerStarter.
		 */
		void CompleteRequest();
    private:	// Functions from base classes
        // None    
    
	public:		//Data
	    // None
        
	protected:	// Data
        // None
        
    private:	// Data
		TThreadId iMainThreadId;					// UITestServerStarter main thread id.
		CActiveScheduler* iMainThreadActiveScheduler;	// Pointer to UITestServerStarter main thread active scheduler.		
	    RLibrary iModule;							// Handle to test module library.
		CTestModuleContainer* iTestModuleContainer; // Pointer to test module container.		
		TOperationType iCurrentOperation;			// Current operation type.
		CTestThreadContainer* iTestThreadContainer;	// Pointer to test thread container.
	
		RSemaphore iOperationOngoing;				// Current operation end semaphore.
		
	    TBool iReusable;             				// Is test module reusable.
	    TBool iSignalFromSuspend;   				// Send signal from suspend state.
	    TBool iInitialized;         				// Is module initialized.
	    TInt iReturnCode;							// Lasst loop result code
		
    public:		// Friend classes
        // None

    protected:	// Friend classes
        // None

    private:	// Friend classes
        // None
	};

#endif // TESTTHREADCONTAINERRUNNER_H_

// End of File
