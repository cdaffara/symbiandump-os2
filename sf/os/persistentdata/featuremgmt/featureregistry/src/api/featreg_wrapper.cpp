// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of wrapper API for querying support for features on a device, It 
// internally uses Feature Manager query APIs.
// 
//

#include <e32uid.h>
#include <featdiscovery.h>  
#include "featregpan.h"
#include "featreg.h"
#include "featregcmn.h"

// Class to hold pointers to Cleanup stack & Feature Manager CFeatureDiscovery
// virtual methods from CBase that result in these _TZ entries being generated.
//NONSHARABLE_CLASS is used to remove _ZTI & _ZTV entry from def file . 
NONSHARABLE_CLASS (CImplFeatDisc ) : public CBase
	{
	public:
		static CImplFeatDisc* NewL();
		~CImplFeatDisc();
	private:
		CImplFeatDisc();
		void ConstructL();
	
	public:
	
	CFeatureDiscovery *iFeatDis;
	CTrapCleanup * iCleanup;
	};
	

CImplFeatDisc::CImplFeatDisc() :   
	iFeatDis( NULL ), 
	iCleanup(NULL)
	{
		
	}
	
CImplFeatDisc::~CImplFeatDisc()
	{
	delete iFeatDis;
	iFeatDis = 0;
	
	delete iCleanup;
	iCleanup = 0;
	}

void CImplFeatDisc::ConstructL()
    {
    iFeatDis = CFeatureDiscovery::NewL();
    }
	

