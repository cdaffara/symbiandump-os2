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

// System includes..
#include <e32base.h>

#include "serialdriverext.h"
#include "serialdriver.h"			// our header

// Some useful logical/physical driver names.
#if defined (__WINS__)
#define PDD_NAME 		_L("ECDRV.PDD")
#define LDD_NAME 		_L("ECOMM.LDD")
#define DEFAULT_PORT  	1
#else
#define PDD_NAME 		_L("EUART")
#define LDD_NAME 		_L("ECOMM")
#define DEFAULT_PORT  	0
#endif

// How many varaiants of the physical driver should we attempt to load before failing.
#ifdef __WINS__
	const TInt KMaxPdds=0;
#else
	const TInt KMaxPdds=10;
#endif

#define IGNORE_VALUE(val)		((val)=(val))

class RSerialPort : public RBusDevComm
	{
	public:
	
	TBool TimedWrite(TRequestStatus & aStatus, 
					 TTimeIntervalMicroSeconds32 aTimeout,
					 const TDesC8 &aDes,
					 TInt aLength);
	
	TBool TimedRead(TRequestStatus & aStatus, 
					TTimeIntervalMicroSeconds32 aTimeout,
					TDes8 &aDes,
					TInt aLength);
	
	};

TBool RSerialPort::TimedRead(TRequestStatus & aStatus, 
							 TTimeIntervalMicroSeconds32 aTimeout,
							 TDes8 &aDes,
							 TInt aLength)
	{
	IGNORE_VALUE(aTimeout);

					
	// Do the actual read using the base class function...
		
	Read(aStatus, aDes, aLength);          // asynchronous				
	
	
	// if the timer was still running when we cancelled it here, then all ok.
	return (ETrue);
	
	}
	
TBool RSerialPort::TimedWrite(TRequestStatus & aStatus,
							  TTimeIntervalMicroSeconds32 aTimeout,
							  const TDesC8 &aDes,
							  TInt aLength)
	{
	
	IGNORE_VALUE(aTimeout);
						
	// Do the actual write using the base class function...
	Write(aStatus, aDes, aLength);          // asynchronous				
	
	return (ETrue);	
	
	}	
	
	
/////////////////////
/// CSerialServer ///
/////////////////////

CSerialServer::CSerialServer()
{

	m_errLogicalLoad = KErrNone;
	m_errPhysicalLoad = KErrNone;

	for (TUint index = 0; index < MAX_PORTS; index++)
		portsInUse[index] = NULL;

}

CSerialServer::~CSerialServer()
	{
	// Delete any ports that are still in use...
	for (TUint index = 0; index < MAX_PORTS; index++)
		{
		if (portsInUse[index] != NULL)
			{
			delete portsInUse[index];
			portsInUse[index]= NULL;    // just tidying up
			}
		}
	}

TBool CSerialServer::Open(void)
	{

	TBool bSuccess = EFalse;

	// Load the logical driver.
	m_errLogicalLoad = User::LoadLogicalDevice(LDD_NAME);

	if (m_errLogicalLoad == KErrNone || m_errLogicalLoad == KErrAlreadyExists)
		{

		// Load the physical driver.
		TInt loadErr;
		TBuf<10> pddName=PDD_NAME;

		// Try EUART as a driver name first, then EUART0, EUART1, EUART2...EUARTn.
		TInt driver;
		for (driver=-1; driver<KMaxPdds && bSuccess == EFalse; ++driver)
			{
			if (driver==0)
				pddName.Append(TChar('0'));
			else if (driver>0)
				pddName[pddName.Length()-1] = (TText)('0'+driver);
			loadErr = User::LoadPhysicalDevice(pddName);
			if (loadErr == KErrNone || loadErr==KErrAlreadyExists)
				bSuccess = ETrue;
			}

		// If failed to load physical driver, unload the logical if this wasn't originally present
		if (bSuccess == EFalse)
			{
			if (m_errLogicalLoad == KErrNone)
				User::FreeLogicalDevice(LDD_NAME);
			}
		}

	return(bSuccess);
	}

TBool CSerialServer::Close(void)
	{
	TInt logUnloadError = KErrNone;
	TInt phyUnloadError = KErrNone;
	TBool bSuccess = EFalse;

	// If not originally loaded, then unload the physical device.
	if (m_errPhysicalLoad == KErrNone)
		phyUnloadError = User::FreePhysicalDevice(PDD_NAME);

	// If not originally loaded, then unload the logical device.
	if (m_errLogicalLoad == KErrNone)
		logUnloadError = User::FreeLogicalDevice(LDD_NAME);

	if (phyUnloadError == KErrNone && logUnloadError == KErrNone)
		bSuccess = ETrue;

	return (bSuccess);

	}


