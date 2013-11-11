/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Adheres to the UsbMan USB Class API and talks to mass storage file system
*
*/

/**
 @file
 @internalTechnology
*/

#ifndef __CUSBMSCLASSCONTROLLER_H__
#define __CUSBMSCLASSCONTROLLER_H__

#include <e32std.h>
#include <cusbclasscontrollerplugin.h>
#include <massstorage.h>

_LIT(KUsbMsResource, "z:\\private\\101fe1db\\usbms.rsc");

class MUsbClassControllerNotify;

const TInt KMsStartupPriority = 3;

	
/**
 The CUsbMsClassController class

 Implements the USB Class Controller API 
 */
NONSHARABLE_CLASS(CUsbMsClassController) : public CUsbClassControllerPlugIn
	{

public: 
	static CUsbMsClassController* NewL(MUsbClassControllerNotify& aOwner);
	~CUsbMsClassController();

private: 
	// Functions derived from CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

private: 
	// Functions derived from CUsbClassControllerBase
	virtual void Start(TRequestStatus& aStatus);
	virtual void Stop(TRequestStatus& aStatus);

	virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

private:
	CUsbMsClassController(MUsbClassControllerNotify& aOwner);
	void ConstructL();

#ifdef _UNITTEST_DEBUG
public:
#else
private:
#endif

    void ReadMassStorageConfigL();
    void ConfigItem(const TPtrC& source, TDes& target, TInt maxLength);

private:
    RUsbMassStorage iUsbMs;
#ifdef _UNITTEST_DEBUG
public:
#endif
	TMassStorageConfig iMsConfig;
	};

#endif //__CUSBMSCLASSCONTROLLER_H__
