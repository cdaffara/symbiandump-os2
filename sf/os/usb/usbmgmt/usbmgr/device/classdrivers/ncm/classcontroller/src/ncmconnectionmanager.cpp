/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * @file
 * @internalComponent
 */

#ifndef OVERDUMMY_NCMCC
#include <commdbconnpref.h>
#include <es_enum.h>
#endif // OVERDUMMY_NCMCC

#include <usb/usbncm.h>
#include <nifvar.h>
#include <cdbcols.h>
#include <commdb.h>

#include "ncmconnectionmanager.h"
#include "ncmiapprogresswatcher.h"
#include "ncmiapreader.h"
#include "ncmdhcpnotifwatcher.h"

#ifdef OVERDUMMY_NCMCC
#include <usb/testncmcc/dummy_ncminternalsrv.h>
#else
#include "ncminternalsrv.h"
#endif // OVERDUMMY_NCMCC

#include "ncmclasscontroller.h"
#include "ncmconnectionmanagerobserver.h"
#include "ncmconnectionmanagerhelper.h"

// For OST tracing
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmconnectionmanagerTraces.h"
#endif

using namespace UsbNcm;

#if defined(_DEBUG)
_LIT(KNcmConnManagerPanic, "UsbNcmCM"); // must be <=16 chars
#endif

extern const TUint KEthernetFrameSize;

// Panic codes
enum TNcmCMPanicCode
	{
	ENcmCMPanicOutstandingRequestFromDevice = 1,
    ENcmCMPanicNoRegisteredWatcher = 2,
	ENcmCMEndMark
	};

/**
 * Constructs a CNcmConnectionManager object.
 * @param[in]   aObserver, NCM connection observer
 * @param[in]   aHostMacAddress, the NCM interface MAC address
 * @param[in]   aDataBufferSize, the EndPoint buffer size
 * @param[in]   aNcmInternalSvr, the NCM internal server
 * @return      Ownership of a new CNcmConnectionManager object
 */
CNcmConnectionManager* CNcmConnectionManager::NewL(
        MNcmConnectionManagerObserver& aObserver,
        const TNcmMacAddress& aMacAddress, TUint aDataBufferSize,
        RNcmInternalSrv&   aNcmInternalSvr)
	{	
	OstTraceFunctionEntry0(CNCMCONNECTIONMANAGER_NEWL_ENTRY);
	
	CNcmConnectionManager* self = new (ELeave) CNcmConnectionManager(aObserver,
            aMacAddress, aDataBufferSize, aNcmInternalSvr);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	OstTraceFunctionExit0(CNCMCONNECTIONMANAGER_NEWL_EXIT);
	return self;
	}

/**
 * Method to perform second phase construction.
 */
void CNcmConnectionManager::ConstructL()
	{
	OstTraceFunctionEntry0(CNCMCONNECTIONMANAGER_CONSTRUCTL_ENTRY);
	
	TInt err = RProperty::Define(KUsbmanSvrUid, KKeyNcmSharedState, RProperty::EInt,
		KAllowAllPolicy, KCommDDPolicy);
    
    if (KErrNone != err)
        {
        OstTrace1( TRACE_ERROR, CNCMCONNECTIONMANAGER_CONSTRUCTL, "Define NCM shared state P&S key fail: err=%d", err );
        User::Leave(err);
        }
	
	User::LeaveIfError(iProperty.Attach(KUsbmanSvrUid, KKeyNcmSharedState));
	User::LeaveIfError(iProperty.Set(ENcmStateIntial));

	User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iConnection.Open(iSocketServ));

	iIapPrgrsWatcher = new (ELeave) CNcmIapProgressWatcher(*this, iConnection);
	iConnManHelper = new  (ELeave) CNcmConnectionManHelper(*this, iConnection);
	iDhcpNotifWatcher = new  (ELeave) CNcmDhcpNotifWatcher(*this, iNcmInternalSvr);
	iIapReader = new  (ELeave) CNcmIapReader(*this, iConnPref);
	
	OstTraceFunctionExit0(CNCMCONNECTIONMANAGER_CONSTRUCTL_EXIT);
	}

