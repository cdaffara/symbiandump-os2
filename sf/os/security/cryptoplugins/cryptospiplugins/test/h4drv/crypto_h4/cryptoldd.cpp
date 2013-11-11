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
#include "cryptoldd.h"
#include "keyhandle.h"
#include "kmskext.h"

DECLARE_STANDARD_LDD()
    {
    return new DCryptoLddChannelFactory;
    }

//#define HW_PERF_CHECK

_LIT(KCryptoPanicCategory,"DCrypto");

/**
  Constructor
*/
DCryptoLddChannelFactory::DCryptoLddChannelFactory()
	{
	TRACE_FUNCTION("DCryptoLddChannelFactory");
	// Set version number for this device
    iVersion=RCryptoDriver::VersionRequired();
	// Auto load a PDD
    iParseMask=KDeviceAllowPhysicalDevice;
	}

/**
  Destructor
*/
DCryptoLddChannelFactory::~DCryptoLddChannelFactory()
	{
	TRACE_FUNCTION("~DCryptoLddChannelFactory");
	TraceFunction::DumpCounts();
	}


/**
  Second stage constructor for DCryptoLddChannelFactory.
  This must at least set a name for the driver object.

  @return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DCryptoLddChannelFactory::Install()
	{
	TRACE_FUNCTION("Install");
    return SetName(&RCryptoDriver::Name());
	}

/**
  Return the drivers capabilities.
  Called in the response to an RDevice::GetCaps() request.

  @param aDes Descriptor into which capabilities information is to be written.
*/
void DCryptoLddChannelFactory::GetCaps(TDes8& aDes) const
	{
	TRACE_FUNCTION("GetCaps");
    // Create a capabilities object
    RCryptoDriver::TCaps caps;
	caps.iVersion = iVersion;

	// We do not have a handle to the PDD, and it might not have been
	// loaded yet, so we can't ask it for its capabilities...

    // Write it back to user memory
    Kern::InfoCopy(aDes,(TUint8*)&caps,sizeof(caps));
 	return;
	}

/**
  Called by the kernel's device driver framework to create a Logical Channel.
  This is called in the context of the user thread (client) which requested the creation
  of the Logical Channel (e.g. through a call to RBusLogicalChannel::DoCreate).
  The thread is in a critical section.

  @param aChannel Set to point to the created Logical Channel

  @return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DCryptoLddChannelFactory::Create(DLogicalChannelBase*& aChannel)
	{
	TRACE_FUNCTION("Create");
    aChannel=new DCryptoLddChannel;
    if(!aChannel)
        return KErrNoMemory;

    return KErrNone;
	}


//
// Logical Channel
//

/**
  Constructor
*/
DCryptoLddChannel::DCryptoLddChannel()
    : iLddChanRandom(*this),
	  iLddChanAes(*this)
	{
	TRACE_FUNCTION("DCryptoLddChannel");
    // Get pointer to client thread's DThread object
    iClient=&Kern::CurrentThread();

    // Open a reference on client thread so its control block can't disappear until
    // this driver has finished with it.
    // Note, this call to Open() can't fail since it is the thread we are currently running in.
    iClient->Open();
    }


/**
  Destructor
*/
DCryptoLddChannel::~DCryptoLddChannel()
    {
	TRACE_FUNCTION("~DCryptoLddChannel");
    // Cancel all processing that we may be doing
    DoCancel(RCryptoDriver::EAllRequests);
    // Close our reference on the client thread
    Kern::SafeClose((DObject*&)iClient,NULL);
    }


