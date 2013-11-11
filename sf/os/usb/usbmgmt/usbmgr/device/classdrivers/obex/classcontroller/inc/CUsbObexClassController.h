/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Adheres to the UsbMan USB Class API and manages the class.
* 
*
*/



/**
 @file
*/

#ifndef __CUSBOBEXCLASSCONTROLLER_H__
#define __CUSBOBEXCLASSCONTROLLER_H__

#include <e32std.h>
#include <cusbclasscontrollerplugin.h>
#include <d32usbc.h>

const TInt KObexClassPriority = 2;

const TInt KObexDescriptorLength = 18;
const TInt KObexMinNumEndpoints = 3;
const TInt KObexClassNumber = 0x02;
const TInt KObexNumInterfaces = 2;
const TInt KObexSubClassNumber = 0x0b;
const TInt KObexProtocolNumber = 0x0;
const TInt KObexTotalEndpoints = 2;

const TInt KObexDataClass = 0x0A;
const TInt KObexDataSubClass = 0;
const TInt KObexAlt0 = 0;
const TInt KObexAlt1 = 1;
const TInt KObexFunctionalDescriptorLength = 5;
const TInt KObexInterfaceDescriptorLength = 9;

_LIT(KUsbObexLddName, "eusbc");
_LIT(KUsbObexIfc, "OBEX");

NONSHARABLE_CLASS(CUsbObexClassController) : public CUsbClassControllerPlugIn
	{
public: // New functions.
	static CUsbObexClassController* NewL(MUsbClassControllerNotify& aOwner);

public: // Functions derived from CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

public: // Functions derived from CUsbClassControllerBase
	virtual ~CUsbObexClassController();

	virtual void Start(TRequestStatus& aStatus);
	virtual void Stop(TRequestStatus& aStatus);

	virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;


protected:
	CUsbObexClassController(MUsbClassControllerNotify& aOwner);
	TInt SetUpClassAndInterface();

private:
	RDevUsbcClient	iLdd;				// USB logical device drive			
	RDevUsbcClient	iLdd2;
	};

#endif //__CUSBOBEXCLASSCONTROLLER_H__