/**
 * Called when NCM connection starting attempt completed
 */
void CNcmConnectionManager::HandleConnectionCompleteL()
    {
    OstTraceFunctionEntry0( CNCMCONNECTIONMANAGER_HANDLECONNECTIONCOMPLETEL_ENTRY );
    OstTrace0(TRACE_NORMAL, CNCMCONNECTIONMANAGER_HANDLECONNECTIONCOMPLETEL_CHANGE_MTU, "About to change default MTU size!");
    // Change default MTU size
    SetCustomMtuL();

    OstTrace0(TRACE_NORMAL, CNCMCONNECTIONMANAGER_HANDLECONNECTIONCOMPLETEL_CONN_NCM_INTERNAL_SVR, "About to connect to internal NCM server!");
    
    // Connect to internal server in packet driver            
    User::LeaveIfError(iNcmInternalSvr.Connect());

    iNcmInternalSvr.TransferBufferSize(iDataBufferSize);
    iNcmInternalSvr.SetIapId(iConnPref.IapId());
    iDhcpNotifWatcher->StartL();
    OstTraceFunctionExit0( CNCMCONNECTIONMANAGER_HANDLECONNECTIONCOMPLETEL_EXIT );
    }

/**
 * Destructor.
 */
CNcmConnectionManager::~CNcmConnectionManager()
	{
	OstTraceFunctionEntry0(CNCMCONNECTIONMANAGER_CNCMCONNECTIONMANAGER_DESTRUCTOR_ENTRY);
	
	delete iIapReader;
    delete iDhcpNotifWatcher;
	delete iConnManHelper;
	delete iIapPrgrsWatcher;
	
	iConnection.Close();
	iSocketServ.Close();
	iProperty.Close();
	
    RProperty::Delete(KKeyNcmSharedState);
    
	OstTraceFunctionExit0(CNCMCONNECTIONMANAGER_CNCMCONNECTIONMANAGER_DESTRUCTOR_EXIT);
	}

/**
 * Constructor.
 * @param[in]   aObserver, NCM connection observer
 * @param[in]   aHostMacAddress, the NCM interface MAC address
 * @param[in]   aDataBufferSize, the EndPoint buffer size
 * @param[in]   aNcmInternalSvr, the NCM internal server
 */
CNcmConnectionManager::CNcmConnectionManager(MNcmConnectionManagerObserver& aObserver, 
        const TNcmMacAddress& aHostMacAddress, TUint aDataBufferSize,
        RNcmInternalSrv&   aNcmInternalSvr):
        iConnMgrObserver(aObserver),
        iHostMacAddress(aHostMacAddress), iDataBufferSize(aDataBufferSize),
        iNcmInternalSvr(aNcmInternalSvr)
	{
	// No implementation needed.
	}

/**
 *Called by class controller to start the connection building process.
 *@param    aStatus, reference to ncm class controller's iStatus 
 */
void CNcmConnectionManager::Start(TRequestStatus& aStatus)
	{
	OstTraceFunctionEntry0(CNCMCONNECTIONMANAGER_START_ENTRY);
	
	__ASSERT_DEBUG(!iReportStatus, 
		User::Panic(KNcmConnManagerPanic, ENcmCMPanicOutstandingRequestFromDevice));
	
	aStatus = KRequestPending;
	iReportStatus = &aStatus;

	Reset();
	
	// Preparing IAP information	
	iIapReader->Start();
	OstTraceFunctionExit0(CNCMCONNECTIONMANAGER_START_EXIT);
	}

/**
 * Cancel ongoing starting attempt
 */
void CNcmConnectionManager::StartCancel()
    {
    OstTraceFunctionEntry0(CNCMCONNECTIONMANAGER_STARTCANCEL_ENTRY);
    
    __ASSERT_DEBUG(iReportStatus, 
        User::Panic(KNcmConnManagerPanic, ENcmCMPanicNoRegisteredWatcher));
    
    Stop();
    
    User::RequestComplete(iReportStatus, KErrCancel);
    
    OstTraceFunctionExit0(CNCMCONNECTIONMANAGER_STARTCANCEL_EXIT);
    }