/**
  Second stage constructor called by the kernel's device driver framework.
  This is called in the context of the user thread (client) which requested the creation
  of the Logical Channel (e.g. through a call to RBusLogicalChannel::DoCreate())
  The thread is in a critical section.

  @param aUnit The unit argument supplied by the client to RBusLogicalChannel::DoCreate()
  @param aInfo The info argument supplied by the client to RBusLogicalChannel::DoCreate()
  @param aVer The version argument supplied by the client to RBusLogicalChannel::DoCreate()

  @return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DCryptoLddChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& aVer)
    {
	TRACE_FUNCTION("DoCreate");
    // Check Platform Security capabilities of client thread (if required).
    //
    // Here we handle the simple case where:
    // 1. The device driver can only have one client thread
    // 2. The security policy is the binary all-or-nothing policy.
    // E.g. "If you have the right capability you can do anything with the driver
    // and if you don't have the capability you can't do anything"
    // 
    // If only some functionality of the driver is restricted, then the security check should
    // go elsewhere. E.g. in DoRequest/DoControl. In that case Kern::CurrentThreadHasCapability
    // shouldn't be used because the 'current thread' isn't the client.
    //
    // In this example we do a check here for ECapability_None (which always passes)...
    if(!Kern::CurrentThreadHasCapability(ECapability_None,__PLATSEC_DIAGNOSTIC_STRING("Checked by DRIVER1")))
        return KErrPermissionDenied;
     
    // Check version
    if (!Kern::QueryVersionSupported(RCryptoDriver::VersionRequired(),aVer))
        return KErrNotSupported;

     // Give PDD a pointer to this channel
    PddChan()->iCryptoLddChannel = this;

    // Setup LDD for receiving client messages.
    SetDfcQ(PddChan()->DfcQue());
    iMsgQ.Receive();

    // Done
    return KErrNone;
    }


/**
  Called when a user thread requests a handle to this channel.
*/
TInt DCryptoLddChannel::RequestUserHandle(DThread* aThread, TOwnerType aType)
    {
	TRACE_FUNCTION("RequestUserHandle");
    // Make sure that only our client can get a handle
    if (aType!=EOwnerThread || aThread!=iClient)
        return KErrAccessDenied;
    return KErrNone;
    }


/**
  Process a message for this logical channel.
  This function is called in the context of a DFC thread.

  @param aMessage The message to process.
     The iValue member of this distinguishes the message type:
     iValue==ECloseMsg, channel close message
     iValue==KMaxTInt, a 'DoCancel' message
     iValue>=0, a 'DoControl' message with function number equal to iValue
     iValue<0, a 'DoRequest' message with function number equal to ~iValue
*/
void DCryptoLddChannel::HandleMsg(TMessageBase* aMsg)
    {
	TRACE_FUNCTION("HandleMsg");
    TThreadMessage& m=*(TThreadMessage*)aMsg;

    // Get message type
    TInt id=m.iValue;

    // Decode the message type and dispatch it to the relevent handler function...
    
    if (id==(TInt)ECloseMsg)
        {
        // Channel Close
        DoCancel(RCryptoDriver::EAllRequests);
        m.Complete(KErrNone, EFalse);
        return;
        }

    if (id==KMaxTInt)
        {
        // DoCancel
        DoCancel(m.Int0());
        m.Complete(KErrNone,ETrue);
        return;
        }

    if (id<0)
        {
        // DoRequest
        TRequestStatus* pS=(TRequestStatus*)m.Ptr0();
        TInt r=DoRequest(~id,pS,m.Ptr1(),m.Ptr2());
        if (r!=KErrNone)
            Kern::RequestComplete(iClient,pS,r);
        m.Complete(KErrNone,ETrue);
        }
    else
        {
        // DoControl
        TInt r=DoControl(id,m.Ptr0(),m.Ptr1());
        m.Complete(r,ETrue);
        }
    }

/**
  Process synchronous 'control' requests
*/
TInt DCryptoLddChannel::DoControl(TInt aFunction, TAny* a1, TAny *)
    {
	TRACE_FUNCTION("DoControl");
    TInt r;

    switch (aFunction)
        {
        case RCryptoDriver::EGetHwVersions:
            r = GetHwVersions((TDes8*)a1);
            break;

        case RCryptoDriver::EGetConfig:
            r = GetConfig((TDes8*)a1);
            break;

        case RCryptoDriver::ESetConfig:
            r = SetConfig((const TDesC8*)a1);
            break;

        case RCryptoDriver::EAesSetConfig:
            r = iLddChanAes.SetAesConfig((const TDesC8*)a1);
            break;

        default:
            r=KErrNotSupported;
            break;
        }

    return r;
    }


/**
  Process asynchronous requests
*/
TInt DCryptoLddChannel::DoRequest(TInt aReqNo, TRequestStatus* aStatus, TAny* a1, TAny* a2)
    {
	TRACE_FUNCTION("DoRequest");
    (void)a2; // a2 not used in this example

    TInt r;

    switch(aReqNo)
        {
        case RCryptoDriver::ERandom:
			r=iLddChanRandom.Random(aStatus,(TDes8*)a1);
            break;

        case RCryptoDriver::EAesWrite:
			r=iLddChanAes.AesWrite(aStatus,(TDesC8*)a1);
            break;

        case RCryptoDriver::EAesRead:
			r=iLddChanAes.AesRead(aStatus,(TDes8*)a1, (TUint32)a2);
            break;

        default:
            r=KErrNotSupported;
            break;
        }

    return r;
    }


