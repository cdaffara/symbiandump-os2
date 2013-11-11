// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "uloggerserialplugin.h"

#if defined(__LIGHTLOGGER_ENABLED)
#include "lightlogger.h" 
#endif

namespace Ulogger
{

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

//settings for serial output plugin
_LIT(KPortNumber, "output_port");

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

void CSerialWriter::CloseOutputPlugin()
	{
    Disconnect();
	}

TInt CSerialWriter::ConfigureOutputPlugin(const RPointerArray<TPluginConfiguration>& aSettings)
	{
	TInt i = aSettings.Count();
	while(i-->0)
		{
		TPluginConfiguration* key = aSettings[i];
	    
		if(key->Key().Compare(KPortNumber) == 0)
			{
			TLex lex(key->Value());
			lex.Val(iPortNum);
			}
		}

	Disconnect();
	TInt err = Connect();
	if(err != KErrNone)
		return err;
	err = Config();

	return KErrNone;
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


TAny* CSerialWriter::GetInterfaceL(TPluginInterface aInterfaceId)
	{
	if(aInterfaceId == MOutputPlugin::iInterfaceId)
		return static_cast<MOutputPlugin*>(this);
	else
		return NULL;
	}

} // namespace


//Ecom Implementations
// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102836CB,  Ulogger::CSerialWriter::NewL)
    };


// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
    TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

