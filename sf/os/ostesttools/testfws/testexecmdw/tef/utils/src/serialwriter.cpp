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



#include <e32std.h>
#include "serialwriter.h"
#include "tefconst.h"
#include <test/tefexportconst.h>

#if defined (__WINS__)
_LIT(KPddDev, "ECDRV.PDD");
#else
_LIT(KPddDev, "EUART1");
#endif

#if defined (__WINS__)
_LIT(KLddDev, "ECOMM.LDD");
#else
_LIT(KLddDev, "ECOMM");
#endif


/**
Creates an instance of CSerialWriter object
@return a pointer to the new created CSerialWriter Object
@leave KErrNoMemory if no memory
*/
 CSerialWriter* CSerialWriter::NewL()
	{
	CSerialWriter *me = new (ELeave) CSerialWriter;
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop();
	return me;
	}

/**
Public Destructor
*/
CSerialWriter::~CSerialWriter()
	{
    Disconnect();
	}

//default constructor
CSerialWriter::CSerialWriter()
	{
	}

void CSerialWriter::ConstructL()
	{
	iReady = EFalse;
	
    TInt err = User::LoadPhysicalDevice(KPddDev);
    if(err!=KErrNone && err!=KErrAlreadyExists)
        User::Leave(err);

    err = User::LoadLogicalDevice(KLddDev);
    if(err!=KErrNone && err!=KErrAlreadyExists)
        User::Leave(err);
 	}

TInt CSerialWriter::Connect()
    {
    if(!iReady)
        {
        TInt err = KErrNone;
        err = iComm.Open(iPortNum);
        if(err!=KErrNone)
        	{
       		iReady = EFalse;
            return err;
        	}
        	else
		        iReady = ETrue;
        }
    return KErrNone;
    }

TInt CSerialWriter::Config()
    {
    TInt err = KErrNone;
    if(iReady)
        {
        TCommConfig2 cfgBuf;
        TCommConfigV02& cfg = cfgBuf();
        iComm.Config(cfgBuf);
        cfg.iRate = EBps115200;// default EBps9600;
        cfg.iDataBits = EData8;// default EData8;
        cfg.iStopBits = EStop1;// default EStop1;
        cfg.iParity = EParityNone;// default EParityNone;
        cfg.iHandshake = 0;// default KConfigObeyCTS;
        cfg.iParityError = KConfigParityErrorFail;
        cfg.iFifo = EFifoEnable;// default EFifoEnable;
        cfg.iSpecialRate = 0;// default 0;
        cfg.iTerminatorCount = 0;// default 0;
        cfg.iXonChar = 0x11;// default 0x11; // XON
        cfg.iXoffChar = 0x13;// default 0x13; // XOFF
        cfg.iParityErrorChar = 0;// default 0;
        cfg.iSIREnable = ESIRDisable;// default ESIRDisable;
        cfg.iSIRSettings = 0;// no default
        
        err = iComm.SetConfig(cfgBuf);
        if(err!=KErrNone && err!=KErrInUse)
            {
            iReady = EFalse;
            return(err);
            }
        }
    
    return err;
    }

TInt CSerialWriter::Disconnect()
    {
    if(iReady)
        {
	    iReady = EFalse;
        iComm.Close();
        }
    return KErrNone;
    }

TInt CSerialWriter::Write(const TDesC8& aData)
	{
	TInt err = KErrNone;
	
	if(iReady)
		{
		TRequestStatus writeStatus;
	    iComm.Write(writeStatus, aData);
    	User::WaitForRequest(writeStatus);
    	err = writeStatus.Int();  	
		}

   	return err;
	}

void CSerialWriter::AddTime(TDes8& aLogBuffer)
	{
	TTime now;
	now.UniversalTime();
	TDateTime dateTime = now.DateTime();
	_LIT8(KFormat,"%02d:%02d:%02d:%03d ");
	// add the current time 
	aLogBuffer.Append(KTEFNewLine) ; 
	aLogBuffer.AppendFormat(KFormat,dateTime.Hour(),dateTime.Minute(),dateTime.Second(),(dateTime.MicroSecond()/1000)); 
	}

void CSerialWriter::AddSeverity(TDes8& aLogBuffer, const TInt& aSeverity)
	{
	if (aSeverity < 1) 
		{
		return;
		}

	_LIT(KErr,"ERROR - ");
	_LIT(KHigh,"HIGH - ");
	_LIT(KWarn,"WARN - ");
	_LIT(KMedium,"MEDIUM - ");
	_LIT(KInfo,"INFO - ");
	_LIT(KLow,"LOW - ");

	if(aSeverity == ESevrErr)
		aLogBuffer.Append(KErr);
	else if(aSeverity == ESevrHigh)
		aLogBuffer.Append(KHigh);
	else if(aSeverity == ESevrWarn)
		aLogBuffer.Append(KWarn);
	else if(aSeverity == ESevrMedium)
		aLogBuffer.Append(KMedium);
	else if (aSeverity == ESevrInfo)
		aLogBuffer.Append(KInfo);
	else if(aSeverity == ESevrLow)
		aLogBuffer.Append(KLow);
	else //if(aSeverity == ESevrAll)
		aLogBuffer.Append(KInfo);

	}


/**
Decorates the logs with Current Time and Severity 
@returns Error code if any during writing
@leave KErrNoMemory if no memory
*/
TInt CSerialWriter::WriteDecorated(const TDesC8& aText, TInt aSeverity)	
{
	TInt ret = KErrNone; 	
	HBufC8* buffer = HBufC8::New(aText.Length()+30);
	if(buffer)
		{
		TPtr8 ptr(buffer->Des());
		AddTime(ptr);
		AddSeverity(ptr,aSeverity) ; 
		ptr.Append(aText);
		// Ignore error for the time being. Could do an ASSERT
		ret = Write(ptr) ; 
		delete buffer;
		}
	return ret ; 	
}

/**
  Overloaded for TDesC
  @returns Error code if any during writing
  @leave KErrNoMemory if no memory
  */
  TInt CSerialWriter::WriteDecorated(const TDesC& aText, TInt aSeverity)
  {
  	TPtrC8 representation((TUint8*)(&aText)->Ptr(), (&aText)->Size());
  	return WriteDecorated(representation,  aSeverity )	;
  }


TInt CSerialWriter::Settings(const TInt& aPortNumber)
	{
	  	iPortNum = aPortNumber ; 
	  	Disconnect();
	  	TInt err = Connect();
	  	if(err != KErrNone)
	  		return err;
	  	err = Config();
	  	return err;
	}