/**
  Process cancelling of asynchronous requests
*/
void DCryptoLddChannel::DoCancel(TUint aMask)
    {
	TRACE_FUNCTION("DoCancel");
    if(aMask&(1<<RCryptoDriver::ERandom))
		{
		iLddChanRandom.RandomCancel();
		}
	
    if(aMask&(1<<RCryptoDriver::EAesRead))
		{
        iLddChanAes.CancelRead();
		}

    if(aMask&(1<<RCryptoDriver::EAesWrite))
		{
        iLddChanAes.CancelWrite();
		}
	
    }


/**
  Process a GetHwVersions control message. This writes the crypto h/w versions to a
  RCryptoDriver::THwVersionsBuf supplied by the client.
*/
TInt DCryptoLddChannel::GetHwVersions(TDes8* aHwVersionsBuf) const
    {
	TRACE_FUNCTION("GetHwVersions");
    // Create a structure giving the current configuration
    RCryptoDriver::THwVersions versions;
	
	PddChan()->GetHwVersions(versions);

    // Write the config to the client
    TPtrC8 ptr((const TUint8*)&versions,sizeof(versions));
    return Kern::ThreadDesWrite(iClient,aHwVersionsBuf,ptr,0,KTruncateToMaxLength,NULL);
    }

//
// Functions for processing configuration control messages
//

/**
  Process a GetConfig control message. This writes the current driver configuration to a
  RCryptoDriver::TConfigBuf supplied by the client.
*/
TInt DCryptoLddChannel::GetConfig(TDes8* aConfigBuf) const
    {
	TRACE_FUNCTION("GetConfig");
    // Create a structure giving the current configuration
    RCryptoDriver::TConfig config;
    CurrentConfig(config);

    // Write the config to the client
    TPtrC8 ptr((const TUint8*)&config,sizeof(config));
    return Kern::ThreadDesWrite(iClient,aConfigBuf,ptr,0,KTruncateToMaxLength,NULL);
    }

/**
  Process a SetConfig control message. This sets the driver configuration using a
  RCryptoDriver::TConfigBuf supplied by the client.
*/
TInt DCryptoLddChannel::SetConfig(const TDesC8* aConfigBuf)
    {
	TRACE_FUNCTION("SetConfig");
    // Don't allow configuration changes whilst we're busy
	//    if(iSendDataStatus || iReceiveDataStatus)
	//        return KErrInUse;

    // Create a config structure.
    RCryptoDriver::TConfig config;
    CurrentConfig(config);

    // Note: We have filled config with the current settings, this is to allow
    // backwards compatibility when a client gives us an old (and shorter) version
    // of the config structure.

    // Read the config structure from client
    TPtr8 ptr((TUint8*)&config,sizeof(config));
    TInt r=Kern::ThreadDesRead(iClient,aConfigBuf,ptr,0);
    if(r!=KErrNone)
        return r;

    // Use config data to setup the driver. Checking that parameters which aren't settable
    // either contain the correct values or are zero (meaning 'default')
	r=PddChan()->SetFakeDriverSetting(config.iFakeDriverSetting);
    if(r!=KErrNone)
        return r;

    return r;
    }


/**
  Fill a TConfig with the driver's current configuration.
*/
void DCryptoLddChannel::CurrentConfig(RCryptoDriver::TConfig& aConfig) const
    {
	TRACE_FUNCTION("TConfig");
    aConfig.iFakeDriverSetting = PddChan()->FakeDriverSetting();
    }



//
// Functions for processing 'Random'
//

DLddChanRandom::DLddChanRandom(DCryptoLddChannel &aParent)
	: iParent(aParent)
	{
	TRACE_FUNCTION("DLddChanRandom");
	}