RSerialPort * CSerialServer::GetPort(TUint aPortNum)
	{	
	RSerialPort * pRPort = (aPortNum < MAX_PORTS ? portsInUse[aPortNum] : NULL);
	return (pRPort);	
	}
	
	
RSerialPort * CSerialServer::InitialisePort(TUint aPortNum)
{
	RSerialPort * newPort = NULL;
	
	if (aPortNum < MAX_PORTS && portsInUse[aPortNum] == NULL)
	{
	
		// Create a new port...
		newPort = new RSerialPort;   //RClass - so  can't leave.
		
		// Open the real port...
		TInt error = newPort->Open(aPortNum);
		
		if (error != KErrNone )
			{
			// There was a problem, tidy up...
			delete newPort;
			newPort = NULL;
			}
		else
			{			
			// Record a pointer to our port for later use.
			portsInUse[aPortNum] = newPort;
			}
	}
	
	return (newPort);
}

TBool CSerialServer::ClosePort (TUint aPortNum)
{
	TBool closed = EFalse;
	
	// if port is valid...
	if (aPortNum < MAX_PORTS && portsInUse[aPortNum])
		{
		// Close the physical port!
		portsInUse[aPortNum]->Close();
		
		// Delete the memory and update the pointer.
		delete portsInUse[aPortNum];
		portsInUse[aPortNum] = NULL;
		
		// Update flag to indicate all closed ok.
		closed = ETrue;
	
		}
	return (closed);
	
}

///////////////////
/// CSerialPort ///
///////////////////


CSerialPort::CSerialPort()
{
	m_pOurServer = NULL;
}

CSerialPort::~CSerialPort()
{
	
}

TBool CSerialPort::Open(CSerialServer & aServer, TUint aPort)
	{
	TBool bOpenedOk = EFalse;
	
	// The server holds the real port details, so call the server to do the business...
	if (aServer.InitialisePort(aPort) != NULL)
		{
		m_pOurServer = &aServer;
		
		m_portId = aPort;
						
		bOpenedOk = ETrue;
		}
	
	return (bOpenedOk);
	}


TBool CSerialPort::Close(void)
	{
	TBool bClosed = EFalse;
		
	// The server object holds all the real details, so call the appropriate
	// server function.
	if (m_pOurServer)		
		bClosed = m_pOurServer->ClosePort(m_portId);
	
	return (bClosed);
	}

TBool CSerialPort::Read(TRequestStatus & aStatus, 
						TTimeIntervalMicroSeconds32 aTimeout,
						TDes8 &aDes,
						TInt aLength)
	{
	TBool success = EFalse;

	RSerialPort * pPort = GivePortDetails();
	
	if (pPort)
		{
		if (pPort->TimedRead(aStatus, aTimeout, aDes, aLength))
			success = ETrue;
		}
		
		// return success or otherwise!
		return (success);
	}


TBool CSerialPort::Write(TRequestStatus & aStatus,
				   TTimeIntervalMicroSeconds32 aTimeout,
				   const TDesC8 &aDes,
				   TInt aLength)
	{
	
	TBool success = EFalse;

	RSerialPort * pPort = GivePortDetails();
	
	if (pPort)
		{
		if (pPort->TimedWrite(aStatus, aTimeout, aDes, aLength))
			success = ETrue;
		}
		
	// return success or otherwise!
	return (success);
	}


TInt CSerialPort::SetConfig(const TCommConfig & aRequiredConfig)
	{
	TInt errConfig = KErrUnknown;
		
	RSerialPort * pPort = GivePortDetails();

	if (pPort)
		{
		errConfig = pPort->SetConfig(aRequiredConfig);
		}
		
	return (errConfig);
	}

TBool CSerialPort::GetPortConfig(TCommConfig & aPortSettings)
{
	TBool validAction = EFalse;

	RSerialPort * pPort = GivePortDetails();
	
	if (pPort)
		{		
		pPort->Config( aPortSettings );
		validAction = ETrue;
		}

	return (validAction);
}

TBool CSerialPort::ReadCancel(void)
	{
	TBool validAction = EFalse;

	RSerialPort * pPort = GivePortDetails();
	
	if (pPort)
		{		
		pPort->ReadCancel();		
		validAction = ETrue;
		}
		
	return (validAction);
	}


TBool CSerialPort::WriteCancel(void)
	{
	TBool validAction = EFalse;

	RSerialPort * pPort = GivePortDetails();
	
	if (pPort)
		{		
		pPort->WriteCancel();		
		validAction = ETrue;
		}
	return (validAction);
	
	}

TBool CSerialPort::SetReceiveBufferLength(TInt aSize)
	{
	TBool validAction = EFalse;

	RSerialPort * pPort = GivePortDetails();
	
	if (pPort)
		{
		pPort->SetReceiveBufferLength(aSize);
		validAction = ETrue;		
		}
		
	return (validAction);
	
	}

