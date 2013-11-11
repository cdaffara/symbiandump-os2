/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cmn.h>
#include <e32debug.h>

#include "usbchargingarmtest.h"
#include "usbchargingpublishedinfo.h"
#include "testmanager.h"
#include "usbstatewatcher.h"
#include "usbchargingcurrentwatcher.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "usbchargingarmtestTraces.h"
#endif

_LIT(KMsFsy, "MSFS.FSY");


CUsbChargingArmTest::CUsbChargingArmTest(MTestManager& aManager)
 :	CTestBase(aManager)
	{	
	OstTraceFunctionEntry0( DUP1_CUSBCHARGINGARMTEST_CUSBCHARGINGARMTEST_ENTRY );
	OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_CUSBCHARGINGARMTEST_ENTRY );
	OstTraceFunctionExit0( CUSBCHARGINGARMTEST_CUSBCHARGINGARMTEST_EXIT );	
	OstTraceFunctionExit0( DUP1_CUSBCHARGINGARMTEST_CUSBCHARGINGARMTEST_EXIT );
	}

CTestBase* CUsbChargingArmTest::NewL(MTestManager& aManager)
	{
	OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_NEWL_ENTRY );
	CUsbChargingArmTest* self = new(ELeave) CUsbChargingArmTest(aManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBCHARGINGARMTEST_NEWL_EXIT );
	return self;
	}


int CUsbChargingArmTest::StartMassStorage()
	{
    OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_STARTMASSSTORAGE_ENTRY );
    TInt r = KErrNone;
    OstTrace0( TRACE_NORMAL, CUSBCHARGINGARMTEST_STARTMASSSTORAGE, "CUsbChargingArmTest::StartMassStorage AddFileSystem" );
	iManager.Write(_L8("Start Mass Storage"));
	r = iFs.Connect();
    if (r != KErrNone)
        {
        OstTrace0( TRACE_ERROR, DUP1_CUSBCHARGINGARMTEST_STARTMASSSTORAGE, "CUsbChargingArmTest::StartMassStorage Connect to fs failed" );
        iManager.Write(_L8("Connect to Fs failed"));    
        OstTraceFunctionExit0( CUSBCHARGINGARMTEST_STARTMASSSTORAGE_EXIT );
        return r;
        }
	r = iFs.AddFileSystem(KMsFsy);
	if (r != KErrNone)
		{
		OstTrace0( TRACE_ERROR, DUP2_CUSBCHARGINGARMTEST_STARTMASSSTORAGE, "CUsbChargingArmTest::StartMassStorage AddFileSystem failed" );
		iManager.Write(_L8("AddFileSystem failed"));	
		OstTraceFunctionExit0( DUP1_CUSBCHARGINGARMTEST_STARTMASSSTORAGE_EXIT );
		return r;
		}

	OstTrace0( TRACE_NORMAL, DUP3_CUSBCHARGINGARMTEST_STARTMASSSTORAGE, "CUsbChargingArmTest::StartMassStorage AddFileSystem success" );
	iManager.Write(_L8("AddFileSystem success"));			
	OstTraceFunctionExit0( DUP3_CUSBCHARGINGARMTEST_STARTMASSSTORAGE_EXIT );
	return r;
	}


void CUsbChargingArmTest::ConstructL()
	{
	OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_CONSTRUCTL_ENTRY );
	TInt res;
	res = StartMassStorage();
	if(res != KErrNone)
		{
		iManager.Write(_L8("start KMsFsy failed"));
		return;
		}
	else
		{
		iManager.Write(_L8("start KMsFsy success"));
		}

	// -- Insert initialization code here
	iManager.Write(_L8("About to connect to USBMan"));
	res = iUsbMan.Connect();
	if(res != KErrNone)
		{
		iManager.Write(_L8("Connection to USBMan failed"));
		return;
		}
	else
		{
		iManager.Write(_L8("Cconnected to USBMan success"));
		}
	iChargingCurrentWatcher = CUsbChargingCurrentWatcher::NewL(*this);

