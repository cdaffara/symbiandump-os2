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



#ifndef FEATDISCOVERY_H
#define FEATDISCOVERY_H

//  INCLUDES
#include <e32base.h>

class CFeatureDiscoveryImpl;

/**
Wrapper class used for multiple feature queries.
@publishedAll
@released
*/

class TFeatureSet
    {
    public:

		/**
		Constructor
	    */
        IMPORT_C TFeatureSet();

		/**
		Destructor
	    */
        IMPORT_C ~TFeatureSet();

		/**
		 Method to add features before querying support statuses. This
		 method must be called to initialize feature array before querying
		 support statuses from server with FeaturesSupported(L) and finally
		 check the status with a call to IsFeatureSupported or AreAllFeaturesSupported.

	     @return KErrNone if feature addition succeded.
	             Otherwise one of the Symbian OS error codes
	    */
	    IMPORT_C TInt Append( TUid aFeature );

		/**
	     Method to check feature's support status.

	     @param aFeature is the feature UID of the feature that is queried.
		 @return a TBool indicating whether the feature is supported (ETrue)
		 or not (EFalse). If the feature does not exist, the return value is
		 EFalse.
	    */
        IMPORT_C TBool IsFeatureSupported( TUid aFeature ) const;

		/**
		 Method to check whether all features queried are supported.

	     @return ETrue if all features queried are supported or no features have been queried.
	             Otherwise EFalse.
	    */
	    IMPORT_C TBool AreAllFeaturesSupported() const;

    public: // For CFeatureDiscoveryImpl internal use

        // Count number of features in TFeatureStat array.
        TInt Count();

        // Return id of feature in requested index from TFeatureStat array.
        TUid FeatureId( TInt aIndex ) const;

        // Reset TFeatureStat array.
        void Reset();

        // Append feature and status object into TFeatureStat array
        TInt Append( TUid aFeature, TBool aSupported );

    private:
        struct TFeatureStat
        {
            TUid iFeatureID;
            TBool iSupported;
        };

    private:
        // Feature id, status array
        RArray<TFeatureStat> iStatus;

        // Counter for checking feature count before and after query
        TInt iCount;

        // Reserved for future use.
        TUint32 iReserved;
    };

/**
 The feature discovery API provides methods which are used to query which
 features are supported in the environment.

@publishedAll
@released
*/
class CFeatureDiscovery : public CBase
    {
    public:

	    /**
	     This is a two-phase constructor method that is used to create
	     a new instance of the CFeatureDiscovery class.

	     @return a pointer to a new instance of the CFeatureDiscovery class.

	     @leave Any One of the Symbian OS system-wide error codes
	    */
		IMPORT_C static CFeatureDiscovery* NewL();

	    /**
	     This is a two-phase constructor method that is used to create
	     a new instance of the CFeatureDiscovery class. This method leaves
	     the instance of the object on the cleanup stack.

	     @return a pointer to a new instance of the CFeatureDiscovery class.

	      @leave Any One of the Symbian OS system-wide error codes
	    */
		IMPORT_C static CFeatureDiscovery* NewLC();

	    /**
	     Destructor.
	    */
		virtual ~CFeatureDiscovery();

		/**
		 Static method to query the supported status of a feature on the
		 device.

		 @deprecated Use IsFeatureSupportedL(TUid aFeature) instead.

		 @param aFeature is the feature ID of the feature that is queried.
		 @return a TBool indicating whether the feature is supported (ETrue)
		 or not (EFalse). If the feature does not exist, the return value is
		 EFalse.

		  @leave Any One of the Symbian OS system-wide error codes
		*/
		IMPORT_C static TBool IsFeatureSupportedL( TInt aFeature );

		/**
	     Dynamic method to query the supported status of a feature on the
	     device. Before calling the method an instance of the CFeatureDiscovery
	     class must be created by using one of the factory methods,
	     NewL() or NewLC(). The created instance must be deleted after use.

	     @deprecated Use IsSupported(TUid aFeature) instead.

	     @param aFeature is the feature ID of the feature that is queried.
	     @return a TBool indicating whether the feature is supported (ETrue)
	     or not (EFalse). If the feature does not exist, the return value is
	     EFalse.
	    */
		IMPORT_C TBool IsSupported( TInt aFeature ) const ;

		/**
	     Static method to query the supported status of a feature on the device.

	     @param aFeature is the feature UID of the feature that is queried.
	     @return a TBool indicating whether the feature is supported (ETrue)
	     or not (EFalse). If the feature does not exist, the return value is
	     EFalse.

	     @leave Any One of the Symbian OS system-wide error codes
	    */
	    IMPORT_C static TBool IsFeatureSupportedL( TUid aFeature );

		/**
	     Dynamic method to query the supported status of a feature on the device.

	     Before calling the method an instance of the CFeatureDiscovery class must
	     be created by using one of the factory methods, NewL() or NewLC().
	     The created instance must be deleted after use.

	     @param aFeature is the feature UID of the feature that is queried.
	     @return a TBool indicating whether the feature is supported (ETrue)
	     or not (EFalse). If the feature does not exist, the return value is
	     EFalse.
	    */
		IMPORT_C TBool IsSupported( TUid aFeature ) const ;

		/**
		 Static method to query the supported status of a set of features
		 on the device.

		 @param aFeatures is the wrapper class for feature array queried.

		 @leave Any One of the Symbian OS system-wide error codes
		*/
		IMPORT_C static void FeaturesSupportedL( TFeatureSet& aFeatures );

		/**
	     Dynamic method to query the supported status of a set of features
	     on the device. Before calling the method an instance of the
	     CFeatureDiscovery class need to be created by using one of the
	     factory methods, NewL() or NewLC(). The created instance must be
	     deleted after use.

	     @param aFeatures is the wrapper class for feature array queried.
	     @return KErrNone if status query succeeded.
	             Otherwise one of the Symbian OS error codes
	    */
		IMPORT_C TInt FeaturesSupported( TFeatureSet& aFeatures ) const;

    private:

	    /**
	     C++ default constructor.
	    */
		CFeatureDiscovery();

	    /**
	     By default Symbian OS constructor is private.
	    */
	    void ConstructL();

    private:

		// Feature discovery implementation class
		CFeatureDiscoveryImpl* iImpl;
    } ;