TBool CSerialPort::ReceiveBufferLength(TInt & aSize)
	{
	TBool validAction = EFalse;

	RSerialPort * pPort = GivePortDetails();
	
	if (pPort)
		{
		aSize = pPort->ReceiveBufferLength();
		validAction = ETrue;		
		}
		
	return (validAction);	
	}



RSerialPort * CSerialPort::GivePortDetails(void)
	{	
	RSerialPort * pPort = NULL;

	if (m_pOurServer)	
		pPort = m_pOurServer->GetPort(m_portId);
	
	return (pPort);	
	}

//////////////////////////////
// The external interface
// for users of the library
//////////////////////////////
	
EXPORT_C SerialServer OpenSerialServerL(void)
	{
	CSerialServer * pServer;
	pServer = new(ELeave) CSerialServer;

	// Declare a server and open it.
	if ( pServer->Open() == EFalse)
		{			
		delete pServer;
		pServer = NULL;
		}
		
	return (static_cast<SerialServer>(pServer));		
	}

EXPORT_C void CloseSerialServer(SerialServer aServer)
	{
	if (aServer)
		{			
		CSerialServer * pServer = static_cast<CSerialServer *>(aServer);
		delete pServer;	
		}
	}

EXPORT_C TAny * OpenSerialPortL(SerialServer aServer, TUint aPort)
	{	
	CSerialServer * pServer = static_cast<CSerialServer *>(aServer);	
	
	CSerialPort * pPort = new(ELeave) CSerialPort; 
	
	if (pPort->Open(*pServer, aPort) == EFalse)
		{
		delete pPort;
		pPort = NULL;		
		}
	
	return (static_cast<TAny *>(pPort));
	}

EXPORT_C TBool CloseSerialPort(SerialPort aPort)
{
	CSerialPort * pPort = static_cast<CSerialPort *>(aPort);
	
	TBool returnVal = pPort->Close();
	
	delete pPort;
		
	return (returnVal);	
}

EXPORT_C TBool Read(SerialPort aPort,
			  		TRequestStatus & aStatus, 
			  		TTimeIntervalMicroSeconds32 aTimeout,
			  		TDes8 &aDes,
			  		TInt aLength)
	{
	TBool ok = EFalse;
	if (aPort)
		{
		CSerialPort * pPort = static_cast<CSerialPort *>(aPort);
		ok = pPort->Read(aStatus, aTimeout, aDes, aLength);
		}
		
	return ok;	 
	}

EXPORT_C TBool Write(SerialPort aPort,
					 TRequestStatus & aStatus,
				     TTimeIntervalMicroSeconds32 aTimeout,
				     const TDesC8 &aDes,
				     TInt aLength)
	{
	TBool ok = EFalse;
	
	if (aPort)
		{			
		CSerialPort * pPort = static_cast<CSerialPort *>(aPort);	
		ok = pPort->Write(aStatus, aTimeout, aDes, aLength);			
		}
	return (ok);
	}

EXPORT_C TInt SetConfig(SerialPort aPort, const TCommConfig & aRequiredConfig )
	{	
	TInt error = KErrUnknown;

	if (aPort)
		{			
		CSerialPort * pPort = static_cast<CSerialPort *>(aPort);	
		error = pPort->SetConfig(aRequiredConfig);
		}
	return error;
	}

EXPORT_C TBool ReadCancel(SerialPort aPort)
	{	
	TBool ok = EFalse;

	if (aPort)
		{			
		CSerialPort * pPort = static_cast<CSerialPort *>(aPort);	
		ok = pPort->ReadCancel();
		}
	return (ok);
	}


EXPORT_C TBool WriteCancel(SerialPort aPort)
	{	
	TBool ok = EFalse;

	if (aPort)
		{			
		CSerialPort * pPort = static_cast<CSerialPort *>(aPort);	
		ok = pPort->WriteCancel();
		}
	return (ok);
	}

EXPORT_C TBool GetPortConfig (SerialPort aPort, TCommConfig & config)
	{		
	TBool ok = EFalse;

	if (aPort)
		{			
		CSerialPort * pPort = static_cast<CSerialPort *>(aPort);	
		ok = pPort->GetPortConfig(config);
		}
	return (ok);
	}


EXPORT_C TBool SetReceiveBufferLength(SerialPort aPort, TInt aSize)
	{
	TBool ok = EFalse;

	if (aPort)
		{			
		CSerialPort * pPort = static_cast<CSerialPort *>(aPort);	
		ok = pPort->SetReceiveBufferLength(aSize);
		}
	return (ok);
	}

EXPORT_C TBool ReceiveBufferLength(SerialPort aPort, TInt & aSize)
	{
	TBool ok = EFalse;

	if (aPort)
		{			
		CSerialPort * pPort = static_cast<CSerialPort *>(aPort);	
		ok = pPort->ReceiveBufferLength(aSize);
		}
	return (ok);
	}
