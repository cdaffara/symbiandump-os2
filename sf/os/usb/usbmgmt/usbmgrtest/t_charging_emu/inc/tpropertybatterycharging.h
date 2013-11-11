/**
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
* Usb Battery Charging test code to write to the Repository
* 
*
*/



#include <e32base.h>
#include "activepropertysubscribercharging.h"

class CRepository;
class CActivePropertySubscriber;


NONSHARABLE_CLASS(CPropertyBatteryCharging) : public CBase
											 ,public MPropertyWriteRepositoryObserver
											 ,public MPropertyReadBatteryChargingCurrentObserver
	{
public:
	static CPropertyBatteryCharging* NewL();
	~CPropertyBatteryCharging();

private:
	CPropertyBatteryCharging();
	void ConstructL();
	void DefinePropertiesL();
	
	void DefinePropertyL(const TInt32 aCategory, TUint aKey,RProperty::TType eType);

private:
	// from MPropertyWriteRepositoryObserver
	void MpsoPropertyWriteChanged(const TInt aValue);
	
	// from MPropertyReadBatteryChargingCurrentObserver
	void MpsoPropertyReadChanged();
	
	
	

private: // owned
	CActivePropertyWriteRepository* iActiveWriteRepository;
	CActivePropertyReadChargingCurrent* iActiveReadChargingCurrent;
	CRepository* iRepository;
	
	RProperty iProperty; // only one protected needed ?
	
	
	};