/**
  Start processing a Random request.
*/
TInt DLddChanRandom::Random(TRequestStatus* aStatus,TDes8* aPtr)
    {
	TRACE_FUNCTION("Random");
    // Check that a 'Random' isn't already in progress
    if(iRandomStatus)
        {
        Kern::ThreadKill(iParent.iClient,EExitPanic,ERequestAlreadyPending,KCryptoPanicCategory);
        return KErrInUse;
        }

	// We only support a single outstanding Random request in this LDD
	// channel, but the PDD supports multiple requests.

    // Save the client request status and descriptor
    iRandomStatus = aStatus;
    iRandomDescriptor = aPtr;

	// Retrieve user request length (ie. CURRENT length of user descriptor) into iRequestLength
	TInt tmp;
	TUint8 *tmp2;
	TInt r=Kern::ThreadGetDesInfo(iParent.iClient, iRandomDescriptor, iRequestLength, tmp, tmp2, ETrue);
	if(r != KErrNone)
		{
		return r;
 		}

	// Set current offset into user descriptor
	iCurrentIndex = 0;

	iJob = iParent.PddChan()->GetJobRandom();

	// Setup PDD job
	iJob->SetDetails(&iParent.LddFactory()->iJSRandom, this, iRequestLength);
	// Register it, which will also (maybe later) call DoSlice to run it
	iParent.LddFactory()->iJSRandom.ScheduleJob(iJob);
		
	return KErrNone;
    }



TInt DLddChanRandom::DataRequired()
	{
	TRACE_FUNCTION("DataRequired");
	return KErrNone; // We never pass data to the h/w...
	}

TInt DLddChanRandom::DataAvailable()
	{
	TRACE_FUNCTION("DataAvailable");
	//	Kern::Printf("DLddChanRandom::DataAvailable()");
	TInt r = KErrNone;
	
	TUint8 *buf;
	TUint32 bufLen;
	TBool check = ETrue;
	while(check)
		{
		iJob->GetFromPddBuffer(buf, bufLen, check);
		if(bufLen)
			{
			TInt required = iRequestLength - iCurrentIndex;
			TInt toTransfer = bufLen;
			if(toTransfer > required)
				{
				toTransfer = required;
				}
			// Copy the current buffer to user land
			TPtr8 des(buf, toTransfer, toTransfer);
			r=Kern::ThreadDesWrite(iParent.iClient, iRandomDescriptor, des, iCurrentIndex);
			if(r != KErrNone)
				{
				break;
				}
			
			// Update our index into the user descriptor
			iCurrentIndex += toTransfer;

			// Update h/w with number of bytes actually read
			iJob->BytesReadFromPdd(toTransfer);

			if(toTransfer != bufLen)
				{
				// We did not read all the available data, so do not
				// re-query h/w
				check = EFalse;
				}
			}
		};

	return r;
	}

void DLddChanRandom::JobComplete(TInt aResult)
	{
	TRACE_FUNCTION("JobComplete");
	if(iRandomStatus)
		{
		// Finished with client descriptor, so NULL it to help detect coding errors
		iRandomDescriptor = NULL;
		// Complete client's request
		Kern::RequestComplete(iParent.iClient,iRandomStatus, aResult);
		}
	}


/**
  Cancel a Random request.
*/
void DLddChanRandom::RandomCancel()
    {
	TRACE_FUNCTION("RandomCancel");
    if(iRandomStatus)
        {
        // Tell PDD to stop processing the request
        iParent.LddFactory()->iJSRandom.DeScheduleJob(iParent.PddChan()->GetJobRandom());
        // Finished with client descriptor, so NULL it to help detect coding errors
        iRandomDescriptor = NULL;
        // Complete clients request
        Kern::RequestComplete(iParent.iClient,iRandomStatus,KErrCancel);
        }
    }

//
// Functions for processing 'Aes'
//

DLddChanAes::DLddChanAes(DCryptoLddChannel &aParent)
	: iParent(aParent)
	{
	TRACE_FUNCTION("DLddChanAes");
	}


