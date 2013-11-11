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
* Description: This file contains the header file of the CCallBack 
* class.
*
*/

#ifndef CALLBACK_H
#define CALLBACK_H

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
// None

// CLASS DECLARATION

// DESCRIPTION
// Simple class for callbacks.

class CCallBack 
    :public CAsyncCallBack
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor
        /**
        * C++ constructor.
        */
        CCallBack( TInt aPriority ):CAsyncCallBack( aPriority ) {}

        /**
        * C++ constructor.
        */
        CCallBack( TCallBack& aCallBack, TInt aPriority ):
            CAsyncCallBack( aCallBack, aPriority ) {}
    
    public: // New functions
        /**
        * Return reference to TRequestStatus member.
        */
        TRequestStatus& Status(){ return iStatus; }

        /**
        * Set CCallBack active.
        */
        void SetActive()
            { 
            iStatus = KRequestPending; 
            CActive::SetActive(); 
            };
    
    public: // Functions from base classes
   
        /**
        *
        */
        void RunL(){ iCallBack.CallBack(); }

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

	public:     // Data
    
	protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };


template <class T>
class CActiveCallback 
        :public CActive
    {
    public: // Enumerations

        /**
        * Callback function type definitions
        */
       typedef void (T::* TestFunction)();    

    private: // Enumerations

    public:  // Constructors and destructor
        /**
        * C++ constructor.
        */
        CActiveCallback ( T* aClass, TestFunction aFunction ) : CActive (EPriorityStandard)
            {
            iMethod = aFunction;
            iClass = aClass;
            iCompleted = EFalse;
            }
    
    public: // New functions
        /**
        * Return reference to TRequestStatus member.
        */
        TRequestStatus& Status(){ return iStatus; }

        /**
        * Set active object to active and return reference to TRequestStatus member
        */
        TRequestStatus& Activate()
            {        
            iCompleted = EFalse;
            SetActive();
            return iStatus;
            }

        /**
        * Is callback completed?
        */
        TBool isCompleted()
            {
            return iCompleted;
            }
    
    public: // Functions from base classes
   
        /**
        * Call the callback
        */
        void RunL()
            {
            iCompleted = ETrue;
            if ( iMethod != NULL )
                {
                (iClass->*iMethod) ();
                }        
            }

        /**
        * Just forward errors to framework
        */
        TInt RunError ( TInt aError )
            {
            return aError;
            }

        /**
        * Cancel is not supported.
        */
        void DoCancel ()
            {  
            }

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:



	public:     // Data
    
	protected:  // Data

    private:    // Data
        TestFunction iMethod;           // Pointer to method to be called
        T* iClass;                      // Pointer to calls to be called
        TBool iCompleted;               // Is callback completed?

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // CALLBACK_H  

// End of File
