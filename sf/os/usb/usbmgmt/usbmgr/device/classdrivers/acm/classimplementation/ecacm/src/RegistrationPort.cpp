/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <usb/acmserver.h>
#include "RegistrationPort.h"
#include "AcmConstants.h"
#include "AcmUtils.h"
#include "acmcontroller.h"
#include "acmserverconsts.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "RegistrationPortTraces.h"
#endif


CRegistrationPort* CRegistrationPort::NewL(MAcmController& aAcmController, 
										   TUint aUnit)
/**
 * Factory function.
 *
 * @param aOwner Observer (the port factory).
 * @param aUnit The port number.
 * @return Ownership of a newly created CRegistrationPort object
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NEWL_ENTRY );
	CRegistrationPort* self = new(ELeave) CRegistrationPort(aAcmController);
	CleanupClosePushL(*self);
	self->ConstructL(aUnit);
	CleanupStack::Pop();
	OstTraceFunctionExit0( CREGISTRATIONPORT_NEWL_EXIT );
	return self;
	}

void CRegistrationPort::ConstructL(TUint aUnit)
/**
 * 2nd-phase constructor. 
 *
 * @param aUnit The port number.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_CONSTRUCTL_ENTRY );
	TName name;
	name.Num(aUnit);
	TInt err = SetName(&name);
	if (err < 0)
		{
		OstTrace1( TRACE_NORMAL, CREGISTRATIONPORT_CONSTRUCTL, "CRegistrationPort::ConstructL;err=%d", err );
		User::Leave(err);
		}
	OstTraceFunctionExit0( CREGISTRATIONPORT_CONSTRUCTL_EXIT );
	}

CRegistrationPort::CRegistrationPort(MAcmController& aAcmController) 
/**
 * Constructor.
 *
 * @param aAcmController To use when creating and destroying ACM functions.
 */
 :	iAcmController(aAcmController)
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_CREGISTRATIONPORT_CONS_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_CREGISTRATIONPORT_CONS_EXIT );
	}

void CRegistrationPort::StartRead(const TAny* /*aClientBuffer*/, 
								  TInt /*aLength*/)
/**
 * Queue a read
 *
 * @param aClientBuffer pointer to the Client's buffer
 * @param aLength number of bytes to read
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_STARTREAD_ENTRY );
	ReadCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_STARTREAD_EXIT );
	}

void CRegistrationPort::ReadCancel()
/**
 * Cancel a read
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_READCANCEL_ENTRY );
	ReadCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_READCANCEL_EXIT );
	}


TInt CRegistrationPort::QueryReceiveBuffer(TInt& /*aLength*/) const
/**
 * Get the size of the receive buffer
 *
 * @param aLength reference to where the size will be written to
 * @return KErrNotSupported always.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_QUERYRECEIVEBUFFER_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_QUERYRECEIVEBUFFER_EXIT );
	return KErrNotSupported;
	}

void CRegistrationPort::ResetBuffers(TUint)
/**
 * Reset Tx and Rx buffers.
 * Not supported.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_RESETBUFFERS_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_RESETBUFFERS_EXIT );
	}

void CRegistrationPort::StartWrite(const TAny* /*aClientBuffer*/, 
								   TInt /*aLength*/)
