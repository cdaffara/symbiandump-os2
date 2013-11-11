/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
@file
@internalComponent
*/

#ifndef __SERIAL_DRIVER_H
#define __SERIAL_DRIVER_H

#define MAX_PORTS					10     // randomly high value...

#include <e32std.h>
#include <e32cmn.h>

class RSerialPort;            				// actually defined within the implementation


class CSerialServer
{

friend class CSerialPort;  // declare CSerialPort our friemd

public:
	CSerialServer();
	virtual ~CSerialServer();

	TBool Open(void);
	TBool Close(void);
	
private:

	// These next methods are for friends and not intended
	// as part of the truely public interface.	
	RSerialPort * GetPort(TUint aPortNum);		
	RSerialPort * InitialisePort(TUint aPortNum);
	TBool ClosePort(TUint aPortNum);

	// member variables

	TInt m_errPhysicalLoad;
	TInt m_errLogicalLoad;

	RSerialPort * portsInUse[MAX_PORTS]; // array for holding set of port info
};


class CSerialPort
{
public:
	CSerialPort();
	virtual ~CSerialPort();

	TBool Open(CSerialServer & aServer, TUint aPort);
	TBool Close(void);

	TBool Read(TRequestStatus & aStatus, 
			   TTimeIntervalMicroSeconds32 aTimeout,
			   TDes8 &aDes,
			   TInt aLength);

	TBool Write(TRequestStatus & aStatus,
						 TTimeIntervalMicroSeconds32 aTimeout,
						 const TDesC8 &aDes,
						 TInt aLength);

	TInt SetConfig(const TCommConfig & aRequiredConfig);

	TBool GetPortConfig(TCommConfig & aPortSettings);
	
	TBool ReadCancel(void);

	TBool WriteCancel(void);
	
	TBool SetReceiveBufferLength(TInt aSize);
	
	TBool ReceiveBufferLength(TInt & aSize);
	
	public:

	//functions
	RSerialPort * GivePortDetails(void);
	
	// Member variables	
	CSerialServer * m_pOurServer;
	
	TUint m_portId;
	
};

#endif
