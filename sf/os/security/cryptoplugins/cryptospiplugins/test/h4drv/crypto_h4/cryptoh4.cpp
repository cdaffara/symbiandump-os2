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
#include <kernel/kern_priv.h>
#include "cryptodriver.h"
#ifdef __MARM__
#include <omap_hrp/assp/shared/omap_reg.h>
#include <omap_hrp/assp/shared/omap_interrupt.h>
#endif
#include "cryptoh4.h"
#include "cryptoh4rng.h"
#include "cryptoh4aes.h"

DECLARE_STANDARD_PDD()
    {
    return new DCryptoPddFactory;
    }

// Name for PDD, must match LDD name with a '.' and distinguishing name appended
_LIT(KCryptoH4PddName,"crypto.h4");

DCryptoPddFactory::DCryptoPddFactory()
	// : iDfcQueInitialised(EFalse)
    {
	TRACE_FUNCTION("DCryptoPddFactory");
	// Set version number for this device
	//
	// We re-use the version number of the LDD which effectively
	// specifies the version of both the external RCryptoDriver
	// interface to the LDD and the internal LDD to PDD interface.
    iVersion=RCryptoDriver::VersionRequired();
    }

void DCryptoPddFactory::DeleteDfcQFunc(TAny *p)
	{
	TRACE_FUNCTION("DeleteDfcQFunc");
	// We do not need to delete the TDfc because it is a static
	// Delete the DFC queue.
	TDfcQue *q = reinterpret_cast<TDfcQue *>(p);
	delete q;
	// Delete our thread
	Kern::Exit(KErrNone);
	}

TDfc deleteDfcQDfc(0, 0, 0); // Dummy create, will be overwritten later

DCryptoPddFactory::~DCryptoPddFactory()
	{
	TRACE_FUNCTION("~DCryptoPddFactory");
	if(iDfcQueInitialised)
		{
		// Need to shutdown our DFC queue.
		// 
		// In theory the DFC could run after we have returned and the
		// framework has deleting everything...
		// 
		// But the DLL will NOT be unloaded until the next time the
		// IDLE task runs, so we know the DFC will reach completion
		// before then (but it most not use any of our members....
		//
		// For the same reason we should use a TDfc which is not
		// inside this object.
		deleteDfcQDfc = TDfc(DeleteDfcQFunc, iDfcQue, iDfcQue, 0);
		deleteDfcQDfc.Enque();
		iDfcQueInitialised = EFalse;
		}
	}




// Missing h/w registers & masks
const TUint KHtRngSysResetStatus = KBit0;

/**
  Second stage constructor for DPhysicalDevice derived objects.
  This must at least set a name for the driver object.

  @return KErrNone or standard error code.
*/
_LIT(KCryptoHwQueueName, "CryptoHwDfcQue");
TInt DCryptoPddFactory::Install()
    {
	TRACE_FUNCTION("Install");
	// 28 is the recommended setting for a low priority kernel driver
	// (I can't find an enum for this).
	TInt r = Kern::DfcQCreate(iDfcQue, 28, &KCryptoHwQueueName);
	if(r != KErrNone)
		{
		return r;
		}
	iDfcQueInitialised = ETrue;

	// Reset h/w
	Kern::Printf("DCryptoPddFactory::Install - Reseting h/w");
#ifdef __MARM__
	// RNG
	// The RNG has no state worth reseting - a reset would delay the
	// availabilty of a new random number (and maybe reduce its
	// quality).
	//	TOmap::SetRegister32(KHwBaseRngReg + KHoRng_Mask, KHtRngMaskSoftReset);

	// DES/3DES
	TOmap::SetRegister32(KHwBaseDes3DesReg + KHoDES_MASK, KHtDesMaskSoftReset);

	// SHA1/MD5
	TOmap::SetRegister32(KHwBaseSha1Md5Reg + KHoSHA_MASK, KHtShaMaskSoftReset);

	// AES
	TOmap::SetRegister32(KHwBaseAesReg + KHoAES_MASK, KHtAesMaskSoftReset);

	// PKA
	TOmap::SetRegister32(KHwBasePkaReg + KHoPKA_MASK, KHtPkaMaskSoftReset);
	
	// Make sure all cypto h/w (rng/des/sha1/aes/pka) is out of reset
	while(((TOmap::Register32(KHwBaseRngReg + KHoRng_SysStatus) & KHtRngSysResetStatus) == 0) ||
		  ((TOmap::Register32(KHwBaseDes3DesReg + KHoDES_SYSSTATUS) & KHtDesSysResetStatus) == 0) ||
		  ((TOmap::Register32(KHwBaseSha1Md5Reg + KHoSHA_SYSSTATUS) & KHtShaSysResetStatus) == 0) ||
		  ((TOmap::Register32(KHwBaseAesReg + KHoAES_SYSSTATUS) & KHtAesSysResetStatus) == 0) ||
		  ((TOmap::Register32(KHwBasePkaReg + KHoPKA_SYSSTATUS) & KHtPkaSysResetStatus) == 0))
		{
		Kern::Printf("DCryptoPddFactory::Install - waiting for h/w");
		}

#endif
	//	Kern::Printf("DCryptoPddFactory::Install - h/w reset complete");

    return SetName(&KCryptoH4PddName);
    }




