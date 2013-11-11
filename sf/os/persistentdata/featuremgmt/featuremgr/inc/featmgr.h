/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




#ifndef FEATMGR_H 
#define FEATMGR_H 

//  INCLUDES
#include <e32std.h>
#include <e32svr.h>

// FORWARD DECLARATIONS
class CFeatMgrTlsData;

// DEFINES

// CLASS DECLARATION

// CONSTANTS

/**
 Feature manager API.
 Feature manager API offers the following functionality:
 - Inquire whether a certain static feature is supported.
 For usage, see example code at the end of the header file.

@publishedPartner
@deprecated Use the class CFeatureDiscovery for basic feature queries, or the 
             class RFeatureControl for advanced feature queries and control.
*/
class FeatureManager
    {
    public: 

        /**
         This must be called in the scope of the thread before calling
         any other methods. It sets up TLS. Uninitialization is done
         by calling the UnInitializeLib() function.
        
         @leave KErrNoMemory Memory allocation failure. 
        
         @deprecated Use the class CFeatureDiscovery for basic feature queries, or the 
                     class RFeatureControl for advanced feature queries and control.
        */
        IMPORT_C static void InitializeLibL();

        /**
         This must be called in the scope of the thread after calling
         InitializeLibL(). It frees the allocated TLS. Do not call UnInitializeLib() 
         if InitalizeLibL() leaves.
        
         @deprecated Use the class CFeatureDiscovery for basic feature queries, or the 
                     class RFeatureControl for advanced feature queries and control.
        */
        IMPORT_C static void UnInitializeLib();
        
		/**
         Fetches information whether a certain feature is supported.
        
         @param aFeature feature id.
         @return feature support status.
        
         @deprecated Use the class CFeatureDiscovery for basic feature queries, or the 
                     class RFeatureControl for advanced feature queries and control.
        */		
        IMPORT_C static TBool FeatureSupported(TInt aFeature);

	private: 

        /**
         Get TlsData pointer.
        
         @return object stored in TLS
        */
        static CFeatMgrTlsData* TlsData();

    private:

        /**
         C++ default constructor.
         Prohibits instantiation of this class.
        */
        FeatureManager();
        
      
    };

/**
 Example usage:

 @code
 // replace <featureUID> with a real UID 

 #include <featmgr/featmgr.h>
 #include <bldvariant.hrh> // for feature definitions

 CMyClass::ConstructL()
     {
     // Sets up TLS, must be done before FeatureManager is used.
     FeatureManager::InitializeLibL();
     // Used in destructor. 
     iFeatMgrInitialized = ETrue;
     }

 CMyClass::ShowMenuL()
     {
     if ( FeatureManager::FeatureSupported( <featureUID> ) )
         {
         // Feature supported, show menu item associated with it.
         }
     }

 CMyClass::~CMyClass()
     {
     // Do not call UnInitializeLib() if InitalizeLib() leaves.
     if ( iFeatMgrInitialized )
         {
	      // Frees the TLS. Must be done after FeatureManager is used.
         FeatureManager::UnInitializeLib();  
         }
     }
 @endcode

*/
#endif      // FEATMGR_H 
            
// End of File
