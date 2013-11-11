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
#ifndef __CRYPTOH4__
#define __CRYPTOH4__
#include "cryptoldd.h"

class DCryptoPddFactory : public DPhysicalDevice
    {
public:
    DCryptoPddFactory();
	~DCryptoPddFactory();
    // Inherited from DPhysicalDevice
    virtual TInt Install();
    // Inherited from DPhysicalDevice
    virtual void GetCaps(TDes8& aDes) const;
    // Inherited from DPhysicalDevice
    virtual TInt Create(DBase*& aChannel, TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);
    // Inherited from DPhysicalDevice
    virtual TInt Validate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);

	TDfcQue *DfcQue();
	static void DeleteDfcQFunc(TAny *any);

public:
    enum TMinimumLDDVersion
        {
        EMinimumLddMajorVersion=1,
        EMinimumLddMinorVersion=0,
        EMinimumLddBuild=0 //Not used
        };
private:
	TBool iDfcQueInitialised;
	TDfcQue *iDfcQue; // Will be deleted by the deleteDfcQDfc started by our destructor
    };



class CryptoJobRandom;
class CryptoJobAes;
class DCryptoH4Chan : public DCryptoPddChannel
	{
public:
	DCryptoH4Chan(DCryptoPddFactory &iCryptoPddFactory);
    ~DCryptoH4Chan();

    TInt DoCreate();

	virtual TDfcQue* DfcQue();

    virtual void GetHwVersions(RCryptoDriver::THwVersions& aHwVersions) const;

    virtual TInt FakeDriverSetting() const;
    virtual TInt SetFakeDriverSetting(TInt aFakeDriverSetting);

	virtual CryptoJobRandom *GetJobRandom(TBool aAutoCreate = ETrue);
	virtual CryptoJobAes *GetJobAes(TBool aAutoCreate = ETrue);

private:
	DCryptoPddFactory &iCryptoPddFactory;

    TInt iFakeDriverSetting;

	CryptoJobRandom *iCryptoJobRandom;
	CryptoJobAes *iCryptoJobAes;
	};




#endif
