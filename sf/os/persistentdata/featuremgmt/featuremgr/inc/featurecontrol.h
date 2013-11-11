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





#ifndef FEATURECONTROL_H
#define FEATURECONTROL_H

//  INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <featmgr/featurecmn.h>

// FORWARD DECLARATIONS

// DEFINES

// CLASS DECLARATION

// CONSTANTS

// MACROS

/**

 The Feature Control API provides methods that can be used to query which
 features are supported on the device.  The API also provides  methods to
 get status flags that define that behaviour of the feature flag and data
 associated with the feature.

 The Feature Control API provides methods to enable and disable features at
 runtime e.g. when new software is installed or uninstalled. The API provides
 also methods for setting feature data, adding a new feature to the device
 and listing all supported features on the device.

 It is only possible to enable or disable a feature or set feature data
 at runtime if that feature is modifiable (this is defined by the status
 32 bit word associated with the feature flag).  It is also possible to enable or
 disable a feature UID that is not even defined in the device yet.
 When adding a new feature to the device status flags and data of the feature
 must be set.

 The meaning of the bits of the 32-bit feature status flags are as follows:

 Bit   Flag Name	    Meaning
 - 0   Supported	    If set, feature is supported and available for use;
                       if not, feature is not supported.
 - 1   Upgradeable	    If set, feature is upgradeable. The feature is known to
                       the device but it must be upgraded to enable it. If a feature
                       is blacklisted, its upgradeable flag is unset.
 - 2   Modifiable	    If set, the feature is modifiable and may be enabled/disabled at
                       run-time. The initial flag values for such a feature flag are
                       defined in a ROM image obey file.
 - 3   Blacklisted	    If set, the feature has been blacklisted, and may not be changed
                       at run-time. This also prevents a feature from being upgraded.
 - 4   Uninitialised	If set, the status flag Supported is unknown at build-time and is
                       initialised at run-time by system software. The Feature Manager
                       will ignore the Supported flag in the file. A run-time call to
                       RFeatureControl will be needed to set the feature's supported
                       flag. Look ups of uninitialised features result in a KErrNotReady
                       error code.
 - 5   Persisted	    If set, this flag is saved to the system drive when modified
                       preserving its value across reboots/power downs
 - 6 ..23              Reserved for future use, always zero.
 - 24..31 	           Reserved for internal use.

 Enumeration TFeatureFlags defined in featurecmn.h should be used for
 accessing feature status flags e.g. flags.IsSet( EFeatureUpgradeable ),
 flags.IsSet( EFeatureModifiable ).

 Feature data is 32-bit word for client read and write. Feature data can
 contain any data that needs to be associated with the Feature Flag.
 Feature data can only be set if the feature is modifiable or a new feature.

 The use of the Feature Manager API to set the supported status of a feature,
 it in no way means that the functionality itself is modified on the device.
 All the corresponding DLLs, including feature functionality and resources still
 remain on the device. To properly support runtime activation/deactivation
 of the feature, all feature APIs need to implement runtime checking of the
 feature status in their code.

 The API consists of the RFeatureControl class which is
 used together with feature UIDs defined in featureUIDs.h, TFeatureFlags,
 TFeatureEntry and RFeatureUidArray defined in featurecmn.h.


 Usage:

 Before calling any other methods the client must call the Open()
 method that connects to the Feature Manager Server.
 When the client has finished using the RFeatureControl it should
 disconnect using the Close() method.

@publishedPartner
@released
*/
NONSHARABLE_CLASS(RFeatureControl)
    {
    public:
        /**
         C++ default constructor.
        */
        IMPORT_C RFeatureControl();

        /**
         Connect() must be called before calling any other methods of
         RFeatureControl.
         It connects to the Feature Manager Server.
         After usage Close() function must be called.

         @return Error Code. One of the Symbian error codes.

         @deprecated Use Open() instead.
        */
        IMPORT_C TInt Connect();

        /**
         Open() must be called before calling any other methods of
         RFeatureControl.
         After usage Close() function must be called.

         @return Error Code. One of the Symbian error codes.
        */
        IMPORT_C TInt Open();

        /**
         Remember to call Close after using RFeatureControl.
        */
        IMPORT_C void Close();

		/**
         This method returns the supported status on the device of the feature
         specified in the aFeature parameter.

	     @param aFeature Feature UID.
	     @return KFeatureSupported if the feature is supported.
	             KFeatureUnsupported if the feature is not supported.
	             KErrNotFound if the feature does not exist.
	             KErrNotReady it the feature is uninitialized.
	             Otherwise one of the Symbian error codes.


	    */
        IMPORT_C TInt FeatureSupported( TUid aFeature );

        /**
         This method returns the supported status on the device of the feature
         described by the aFeature parameter.  The feature UID should be set in
         TFeatureEntry class before the FeatureSupported method is called.  If
         FeatureSupported returns successufully the TFeatureEntry parameter will
         also have been populated with the feature's status flags and the data
         word associates with the feature.

	     @param aFeature A reference to a a client owned TFeatureEntry object.
	                     Must contain UID of the queried feature.
	                     After returning contains status flags and data of
	                     the feature.
	     @return KFeatureSupported if the feature is supported.
	             KFeatureUnsupported if the feature is not supported.
	             KErrNotFound if the feature does not exist.
	             KErrNotReady if the feature is uninitialized.
	             Otherwise one of the Symbian error codes.


	    */
        IMPORT_C TInt FeatureSupported( TFeatureEntry& aFeature );

        /**
         This method allows the user to pass an array of type TFeatureEntry
         to Feature Manager.  In this way the user can retrieve the supported status and
         associated information for the features whose UIDs have been set in
         TFeatureEntry classes.  In order for the client to know whether
         feature is just disabled or does not exist at all it must check
         this from response feature array. If the feature exists the EFeatureSupported
         status bit will contain the support status. If the feature does not exist
         it will be removed from array after the query. For this reason instead of hard
         coding array accesses for features the Find method should always
         be used to query feature index in array after query.

	     @param aFeatures A reference to a a client owned RFeatureArray array.
	                     Must contain UIDs of the queried features.
	     @return KErrNone if fetching of features statuses successful.
	             KErrArgument if query made with empty array.
	             Otherwise one of the Symbian error codes.

	    */
        IMPORT_C TInt FeaturesSupported( RFeatureArray& aFeatures );

        /**
         Enables the specified feature. WriteDeviceData capability required.

         @param aFeature Feature UID.
         @return Error Code. KErrNone if the feature was succesfully enabled.
                             KErrAccessDenied if the feature is not modifiable.
                             KErrPermissionDenied if a capability check fails.
                             KErrNotFound if the feature does not exist.
                             KErrArgument if the maximum number of cached features 
                             during a Software Installer (SWI) install/uninstall
                             session has been exceeded.
                             Otherwise one of the Symbian error codes. 
        */
        IMPORT_C TInt EnableFeature( TUid aFeature );

        /**
         Disables the specified feature. WriteDeviceData capability required.

         @param aFeature Feature UID.
         @return Error Code. KErrNone if the feature was succesfully disabled.
                             KErrAccessDenied if the feature is not modifiable.
                             KErrPermissionDenied if a capability check fails.
                             KErrNotFound if the feature does not exist.
                             KErrArgument if the maximum number of cached features 
                             during a Software Installer (SWI) install/uninstall
                             session has been exceeded.
                             Otherwise one of the Symbian error codes. 
        */
        IMPORT_C TInt DisableFeature( TUid aFeature );

        /**
         Enables or disables the specified feature and sets feature data.
         WriteDeviceData capability required.

         @param aFeature Feature UID.
         @param aEnabled ETrue if enabling the feature, EFalse if disabling.
         @param aData Feature data.
         @return Error Code. KErrNone if the feature was succesfully enabled.
                             KErrAccessDenied if the feature is not modifiable.
                             KErrPermissionDenied if a capability check fails.
                             KErrNotFound if the feature does not exist.
                             KErrArgument if the maximum number of cached features 
                             during a Software Installer (SWI) install/uninstall
                             session has been exceeded. 
                             Otherwise one of the Symbian error codes. 
        */
        IMPORT_C TInt SetFeature( TUid aFeature, TBool aEnabled, TUint32 aData );

        /**
         Sets feature data. WriteDeviceData capability required.

         @param aFeature Feature UID.
         @param aData Feature data.
         @return Error Code. KErrNone if the feature data was succesfully set.
                             KErrAccessDenied if the feature is not modifiable.
                             KErrPermissionDenied if a capability check fails.
                             KErrNotFound if the feature does not exist. 
                             KErrArgument if the maximum number of cached features 
                             during a Software Installer (SWI) install/uninstall
                             session has been exceeded.
                             Otherwise one of the Symbian error codes. 
        */
        IMPORT_C TInt SetFeature( TUid aFeature, TUint32 aData );

        /**
         Adds a new feature and sets status flags and data of the feature.
         Note: When adding a new feature to the device status flags and
         data of the feature must be set. WriteDeviceData capability required.

         @param aFeature A reference to a client owned TFeatureEntry object.
                         Must contain UID, status flags and data of the feature.
         @return Error Code. KErrNone if the feature was succesfully added.
                             KErrPermissionDenied if capability check fails.
                             KErrAlreadyExists if the feature already exists 
                             in the device. 
                             KErrArgument if the maximum number of cached features 
                             during a Software Installer (SWI) install/uninstall
                             session has been exceeded, or when "out of memory"
                             occured in a previous call to this function during 
                             the caching process. 
                             KErrNoMemory if out-of-memory occured during caching.
                             Otherwise one of the Symbian error codes.

         @see TFeatureEntry
	     @see TFeatureFlags
        */
        IMPORT_C TInt AddFeature( TFeatureEntry& aFeature );

        /**
         Deletes a feature.
         Note: WriteDeviceData capability required.

         @param aFeature Feature UID.
         @return Error Code. KErrNone if the feature was successfully deleted.
                             KErrAccessDenied if the feature is not runtime.
                             KErrPermissionDenied if capability check fails.
                             KErrNotFound if the feature does not exist. 
                             KErrArgument if the maximum number of cached features 
                             during a Software Installer (SWI) install/uninstall
                             session has been exceeded.
                             KErrNoMemory if out-of-memory occured during caching.
                             Otherwise one of the Symbian error codes. 

         @see TFeatureEntry
	     @see TFeatureFlags
        */
        IMPORT_C TInt DeleteFeature( TUid aFeature );

        /**
         Lists all supported features.

         @param aSupportedFeatures A reference to a client owned RFeatureUidArray array which
                                   will be filled with the UIDs of supported features.
         @return Error code. KErrNone if client owned array was successfully
                             filled with IDs of supported features.
                             KErrServerBusy if there was ongoing enabling or
                             disabling of some feature(s) and Feature Manager Server
                             could not return unambiguous list of supported features.
                             KErrNoMemory if out-of-memory occured during caching. 
                             Otherwise one of the Symbian error codes.
        */
        IMPORT_C TInt ListSupportedFeatures( RFeatureUidArray& aSupportedFeatures );

        /**
         Calling this method informs Feature Manager that a software installer (SWI) 
         launched executable is going to set, add, delete, enable or disable features 
         in Feature Manager. This mandates Feature Manager to cache all modification 
         calls issued by that executable until a call to SWIEnd is made to commit them 
         to Feature Manager. This prevents against situations where the installation is
         user-aborted without being able to roll back direct feature manipulations. 

         @return Error code. KErrNone if Feature Manager has accepted the request to use 
        					 the caching APIs. 
							 KErrInUse if the caching APIs are already in use by another 
							 exe launched by the SWI. 
							 KErrNotReady if there is no software installation in progress. 
        */
        IMPORT_C TInt SWIStart();

        /**
         Mandates Feature Manager to stop caching and prepare to commit the cached
         feature manipulations performed by the executable launched by SWI during
         installation into Feature Manager. The cached features are committed only
         after that installation of the executable is successfully completed. 
         This API must be called before a timeout managed by Feature Manager times 
         out. Otherwise the cached features will be dumped.

         @return Error code. KErrNone when SWIEnd call is received by a client or when an abort occurs 
         					 prior to the timeout managed by Feature Manager expires. 
							 KErrNotReady if SWIStart was not called before calling this 
							 function, or because the timeout managed by Feature Manager 
							 had timed out. 
							 KErrGeneral an indication that an out-of-memory condition
							 was encountered during caching which will force FeatMgr to 
							 disregard the whole cache.
							 KErrNoMemory if an out-of-memory condition was encountered 
							 whilst reserving memory in the features list for the new 
							 features t be added, or if an out-of-memory occured in a call
							 to AddFeaure() during caching..
							 Otherwise one of the Symbian error codes.							 
        */
        IMPORT_C TInt SWIEnd();
        
	private:         
	    
	    /**
	     Used for resource counting.
	    */
	    TBool iInitialized;

	    /**
	     Reserved for future use. Initialised to 0 on construction.
	    */
        TUint32 iReserved1;

	     /**
	     Reserved for future use. Initialised to 0 on construction.
	    */
	    TUint32 iReserved2;

    };

