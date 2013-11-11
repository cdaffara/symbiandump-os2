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



#ifndef FEATDISCOVERYIMPL_H 
#define FEATDISCOVERYIMPL_H 

//  INCLUDES
#include <e32base.h>

#include "featurecontrol.h"

class CFeatureDiscoveryImpl : public CBase
    {
    public:
	    
	    /**
	    * This is a two-phase constructor method that is used to create
	    * a new instance of the CFeatureDiscoveryImpl class.
	    *
	    * @return a pointer to a new instance of the CFeatureDiscoveryImpl class.
	    *
	    * @leave Any One of the Symbian OS system-wide error codes
	    */
		static CFeatureDiscoveryImpl* NewL();
	
	    /**
	    * Destructor.
	    */
		virtual ~CFeatureDiscoveryImpl();
	
		/**
	    * Static way to fetch information whether a certain feature is 
	    * supported in the current envinronment. There is no need to create
	    * an instance of the class when using this method.
	    *
	    * @param aFeature is the feature UID of the feature that is queried.
	    * @return TBool indicating whether the feature is supported (ETrue)
	    * or not (EFalse). If the feature does not exist, the return value is 
	    * EFalse.
	    *
	    * @leave Any One of the Symbian OS system-wide error codes
	    */
	    static TBool IsFeatureSupportedL(TUid aFeature);
	
		/**
	    * Dynamic way to fetch information whether a certain feature is 
 		* supported in the current environment. Before calling the method 
	    * an instance of the CFeatureDiscoveryImpl class need to be created by 
	    * using one of the factory methods, NewL() or NewLC(). The created 
	    * instance must be deleted after use.
	    *
	    * @param aFeature is the feature UID of the feature that is queried.
	    * @return TBool indicating whether the feature is supported (ETrue)
	    * or not (EFalse). If the feature does not exist, the return value is 
	    * EFalse.
	    */
		TBool IsSupported(TUid aFeature);

		/**
		* Static way to fetch information whether set of features are 
		* supported in the current envinronment. There is no need to create
		* an instance of the class when using this method.
		*
		* @param aFeatures is the wrapper class for feature array queried.
		* @leave Any One of the Symbian OS system-wide error codes
		*/
		static void FeaturesSupportedL( TFeatureSet& aFeatures );

		/**
	    * Dynamic way to fetch information whether set of features are 
 		* supported in the current environment. Before calling the method 
	    * an instance of the CFeatureDiscovery class need to be created by 
	    * using one of the factory methods, NewL() or NewLC(). The created 
	    * instance must be deleted after use.
	    *	    
	    * @param aFeatures is the wrapper class for feature array queried.
	    * @return KErrNone if status query succeeded.
	    *         Otherwise one of the Symbian OS error codes
	    */
		TInt FeaturesSupported( TFeatureSet& aFeatures );
			  
    private:

	    /**
	    * C++ default constructor.
	    */
			CFeatureDiscoveryImpl();
	
	    /**
	    * By default Symbian OS constructor is private.
	    */
	    void ConstructL();
	    
    private:

		// Feature Manager server client 
		RFeatureControl iFeatControl;
    } ;


#endif  // FEATDISCOVERYIMPL_H 

// EOF