/**
 * Called by class controller to stop the connection.
 */
void CNcmConnectionManager::Stop()
	{
    OstTraceFunctionEntry0(CNCMCONNECTIONMANAGER_STOP_ENTRY);
    
    // Cancel any current ongoing operations
    Reset();
    
    // Reset shared state
    iProperty.Set(ENcmStateIntial);
	
	iConnManHelper->Stop();
	OstTraceFunctionExit0(CNCMCONNECTIONMANAGER_STOP_EXIT);
	}

/**
 * Reset conneciton manager itself
 */
void CNcmConnectionManager::Reset()
	{
    OstTraceFunctionEntry0(CNCMCONNECTIONMANAGER_RESET_ENTRY);
    
	iConnComplete = EFalse;
	
	iIapReader->Cancel();
	iDhcpNotifWatcher->Cancel();
	iIapPrgrsWatcher->Cancel();
	iConnManHelper->Cancel();
	
	OstTraceFunctionExit0(CNCMCONNECTIONMANAGER_RESET_EXIT);
	}
/**
 * Change the default MTU size on NCM networking connection
 */
void CNcmConnectionManager::SetCustomMtuL()
    {    
    OstTraceFunctionEntry0(CNCMCONNECTIONMANAGER_SETCUSTOMMTUL_ENTRY);
    
    RSocket socket;
    User::LeaveIfError(socket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp,
            iConnection));

    TPckgBuf<TSoInet6InterfaceInfo> interfaceInfo;
    TPckgBuf<TConnInterfaceName> interfaceName;
    
    TUint cnt = 0;
    iConnection.EnumerateConnections(cnt); // Count all underlying interfaces
    
    TUint iap = iConnPref.IapId();
    TInt index = 1;
    for (index = 1; index <= cnt; index++)
        {
        TConnectionInfoBuf info;
        iConnection.GetConnectionInfo(index, info);
        if (info().iIapId == iap)
            {
            break;
            }
        }
    
    if (index > cnt)
        {
        OstTrace0( TRACE_NORMAL, CNCMCONNECTIONMANAGER_SETCUSTOMMTUL, "Can not find an interface based on NCM!" );
        User::Leave(KErrNotFound);
        }
    
    interfaceName().iIndex = index;    
    OstTraceExt2( TRACE_NORMAL, CNCMCONNECTIONMANAGER_SETCUSTOMMTUL_INTER_INFO, ";cnt=%d;interfaceName().iIndex=%u", cnt, interfaceName().iIndex );
    User::LeaveIfError(iConnection.Control(KCOLProvider, KConnGetInterfaceName, interfaceName));
    OstTraceExt3( TRACE_NORMAL, CNCMCONNECTIONMANAGER_SETCUSTOMMTUL_NEW_MTU_SIZE, ";Interface Name=%S, ;current MTU=%d. MTU size will be changed to %d!", interfaceName().iName, interfaceInfo().iMtu, KEthernetFrameSize);
        
    TInt err = socket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl);
    if (err != KErrNone)
        {
        OstTrace1(TRACE_NORMAL, CNCMCONNECTIONMANAGER_SETCUSTOMMTUL_START_INTER_ENUM, "Failed to set KSoInetEnumInterfaces option [%d]", err);
        User::LeaveIfError(err);              
        }
        
    err = KErrNotFound;
    while (socket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, interfaceInfo) == KErrNone)
        {
        OstTraceExt1(TRACE_NORMAL, CNCMCONNECTIONMANAGER_SETCUSTOMMTUL_INTER_NAME, "InterfaceInfo().iName = %S", interfaceInfo().iName);
        OstTrace1(TRACE_NORMAL, CNCMCONNECTIONMANAGER_SETCUSTOMMTUL_INTER_MTU, "InterfaceInfo().iMtu = %d", interfaceInfo().iMtu);
        if (interfaceInfo().iName == interfaceName().iName)
            {
            // found the interface
            err = KErrNone;
            break;
            }
        }
    if (KErrNone != err)
        {
        OstTrace1(TRACE_ERROR, CNCMCONNECTIONMANAGER_SETCUSTOMMTUL_SEARCH_FAIL, "Can not find NCM connection: err=%x", err);
        User::LeaveIfError(err);
        }
    
    // Set new MTU size
    TPckgBuf<TSoInet6InterfaceInfo> newinterfaceInfo;
    newinterfaceInfo().iMtu = KEthernetFrameSize;	
    newinterfaceInfo().iDoPrefix = 0;
    newinterfaceInfo().iDoId = 0;
    newinterfaceInfo().iDoState = 0;
    newinterfaceInfo().iDoAnycast = 0;
    newinterfaceInfo().iDoProxy = 0;
    newinterfaceInfo().iAlias = interfaceInfo().iAlias;
    newinterfaceInfo().iDelete = interfaceInfo().iDelete;
    newinterfaceInfo().iState = interfaceInfo().iState;
    newinterfaceInfo().iSpeedMetric = interfaceInfo().iSpeedMetric;
    newinterfaceInfo().iFeatures = interfaceInfo().iFeatures;
    newinterfaceInfo().iTag = interfaceInfo().iTag;
    newinterfaceInfo().iName = interfaceInfo().iName;
    newinterfaceInfo().iSpeedMetric = interfaceInfo().iSpeedMetric;
    newinterfaceInfo().iFeatures = interfaceInfo().iFeatures;	
    newinterfaceInfo().iHwAddr = interfaceInfo().iHwAddr;
    newinterfaceInfo().iAddress = interfaceInfo().iAddress;
    newinterfaceInfo().iNetMask = interfaceInfo().iNetMask;
    newinterfaceInfo().iDefGate = interfaceInfo().iDefGate;
    newinterfaceInfo().iNameSer2 = interfaceInfo().iNameSer2;
    newinterfaceInfo().iNameSer1 = interfaceInfo().iNameSer1;
	
    OstTrace0( TRACE_NORMAL, CNCMCONNECTIONMANAGER_SETCUSTOMMTUL_PRE_SETMTU, "About to change the default MTU size." );
	User::LeaveIfError(socket.SetOpt(KSoInetConfigInterface, KSolInetIfCtrl, newinterfaceInfo));

    socket.Close();
    
    OstTraceFunctionExit0(CNCMCONNECTIONMANAGER_SETCUSTOMMTUL_EXIT);
    }