/**
   Returns the drivers capabilities.

   This is required by the Symbian OS device driver framework.
   
   It can NOT be used by the LDD factory GetCaps function because that
   class does not contain a link tous (and we might not be loaded)....

   NOT USED IN THIS DRIVER.
   
   @param aDes Descriptor to write capabilities information into
*/
void DCryptoPddFactory::GetCaps(TDes8& /* aDes */) const
	{
	TRACE_FUNCTION("GetCaps");
	}


/**
  Called by the kernel's device driver framework to create a Physical Channel.
  This is called in the context of the user thread (client) which requested the creation of a Logical Channel
  (E.g. through a call to RBusLogicalChannel::DoCreate)
  The thread is in a critical section.

  @param aChannel Set to point to the created Physical Channel
  @param aUnit The unit argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aInfo The info argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aVer The version number of the Logical Channel which will use this Physical Channel 

  @return KErrNone or standard error code.
*/
TInt DCryptoPddFactory::Create(DBase*& aChannel, TInt aUnit, const TDesC8* aInfo, const TVersion& aVer)
    {
	TRACE_FUNCTION("Create");
    // Ignore the parameters we aren't interested in...
    (void)aUnit;
    (void)aInfo;
    (void)aVer;

    // Create a new physical channel
DCryptoH4Chan* device=new DCryptoH4Chan(*this);
    aChannel=device;
    if (!device)
        return KErrNoMemory;
    return device->DoCreate();
    }

/**
  Called by the kernel's device driver framework to check if this PDD is suitable for use with a Logical Channel.
  This is called in the context of the user thread (client) which requested the creation of a Logical Channel
  (E.g. through a call to RBusLogicalChannel::DoCreate)
  The thread is in a critical section.

  @param aUnit The unit argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aInfo The info argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aVer The version number of the Logical Channel which will use this Physical Channel 

  @return KErrNone or standard error code.
*/
TInt DCryptoPddFactory::Validate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer)
    {
	TRACE_FUNCTION("Validate");
    // Check version numbers
    if ((!Kern::QueryVersionSupported(iVersion,aVer)) || (!Kern::QueryVersionSupported(aVer,TVersion(EMinimumLddMajorVersion,EMinimumLddMinorVersion,EMinimumLddBuild))))
        return KErrNotSupported;

    // We don't support units
    if (aUnit != -1)
        return KErrNotSupported;

    // Ignore extra info, (this could be used for validation purposes)
    // Note, aInfo is a pointer to a descriptor in user memory, therefore safe methods should
    // be used for reading its contents. E.g. using Kern::KUDesGet()
    (void)aInfo;

    // OK
    return KErrNone;
    }


TDfcQue *DCryptoPddFactory::DfcQue()
	{
	TRACE_FUNCTION("DfcQue");
	return iDfcQue;
	}