#ifndef	COMMANDLINE_PARAM	
	iManager.Write(_L8("About to Set Control session"));
	res = iUsbMan.SetCtlSessionMode(ETrue);
	if(res != KErrNone)
		{
		iManager.Write(_L8("Set Control session failed"));
		return;
		}
	else
		{
		iManager.Write(_L8("Set Control session Success"));
		}
	
	iManager.Write(_L8("About to start USBMan"));
	TRequestStatus requestStatus;
	iUsbMan.TryStart(1, requestStatus);
	User::WaitForRequest(requestStatus);
	if(requestStatus.Int() == KErrNone)
		{
		iManager.Write(_L8("USBMan Started Success"));
	    iUsbStateWatcher = CUsbStateWatcher::NewL(*this);
		}
	else
		{
		iManager.Write(_L8("Failed to start USBMan"));
		}
#endif	
	OstTraceFunctionExit0( CUSBCHARGINGARMTEST_CONSTRUCTL_EXIT );
	}

CUsbChargingArmTest::~CUsbChargingArmTest()
	{
	OstTraceFunctionEntry0( DUP2_CUSBCHARGINGARMTEST_CUSBCHARGINGARMTEST_ENTRY );

	// -- Insert cleanup code here
	delete iChargingCurrentWatcher;
	delete iUsbStateWatcher;
	iUsbMan.Close();
	iFs.Close();
	OstTraceFunctionExit0( DUP2_CUSBCHARGINGARMTEST_CUSBCHARGINGARMTEST_EXIT );
	}

void CUsbChargingArmTest::ProcessKeyL(TKeyCode aKeyCode)
	{
    OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_PROCESSKEYL_ENTRY );
    TInt res;
	iManager.Write(_L8("CUsbChargingArmTest::ProcessKeyL"));

	switch ( aKeyCode )
		{
	case '1':
		{
	    iManager.Write(_L8("About to Set Control session"));
	    res = iUsbMan.SetCtlSessionMode(ETrue);
	    if(res != KErrNone)
	        {
	        iManager.Write(_L8("Set Control session failed"));
	        return;
	        }
	    else
	        {
	        iManager.Write(_L8("Set Control session Success"));
	        }
	    
		iManager.Write(_L8("About to start USBMan"));
		TRequestStatus requestStatus;
		iUsbMan.Start(requestStatus);
		User::WaitForRequest(requestStatus);
		if(requestStatus.Int() == KErrNone)
			{
			iManager.Write(_L8("USBMan Started OK"));
			if(!iChargingCurrentWatcher)
			    {
			    iChargingCurrentWatcher = CUsbChargingCurrentWatcher::NewL(*this);
			    }
			if(!iUsbStateWatcher)
			    {
			    iUsbStateWatcher = CUsbStateWatcher::NewL(*this);
			    }			
			}
		else
			{
			iManager.Write(_L8("Failed to start USBMan"));
			}
		}
		break;
	case '2':
		{
		iManager.Write(_L8("About to stop USBMan"));
		TRequestStatus requestStatus;
		iUsbMan.Stop(requestStatus);
		User::WaitForRequest(requestStatus);
		if(requestStatus.Int() == KErrNone)
			{
			iManager.Write(_L8("USBMan Stopped OK"));
			}
		else
			{
			iManager.Write(_L8("Failed to stop USBMan"));
			}
		}
		break;


	default:
		iManager.Write(_L8("Unknown selection"));
		break;
		}
	OstTraceFunctionExit0( CUSBCHARGINGARMTEST_PROCESSKEYL_EXIT );
	}

void CUsbChargingArmTest::DisplayTestSpecificMenu()
	{
	OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_DISPLAYTESTSPECIFICMENU_ENTRY );
	OstTraceFunctionExit0( CUSBCHARGINGARMTEST_DISPLAYTESTSPECIFICMENU_EXIT );
	}

