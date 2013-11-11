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
* Adheres to the UsbMan USB Class Controller API.
* 
*
*/



/**
 @file
*/

#ifndef CUSBDUMMYCLASSCONTROLLER_H__
#define CUSBDUMMYCLASSCONTROLLER_H__

#include <cusbclasscontrollerbase.h>

class CIniFile;

NONSHARABLE_CLASS(CUsbDummyClassController) : public CUsbClassControllerBase
/**
 * A test utility which, depending on the contents of c:\\dummy.ini, displays 
 * different start up and shutdown behaviour.
 * The USB manager can instantiate a number of instances of this class 
 * controller. Each one reads its behaviour from a section of dummy.ini. The 
 * options are, for each of startup and shutdown: (a) synchronous or 
 * asynchronous, (b) completing after a variable amount of time, or 
 * immediately, or never, and (c) completing with a given error code.
 * NB This class controller is a dummy- it does not communicate with a class 
 * implementation. It registers no USB interfaces.
 * Note that for each of startup and shutdown, Type, Time and Error *must* be 
 * filled in, even if one or more are irrelevant. They are applied in the 
 * following order: Type, Time, Error. That is, if Type is 'never', the CC 
 * never completes, but it does this asynchronously, i.e. without blocking the 
 * CC's (=USBMAN's) thread. If Time is '0', it completes after a synchronous 
 * or asynchronous wait for 0 microseconds (as determined by the Type). 
 * Note that the ini file is re-read whenever Start or Stop is requested. 
 * Otherwise, a CC which is set to not Stop would stop us being able to Stop, 
 * ever, or shut down USBMAN, which would make wrapping up the test framework 
 * impossible.
 * Note that we inherit the TUsbServiceState iState member of 
 * CUsbClassControllerBase. We only use this to mark whether we're currently 
 * starting or stopping, or doing nothing. Requests from the device may come 
 * in at any point- the only proviso is that we are always Cancelled before 
 * another Start or Stop comes in first (this is enforced by us through 
 * iReportStatus). It happens that it's useless to assert our iState in Start 
 * and Stop.
 */
	{
public:
	static CUsbDummyClassController* NewL(MUsbClassControllerNotify& aOwner, TUint aIndex);
	static CUsbDummyClassController* NewL(MUsbClassControllerNotify& aOwner, TUint aIndex, TInt aPriority);
	~CUsbDummyClassController();

private:
	CUsbDummyClassController(MUsbClassControllerNotify& aOwner, TUint aIndex);
	CUsbDummyClassController(MUsbClassControllerNotify& aOwner,TUint aIndex, TInt aPriority);
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
	void GetBehaviour(CIniFile& aInifile, 
		const TDesC& aSection, 
		TBehaviour& aBehaviour);
	void DoGetConfigL();

private: // unowned
	TRequestStatus* iReportStatus;

private: // owned
	RTimer iTimer;
	// The index of this instance (USBMAN may have more than one, and this is 
	// used to get the required behaviour for this instance from the ini 
	// file).	
	const TUint iIndex;

	TBehaviour iStartupBehaviour;
	TBehaviour iShutdownBehaviour;
	};

#endif // CUSBDUMMYCLASSCONTROLLER_H__
