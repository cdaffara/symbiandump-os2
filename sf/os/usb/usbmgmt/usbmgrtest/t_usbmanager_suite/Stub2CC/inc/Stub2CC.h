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
* Adheres to the UsbMan USB Class API and talks to C32
* to manage the stub2.CSY that is used to provide a virtual
* serial port service to clients
* 
*
*/



/**
 @file
*/

#ifndef __CUSBstub2CLASSCONTROLLER_H__
#define __CUSBstub2CLASSCONTROLLER_H__

#include <e32std.h>
#include <cusbclasscontrollerplugin.h>
#include <d32usbc.h>

class MUsbClassControllerNotify;

const TInt Kstub2StartupPriority = 3;

const TInt Kstub2CCDefaultDelay = 500; //0.5 sec default delay for start and stop

const TInt Kstub2NumberOfInterfacesPerstub2Function = 2; // data and control interfaces


// Lengths of the various bits of the  descriptor. Taken from the USB
// WMCDC specification, v1.0.
const TInt Kstub2InterfaceDescriptorLength = 3;
const TInt Kstub2CcHeaderDescriptorLength = 5;
const TInt Kstub2FunctionalDescriptorLength = 4;
const TInt Kstub2CcUfdDescriptorLength = 5;
const TInt Kstub2NotificationEndpointDescriptorLength = 7;
const TInt Kstub2DataClassInterfaceDescriptorLength = 3;
const TInt Kstub2DataClassHeaderDescriptorLength = 5;
const TInt Kstub2DataClassEndpointInDescriptorLength = 7;
const TInt Kstub2DataClassEndpointOutDescriptorLength = 7;

const TInt Kstub2DescriptorLength =
	Kstub2InterfaceDescriptorLength +
	Kstub2CcHeaderDescriptorLength +
	Kstub2FunctionalDescriptorLength +
	Kstub2CcUfdDescriptorLength +
	Kstub2NotificationEndpointDescriptorLength +
	Kstub2DataClassInterfaceDescriptorLength +
	Kstub2DataClassHeaderDescriptorLength +
	Kstub2DataClassEndpointInDescriptorLength +
	Kstub2DataClassEndpointOutDescriptorLength;
	
/**
 * The CUsbstub2ClassController class
 *
 * Implements the USB Class Controller API and manages the stub2.CSY
 */
NONSHARABLE_CLASS(CUsbstub2ClassController) : public CUsbClassControllerPlugIn
	{

public: // New functions.
	static CUsbstub2ClassController* NewL(MUsbClassControllerNotify& aOwner);

public: // Functions derived from CBase.
	virtual ~CUsbstub2ClassController();

public: // Functions derived from CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

public: // Functions derived from CUsbClassControllerBase
	virtual void Start(TRequestStatus& aStatus);
	virtual void Stop(TRequestStatus& aStatus);

	virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

protected:
	CUsbstub2ClassController(MUsbClassControllerNotify& aOwner);
	void ConstructL();



private:
	TInt SetUpInterface();


	// delays in microseconds
	TInt iStartDelay;
	TInt iStopDelay;
	
	TBool iFailToStart;
	TBool iFailToStop;
	
	RTimer iTimer;
	TRequestStatus* iReportStatus;
	RDevUsbcClient  iLdd;

	
	
	};

#endif //__CUSBstub2CLASSCONTROLLER_H__
