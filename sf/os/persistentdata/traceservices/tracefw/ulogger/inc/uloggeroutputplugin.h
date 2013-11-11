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
// ULogger output plug-in interface
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef ULOGGEROUTPUTPLUGIN_H
#define ULOGGEROUTPUTPLUGIN_H

#include <e32base.h>
#include "uloggerplugin.h"
#include "uloggerdatatypes.h"

namespace Ulogger
{

//! Abstract class for ULogger output plug-ins.
/*!
A ULogger output plug-in is responsible for writing Trace log data to some
output medium. Examples for output media are files, serial ports or TCP sockets.
Whenever ULogger needs to output Trace log data, it passes this to the currently
selected output plug-in, which then handles the actual writing to an output
medium.

All output plug-ins must derive from this class in order to be compatible with
ULogger. They must also derive from ULogger::CPlugin (whose header is already
included by this header, for convenience) in order to be compatible with the
ECom framework, which ULogger uses to load its output plug-ins.
*/
class MOutputPlugin
	{
public:	 
	/** 
	Writes the given data to the output media that is represented by the
	plug-in implementation. This method is called by ULogger whenever Trace log
	data becomes available. How much data is passed to this function depends
	on how ULogger is configured; the length of the given descriptor
	indicates the size of the data packet. The intervals at which
	this method is called depends on the amount of Trace data that is logged
	from code in the currently running processes.

	@param aData the Trace data to output, in BTrace format
	@return KErrNone, if successful; otherwise one of the other system wide
	                  error codes.
	*/
	virtual TInt Write(const TDesC8& aData) = 0; 

	/**
	Called by ULogger as first method after construction or after changes in
	config file. This allows the output plug-in to initialize itself with its
	private	settings.

	@param aConfigs actual configurations valid for this instance
	@return KErrNone, if successful; otherwise one of the other system wide
	        error codes.
	*/
	virtual TInt ConfigureOutputPlugin(const RPointerArray<TPluginConfiguration>& aConfigs) = 0; 

	/**
	Called by ULogger to indicate that the output plug-in must flush all buffers
	and	release any locked resources. Any resources may be locked only after any
	other method is called.
	*/
	virtual void CloseOutputPlugin() = 0;

	/** Virtual destructor. */
	virtual ~MOutputPlugin(){}

	/**
	Output plug-in interface id. This is for ULogger to distinguish between the
	different types of plug-ins (e.g. Output vs Input plug-ins).
	*/
	static const CPlugin::TPluginInterface iInterfaceId = CPlugin::EOutput;
	};

} //end of namespace

#endif // ULOGGEROUTPUTPLUGIN_H