CImplFeatDisc* CImplFeatDisc::NewL()
    {
    CImplFeatDisc* self = new( ELeave ) CImplFeatDisc();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


/**
 * Dummy feature registry implementation object - never instantiated.
 * @internalComponent
 */
class RFeatureRegistry::TImpl
	{
	TUint32 iDummy;
	};

/**
 * Opens connection to the Feature Registry for making non-static queries.
 * Note all non-static queries return state at the time Open() was called;
 * Feature Registry changes are not observed until instance closed and re-opened.
 *
 * @return KErrNone if successful, negative system-wide error code if fails
 * @publishedPartner
 * @deprecated
 */
EXPORT_C TInt RFeatureRegistry::Open()
	{
	__ASSERT_ALWAYS(iImpl == NULL, Panic(EFeatRegInvalidUse));
	
	CTrapCleanup *cleanup = NULL;
	
	if(User::TrapHandler() == NULL)
		{
		cleanup = CTrapCleanup::New();	
		if(cleanup == NULL)
	        {
	        return KErrNoMemory;
	        }
		}
	
	CImplFeatDisc  *ptrh = NULL; 	
	TRAPD( err,  ptrh = CImplFeatDisc::NewL() );
    if ( err == KErrNone )
        {
    	ptrh->iCleanup = cleanup;
        iImpl = reinterpret_cast<TImpl*> (ptrh);
        }
    else
    	delete cleanup;
	return err;
	}

/**
 * Queries support for feature on the device.
 * Non-static version requiring open instance of class.
 * Recommended when making multiple queries.
 * Note: returns support for feature from the time Open() was called.
 *
 * @param aFeatureUid Unique identifier of feature being queried
 * @return positive value if feature is supported, zero if feature is not supported,
 *     or negative system-wide error code if could not be determined.
 * @pre this registry instance is open
 * @panic FeatReg EFeatRegInvalidUse if this registry instance is not open
 * @publishedPartner
 * @deprecated
 */
EXPORT_C TInt RFeatureRegistry::QuerySupport(TUid aFeatureUid)
	{
	
	TUint32 dummyInfo;
	return QuerySupport(aFeatureUid, dummyInfo);
	}

/**
 * Queries support for feature on the device.
 * Non-static version requiring open instance of class.
 * Recommended when making multiple queries.
 * Note: returns support for feature from the time Open() was called.
 *
 * @param aFeatureUid Unique identifier of feature being queried
 * @param aInfo addition status information about feature
 * @return positive value if feature is supported, zero if feature is not supported,
 *     or negative system-wide error code if could not be determined.
 * @pre this registry instance is open
 * @panic FeatReg EFeatRegInvalidUse if this registry instance is not open
 * @publishedPartner
 * @deprecated
 */
EXPORT_C TInt RFeatureRegistry::QuerySupport(TUid aFeatureUid, TUint32& aInfo)
	{
	__ASSERT_ALWAYS(iImpl != NULL, Panic(EFeatRegInvalidUse));

	CImplFeatDisc *ptrh  = reinterpret_cast<CImplFeatDisc*> (iImpl);
	
	if(ptrh->iFeatDis->IsSupported(aFeatureUid))
		{
		
		//FeatReg support only ROM features so EStatusUpgradableBit has no meaning
		//So aInfo always set to EStatusSupportBit in this wrapper
		aInfo = EStatusSupportBit;
		return EStatusSupportBit;		
		}

	// feature not supported
	aInfo =0;
	return 0;
	}

/**
 * Closes this registry instance.
 * @publishedPartner
 * @deprecated
 */
EXPORT_C void RFeatureRegistry::Close()
	{
	CImplFeatDisc *ptrh  = reinterpret_cast<CImplFeatDisc*> (iImpl);
	delete ptrh;
	ptrh = 0;
	}

/**
 * Queries support for feature on the device.
 * Static version recommended for single queries.
 *
 * @param aFeatureUid Unique identifier of feature being queried
 * @return positive value if feature is supported, zero if feature is not supported,
 *     or negative system-wide error code if could not be determined.
 * @publishedPartner
 * @deprecated
 */
EXPORT_C TInt RFeatureRegistry::QuerySupportS(TUid aFeatureUid)
	{
	TUint32 dummyInfo;
	return QuerySupportS(aFeatureUid, dummyInfo);
	}

/**
 * Queries support for feature on the device.
 * Static version recommended for single queries.
 *
 * @param aFeatureUid Unique identifier of feature being queried
 * @param aInfo addition status information about feature
 * @return positive value if feature is supported, zero if feature is not supported,
 *     or negative system-wide error code if could not be determined.
 * @publishedPartner
 * @deprecated
 */
EXPORT_C TInt RFeatureRegistry::QuerySupportS(TUid aFeatureUid, TUint32& aInfo)
	{
	CTrapCleanup * cleanup = NULL;
	if(User::TrapHandler() == NULL)
		{
		cleanup = CTrapCleanup::New();
		if(cleanup == NULL)
	        {
	        return KErrNoMemory;
	        }
		}
	TBool supported = EFalse;
	TRAPD( err,  supported = CFeatureDiscovery::IsFeatureSupportedL(aFeatureUid) );
	
	if(cleanup != NULL)
		delete cleanup;
     
	if (err != KErrNone)
		{
		return err;
		}
		
    if ( supported)
        {
		//FeatReg support only ROM features so EStatusUpgradableBit has no meaning
		//So aInfo always set to EStatusSupportBit in this wrapper
		aInfo = EStatusSupportBit;
		return EStatusSupportBit;		
        }
   
	// feature not supported
	aInfo =0;
	return 0;

	}
	

// CLASS DECLARATION

/**
Dummy class. Contains placeholder for a removed RFeatureRegistryNotify function to prevent BC break.
@internalComponent
*/
class Dummy
    {

    public: // New functions

        IMPORT_C static void Dummy1();
        IMPORT_C static void Dummy2();
        IMPORT_C static TInt Dummy3();
        IMPORT_C static void Dummy4(TRequestStatus &);
    };

// ================= MEMBER FUNCTIONS =======================


/**
Dummy method
@internalComponent
*/
EXPORT_C void Dummy::Dummy1() { }

/**
Dummy method
@internalComponent
*/
EXPORT_C void Dummy::Dummy2() {  }
/**
Dummy method
@internalComponent
@return KErrNotSupported
*/
EXPORT_C TInt Dummy::Dummy3() 
{  
return KErrNotSupported;
}

/**
Dummy method
@internalComponent
*/
EXPORT_C void Dummy::Dummy4(TRequestStatus &) {  }