/**
 * Called by connection watcher when it detects the class fatal error.
 */
void CNcmConnectionManager::MipsoHandleClassFatalError(TInt aError)
	{
	OstTraceFunctionEntryExt( CNCMCONNECTIONMANAGER_MIPSOHANDLECLASSFATALERROR_ENTRY, this );
	
	Reset();
	
	__ASSERT_DEBUG(!iReportStatus, User::Panic(KNcmConnManagerPanic, ENcmCMPanicOutstandingRequestFromDevice));
		
	OstTrace1(TRACE_ERROR, CNCMCONNECTIONMANAGER_MIPSOHANDLECLASSFATALERROR, "NCM connection fatal error occurs: %d", aError);
	
	iConnMgrObserver.McmoErrorIndication(aError);
	
	OstTraceFunctionExit1( CNCMCONNECTIONMANAGER_MIPSOHANDLECLASSFATALERROR_EXIT, this );
	}

/**
 * Called by connection help when it get notification the 
 * connection over NCM has been established.
 */
void CNcmConnectionManager::MicoHandleConnectionComplete(TInt aError)
    {
    OstTraceFunctionEntryExt( CNCMCONNECTIONMANAGER_MICOHANDLECONNECTIONCOMPLETE_ENTRY, this );
    
    // Reset shared state
    iProperty.Set(ENcmStateIntial);
    
    if (KErrNone == aError)
        {
        TRAPD(err, HandleConnectionCompleteL());
        User::RequestComplete(iReportStatus, err);
        }
    else
        {
        // Leave if connection build failed
        User::RequestComplete(iReportStatus, aError);
        }
    
    OstTraceFunctionExit1( CNCMCONNECTIONMANAGER_MICOHANDLECONNECTIONCOMPLETE_EXIT, this );
    }