//
// DCryptoH4Chan
//

DCryptoH4Chan::DCryptoH4Chan(DCryptoPddFactory &iCryptoPddFactory)
    : iCryptoPddFactory(iCryptoPddFactory),
	  iFakeDriverSetting(100000)
    {
	TRACE_FUNCTION("DCryptoH4Chan");
    }

DCryptoH4Chan::~DCryptoH4Chan()
    {
	TRACE_FUNCTION("~DCryptoH4Chan");
	if(iCryptoJobRandom)
		{
		// Make sure we are not queued on a job scheduler.
		iCryptoJobRandom->DeScheduleJob();
		delete iCryptoJobRandom;
		}
	if(iCryptoJobAes)
		{
		// Make sure we are not queued on a job scheduler.
		iCryptoJobAes->DeScheduleJob();
		delete iCryptoJobAes;
		}
    }


TInt DCryptoH4Chan::DoCreate()
    {
	TRACE_FUNCTION("DoCreate");


    return KErrNone;
    }

TDfcQue* DCryptoH4Chan::DfcQue()
    {
	TRACE_FUNCTION("DfcQue");
    return iCryptoPddFactory.DfcQue();
    }

void DCryptoH4Chan::GetHwVersions(RCryptoDriver::THwVersions &aHwVersions) const
	{
	TRACE_FUNCTION("THwVersions");
#ifdef __MARM__
	aHwVersions.iRngHwVersion = TOmap::Register32(KHwBaseRngReg + KHoRng_Rev);
	aHwVersions.iDes3DesHwVersion = TOmap::Register32(KHwBaseDes3DesReg + KHoDES_REV);
	aHwVersions.iSha1Md5HwVersion = TOmap::Register32(KHwBaseSha1Md5Reg + KHoSHA_REV);
	aHwVersions.iAesHwVersion = TOmap::Register32(KHwBaseAesReg + KHoAES_REV);
	aHwVersions.iPkaHwVersion = TOmap::Register32(KHwBasePkaReg + KHoPKA_REV);
	TOmap::SetRegister32(KHwBasePkaReg + KHoPKA_REV,42);
	aHwVersions.iPkaHwVersion = TOmap::Register32(KHwBasePkaReg + KHoPKA_REV);
#else
	// Return fake hardware versions for testing
	aHwVersions.iRngHwVersion = 42;
	aHwVersions.iDes3DesHwVersion = 43;
	aHwVersions.iSha1Md5HwVersion = 44;
	aHwVersions.iAesHwVersion = 45;
	aHwVersions.iPkaHwVersion = 46;
#endif
    }

TInt DCryptoH4Chan::FakeDriverSetting() const
    {
	TRACE_FUNCTION("FakeDriverSetting");
    return iFakeDriverSetting;
    }

TInt DCryptoH4Chan::SetFakeDriverSetting(TInt aFakeDriverSetting)
    {
	TRACE_FUNCTION("SetFakeDriverSetting");
    if(aFakeDriverSetting<=0)
        return KErrArgument;
    iFakeDriverSetting = aFakeDriverSetting;
    return KErrNone;
    }

CryptoJobRandom *DCryptoH4Chan::GetJobRandom(TBool aAutoCreate)
	{
	TRACE_FUNCTION("GetJobRandom");
	if(aAutoCreate && iCryptoJobRandom == 0)
		{
		iCryptoJobRandom = new CryptoH4JobRandom(iCryptoLddChannel->iLddChanRandom);
		iCryptoJobRandom->SetDfcQ(DfcQue());
		}
	return iCryptoJobRandom;
	}

CryptoJobAes *DCryptoH4Chan::GetJobAes(TBool aAutoCreate)
	{
	TRACE_FUNCTION("GetJobAes");
	if(aAutoCreate && iCryptoJobAes == 0)
		{
		iCryptoJobAes = new CryptoH4JobAes(iCryptoLddChannel->iLddChanAes);
		iCryptoJobAes->SetDfcQ(DfcQue());
		}
	return iCryptoJobAes;
	}


// End of file
