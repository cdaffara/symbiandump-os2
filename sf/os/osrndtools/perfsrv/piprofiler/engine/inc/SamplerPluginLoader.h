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
* Description:  
*
*/


#ifndef  SAMPLERPLUGINLOADER_H
#define  SAMPLERPLUGINLOADER_H

// INCLUDES
#include    <e32base.h>
#include    <ecom/implementationinformation.h>
#include 	<piprofiler/ProfilerConfig.h>
#include 	<piprofiler/SamplerPluginInterface.h>

// CONSTANTS
// Value for a to b comparison result when logically a == b.
const TInt KSamplerComparisonEqual     = 0;

// Value for a to b comparison result when logically a < b.
const TInt KSamplerComparisonBefore    = -1;

// Value for a to b comparison result when logically a > b.
const TInt KSamplerComparisonAfter     = 1;

// CLASS DEFINITIONS
class CSamplerPluginInterface;
class MSamplerPluginLoadObserver;
class REComSession;

/**
* CSamplerPluginLoader. Mechanism used to load plugins asynchronously. Uses
* MSamplerPluginLoadObserver as a callback.
*
* @lib ProfilerEngine.exe/.lib???????????
* @since Series60_30.1
*/
class CSamplerPluginLoader : public CActive
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        *
        * @param aAppUi Pointer to application UI. Does not take ownership.
        */
        static CSamplerPluginLoader* NewL();

        /**
        * Destructor
        */
        ~CSamplerPluginLoader();

    private: // Internal construction

        /**
        * Default C++ contructor
        */
        CSamplerPluginLoader();

        /**
        * Symbian OS default constructor
        * @return void
        */
        void ConstructL();

    public: // API

        /**
        * Starts loading GS plug-ins asynchronously. Will call
        * MSamplerPluginLoadObserver::HandlePluginLoaded() each time a plug-in is
        * loaded and when all plugins are loaded.
        *
        * CSamplerPluginLoader transfers the ownership of each loaded plugin view to
        * CAknViewAppUi. It is client's responsibility to remove the views from
        * CAknViewAppUi and delete the plugins if necessary.
        *
        * @param aInterfaceUid Uid ofthe interfaces to be loaded.
        * @param aParentUid Uid of the parent. Only children of this parent
        *        will be loaded.
        * @param aPluginArray An array for the loaded GS plug-ins.
        *        CSamplerPluginLoader does not take the ownership of this array.
        */
        void LoadAsyncL( CArrayPtrFlat<CSamplerPluginInterface>* aPluginArray );

        /**
        * Load a specific plugin instance synchronously.
        *
        * @param aInterfaceUid Uid ofthe interfaces to be loaded.
        * @param aImplementationUid Uid of the implementation to load
        */
        void LoadSyncL( CArrayPtrFlat<CSamplerPluginInterface>* aPluginArray );

        /**
        * Sets observer for this loader. Only one observer per loader in one
        * time is possible.
        */
        void SetObserver(MSamplerPluginLoadObserver* aObserver);

        /**
        * Aborts asynchronous loading of the GS plug-ins.
        */
        void AbortAsyncLoad();


        /**
        * Sorts the plugin array.
        *
        * Sorting criterias:
        *
        * 1. Order number if provider category is Internal.
        * 2. Provider category. Precedence as follows:
        *       1. KGSPluginProviderInternal
        *       2. EGSPluginProviderOEM
        *       3. EGSPluginProviderOperator
        *       4. EGSPluginProvider3rdParty
        * 3. Alphabetical
        *
        * @param aPlugins The array which will be sorted.
        */
        void SortPluginsL( CArrayPtrFlat<CSamplerPluginInterface>* aPlugins );

    private: // Internal methods

        /**
        * Starts loading next plugin.
        */
        void LoadNextPluginL();

        /**
        * Creates a plugin instance from given UID. Ownership is transferred.
        */
        CSamplerPluginInterface& CreatePluginInstanceL( 
                const CImplementationInformation& aImpInfo );

        /**
        * Notifies MPluginLoadObserver.
        */
        void NotifyProgress();

        /**
        * Notifies MPluginLoadObserver.
        */
        void NotifyFinished();

        /**
        * Wait for the next round of CActive execution.
        */
        void CompleteOwnRequest();

        // Insertion function used by sorting:

        /**
        * Inserts plugin in the correct position in the array using sorting 
        * criterias. Assumes aPlugins is ordered.
        *
        * @param aPlugin The plugin to be inserted.
        * @param aPlugins Array in which the plugin is inserted into the
        *        corresponding location.
        */
        void InsertPluginInOrderL(
            CSamplerPluginInterface* aPlugin,
            CArrayPtrFlat<CSamplerPluginInterface>* aPlugins );

        // Comparison functions:

        /**
        * Compares plugins according to comparison criterias.
        * 
        * Note: GS internal comparison constants such as KGSComparisonEqual are 
        * different from the ones outputted by this function. This is because 
        * this function is also usable by RArray sort -functionality but BC 
        * cannot be broken in GS.
        * 
        * @return Negative value: If aFirst before aSecond.
        *                      0: If equal.
        *         Positive value: If aSecond before aFirst.
        */
        static TInt Compare( const CSamplerPluginInterface& aFirst,    
                             const CSamplerPluginInterface& aSecond );

        /**
        *
        * @return   KGSComparisonEqual  = equal indexes
        *           KGSComparisonBefore = aFirst is before aSecond
        *           KGSComparisonAfter  = aFirst is after aSecond
        */

        static TInt CompareIndex( 
                const CSamplerPluginInterface& aFirst,
                const CSamplerPluginInterface& aSecond );

    private: // Utility methods

        /**
        * Parses descriptor to UID.
        */
        static TInt ParseToUid( const TDesC8& aSource, TUid& aTarget );

        /**
        * Parsers plugin's order number
        */
        static TInt ParseOrderNumber( const TDesC8& aSource, TInt& aOrderNumber );

    private: // From CActive

        /**
        * See base class.
        */
        void RunL();

        /**
        * See base class.
        */
        TInt RunError( TInt aError );

        /**
        * See base class.
        */
        void DoCancel();

    private: // Data
        // Pluginloader goes through this array and loads the plugins into
        // iPluginArray if they fulfill the criterias.
        RImplInfoPtrArray iImplInfoArray;

        // Used as an iterator to maintain location in iImplInfoArray.
        TInt iImplInfoArrayIterator;

        // Array of loaded plugins.Plugins are owned by iAppUi. Only the array
        // pointers are owned by this class and therefore only reset array.
        CArrayPtrFlat<CSamplerPluginInterface>* iPluginArray;

        // Pointer to observer. Not owned.
        MSamplerPluginLoadObserver* iObserver;

        // Number of RunL calls.
        TInt iRunLDebugCount;

    };


/**
* Interface for SamplerPluginLoader observer. MSamplerPluginLoadObserver gets
* notifications when plugins are loaded.
*
* @lib ProfilerEngine.exe/.lib???????????
* @since Series60_30.1
*/

class MSamplerPluginLoadObserver
    {
    public: // Enums
        enum KSamplerPluginLoaderStatus
            {
            // One plugin loaded successfully, continue to next.
            ESamplerSuccess,
            // Loading one plugin failed, contiue to next.
            ESamplerFail,
            // Client called AbortAsyncLoad(), finished loading.
            ESamplerAborted,
            // All plugins loaded successfully, finished loading.
            ESamplerFinished,
            // Severe error with loader, finished loading.
            ESamplerError
            };

    public: // New

        //CSamplerPluginLoader calls this function when each plug-in is loaded or
        //loading is finished..
        
        IMPORT_C virtual void HandlePluginLoaded(
            KSamplerPluginLoaderStatus aStatus ) = 0;
    };


#endif // SAMPLERPLUGINLOADER_H
// End of File
