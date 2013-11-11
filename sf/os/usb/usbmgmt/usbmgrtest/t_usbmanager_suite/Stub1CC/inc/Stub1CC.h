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
* to manage the stub1.CSY that is used to provide a virtual
* serial port service to clients
* 
*
*/



/**
 @file
*/

#ifndef __CUsbstub1CLASSCONTROLLER_H__
#define __CUsbstub1CLASSCONTROLLER_H__

#include <e32std.h>
#include <cusbclasscontrollerplugin.h>
#include <d32usbc.h>

class MUsbClassControllerNotify;

const TInt Kstub1StartupPriority = 4;

const TInt Kstub1CCDefaultDelay = 500; //0.5 sec default delay for start and stop

const TInt Kstub1NumberOfInterfacesPerstub1Function = 2; // data and control interfaces


// Lengths of the various bits of the  descriptor. Taken from the USB
// WMCDC specification, v1.0.
const TInt Kstub1InterfaceDescriptorLength = 3;
const TInt Kstub1CcHeaderDescriptorLength = 5;
const TInt Kstub1FunctionalDescriptorLength = 4;
const TInt Kstub1CcUfdDescriptorLength = 5;
const TInt Kstub1NotificationEndpointDescriptorLength = 7;
const TInt Kstub1DataClassInterfaceDescriptorLength = 3;
const TInt Kstub1DataClassHeaderDescriptorLength = 5;
const TInt Kstub1DataClassEndpointInDescriptorLength = 7;
const TInt Kstub1DataClassEndpointOutDescriptorLength = 7;

const TInt Kstub1DescriptorLength =
	Kstub1InterfaceDescriptorLength +
	Kstub1CcHeaderDescriptorLength +
	Kstub1FunctionalDescriptorLength +
	Kstub1CcUfdDescriptorLength +
	Kstub1NotificationEndpointDescriptorLength +
	Kstub1DataClassInterfaceDescriptorLength +
	Kstub1DataClassHeaderDescriptorLength +
	Kstub1DataClassEndpointInDescriptorLength +
	Kstub1DataClassEndpointOutDescriptorLength;
	
/**
 * The CUsbstub1ClassController class
 *
 * Implements the USB Class Controller API and manages the stub1.CSY
 */
NONSHARABLE_CLASS(CUsbstub1ClassController) : public CUsbClassControllerPlugIn
	{

public: // New functions.
	static CUsbstub1ClassController* NewL(MUsbClassControllerNotify& aOwner);

public: // Functions derived from CBase.
	virtual ~CUsbstub1ClassController();

public: // Functions derived from CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

public: // Functions derived from CUsbClassControllerBase
	virtual void Start(TRequestStatus& aStatus);
	virtual void Stop(TRequestStatus& aStatus);

	virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

protected:
	CUsbstub1ClassController(MUsbClassControllerNotify& aOwner);
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

#endif //__CUsbstub1CLASSCONTROLLER_H__