TInt DLddChanAes::SetAesConfig(const TDesC8* aConfigBuf)
	{
	TRACE_FUNCTION("SetAesConfig");
	// Note we need to validate arguments kernel side otherwise
	// someone could easily crash the kernel...

	RCryptoDriver::TAesConfig config;
    // Read the config structure from client
    TPtr8 ptr((TUint8*)&config,sizeof(config));
    TInt r=Kern::ThreadDesRead(iParent.iClient,aConfigBuf,ptr,0);
    if(r!=KErrNone)
		{
		return r;
		}

	//	Kern::Printf("iEncrypt = 0x%x",config.iEncrypt);
	//	Kern::Printf("iMode = 0x%x",config.iMode);
	//	Kern::Printf("iKey = 0x%x",config.iKey);
	//	Kern::Printf("iIV = 0x%x",config.iIV);


	// Remember direction
	iEncrypt = config.iEncrypt;

	// Remember mode
	iMode = config.iMode;

	//
	// Handle key
	//

	// Retrieve user key length into iKeyLengthBytes
	TInt tmp;
	TUint8 *tmp2;
	r=Kern::ThreadGetDesInfo(iParent.iClient, (TAny *)config.iKey, iKeyLengthBytes, tmp, tmp2, EFalse);
    if(r!=KErrNone)
		{
		return r;
		}

	HBuf8 *embeddedKeyData = 0;
	if(iKeyLengthBytes == 4)
		{
		// Some form of embedded key so value is a 32 bit handle
		TPckgBuf<TKeyHandle> keyHandlePkg;	
		
		r=Kern::ThreadDesRead(iParent.iClient, (TAny *)config.iKey, keyHandlePkg, 0);
		if(r != KErrNone)
			{
			return r;
			}

		TKeyHandle &keyHandle = keyHandlePkg();

		r = HwKeyStore::ExtractKey(iParent.iClient->iOwningProcess, keyHandle, 0 /*operation*/, embeddedKeyData);
		if(r != KErrNone)
			{
			return r;
			}

		// Update key length
		iKeyLengthBytes = embeddedKeyData->Length();
		}

	switch(iKeyLengthBytes)
		{
		case 16: // 128 bits
			break;
		case 24: // 192 bits
			break;
		case 32: // 256 bits
			break;
		default: // Illegal length
			if(embeddedKeyData) delete embeddedKeyData;
			return KErrArgument;
		}

	// Make sure iJob is valid before we use it
	iJob = iParent.PddChan()->GetJobAes();

	TUint8 *keyBuffer = iJob->GetKeyBuffer();
	TPtr8 des(keyBuffer, iKeyLengthBytes);

	if(embeddedKeyData)
		{
		// Copy embedded key to PDD
		des = *embeddedKeyData;
		delete embeddedKeyData;
		embeddedKeyData = 0;
		}
	else
		{
		// Retrieve key from user
		r=Kern::ThreadDesRead(iParent.iClient, (TAny *)config.iKey, des, 0);
		if(r != KErrNone)
			{
			return r;
			}
		}
	
		
	//
	// Handle IV
	//

	// Retrieve IV length
	TInt ivLength;
	r=Kern::ThreadGetDesInfo(iParent.iClient, (TAny *)config.iIV, ivLength, tmp, tmp2, EFalse);
	//	Kern::Printf("DLddChanAes::SetAesConfig r=%d 5",r);
	if(r != KErrNone)
		{
		return r;
		}
	
	if((ivLength != 0) && (ivLength != 16))
		{
		return KErrArgument;
		}

	if(ivLength == 16)
		{
		TUint8 *ivBuffer = iJob->GetIVBuffer();
		TPtr8 des(ivBuffer, 16);
		r=Kern::ThreadDesRead(iParent.iClient, (TAny *)config.iIV, des, 0);
		if(r != KErrNone)
			{
			return r;
			}
		}


	// Set details
	r = iJob->SetDetails(&iParent.LddFactory()->iJSAes, this, iEncrypt, iKeyLengthBytes, iMode);
	if(r != KErrNone)
		{
		return r;
		}
	
	return KErrNone;
	}

/**
  Start processing a Aes request.
*/
TInt DLddChanAes::AesWrite(TRequestStatus* aStatus, TDesC8* aPtr)
    {
	TRACE_FUNCTION("AesWrite");
	//	Kern::Printf("DLddChanAes::AesWrite");
    // Check that an Aes Write isn't already in progress
    if(iAesWriteStatus)
        {
        Kern::ThreadKill(iParent.iClient,EExitPanic,ERequestAlreadyPending,KCryptoPanicCategory);
        return KErrInUse;
        }

	// We only support a single outstanding Aes Write request in this LDD
	// channel, but the PDD supports multiple requests.

    // Save the client request status and descriptor
    iAesWriteStatus = aStatus;
    iAesWriteDescriptor = aPtr;


	// Retrieve user request length (ie. current length of user descriptor) into iRequestLength
	TInt maxLen;
	TUint8 *aesWriteDescriptorBufferRaw;
	TInt r=Kern::ThreadGetDesInfo(iParent.iClient, iAesWriteDescriptor, iWriteRequestLength, maxLen, aesWriteDescriptorBufferRaw, EFalse);
	if(r != KErrNone)
		{
		iAesWriteStatus = 0;
		return r;
 		}

	// Set current offset into user descriptor
	iCurrentUserWriteIndex = 0;

	// Make sure iJob is valid before we use it
	iJob = iParent.PddChan()->GetJobAes();


#ifdef HW_PERF_CHECK
	iJob->HwPerfCheck();
#else
	// Feed some data to the PDD
	r = DataRequired(); // Fake PDD callback to get data from us.
	if(r != KErrNone)
		{
		iAesWriteStatus = 0;
		return r;
 		}

	// Make sure job is not stalled, and maybe (re)add to job list.
	iJob->Resume();
#endif
	
	return KErrNone;
    }


