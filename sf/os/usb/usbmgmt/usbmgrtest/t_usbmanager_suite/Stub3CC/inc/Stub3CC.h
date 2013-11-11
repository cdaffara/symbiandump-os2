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
* to manage the stub3.CSY that is used to provide a virtual
* serial port service to clients
* 
*
*/



/**
 @file
*/

#ifndef __CUSBstub3CLASSCONTROLLER_H__
#define __CUSBstub3CLASSCONTROLLER_H__

#include <e32std.h>
#include <cusbclasscontrollerplugin.h>
#include <d32usbc.h>

class MUsbClassControllerNotify;

const TInt Kstub3StartupPriority = 2;

const TInt Kstub3CCDefaultDelay = 500; //0.5 sec default delay for start and stop

const TInt Kstub3NumberOfInterfacesPerstub3Function = 2; // data and control interfaces


// Lengths of the various bits of the  descriptor. Taken from the USB
// WMCDC specification, v1.0.
const TInt Kstub3InterfaceDescriptorLength = 3;
const TInt Kstub3CcHeaderDescriptorLength = 5;
const TInt Kstub3FunctionalDescriptorLength = 4;
const TInt Kstub3CcUfdDescriptorLength = 5;
const TInt Kstub3NotificationEndpointDescriptorLength = 7;
const TInt Kstub3DataClassInterfaceDescriptorLength = 3;
const TInt Kstub3DataClassHeaderDescriptorLength = 5;
const TInt Kstub3DataClassEndpointInDescriptorLength = 7;
const TInt Kstub3DataClassEndpointOutDescriptorLength = 7;

const TInt Kstub3DescriptorLength =
	Kstub3InterfaceDescriptorLength +
	Kstub3CcHeaderDescriptorLength +
	Kstub3FunctionalDescriptorLength +
	Kstub3CcUfdDescriptorLength +
	Kstub3NotificationEndpointDescriptorLength +
	Kstub3DataClassInterfaceDescriptorLength +
	Kstub3DataClassHeaderDescriptorLength +
	Kstub3DataClassEndpointInDescriptorLength +
	Kstub3DataClassEndpointOutDescriptorLength;
	
/**
 * The CUsbstub3ClassController class
 *
 * Implements the USB Class Controller API and manages the stub3.CSY
 */
NONSHARABLE_CLASS(CUsbstub3ClassController) : public CUsbClassControllerPlugIn
	{

public: // New functions.
	static CUsbstub3ClassController* NewL(MUsbClassControllerNotify& aOwner);

public: // Functions derived from CBase.
	virtual ~CUsbstub3ClassController();

public: // Functions derived from CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

public: // Functions derived from CUsbClassControllerBase
	virtual void Start(TRequestStatus& aStatus);
	virtual void Stop(TRequestStatus& aStatus);

	virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

protected:
	CUsbstub3ClassController(MUsbClassControllerNotify& aOwner);
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

#endif //__CUSBstub3CLASSCONTROLLER_H__
