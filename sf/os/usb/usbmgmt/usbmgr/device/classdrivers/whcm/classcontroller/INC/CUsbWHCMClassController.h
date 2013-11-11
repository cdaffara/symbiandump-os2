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

#ifndef __CUSBWHCMCLASSCONTROLLER_H__
#define __CUSBWHCMCLASSCONTROLLER_H__

#include <e32std.h>
#include <cusbclasscontrollerplugin.h>
#ifndef __OVER_DUMMYUSBLDD__
#include <d32usbc.h>
#else
#include <dummyusblddapi.h>
#endif

const TInt KWHCMPriority = 1;
const TInt KWHCMSubClass = 0x08;
const TInt KWHCMProtocol = 0;
const TInt KWHCMFuncDescID = 0x11;

/**
 * A structure to hold most of the data for the descriptors in a WHCM header.
 * The rest is filled out dynamically.
 */
static const TUint8 WHCMheader[] =
	{
	// Comms Class Header Functional Desctriptor
	5, KUsbDescType_CS_Interface, 0, 0x10, 0x01,

	// WHCM Functional Descriptor
	5, KUsbDescType_CS_Interface, KWHCMFuncDescID, 0x00, 0x01,

	// Union Functional Descriptor
	4 /* length, updated later once subordinates are known */, KUsbDescType_CS_Interface, 0x06
	// interface number and subordinates added later
	};

/**
 * Class Controller for the WHCM union class.
 */
NONSHARABLE_CLASS(CUsbWHCMClassController) : public CUsbClassControllerPlugIn
	{
public: // New functions.
	static CUsbWHCMClassController* NewL(
		MUsbClassControllerNotify& aOwner);

public: // Functions derived from CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

public: // Functions derived from CUsbClassControllerBase
	virtual ~CUsbWHCMClassController();

	virtual void Start(TRequestStatus& aStatus);
	virtual void Stop(TRequestStatus& aStatus);

	virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

protected:
	CUsbWHCMClassController(MUsbClassControllerNotify& aOwner);
	void ConstructL();
	void SetUpWHCMDescriptorL();

private:
	RDevUsbcClient	iLdd;				// USB logical device drive			
	};

#endif //__CUSBWHCMCLASSCONTROLLER_H__