TInt DLddChanAes::AesRead(TRequestStatus* aStatus, TDes8* aPtr, TUint32 aLength)
    {
	TRACE_FUNCTION("AesRead");
	//	Kern::Printf("DLddChanAes::AesRead");
    // Check that an Aes Read isn't already in progress
    if(iAesReadStatus)
        {
        Kern::ThreadKill(iParent.iClient,EExitPanic,ERequestAlreadyPending,KCryptoPanicCategory);
        return KErrInUse;
        }

	// We only support a single outstanding Aes Read request in this LDD
	// channel, but the PDD supports multiple requests.

    // Save the client request status and descriptor
    iAesReadStatus = aStatus;
    iAesReadDescriptor = aPtr;
	iReadRequestLength = aLength;

	// Retrieve user request length (ie. current length of user descriptor) into iRequestLength
	//	TInt iOriginalUserReadDescLength;
	TUint8 *aesReadDescriptorBufferRaw;
	TInt maxLen;
	TInt r=Kern::ThreadGetDesInfo(iParent.iClient, iAesReadDescriptor, iOriginalUserReadDescLength, maxLen, aesReadDescriptorBufferRaw, ETrue);
	if(r != KErrNone)
		{
		iAesReadStatus = 0;
		return r;
 		}

	if(iReadRequestLength > (maxLen - iOriginalUserReadDescLength))
		{
		return KErrArgument;
		}

	// Set current offset into user descriptor
	iCurrentUserReadIndex = 0;

	// Tell PDD our request length so it can optimse returning the data to us.
	iJob->NotifyReadRequestLength(iReadRequestLength);
	

#ifndef HW_PERF_CHECK
	// Make sure iJob is valid before we use it
	iJob = iParent.PddChan()->GetJobAes();

	// Process any data already available
	r = DataAvailable(); // Fake PDD callback to read data from PDD
	if(r != KErrNone)
		{
		iAesReadStatus = 0;
		return r;
 		}

	// Make sure job is not stalled (and maybe re-add to job list) We
	// do this even if we completely satisified the read from the PDD
	// buffer because a write might be waiting for space in the
	// buffer...
	iJob->Resume();
#endif

	return KErrNone;
	}



TInt DLddChanAes::DataRequired()
	{
	TRACE_FUNCTION("DataRequired");
	//	Kern::Printf("DLddChanAes::DataRequired");
	if(!iAesWriteStatus)
		{
		return KErrNone;
		}
	
	TBool moreSpace = ETrue;
	TInt toWrite = (iWriteRequestLength - iCurrentUserWriteIndex);
	while(toWrite && moreSpace)
		{
		// Get details of PDD write buffer
		TUint8 *pddBuf; // always valid, though pddBufLen might be 0
		TUint32 pddBufLen; // maybe 0
		iJob->GetToPddBuffer(pddBuf, pddBufLen, moreSpace);

		if(toWrite > pddBufLen)
			{
			toWrite = pddBufLen;
			}

		// Make sure there is some data to write
		if(toWrite == 0)
			{
			break;
			}

		TPtr8 des(pddBuf, toWrite);
		TInt r=Kern::ThreadDesRead(iParent.iClient, iAesWriteDescriptor, des, iCurrentUserWriteIndex);
		//TInt r = 0;
		if(r!=KErrNone)
			return r;
		iCurrentUserWriteIndex += toWrite;
		// Tell the PDD how many bytes we wrote to it.
		// If we wrote data, and the job was not already queued, the
		// PDD will call ScheduleJob for it.
		iJob->BytesWrittenToPdd(toWrite);

		// Update count of bytes left to write
		toWrite = (iWriteRequestLength - iCurrentUserWriteIndex);
		};

	if((iWriteRequestLength - iCurrentUserWriteIndex) <= 0)
		{
		// Write request is complete
		// Finished with client descriptor, so NULL it to help detect coding errors
		iAesWriteDescriptor = NULL;
		// Complete client's request
		Kern::RequestComplete(iParent.iClient,iAesWriteStatus, KErrNone);
		return KErrNone;
		}

	return KErrNone;
	}

