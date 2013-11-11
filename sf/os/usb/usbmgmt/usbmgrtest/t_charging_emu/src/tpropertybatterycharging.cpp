/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tpropertybatterycharging.h"

#include <e32property.h> //Publish & Subscribe header

#include <centralrepository.h>
#include "usbbatterycharging.h"
#include"tbatterychargingdefinitions.h"


LOCAL_C void ConsoleMainL();

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	
	TRAPD(error, ConsoleMainL() );
	
	__ASSERT_ALWAYS(!error,User::Panic(_L("TPropertyBatteryCharging"), error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return KErrNone;
	}

void ConsoleMainL()
	{
    CActiveScheduler* myScheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(myScheduler);
    CActiveScheduler::Install(myScheduler);	
    
	CPropertyBatteryCharging* propertyBatCharg = CPropertyBatteryCharging::NewL();
	CleanupStack::PushL(propertyBatCharg);
	
	RProcess::Rendezvous(KErrNone);
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, myScheduler);
	}

CPropertyBatteryCharging* CPropertyBatteryCharging::NewL()
/**
 * Constructs a CDummyUsbDevice object.
 *
 * @return	A new CDummyUsbDevice object
 */
	{
	CPropertyBatteryCharging* r = new (ELeave) CPropertyBatteryCharging();
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CPropertyBatteryCharging::~CPropertyBatteryCharging()
/**
 * Destructor.
 */
	{
	
	delete iActiveWriteRepository;
	delete iActiveReadChargingCurrent;
	delete iRepository;
	iProperty.Close();
	
	}

CPropertyBatteryCharging::CPropertyBatteryCharging() 
/**
 * Constructor.
 */
	{
	
	}

void CPropertyBatteryCharging::ConstructL()
/**
 * Performs 2nd phase construction of the USB device.
 */
	{	
	
	iActiveWriteRepository = CActivePropertyWriteRepository::NewL(*this);
	iActiveReadChargingCurrent = CActivePropertyReadChargingCurrent::NewL(*this);	
		
	iRepository = CRepository::NewL(KUsbBatteryChargingCentralRepositoryUid);
	
	DefinePropertyL(KBattChargWriteRepositoryUid, KBattChargWriteRepositoryAckKey,RProperty::EInt);
	DefinePropertyL(KBattChargReadPropertyCurrentUid,KBattChargReadCurrentChargingAckKey,RProperty::EByteArray);
	
	iActiveWriteRepository->Request();
	iActiveReadChargingCurrent->Request();
	
	}
	
void CPropertyBatteryCharging::DefinePropertyL(const TInt32 aCategory, TUint aKey,RProperty::TType eType)
	{
	

	_LIT_SECURITY_POLICY_PASS(KAlwaysPass);

	TInt err = iProperty.Define(TUid::Uid(aCategory),
									aKey,
									eType,
									KAlwaysPass,
									KAlwaysPass
									);
	if ( err != KErrAlreadyExists )
		{
		User::LeaveIfError(err);
		}
	}

	
void CPropertyBatteryCharging::MpsoPropertyReadChanged()
	{
		
	TInt current;
	TInt err = RProperty::Get(KPropertyUidUsbBatteryChargingCategory,
				KPropertyUidUsbBatteryChargingChargingCurrent, current);
		
	TDataFromPropBattChargToTBatteryCharging toSend;
	toSend.iCurrent=current;
	toSend.iError=err;
		
	TPckgBuf<TDataFromPropBattChargToTBatteryCharging> tmp(toSend);
	
	err = iProperty.Set(TUid::Uid(KBattChargReadPropertyCurrentUid),
									KBattChargReadCurrentChargingAckKey,
									tmp);
	ASSERT(!err);	
					
	}
	
void CPropertyBatteryCharging::MpsoPropertyWriteChanged(const TInt aValue)
	{
	TInt err = iRepository->Set(KUsbBatteryChargingKeyEnabledUserSetting, aValue);
	
	TInt error = iProperty.Set(TUid::Uid(KBattChargWriteRepositoryUid),
						KBattChargWriteRepositoryAckKey,
						err);
	ASSERT(!error);	
	
	}
	


