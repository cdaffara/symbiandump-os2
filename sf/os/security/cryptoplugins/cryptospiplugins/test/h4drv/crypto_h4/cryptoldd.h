/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file
 @internalComponent
 @released
*/
#ifndef __CRYPTOLDD__
#define __CRYPTOLDD__
#include <e32cmn.h>
#include <e32ver.h>
#include <e32def.h>
#include "cryptojobs.h"


class DCryptoLddChannelFactory : public DLogicalDevice
	{
public:
	DCryptoLddChannelFactory();
	~DCryptoLddChannelFactory();
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);

	// The job scheduler for the Random h/w
	DCryptoJobScheduler iJSRandom;

	// The job scheduler for the AES h/w
	DCryptoJobScheduler iJSAes;
	};

class DCryptoPddChannel;
/**
  Logical Channel classes for 'Crypto'
*/

/**
   DLddChanRandom

   This class just exists to keep all the LDD Chan variables for
   dealing with Random requests in one place.
*/
class DCryptoLddChannel;
class DLddChanRandom : public DBase, public MCryptoJobCallbacks
	{
public:
	DLddChanRandom(DCryptoLddChannel &aParent);
	// Cleanup must be done by DCryptoLddChannel destructor because
	// by the time our destructor gets called the DCryptoLddChannel
	// has already been destroyed.

	// Functions are virtual so PDD can call them via a vtable instead
	// of linking directly to them.

    virtual TInt Random(TRequestStatus* aStatus,TDes8* aBuffer);

	// Callbacks for PDD
	virtual TInt DataRequired();
	virtual TInt DataAvailable();
	virtual void JobComplete(TInt aResult);
	
	/**
	   Cancel a user request
	*/
	virtual void RandomCancel();
private:
	DCryptoLddChannel &iParent;
    // Members used for processing a Random request
	
    TRequestStatus* iRandomStatus;	// User request
    TDes8* iRandomDescriptor;		// User descriptor
	TInt iRequestLength;
	TInt iCurrentIndex;

	CryptoJobRandom *iJob; // Ptr to PDD Random job (not owned by us)
	};

/**
   DLddChanAes

   This class just exists to keep all the LDD Chan variables for
   dealing with Aes requests in one place.
*/
class DCryptoLddChannel;
class DLddChanAes : public DBase, public MCryptoJobCallbacks
	{
public:
	DLddChanAes(DCryptoLddChannel &aParent);
	// Cleanup must be done by DCryptoLddChannel destructor because
	// by the time our destructor gets called the DCryptoLddChannel
	// has already been destroyed.

	// Functions are virtual so PDD can call them via a vtable instead
	// of linking directly to them.

    virtual TInt SetAesConfig(const TDesC8 *aConfigBuf);
    virtual TInt AesWrite(TRequestStatus *aStatus, TDesC8 *aBuffer);
    virtual TInt AesRead(TRequestStatus *aStatus, TDes8 *aBuffer, TUint32 aLength);

	// Callbacks for PDD
	virtual TInt DataRequired();
	virtual TInt DataAvailable();
	virtual void JobComplete(TInt aResult);

	/**
	   Cancel a user request
	*/
	virtual void CancelRead();
	virtual void CancelWrite();
private:
	DCryptoLddChannel &iParent;
    // Members used for processing a Aes request
	
    TRequestStatus *iAesWriteStatus;	// User request
    TDesC8 *iAesWriteDescriptor;		// User descriptor
	TInt iWriteRequestLength;
	TInt iCurrentUserWriteIndex;

    TRequestStatus *iAesReadStatus;	// User request
    TDes8 *iAesReadDescriptor;		// User descriptor
	TInt iReadRequestLength;
	TInt iOriginalUserReadDescLength;
	TInt iCurrentUserReadIndex;

	TBool iEncrypt;
	RCryptoDriver::TChainingMode iMode;
	TInt iKeyLengthBytes;

	CryptoJobAes *iJob; // Ptr to PDD AES job (not owned by us)
	};


class DCryptoLddChannel : public DLogicalChannel
    {
public:
    DCryptoLddChannel();
    virtual ~DCryptoLddChannel();
    // Inherited from DObject
    virtual TInt RequestUserHandle(DThread* aThread, TOwnerType aType);
    // Inherited from DLogicalChannelBase
    virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
    // Inherited from DLogicalChannel
    virtual void HandleMsg(TMessageBase* aMsg);

    // Accessor for the LDD Factory
	inline DCryptoLddChannelFactory *LddFactory() const;
	
	DLddChanRandom iLddChanRandom;
	DLddChanAes iLddChanAes;
	
private:
	friend class DLddChanRandom;
	friend class DLddChanAes;
	
    // Panic reasons
    enum TPanic
        {
        ERequestAlreadyPending = 1
        };
    // Implementation for the differnt kinds of requests send through RBusLogicalChannel
    TInt DoControl(TInt aFunction, TAny* a1, TAny* a2);
    TInt DoRequest(TInt aReqNo, TRequestStatus* aStatus, TAny* a1, TAny* a2);
    void DoCancel(TUint aMask);

    // Accessor for the PDD
	inline DCryptoPddChannel* PddChan() const;

    // Query h/w versions
    TInt GetHwVersions(TDes8* aHwVersionsBuf) const;

    // Methods for configuration
    TInt GetConfig(TDes8* aConfigBuf) const;
    TInt SetConfig(const TDesC8* aConfigBuf);
	void CurrentConfig(RCryptoDriver::TConfig& aConfig) const;

private:
    DThread* iClient;
    };
inline DCryptoLddChannelFactory *DCryptoLddChannel::LddFactory() const
	{
	return static_cast<DCryptoLddChannelFactory *>(iDevice);
	}


class DCryptoPddChannel : public DBase
    {
public:
    virtual TDfcQue* DfcQue() = 0;

    virtual void GetHwVersions(RCryptoDriver::THwVersions& aHwVersions) const = 0;
    virtual TInt FakeDriverSetting() const = 0;
    virtual TInt SetFakeDriverSetting(TInt aFakeDriverSetting) = 0;

	virtual CryptoJobRandom *GetJobRandom(TBool aAutoCreate = ETrue) = 0;
	virtual CryptoJobAes *GetJobAes(TBool aAutoCreate = ETrue) = 0;

	// The LDD chan needs to be able to set this, and the Job
	// implementation classes need to be able to read it, so there is
	// no point in having accessor functions for it...
	DCryptoLddChannel *iCryptoLddChannel;
    };






inline DCryptoPddChannel* DCryptoLddChannel::PddChan() const
    {
	return static_cast<DCryptoPddChannel *>(iPdd);
	}




#endif