void CUsbChargingArmTest::ShowUsbChargingValue(TPublishedUsbChargingInfo& aValue)
    {
    OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_SHOWUSBCHARGINGVALUE_ENTRY );
    iManager.Write(_L8("=====Published Current Info====="));
    ShowChargingType(aValue.iChargingPortType);
    ShowConnStatus(aValue.iUsbConnStatus);
    iManager.Write(_L8("MinAvailableVbusCurrent : %d"), aValue.iMinAvailableVbusCurrent);
    iManager.Write(_L8("MaxVbusCurrent          : %d"), aValue.iMaxVbusCurrent);
    iManager.Write(_L8("MinVbusVoltage          : %d"), aValue.iMinVbusVoltage);
    iManager.Write(_L8("===Published Current Info End==="));        

    OstTrace0( TRACE_NORMAL, CUSBCHARGINGARMTEST_SHOWUSBCHARGINGVALUE_DUP1, "CUsbChargingArmTest::ShowUsbChargingValue =====Published Current Info=====" );    
    OstTrace1( TRACE_NORMAL, CUSBCHARGINGARMTEST_SHOWUSBCHARGINGVALUE_DUP2, "CUsbChargingArmTest::ShowUsbChargingValue;aValue.iChargingPortType=%d", aValue.iChargingPortType );
    OstTrace1( TRACE_NORMAL, CUSBCHARGINGARMTEST_SHOWUSBCHARGINGVALUE_DUP3, "CUsbChargingArmTest::ShowUsbChargingValue;aValue.iUsbConnStatus=%d", aValue.iUsbConnStatus );
    OstTrace1( TRACE_NORMAL, CUSBCHARGINGARMTEST_SHOWUSBCHARGINGVALUE_DUP4, "CUsbChargingArmTest::ShowUsbChargingValue;aValue.iMinAvailableVbusCurrent=%u", aValue.iMinAvailableVbusCurrent );
    OstTrace1( TRACE_NORMAL, CUSBCHARGINGARMTEST_SHOWUSBCHARGINGVALUE_DUP5, "CUsbChargingArmTest::ShowUsbChargingValue;aValue.iMaxVbusCurrent=%u", aValue.iMaxVbusCurrent );
    OstTrace1( TRACE_NORMAL, CUSBCHARGINGARMTEST_SHOWUSBCHARGINGVALUE_DUP6, "CUsbChargingArmTest::ShowUsbChargingValue;aValue.iMinVbusVoltage=%u", aValue.iMinVbusVoltage );
    OstTrace0( TRACE_NORMAL, CUSBCHARGINGARMTEST_SHOWUSBCHARGINGVALUE_DUP7, "CUsbChargingArmTest::ShowUsbChargingValue ===Published Current Info End===" );    
     
    OstTraceFunctionExit0( CUSBCHARGINGARMTEST_SHOWUSBCHARGINGVALUE_EXIT );
    }

void CUsbChargingArmTest::ShowUsbDeviceState(TUsbDeviceState aDeviceState)
    {
    OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_SHOWUSBDEVICESTATE_ENTRY );
    TPtrC res(NULL, 0);
    
    _LIT(KUndefined, "Undefined");
    _LIT(KDefault, "Default");
    _LIT(KAttached, "Attached");
    _LIT(KPowered, "Powered");
    _LIT(KConfigured, "Configured");
    _LIT(KAddress, "Address");
    _LIT(KSuspended, "Suspended");
    _LIT(KUnKnown, "UnKnown");  
      
    switch(aDeviceState)
        {
    case EUsbDeviceStateUndefined:
        res.Set(KUndefined);
        break;
    case EUsbDeviceStateDefault:
        res.Set(KDefault);
        break;
    case EUsbDeviceStateAttached:
        res.Set(KAttached);
        break;
    case EUsbDeviceStatePowered:
        res.Set(KPowered);
        break;
    case EUsbDeviceStateConfigured:
        res.Set(KConfigured);
        break;
    case EUsbDeviceStateAddress:
        res.Set(KAddress);
        break;
    case EUsbDeviceStateSuspended:
        res.Set(KSuspended);
        break;
    default:
        res.Set(KUnKnown);
        break;
        }
        
    iManager.Write(_L8("USBDevice State is: %S"), &res);    
    OstTraceFunctionExit0( CUSBCHARGINGARMTEST_SHOWUSBDEVICESTATE_EXIT );
    }

