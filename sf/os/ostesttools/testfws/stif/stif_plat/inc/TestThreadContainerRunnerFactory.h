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
* CTestThreadContainerRunnerFactory class implementations.
*
*/

#ifndef TESTTHREADCONTAINERRUNNERFACTORY_H_
#define TESTTHREADCONTAINERRUNNERFACTORY_H_

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
class CTestThreadContainerRunner;
class CUiEnvProxy;

// CLASS DECLARATION

// DESCRIPTION
// CTestThreadContainerRunnerFactory class interface.
class CTestThreadContainerRunnerFactory: public CActive
	{
	public: // Enumerations
	    // None
	
	private: // Enumerations
		/**
		 * .Current operation type
		 */
		enum TOperationType { ECreate, // Create TestModuleContainer
							  EDelete, // Delete TestModuleContainer
							  ENone    // No operation
							};
	
	public:  // Constructors and destructor
		/**
		 * C++ destructor.
		 */
		~CTestThreadContainerRunnerFactory();
	
	private:  // Constructors and destructor
	    // None
	
	public: // New functions
	    /**
	    * NewL is first phase of two-phased constructor.
	    */		
		IMPORT_C static CTestThreadContainerRunnerFactory* NewL( TThreadId aThreadId, CActiveScheduler* aScheduler, CUiEnvProxy* aUiEnvProxy );
	
		/**
		 * Creates new CTestThreadContainerRunner object.
		 */
		CTestThreadContainerRunner* CreateL();
		
		/**
		 * Deletes CTestThreadContainerRunner object.
		 */
		void DeleteL( CTestThreadContainerRunner* aTestThreadContainerRunner );

		/**
		 * Gets UIEnvProxy.
		 */
		CUiEnvProxy* GetUiEnvProxy();
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
	    * C++ default constructor.
	    */
		CTestThreadContainerRunnerFactory();

		/**
	    * By default Symbian OS constructor is private.
	    */
		void ConstructL( TThreadId aThreadId, CActiveScheduler* aScheduler, CUiEnvProxy* aUiEnvProxy );

    private:	// Functions from base classes
        // None    
    
	public:		//Data
	    // None
        
	protected:	// Data
        // None
        
    private:	// Data
		TThreadId iThreadId;				// Main thread id.
		CActiveScheduler* iScheduler;		// Active scheduler from main thread.
		CUiEnvProxy* iUiEnvProxy;			// UIEnvProxy.
		
		CTestThreadContainerRunner* iTestThreadContainerRunner; // Temporary pointer for CTestThreadContainerRunner
																// which was created or should be delated.
		TOperationType iOperation;			// Indicates current operation.
		RSemaphore iOperationEndSemaphore;	// Operation end semaphore
		
    public:		// Friend classes
        // None

    protected:	// Friend classes
        // None

    private:	// Friend classes
        // None
	};


#endif /*TESTTHREADCONTAINERRUNNERFACTORY_H_*/

// End of File
