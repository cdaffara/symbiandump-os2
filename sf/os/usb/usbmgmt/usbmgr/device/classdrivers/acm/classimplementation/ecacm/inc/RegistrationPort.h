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

#ifndef __REGISTRATIONPORT_H__
#define __REGISTRATIONPORT_H__

#include <cs_port.h>

class MAcmController;

/**
The registration port.
This is a deprecated method for controlling (bringing up and down) ACM 
functions.
ACM Class Controllers open a registration port and use SetSignalsToMark and 
SetSignalsToSpace to bring ACM functions up and down.
The newer method involved creating a session on the ACM server (RAcmServer) 
and using the APIs provided.
*/
NONSHARABLE_CLASS(CRegistrationPort) : public CPort
	{
public:
	static CRegistrationPort* NewL(MAcmController& aAcmController, 
		TUint aUnit);
	~CRegistrationPort();

private:
	CRegistrationPort(MAcmController& aAcmController);
	void ConstructL(TUint aUnit);

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
	virtual TInt SetSignalsToMark(TUint aNoAcms);
	virtual TInt SetSignalsToSpace(TUint aNoAcms);
	virtual TInt GetReceiveBufferLength(TInt& aLength) const;
	virtual TInt SetReceiveBufferLength(TInt aLength);
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

private: // unowned
	MAcmController& iAcmController;

private: // owned
	// Dummy role of the serial port (DTE or DCE)
	TCommRole iRole;			
	};

#endif // __REGISTRATIONPORT_H__