void CUsbChargingArmTest::ShowChargingType(TUsbChargingPortType aChargingType)
    {
    OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_SHOWCHARGINGTYPE_ENTRY );
    TPtrC res(NULL, 0);

    _LIT(KPortTypeNone, "None");
    _LIT(KPortTypeUnsupported, "Unsupported");
    _LIT(KPortTypeChargingPort, "ChargingPort");
    _LIT(KPortTypeDedicatedChargingPort, "DedicatedChargingPort");
    _LIT(KPortTypeStandardDownstreamPort, "StandardDownstreamPort");
    _LIT(KPortTypeChargingDownstreamPort, "ChargingDownstreamPort");
    _LIT(KPortTypeAcaRidA, "RidAAca");
    _LIT(KPortTypeAcaRidB, "RidBAca");
    _LIT(KPortTypeAcaRidC, "RidCAca");
    _LIT(KUnKnown, "UnKnown");  

    switch (aChargingType)
        {
        case EUsbChargingPortTypeNone:
            res.Set(KPortTypeNone);
            break;
        case EUsbChargingPortTypeUnsupported:
            res.Set(KPortTypeUnsupported);
            break;
        case EUsbChargingPortTypeChargingPort:  
            res.Set(KPortTypeChargingPort);
            break;
        case EUsbChargingPortTypeDedicatedChargingPort:
            res.Set(KPortTypeDedicatedChargingPort);
            break;              
        case EUsbChargingPortTypeChargingDownstreamPort:
            res.Set(KPortTypeChargingDownstreamPort);
            break;
        case EUsbChargingPortTypeStandardDownstreamPort:
            res.Set(KPortTypeStandardDownstreamPort);
            break;
        case EUsbChargingPortTypeAcaRidA:
            res.Set(KPortTypeAcaRidA);
            break;
        case EUsbChargingPortTypeAcaRidB:
            res.Set(KPortTypeAcaRidB);
            break;
        case EUsbChargingPortTypeAcaRidC:
            res.Set(KPortTypeAcaRidC);
            break;
        default:
            res.Set(KUnKnown);
            break;
        }
    iManager.Write(_L8("ChargingPortType: %S"), &res);    
    OstTraceFunctionExit0( CUSBCHARGINGARMTEST_SHOWCHARGINGTYPE_EXIT );
    }

void CUsbChargingArmTest::ShowConnStatus(TUsbConnectionStatus aStatus)
    {
    OstTraceFunctionEntry0( CUSBCHARGINGARMTEST_SHOWCONNSTATUS_ENTRY );
    TPtrC res(NULL, 0);
    
    _LIT(KNone, "None");
    _LIT(KConfigured, "Configured");
    _LIT(KSuspended, "Suspend");
    _LIT(KUnKnown, "UnKnown");  
      
    switch(aStatus)
        {
    case EUsbConnectionStatusNone:
        res.Set(KNone);
        break;
    case EUsbConnectionStatusSuspend:
        res.Set(KSuspended);
        break;
    case EUsbConnectionStatusConfigured:
        res.Set(KConfigured);
        break;
    default:
        res.Set(KUnKnown);
        break;
        }
        
    iManager.Write(_L8("ConnectionStatus: %S"), &res);    
    OstTraceFunctionExit0( CUSBCHARGINGARMTEST_SHOWCONNSTATUS_EXIT );
    }    
    