/**
 @code
 // replace <featureUID> with a real UID )

 #include <featmgr/featurecontrol.h>
 #include <featmgr/featureinfo.h> // for feature definitions

 CMyClass::MyMethodL()
     {
     // Replace KFeatureUidx with real feature uid.
     // Open() must be called before calling any other methods.
     // Disconnect is done by calling the Close() method.
     RFeatureControl featureControl;
     TInt err = featureControl.Open();

     if ( err == KErrNone )
         {
         // Query multiple features statuses (single IPC-call, so less overhead)

          RFeatureArray uids;
          CleanupClosePushL(uids);

          uids.AppendL( KFeatureUid1 );
          uids.AppendL( KFeatureUid2 );
          TInt err = control.FeaturesSupported( uids );

          if ( err == KErrNone )
              {
              TInt index;

              index = uids.Find( KFeatureUid1 );
              if(index == KErrNotFound)
                  {
                  // Feature does not exist (result is analogous if KFeatureUid1
                  // is not appended in uids array before query).
                  }
              else
                  {
                  iFeature1Supported = uids[index].FeatureFlags().IsSet( EFeatureSupported );
                  }

              index = uids.Find( KFeatureUid2 );
              if(index == KErrNotFound)
                  {
                  // Feature does not exist (result is analogous if KFeatureUid2
                  // is not appended in uids array before query).
                  }
              else
                  {
                  TBool feature2Supported = uids[index].FeatureFlags().IsSet( EFeatureSupported );
                  TUint32 data = uids[index].FeatureData();
                  // Do something with support-information and data
                  ...
                  }
              }
          CleanupStack::PopAndDestroy(&uids);


         // Query single feature status

          err = control.FeatureSupported( KFeatureUid3 );
          if( err == KFeatureSupported )
              {
              // do something with enabled feature
              }


         // Query single feature status and data

          TFeatureEntry entry( KFeatureUid4 );
          err = control.FeatureSupported( entry );
          if( err == KFeatureSupported )
              {
              TUint32 data = entry.FeatureData();
              // do something with enabled feature's data
              }
          else if( err == KFeatureUnsupported )
              {
              // do or do not something with disabled feature's data
              }


         // Enable some feature.

         // EnableFeature() and DisableFeature() methods return
         // KErrNone if the feature was succesfully enabled or disabled,
         // KErrAccessDenied if the feature is not modifiable.
         // KErrPermissionDenied a capability check fails,
         // KErrNotFound if the feature does not exist and
         // otherwise one of the Symbian error codes.
         err = featureControl.EnableFeature( <featureUID> );

         // do something


         // List all supported features.

         // A client owned RFeatureUidArray array which will be filled with
         // the UIDs of supported features.
         RFeatureUidArray supportedFeatures;

         // ListSupportedFeatures() returns one of the Symbian error codes.
         err = featureControl.ListSupportedFeatures( supportedFeatures );

         // do something

         // Closes the array and frees all memory allocated to the array.
         supportedFeatures.Close();


         // Remember to call CloseL after using RFeatureControl.
         // It disconnects the Feature Manager server.
         featureControl.Close();
         }

     }
 @endcode
*/

#endif      // FEATURECONTROL_H

// End of File