TInt DLddChanAes::DataAvailable()
	{
	TRACE_FUNCTION("DataAvailable");
	//	Kern::Printf("DLddChanAes::DataAvailable()");

	if(!iAesReadStatus)
		{
		return KErrNone;
		}

	TInt r = KErrNone;

	TUint8 *buf;
	TUint32 bufLen;
	TBool check = ETrue;
	while(check)
		{
		iJob->GetFromPddBuffer(buf, bufLen, check);
		if(bufLen)
			{
			TInt required = iReadRequestLength - iCurrentUserReadIndex;
			TInt toTransfer = bufLen;
			if(toTransfer > required)
				{
				toTransfer = required;
				}
			// Copy the current buffer to user land
			TPtr8 des(buf, toTransfer, toTransfer);
			r=Kern::ThreadDesWrite(iParent.iClient, iAesReadDescriptor, des, iOriginalUserReadDescLength + iCurrentUserReadIndex);
			//r  = 0;
			if(r != KErrNone)
				{
				break;
				}
			
			// Update our index into the user descriptor
			iCurrentUserReadIndex += toTransfer;

			// Update h/w with number of bytes actually read
			iJob->BytesReadFromPdd(toTransfer);

			if(toTransfer != bufLen)
				{
				// We did not read all the available data, so do not
				// re-query h/w
				check = EFalse;
				}
			}
		};

	if((iReadRequestLength - iCurrentUserReadIndex) <= 0)
		{
		// Read request is complete
		// Finished with client descriptor, so NULL it to help detect coding errors
		iAesReadDescriptor = NULL;
		// Complete client's request
		Kern::RequestComplete(iParent.iClient,iAesReadStatus, KErrNone);
		return KErrNone;
		}

	return KErrNone;
	}





/**
  Called by PDD from a DFC to indicate that a Aes operation has completed.
*/
void DLddChanAes::JobComplete(TInt aResult)
	{
	TRACE_FUNCTION("JobComplete");
	// Normally not used for AES, instead the job keeps running and
	// DataAvailable/DataRequired complete the AesRead/AesWrite
	// requests.
	// 
	// Will be called if xfer to/from user space fails (or if another
	// fatal error occurs).
    if(iAesReadStatus)
        {
        // Finished with client descriptor, so NULL it to help detect coding errors
        iAesReadDescriptor = NULL;
        // Complete clients request (nb following call set iAesReadStatus to 0)
        Kern::RequestComplete(iParent.iClient,iAesReadStatus, aResult);
        }
    if(iAesWriteStatus)
        {
        // Finished with client descriptor, so NULL it to help detect coding errors
        iAesWriteDescriptor = NULL;
        // Complete clients request (nb following call set iAesWriteStatus to 0)
        Kern::RequestComplete(iParent.iClient,iAesWriteStatus, aResult);
        }
	}



/**
  Cancel a Aes Read request.
*/
void DLddChanAes::CancelRead()
    {
	TRACE_FUNCTION("CancelRead");
    if(iAesReadStatus)
        {
        // Finished with client descriptor, so NULL it to help detect coding errors
        iAesReadDescriptor = NULL;
        // Complete clients request (nb following call set iAesReadStatus to 0)
        Kern::RequestComplete(iParent.iClient,iAesReadStatus,KErrCancel);
        }
    }

/**
  Cancel a Aes Write request.
*/
void DLddChanAes::CancelWrite()
    {
	TRACE_FUNCTION("CancelWrite");
    if(iAesWriteStatus)
        {
        // Finished with client descriptor, so NULL it to help detect coding errors
        iAesWriteDescriptor = NULL;
        // Complete clients request (nb following call set iAesWriteStatus to 0)
        Kern::RequestComplete(iParent.iClient,iAesWriteStatus,KErrCancel);
        }
    }

// End of file
