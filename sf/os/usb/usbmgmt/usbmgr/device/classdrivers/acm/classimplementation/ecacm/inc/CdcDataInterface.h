/*
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
*
*/

#ifndef __CDCDATAINTERFACE_H__
#define __CDCDATAINTERFACE_H__

#include "CdcInterfaceBase.h"
#include "WriteObserver.h"
#include "ReadObserver.h"
#include "ReadOneOrMoreObserver.h"
#include "linkstatenotifier.h"
#include "NotifyDataAvailableObserver.h"

extern const TInt32 KUsbAcmHostCanHandleZLPs;

class CActiveReadOneOrMoreReader;
class CActiveReader;
class CActiveDataAvailableNotifier;
class CActiveWriter;
class CIniFile;

NONSHARABLE_CLASS(CCdcDataInterface) :	public CCdcInterfaceBase,
							public MReadObserver,
							public MReadOneOrMoreObserver,
							public MWriteObserver,
							public MLinkStateObserver,
							public MNotifyDataAvailableObserver
/**
 * ACM data interface subclass.
 */
	{
public:
	static CCdcDataInterface* NewL(const TDesC16& aIfcName);
	~CCdcDataInterface();

public:
	void Write(MWriteObserver& aObserver, const TDesC8& aDes, TInt aLen);
	void CancelWrite();
	void Read(MReadObserver& aObserver, TDes8& aDes, TInt aMaxLen);
	void ReadOneOrMore(MReadOneOrMoreObserver& aObserver, 
		TDes8 &aDes, 
		TInt aMaxLen);
	void CancelRead();
	void NotifyDataAvailable(MNotifyDataAvailableObserver& aObserver);
	void CancelNotifyDataAvailable();

private:
	CCdcDataInterface();
	CCdcDataInterface(const TDesC16& aIfcName);
	void ConstructL();

private: // from MReadOneOrMoreObserver
	void ReadOneOrMoreCompleted(TInt aError);

private: // from MReadObserver
	void ReadCompleted(TInt aError);

private: // from MWriteObserver
	void WriteCompleted(TInt aError);

private: // from CCdcClassBase
	TInt SetUpInterface();

private: // from MLinkStateObserver
	void MLSOStateChange(TInt aPacketSize);

private: // from MNotifyDataAvailableObserver
	void NotifyDataAvailableCompleted(TInt aError);


private: // owned
	CActiveReadOneOrMoreReader* iReadOneOrMoreReader;
	CActiveReader* iReader;
	CActiveWriter* iWriter;
	CActiveDataAvailableNotifier* iDataAvailableNotifier;
	CLinkStateNotifier* iLinkState;
	TBool iHostCanHandleZLPs;  ///< Host USB Device Driver can cope with Zero Length Packets
	TInt iPacketSize;

private: // unowned
	MReadObserver* iReadObserver;
	MReadOneOrMoreObserver* iReadOneOrMoreObserver;
	MWriteObserver* iWriteObserver;
	MNotifyDataAvailableObserver* iNotifyDataAvailableObserver;

#ifdef __HEADLESS_ACM_TEST_CODE__
	// This code is for testing the performance of the stack at the 
	// RDevUsbcClient level. Issue a Read or ReadOneOrMore at the RComm level, 
	// and it will be passed to the LDD. When it completes, it will Write the 
	// data back. When that completes, it will again Read/ROOM. It basically 
	// enters an infinite loop, doing loopback. Drive this from the PC end 
	// using serial.exe.
private: // owned
	// The buffer to do repeated Read/ReadOneOrMore into and Writes from.
	TBuf8<4096> iHeadlessAcmBuffer;
	// The length of the initial Read/ROOM request- we continue to use this 
	// value in subsequent Read/ROOM requests.
	TUint iHeadlessReadLength;
	// Whether we're currently using Read or ROOM. 
	enum THeadlessReadType
		{
		EUnknown,
		ERead,
		EReadOneOrMore
		};
	THeadlessReadType iHeadlessReadType;
#endif // __HEADLESS_ACM_TEST_CODE__
	};

#endif // __CDCDATAINTERFACE_H__
