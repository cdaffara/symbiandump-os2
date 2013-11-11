// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of API for querying support for features on a device, and
// receiving notification if features are added or removed.
// 
//

#include <e32property.h>
#include <e32cmn.h>
#include <e32uid.h>
#include "featreg.h"
#include "featregpan.h"
#include "featregcmn.h"

/**
 * Run setup exe, wait for completion: ends only once property defined, or failed
 * @internalComponent
 * @return KErrNone on success, or system-wide error code
 */
static TInt RunFeaturePropertySetupExe()
	{
	const TUidType setupExeUidType(KExecutableImageUid, TUid::Null(), KFeaturePropCat);
	RProcess setupProc;
	TInt result = setupProc.Create(KFeatRegSetupExe, KNullDesC, setupExeUidType);
	if (result != KErrNone)
		{
		return result;
		}
	TRequestStatus setupStatus;
	// request Rendezvous before Resume() to avoid race condition.
	// Also note if request to rendezvous fails (OOM etc.) then setup exe may
	// complete after query code, with feature property possibly undefined
	setupProc.Rendezvous(setupStatus);
	setupProc.Resume();
	setupProc.Close();
	User::WaitForRequest(setupStatus);
	return setupStatus.Int();
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
	RProperty featureProperty;
	TInt result = featureProperty.Attach(KFeaturePropCat, KFeaturePropKey);
	if (result != KErrNone)
		{
		return result;
		}

	// read feature property header
	TInt propertySize = 0;
	TFeatureHeader header;
	TPckg<TFeatureHeader> headerPckg(header);
	TBool ranSetup = EFalse;
	TInt setupResult = KErrNone;
	while (ETrue)
		{
		result = featureProperty.Get(headerPckg);
		if ((result == KErrOverflow)
			|| ((result == KErrNone) && (headerPckg.Size() >= sizeof(TFeatureHeader))))
			{
			if (header.IsInvalid())
				{
				result = KErrCorrupt;
				}
			else
				{
				propertySize = header.PredictedPropertySize();
				result = KErrOverflow;	// indicates successful outcome from this phase
				}
			break;
			}
		if (ranSetup)
			{
			if (setupResult == KErrNoMemory)
				{
				result = KErrNoMemory;
				}
			else if (setupResult == KErrCorrupt)
				{
				result = KErrCorrupt;
				}
			else
				{
				// must force an error return - other than KErrOverflow
				result = KErrUnknown;
				}
			break;
			}
		setupResult = RunFeaturePropertySetupExe();
		ranSetup = ETrue;
		}

	// allocate and read property. Iterate while overflow reported
	// in case property is republished while reading it
	while (result == KErrOverflow)
		{
		// the feature property data consists of only 32-bit values
		// allocate in TUint32 blocks to cover any alignment issues
		TUint32 size32 = (propertySize + sizeof(TUint32) - 1) / sizeof(TUint32);
		TUint32* propertyBuf32 = new TUint32[size32];
		TUint8* propertyBuf = reinterpret_cast<TUint8*>(propertyBuf32);
		if (propertyBuf == NULL)
			{
			result = KErrNoMemory;
			break;
			}
		TPtr8 propertyDes(propertyBuf, 0, propertySize);
		result = featureProperty.Get(propertyDes);
		if (propertyDes.Size() >= sizeof(TFeatureHeader))
			{
			const TFeatureHeader& headerRef = *(reinterpret_cast<const TFeatureHeader*>(propertyBuf));
			// overflow checking for the following is already done by setup exe
			if ((result == KErrNone) && (!headerRef.IsInvalidOrBadSize(propertyDes.Size())))
				{
				// success
				iImpl = reinterpret_cast<TImpl*>(propertyBuf);
				break;
				}
			// if it's not a valid overflow (where predicted size is indeed larger than maxsize), it's corrupt
			if ((result != KErrOverflow) || (headerRef.PredictedPropertySize() < propertyDes.MaxSize()))
				{
				result = KErrCorrupt;
				}
			}
		else
			{
			result = KErrCorrupt;
			}
		delete[] propertyBuf;
		if (result != KErrOverflow)
			{
			result = KErrCorrupt;
			break;
			}
		}

	featureProperty.Close();
	// panic in debug mode to alert system integrators that the setup exe
	// is absent/inaccessible or the config data is invalid in this OS
	// configuration: a serious problem
	__ASSERT_DEBUG(result != KErrCorrupt, Panic(EFeatRegBadConfig));
	return result;
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

	TFeatureHeader* header = reinterpret_cast<TFeatureHeader*>(iImpl);
	TUint32 featureUid = aFeatureUid.iUid;

	// try to find in feature entries first
	TFeatureEntry* entry = reinterpret_cast<TFeatureEntry*>(header + 1);
	if (header->iFeatureEntryCount > 0)
		{
		RArray<TFeatureEntry> entryArray(sizeof(TFeatureEntry), entry, header->iFeatureEntryCount);	
		TFeatureEntry searchEntry = { featureUid , 0 };
		TInt index = entryArray.FindInUnsignedKeyOrder(searchEntry);
		if (index >= 0)
			{
			aInfo = entryArray[index].iInfo;
			return aInfo & EStatusSupportBit;
			}
		}

	// fall back to default ranges - first range to match wins
	TFeatureRange* range = reinterpret_cast<TFeatureRange*>(entry + header->iFeatureEntryCount);
	for (TInt i = header->iFeatureRangeCount; i > 0; --i, ++range)
		{
		if ((featureUid >= range->iLowUid) && (featureUid <= range->iHighUid))
			{
			aInfo = EStatusSupportBit;
			return EStatusSupportBit;
			}
		}
	
	// final default: feature not supported
	aInfo = 0;
	return 0;
	}

