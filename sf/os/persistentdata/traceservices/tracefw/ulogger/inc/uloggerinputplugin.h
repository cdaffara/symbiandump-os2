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
// ULogger input plug-in interface
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef ULOGGERINPUTPLUGIN_H
#define ULOGGERINPUTPLUGIN_H

#include <e32base.h>
#include "uloggerplugin.h"
#include "uloggerdatatypes.h"

namespace Ulogger
{

//! Abstract class for input plug-ins.
/*!
A ULogger input plug-in listens for ULogger commands on some communication
medium, such as serial, usb or a TCP socket. Whenever a command is received
by the input plug-in, it passes this command to ULogger, which then interprets
the command, acts on it, and returns a response to the input plug-in. The input
plug-in sends any response coming from ULogger back to the client that sent
the command in the first place.

All input plug-ins must derive from this class in order to be compatible with
ULogger. They must also derive from ULogger::CPlugin (whose header is already
included by this header, for convenience) in order to be compatible with the
ECom framework, which ULogger uses to load its input plug-ins.
*/
class MInputPlugin
	{
public:
	/**
	Asynchronous method that reads command data from the input medium. ULogger
	calls this when it's ready to receive command data from the input plug-in.
	When the input plug-in completes the read operation it notifies the caller
	via the TRequestStatus that is passed into this method by reference. It
	provides the command data that has been received in the descriptor that is
	passed into this method by reference.
	Input plug-ins typically implement this method by simply passing the
	TRequestStatus and descriptor arguments on to another asynchronous method,
	such as for example a socket's ReadOneOrMore method.

	@param aStatus The request status used to contain completion information for
	               the function. On completion, contains a system-wide error
	               code.
	@param aData A descriptor reference to store data obtained from input
	             channel.
	@return KErrNone if operation was finished without any problems, system wide
	                 error code otherwise.
	*/
	virtual TInt ReadData(TRequestStatus& aStatus, TDes8& aData) = 0;

	/** Cancels asynchronous operation issued by ReadData method. */
	virtual void CancelReadData() = 0;

	/**
	Synchronous Method that sends the given acknowledgment data back to the
	client that is sending command data to the input plug-in. ULogger calls this
	method whenever it needs to send a response to a previously received
	command.

	@param aData A descriptor which contains error code or other results, for
	             example, array of filters.
	Format of this data depends on previously obtained command.
	@return KErrNone is send operation finished with success otherwise
	                 system wide error code.
	*/
	virtual TInt SendAcknowledgment(const TDesC8& aData) = 0;

	/**
	Called by ULogger as first method after construction or after changes in
	config file. This allows the input plug-in to initialize itself with its
	private settings.

	@param aConfigs actual configurations valid for this instance
	@return KErrNone, if successful; otherwise one of the other system wide
	        error codes.
	*/
	virtual TInt ConfigureInputPlugin(const RPointerArray<TPluginConfiguration>& aConfigs) = 0; 

	/**
	Called by ULogger to indicate that the input plug-in must flush all buffers
	and release any locked resources. Any resources may be locked only after any
	other method is called.
	*/
	virtual void CloseInputPlugin() = 0;

	/**	Virtual destructor.	*/
	virtual ~MInputPlugin(){}

	/**
	Input plug-in interface id. This is for ULogger to distinguish between the
	different types of plug-ins (e.g. Intput vs Output plug-ins).
	*/
	static const CPlugin::TPluginInterface iInterfaceId = CPlugin::EInput;
	};

} //end of namespace

#endif /* ULOGGERINPUTPLUGIN_H */