/**
 Usage:

 @code
  #include <featdiscovery.h>
  #include <featureinfo.h> // for feature definitions

  // replace <featureUIDx> with a real UID )

  // If querying only one feature, it is more efficient to use the class
  // via the static method, IsFeatureSupportedL().
  // When querying more than one feature, it is more efficient to use the
  // class by creating an instance and calling the IsSupported() method.

  // Static way of using the class:
  TBool isSupported = CFeatureDiscovery::IsFeatureSupportedL(<featureUIDx>);

  // Dynamic way of using the class using NewL():

  // Call NewL() to create an instance of CFeatureDiscovery.
  CFeatureDiscovery* testA = CFeatureDiscovery::NewL();

  // Call the exported IsSupported() method to query whether features
  // are supported in the current environment or not.
  TBool usbSupported = testA->IsSupported(<featureUIDx>);
  TBool mmcSupported = testA->IsSupported(<featureUIDx>);

  // Delete the created instance of CFeatureDiscovery.
  delete testA;

  // Dynamic way of using the class using NewLC():

  // Call NewLC() to create an instance of CFeatureDiscovery.
  // The method leaves the instance of the object on the cleanup stack.
  CFeatureDiscovery* testB = CFeatureDiscovery::NewLC();

  // Call the exported IsSupported() method to query whether features
  // are supported in the current environment or not.
  TBool wcdmaSupported = testB->IsSupported(<featureUIDx>);
  TBool gsmSupported = testB->IsSupported(<featureUIDx>);

  // Dynamic way of using multiple feature query. This is preferred
  // way to fetch support statuses if there are several features to be
  // queried, because it involves less inter-process communication.

  TFeatureSet featset;
  User::LeaveIfError( featset.Append( <featureUIDx> ) );
  User::LeaveIfError( featset.Append( <featureUIDx> ) );
  TInt err = testB->FeaturesSupported( featset );
	if(!err)
		{
			TBool uid1Supported = featset.IsFeatureSupported(<featureUIDx>);
			TBool uid2Supported = featset.IsFeatureSupported(<featureUIDx>);
			// ... or whether all QUERIED features are supported
			TBool allSupported = featset.AreAllFeaturesSupported();
		}
  // featset cleans array up in destructor on scope exit

  // Pop and delete the created instance of CFeatureDiscovery.
  CleanupStack::PopAndDestroy();
 @endcode
*/
#endif  // FEATDISCOVERY_H

// End of File