/**
 * Closes this registry instance.
 * @publishedPartner
 * @deprecated
 */
EXPORT_C void RFeatureRegistry::Close()
	{
	TUint8* propertyBuf = reinterpret_cast<TUint8*>(iImpl);
	delete[] propertyBuf;
	iImpl = NULL;
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
	RFeatureRegistry featReg;
	TInt result = featReg.Open();
	if (result == KErrNone)
		{
		result = featReg.QuerySupport(aFeatureUid, aInfo);
		featReg.Close();
		}
	return result;
	}

/**
 * Implementation class allocated when RFeatureRegistryNotify is opened.
 *
 * @internalComponent
 */
class RFeatureRegistryNotify::TImpl
	{
public:
	RProperty iNotifyProperty;

	TImpl()
		: iNotifyProperty()
		{
		}
	};

/**
 * Open instance of notify object so it can be subscribed to.
 *
 * @return KErrNone if successful, negative system-wide error code if not
 * @internalComponent
 */
EXPORT_C TInt RFeatureRegistryNotify::Open()
	{
	iImpl = new TImpl;
	if (iImpl == NULL)
		{
		return KErrNoMemory;
		}
	TInt result = iImpl->iNotifyProperty.Attach(KFeaturePropCat, KFeaturePropKey);
	if (result != KErrNone)
		{
		// must clean up memory allocated above
		delete iImpl;
		iImpl = NULL;
		return result;
		}
	// feature property and notify property are same in current implementation
	// hence must ensure feature property is already published to avoid false
	// notification when it is first published (just-in-time by the next query)
	TFeatureHeader header;
	TPckg<TFeatureHeader> headerPckg(header);
	result = iImpl->iNotifyProperty.Get(headerPckg);
	if (!((result == KErrOverflow)
		|| ((result == KErrNone) && (headerPckg.Size() >= sizeof(TFeatureHeader)))))
		{
		RunFeaturePropertySetupExe();
		}
	// return fact that Attach() succeeded
	return KErrNone;
	}

/**
 * Issues an asynchronous request to be notified the next time the support
 * status of any features change.
 *
 * To ensure that changes are not missed, always re-subscribe before
 * querying the feature registry.
 *
 * If an outstanding request is cancelled through a call to Cancel(), then it
 * completes with KErrCancel.
 *
 * @pre this instance of notify object must be Open and not already Subscribed to.
 * @param aNotifyStatus The request status object to be signalled on update.
 * @panic FeatReg EFeatRegInvalidUse if this registry notify instance is not open
 * @internalComponent
 */
EXPORT_C void RFeatureRegistryNotify::Subscribe(TRequestStatus &aNotifyStatus)
	{
	__ASSERT_ALWAYS(iImpl != NULL, Panic(EFeatRegInvalidUse));
	iImpl->iNotifyProperty.Subscribe(aNotifyStatus);
	}

/**
 * Cancels an outstanding subscription request for notification of feature registry changes.
 *
 * If the request has not already completed, then it completes with KErrCancel.
 *
 * @pre this instance of notify object must be Open
 * @panic FeatReg EFeatRegInvalidUse if this registry notify instance is not open
 * @internalComponent
 */
EXPORT_C void RFeatureRegistryNotify::Cancel()
	{
	__ASSERT_ALWAYS(iImpl != NULL, Panic(EFeatRegInvalidUse));
	iImpl->iNotifyProperty.Cancel();
	}

/**
 * Closes the registry notify instance.
 *
 * Note: automatically cancels any outstanding notify subscription.
 *
 * @internalComponent
 */
EXPORT_C void RFeatureRegistryNotify::Close()
	{
	if (iImpl)
		{
		// Have checked RProperty::Close() cancels the outstanding subscription
		iImpl->iNotifyProperty.Close();
		}
	delete iImpl;
	iImpl = NULL;
	}