/**
 * Queue a write
 *
 * @param aClientBuffer pointer to the Client's buffer
 * @param aLength number of bytes to write
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_STARTWRITE_ENTRY );
	WriteCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_STARTWRITE_EXIT );
	}

void CRegistrationPort::WriteCancel()
/**
 * Cancel a pending write
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_WRITECANCEL_ENTRY );
	WriteCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_WRITECANCEL_EXIT );
	}

void CRegistrationPort::Break(TInt /*aTime*/)
/**
 * Send a break signal to the host.
 *
 * @param aTime Length of break in microseconds
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_BREAK_ENTRY );
	BreakCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_BREAK_EXIT );
	}

void CRegistrationPort::BreakCancel()
/**
 * Cancel a pending break.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_BREAKCANCEL_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_BREAKCANCEL_EXIT );
	}

TInt CRegistrationPort::GetConfig(TDes8& /*aDes*/) const
/**
 * Pass a config request
 *
 * @param aDes config will be written to this descriptor 
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_GETCONFIG_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_GETCONFIG_EXIT );
	return KErrNotSupported;
	}

TInt CRegistrationPort::SetConfig(const TDesC8& /*aDes*/)
/**
 * Set up the comm port.
 *
 * @param aDes descriptor containing the new config
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_SETCONFIG_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_SETCONFIG_EXIT );
	return KErrNotSupported;
	}

TInt CRegistrationPort::SetServerConfig(const TDesC8& /*aDes*/)
/**
 * Set the server config.
 *
 * @param aDes package with new server configurations
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_SETSERVERCONFIG_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_SETSERVERCONFIG_EXIT );
	return KErrNotSupported;
	}

TInt CRegistrationPort::GetServerConfig(TDes8& /*aDes*/)
/**
 * Get the server configs
 *
 * @param aDes server configs will be written to this descriptor
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_GETSERVERCONFIG_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_GETSERVERCONFIG_EXIT );
	return KErrNotSupported;
	}

TInt CRegistrationPort::GetCaps(TDes8& /*aDes*/)
/**
 * Read capabilities from the driver
 *
 * @param aDes caps will be written to this descriptor
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_GETCAPS_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_GETCAPS_EXIT );
	return KErrNotSupported;
	}

TInt CRegistrationPort::GetSignals(TUint& /*aSignals*/)
/**
 * Get the status of the signal pins
 *
 * @param aSignals signals will be written to this descriptor
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_GETSIGNALS_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_GETSIGNALS_EXIT );
	return KErrNotSupported;
	}

TInt CRegistrationPort::SetSignalsToMark(TUint aAcmField)
/**
 * Set selected signals to high (logical 1).
 * 
 * Used in the registration port as an API to create aNoAcms ACM interfaces.
 * This method of creating interfaces is deprecated, use the ACM Server instead.
 *
 * @param aAcmField Low 2 bytes- number of ACM interfaces to create. High 2 
 * bytes- protocol number (from USBCDC 1.1 Table 17).
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_SETSIGNALSTOMARK_ENTRY );
	OstTrace1( TRACE_NORMAL, CREGISTRATIONPORT_SETSIGNALSTOMARK, "CRegistrationPort::SetSignalsToMark;aAcmField = 0x%x", aAcmField );

	// Extract number of interfaces and protocol number
	//	low 2 bytes represent the number of ACMs
	//	high 2 bytes represent the protocol number to use
	TUint interfaces = aAcmField & 0x0000FFFF;
	TUint8 protocolNumber = (aAcmField & 0xFFFF0000) >> 16;
	
	// If there is no protocol number, assume the default
	// i.e. the client is not using this interface to set the protocol number
	// NOTE: This means you cannot set a protocol number of 0 - to do this use the ACM Server.
	protocolNumber = protocolNumber ? protocolNumber : KDefaultAcmProtocolNum;

	TInt ret = iAcmController.CreateFunctions(interfaces, protocolNumber, KControlIfcName, KDataIfcName);
	OstTrace1( TRACE_NORMAL, CREGISTRATIONPORT_SETSIGNALSTOMARK_DUP1, "CRegistrationPort::SetSignalsToMark;ret=%d", ret );
	OstTraceFunctionExit0( CREGISTRATIONPORT_SETSIGNALSTOMARK_EXIT );
	return ret;
	}

TInt CRegistrationPort::SetSignalsToSpace(TUint aNoAcms)
/**
 * Set selected signals to low (logical 0)
 *
 * Used in the registration port as an API to destroy aNoAcms ACM interfaces.
 * This method of destroying interfaces is depricated, use the ACM Server instead.
 *
 * @param aNoAcms Number of ACM interfaces to destroy.
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_SETSIGNALSTOSPACE_ENTRY );
	OstTrace1( TRACE_NORMAL, CREGISTRATIONPORT_SETSIGNALSTOSPACE, "CRegistrationPort::SetSignalsToSpace;aNoAcms=%d", (TInt)aNoAcms );
	iAcmController.DestroyFunctions(aNoAcms);
	OstTrace0( TRACE_NORMAL, CREGISTRATIONPORT_SETSIGNALSTOSPACE_DUP1, "CRegistrationPort::SetSignalsToSpace;ret = KErrNone" );
	OstTraceFunctionExit0( CREGISTRATIONPORT_SETSIGNALSTOSPACE_EXIT );
	return KErrNone;
	}

TInt CRegistrationPort::GetReceiveBufferLength(TInt& /*aLength*/) const
/**
 * Get size of Tx and Rx buffer
 *
 * @param aLength reference to where the length will be written to
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_GETRECEIVEBUFFERLENGTH_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_GETRECEIVEBUFFERLENGTH_EXIT );
	return KErrNotSupported;
	}

TInt CRegistrationPort::SetReceiveBufferLength(TInt /*aLength*/)
/**
 * Set the size of Tx and Rx buffer
 *
 * @param aLength new length of Tx and Rx buffer
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_SETRECEIVEBUFFERLENGTH_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_SETRECEIVEBUFFERLENGTH_EXIT );
	return KErrNotSupported;
	}

void CRegistrationPort::Destruct()
/**
 * Destruct - we must (eventually) call delete this
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_DESTRUCT_ENTRY );
	delete this;
	OstTraceFunctionExit0( CREGISTRATIONPORT_DESTRUCT_EXIT );
	}

void CRegistrationPort::FreeMemory()
/**
 * Attempt to reduce our memory foot print.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_FREEMEMORY_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_FREEMEMORY_EXIT );
	}

void CRegistrationPort::NotifyDataAvailable()
/**
 * Notify client when data is available
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYDATAAVAILABLE_ENTRY );
	NotifyDataAvailableCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYDATAAVAILABLE_EXIT );
	}

void CRegistrationPort::NotifyDataAvailableCancel()
/**
 * Cancel an outstanding data availalbe notification
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYDATAAVAILABLECANCEL_ENTRY );
	NotifyDataAvailableCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYDATAAVAILABLECANCEL_EXIT );
	}

TInt CRegistrationPort::GetFlowControlStatus(TFlowControl& /*aFlowControl*/)
/**
 * Get the flow control status
 *
 * @param aFlowControl flow control status will be written to this descriptor
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_GETFLOWCONTROLSTATUS_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_GETFLOWCONTROLSTATUS_EXIT );
	return KErrNotSupported;
	}

void CRegistrationPort::NotifyOutputEmpty()
/**
 * Notify the client when the output buffer is empty.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYOUTPUTEMPTY_ENTRY );
	NotifyOutputEmptyCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYOUTPUTEMPTY_EXIT );
	}

void CRegistrationPort::NotifyOutputEmptyCancel()
/**
 * Cancel a pending request to be notified when the output buffer is empty.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYOUTPUTEMPTYCANCEL_ENTRY );
	NotifyOutputEmptyCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYOUTPUTEMPTYCANCEL_EXIT );
	}

void CRegistrationPort::NotifyBreak()
/**
 * Notify a client of a break on the serial line.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYBREAK_ENTRY );
	BreakNotifyCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYBREAK_EXIT );
	}

void CRegistrationPort::NotifyBreakCancel()
/**
 * Cancel a pending notification of a serial line break.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYBREAKCANCEL_ENTRY );
	BreakNotifyCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYBREAKCANCEL_EXIT );
	}

void CRegistrationPort::NotifyFlowControlChange()
/**
 * Notify a client of a change in the flow control state.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYFLOWCONTROLCHANGE_ENTRY );
	FlowControlChangeCompleted(EFlowControlOn,KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYFLOWCONTROLCHANGE_EXIT );
	}

void CRegistrationPort::NotifyFlowControlChangeCancel()
/**
 * Cancel a pending request to be notified when the flow control state changes.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYFLOWCONTROLCHANGECANCEL_ENTRY );
	FlowControlChangeCompleted(EFlowControlOn,KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYFLOWCONTROLCHANGECANCEL_EXIT );
	}

void CRegistrationPort::NotifyConfigChange()
/**
 * Notify a client of a change to the serial port configuration.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYCONFIGCHANGE_ENTRY );
	ConfigChangeCompleted(KNullDesC8, KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYCONFIGCHANGE_EXIT );
	}

void CRegistrationPort::NotifyConfigChangeCancel()
/**
 * Cancel a pending request to be notified of a change to the serial port 
 * configuration.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYCONFIGCHANGECANCEL_ENTRY );
	ConfigChangeCompleted(KNullDesC8, KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYCONFIGCHANGECANCEL_EXIT );
	}

void CRegistrationPort::NotifySignalChange(TUint /*aSignalMask*/)
/**
 * Notify a client of a change to the signal lines.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYSIGNALCHANGE_ENTRY );
	SignalChangeCompleted(0, KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYSIGNALCHANGE_EXIT );
	}

void CRegistrationPort::NotifySignalChangeCancel()
/**
 * Cancel a pending client request to be notified about a change to the signal 
 * lines.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_NOTIFYSIGNALCHANGECANCEL_ENTRY );
	SignalChangeCompleted(0, KErrNotSupported);
	OstTraceFunctionExit0( CREGISTRATIONPORT_NOTIFYSIGNALCHANGECANCEL_EXIT );
	}

TInt CRegistrationPort::GetRole(TCommRole& aRole)
/**
 * Get the role of this port unit
 *
 * @param aRole reference to where the role will be written to
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_GETROLE_ENTRY );
	aRole = iRole;
	OstTrace1( TRACE_NORMAL, CREGISTRATIONPORT_GETROLE, "CRegistrationPort::GetRole;aRole=%d", (TInt)aRole );
	OstTraceFunctionExit0( CREGISTRATIONPORT_GETROLE_EXIT );
	return KErrNone;
	}

TInt CRegistrationPort::SetRole(TCommRole aRole)
/**
 * Set the role of this port unit
 *
 * @param aRole the new role
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_SETROLE_ENTRY );
	// This is required to keep C32 happy while opening the port.
	// All we do is store the role and return it if asked.
	// Note that this is needed for multiple ACM ports because C32 doesn't 
	// check the return value for multiple ports so opening registration port 
	// more than once will fail.
	iRole = aRole;
	OstTraceFunctionExit0( CREGISTRATIONPORT_SETROLE_EXIT );
	return KErrNone; 
	}

CRegistrationPort::~CRegistrationPort()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CREGISTRATIONPORT_CREGISTRATIONPORT_DES_ENTRY );
	OstTraceFunctionExit0( CREGISTRATIONPORT_CREGISTRATIONPORT_DES_EXIT );
	}

//
// End of file
