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

#ifndef __ACMPORT_H__
#define __ACMPORT_H__

#include <cs_port.h>
#include "CdcAcmClass.h"
#include "HostPushedChangeObserver.h"
#include "BreakObserver.h"

class CAcmPortFactory;
class CAcmReader;
class CAcmWriter;
class MAcmPortObserver;

NONSHARABLE_CLASS(CAcmPort) :	public CPort, 
					public MHostPushedChangeObserver,
					public MBreakObserver
/**
 * Concrete ACM port type, derived from C32's CPort.
 */
	{
public:
	static CAcmPort* NewL(const TUint aUnit, MAcmPortObserver& aFactory);
	~CAcmPort();

public:
	void SetAcm(CCdcAcmClass* aAcm);
	inline CCdcAcmClass* Acm();

private:
	CAcmPort(const TUint aUnit, MAcmPortObserver& aFactory);
	void ConstructL();

private: // from CPort
	virtual void StartRead(const TAny* aClientBuffer,TInt aLength);
	virtual void ReadCancel();
	virtual TInt QueryReceiveBuffer(TInt& aLength) const;
	virtual void ResetBuffers(TUint aFlags);
	virtual void StartWrite(const TAny* aClientBuffer,TInt aLength);
	virtual void WriteCancel();
	virtual void Break(TInt aTime);
	virtual void BreakCancel();
	virtual TInt GetConfig(TDes8& aDes) const;
	virtual TInt SetConfig(const TDesC8& aDes);
	virtual TInt SetServerConfig(const TDesC8& aDes);
	virtual TInt GetServerConfig(TDes8& aDes);
	virtual TInt GetCaps(TDes8& aDes);
	virtual TInt GetSignals(TUint& aSignals);
	virtual TInt SetSignalsToMark(TUint aSignals);
	virtual TInt SetSignalsToSpace(TUint aSignals);
	virtual TInt GetReceiveBufferLength(TInt& aLength) const;
	virtual TInt SetReceiveBufferLength(TInt aSignals);
	virtual void Destruct();
	virtual void FreeMemory();
	virtual void NotifySignalChange(TUint aSignalMask);
	virtual void NotifySignalChangeCancel();
	virtual void NotifyConfigChange();
	virtual void NotifyConfigChangeCancel();
	virtual void NotifyFlowControlChange();
	virtual void NotifyFlowControlChangeCancel();
	virtual void NotifyBreak();
	virtual void NotifyBreakCancel();
	virtual void NotifyDataAvailable();
	virtual void NotifyDataAvailableCancel();
	virtual void NotifyOutputEmpty();
	virtual void NotifyOutputEmptyCancel();
	virtual TInt GetFlowControlStatus(TFlowControl& aFlowControl);
	virtual TInt GetRole(TCommRole& aRole);
	virtual TInt SetRole(TCommRole aRole);

private: // from MBreakObserver
	void BreakRequestCompleted();
	void BreakStateChange();

private: // from MHostPushedChangeObserver
	virtual void HostConfigChange(const TCommConfigV01& aConfig);
	virtual void HostSignalChange(TBool aDtr, TBool aRts);

private: // utility
	TInt SetSignals(TUint32 aNewSignals);
	void HandleConfigNotification(TBps aBps,
		TDataBits aDataBits,
		TParity aParity,
		TStopBits aStopBits,
		TUint aHandshake);
	// iSignals stores the lines in DCE mode- these functions convert between 
	// this and whatever the client expects, according to our current mode.
	TUint32 ConvertSignals(TUint32 aSignals) const;
	TUint32 ConvertAndFilterSignals(TUint32 aSignals) const;
	TInt DoSetBufferLengths(TUint aLength);

public: // owned data
	TCommConfigV01 iCommConfig;
	TCommNotificationPckg iCommNotificationDes;
	TCommNotificationV01& iCommNotification;

private: // unowned
	CCdcAcmClass* iAcm;
	MAcmPortObserver& iObserver;

private: // owned
	CAcmReader* iReader;
	CAcmWriter* iWriter;					

	// Current server configuration settings
	TCommServerConfigV01 iCommServerConfig;
	// Flag indicating whether there is a current signal line change 
	// notification request outstanding
	TBool iNotifySignalChange;
	// Flag indicating whether there is a current configuration change 
	// notification request outstanding
	TBool iNotifyConfigChange;
	// Flag indicating whether there is a current break notification 
	// request outstanding
	TBool iNotifyBreak;
	// The current emulated signal lines
	TUint32 iSignals;						
	// The signal mask associated with the current signal change notification 
	// request
	TUint iNotifySignalMask;				
	// Role of the serial port (DTE or DCE)
	TCommRole iRole;			
	TBool iCancellingBreak;
	// Flag indicating whether we're currently requesting a break.
	TBool iBreak;

	// The port number.
	const TUint iUnit;
	};

// Inlines

CCdcAcmClass* CAcmPort::Acm()
/**
 * Accessor function for the ACM class.
 *
 * @return Pointer to the port's ACM class instance.
 */
	{
	return iAcm;
	}

#endif // __ACMPORT_H__