/**
 * Called by DHCP notification watcher.
 * When the NCM internal server send DHCP request notification to 
 * the watcher, watcher call this function to do DHCP provisioning
 * for NCM connection.
 */
void CNcmConnectionManager::MdnoHandleDhcpNotification(TInt aError)
    {
    OstTraceFunctionEntryExt( CNCMCONNECTIONMANAGER_MDNOHANDLEDHCPNOTIFICATION_ENTRY, this );
    if (KErrNone != aError)
        {
        // Report this failure to the observer
        // Finally this will report to usbman and NCM class will be stopped.
        iConnMgrObserver.McmoErrorIndication(aError);
        OstTraceFunctionExit1( CNCMCONNECTIONMANAGER_MDNOHANDLEDHCPNOTIFICATION_EXIT, this );
        return;
        }
    
    // Doing DHCP for NCM
    aError = KErrNone;

    // Configuring DHCP server to assign the IP address to PC(host)
    TRequestStatus stat;
    OstTrace0(TRACE_NORMAL, CNCMCONNECTIONMANAGER_MDNOHANDLEDHCPNOTIFICATION_CALL_IOCTL, "About to call to Ioctl()");
    
    TNcmMacAddress macAddr = iHostMacAddress;
    iConnection.Ioctl(KCOLConfiguration, KConnDhcpSetHwAddressParams, stat,
            &macAddr);
    User::WaitForRequest(stat);
    aError = stat.Int();
    OstTrace1( TRACE_NORMAL, CNCMCONNECTIONMANAGER_MDNOHANDLEDHCPNOTIFICATION_RETURN_IOCTL, "Return from Ioctl:aError=%d", aError );
    
    if (KErrNone == aError)
        {
        // Disable timer on interface and wont let interface coming down due to timer expiry.
        iConnection.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
        iIapPrgrsWatcher->Start();
        }
    else
        {
        // Silent stop the NCM connection. 
        iConnManHelper->Stop();
        
        // Report this failure to the observer
        // Finally this will report to usbman and NCM class will be stopped.
        iConnMgrObserver.McmoErrorIndication(aError);        
        }
    
    // Set DHCP result to NCM internal server
    iNcmInternalSvr.SetDhcpResult(aError);
    OstTraceFunctionExit1( CNCMCONNECTIONMANAGER_MDNOHANDLEDHCPNOTIFICATION_EXIT_DUP1, this );
    }

/**
 * Called by IAP reader.
 * When IAP reader finished reading NCM IAP preference from CentRep,
 * it will call this function to start building ethernet connection
 * using NCM IAP configuration.
 */
void CNcmConnectionManager::MicoHandleIAPReadingComplete(TInt aError)
    {
    OstTraceFunctionEntryExt( CNCMCONNECTIONMANAGER_MICOHANDLEIAPREADINGCOMPLETE_ENTRY, this );
    if (KErrNone != aError)      
      {
      OstTrace1( TRACE_ERROR, CNCMCONNECTIONMANAGER_MICOHANDLEIAPREADINGCOMPLETE_IAP_RESULT, "IAP reading failed with error;aError=%d", aError );
      User::RequestComplete(iReportStatus, aError);
      
      OstTraceFunctionExit1( CNCMCONNECTIONMANAGER_MICOHANDLEIAPREADINGCOMPLETE_EXIT, this );
      return;
      }
    
    aError = iProperty.Set(ENcmStartRequested);
    
    if (KErrNone != aError)
      {
      OstTrace1( TRACE_ERROR, CNCMCONNECTIONMANAGER_MICOHANDLEIAPREADINGCOMPLETE_SET_SHARED_STATE, "Property set failed with error:;aError=%d", aError );
      User::RequestComplete(iReportStatus, aError);

      OstTraceFunctionExit1( CNCMCONNECTIONMANAGER_MICOHANDLEIAPREADINGCOMPLETE_EXIT_DUP1, this );
      return;
      }

    iConnManHelper->Start(iConnPref);
    OstTraceFunctionExit1( CNCMCONNECTIONMANAGER_MICOHANDLEIAPREADINGCOMPLETE_EXIT_DUP2, this );
    }
