/**
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Adheres to the UsbMan USB Class Controller API.
* 
*
*/



/**
 @file
*/

#ifndef CUSBDUMMYCLASSCONTROLLERLDD_H__
#define CUSBDUMMYCLASSCONTROLLERLDD_H__
#include <e32std.h>
#include <e32property.h>
#include <cusbclasscontrollerplugin.h>

NONSHARABLE_CLASS(CUsbDummyClassControllerLdd) : public CUsbClassControllerPlugIn

	{
public:
	static CUsbDummyClassControllerLdd* NewL(MUsbClassControllerNotify& aOwner, TUint aIndex);
	~CUsbDummyClassControllerLdd();

private:
	CUsbDummyClassControllerLdd(MUsbClassControllerNotify& aOwner, TUint aIndex);
	CUsbDummyClassControllerLdd(MUsbClassControllerNotify& aOwner,TUint aIndex, TInt aPriority);
	void ConstructL();

private: // from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private: // from CUsbClassControllerBase
	void Start(TRequestStatus& aStatus);
	void Stop(TRequestStatus& aStatus);
	void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const;

private:
	enum TSynchronicity 
		{
		EUndefined = 0,
		ESynchronous,
		EAsynchronous,
		ENever,
		};
	
	NONSHARABLE_STRUCT( TBehaviour )
	/** 
	 * Defines the behaviour for a startup or a shutdown.
	 */
		{
		TSynchronicity iSynchronicity;
		TTimeIntervalMicroSeconds32 iDelay;
		TInt iErrorCode;
		};

private: // utility
	void GetConfig();
	void DoGetConfigL();

private: // unowned
	TRequestStatus* iReportStatus;

private: // owned
	RTimer iTimer;
	const TUint iIndex;
	RProperty iProperty;
	TBool     iPropertyExist;
	TBehaviour iStartupBehaviour;
	TBehaviour iShutdownBehaviour;
	};

#endif // CUSBDUMMYCLASSCONTROLLER_H__
