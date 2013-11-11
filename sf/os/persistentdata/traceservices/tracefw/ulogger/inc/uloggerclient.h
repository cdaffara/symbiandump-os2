// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ULogger Client API header
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef ULOGGERCLIENT_H
#define ULOGGERCLIENT_H

#include <e32std.h> // RSessionBase
#include <e32base.h>
#include "uloggerdatatypes.h"

namespace Ulogger
{

/**
Client-side session to the ULogger server.
This class should be used to configure ULogger server.
*/
class RULogger : public RSessionBase
	{	

public:
	/*
	Default constructor.
	*/
	IMPORT_C RULogger();

	/*
	Default destructor.
	Closes the client session.
	*/
	IMPORT_C ~RULogger();

	/**
	Connects a client to the ULogger server.
	If the server isn't already running it will be started.
	To end the server session, use Close().

	@return KErrNone, if successful, otherwise one of the other system-wide
	        error codes returned by RProcess:Create() or
	        RSessionBase::CreateSession().
	*/
	IMPORT_C TInt Connect();

	/**
	Not yet supported, does nothing and returns KErrNotSupported.

	Starts/stops running ULogger as a service. Running ULogger as a service
	means that ULogger is always active in the background, which is necessary
	if the objective is to connect to ULogger from a remote machine, so that 
	ULogger is then listening for remote connections.
	If ULogger is not running as a service, it is only active when it is
	handling a client request or it is currenty logging traces (i.e. the Start()
	function was called).

	@param aRunAsService The flag to indicate whether the server should run as a
	                     service or not
	@return KErrNotSupported, as this function is not yet implemented in the server.
	*/
	IMPORT_C TInt RunAsService(TBool aRunAsService);

	/**
	Starts outputting trace packets.
	Pre-requisite: This will only output trace packets containing filter identifers that 
	match enabled filters. Use SetPrimaryFiltersEnabled(), SetSecondaryFiltersEnabled() and
	SetSecondaryFilteringEnabled() to enable the relevant filters.

	@return KErrNone, if sending the Start message to the server was successful;
			KErrCorrupt, if the ULogger configuration file is corrupt;
	        KErrServerTerminated, if the server is no longer present;
	        KErrServerBusy, if there are no message slots available;
	        KErrNoMemory, if there is insufficient memory available;
	        KErrInUse, if server was started previously;
	        otherwise one of the other system-wide error codes.
	*/	
	IMPORT_C TInt Start();

	/**
	Stops outputting trace packets. 

	@return KErrNone, if sending the Stop message to the server was successful;
	        KErrServerTerminated, if the server is no longer present;
	        KErrServerBusy, if there are no message slots available;
	        KErrNoMemory, if there is insufficient memory available;
	        KErrNotReady, if ULogger wasn't started previously.
	*/
	IMPORT_C TInt Stop();

	/**
	Restarts server.

	@return KErrNone, if sending the Restart message to the server was successful;
			KErrCorrupt, if the ULogger configuration file is corrupt;
	        KErrServerTerminated, if the server is no longer present;
	        KErrServerBusy, if there are no message slots available;
	        KErrNoMemory, if there is insufficient memory available;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt Restart();

	/**
	Enables/disables the specified primary filters.
	This allows output of trace packets containing primary filter identifiers that match the
	enabled primary filters.
	Note: If a trace packet contains a secondary filter identifier, and secondary filtering is
	enabled, the matching secondary filter must also be enabled to allow this packet to be output.

	@param	aFilters	The primary filters to enable/disable
	@param	aEnabled    The boolean to specify the action. ETrue to enable and EFalse to disable.
	@return	KErrNone, if sending the SetPrimaryFiltersEnabled message to the
	                  server was successful;
	        KErrArgument, if the specified array of filters contains more than 
	                      KMaxPrimaryFiltersLimit elements or the array is empty;
	        KErrCorrupt, if the ULogger configuration file is corrupt;
            otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt SetPrimaryFiltersEnabled(const CArrayFixFlat<TUint8>& aFilters, TBool aEnabled);

	/**
	Gets all enabled primary filters.

	@param	aFilters 	The array to be populated with enabled filters
	@return	KErrNone, if sending the PrimaryFiltersEnabled message to the
	                  server was successful;
	        KErrCorrupt, if the ULogger configuration file is corrupt;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt GetPrimaryFiltersEnabled(CArrayFixFlat<TUint8>& aFilters);

	/**
	Enables/disables the specified secondary filters.
	This allows output of trace packets containing secondary filter identifiers that match the 
	enabled secondary filters.
	Note1: Secondary filters are only relevant when secondary filtering is
	       enabled (see SetSecondaryFilteringEnabled()). If secondary filtering
	       is disabled, trace packets are logged regardless of any secondary filter identifiers. 
	Note2: Enabling secondary filters will automatically enable secondary filtering.
	Note3: Primary filtering is always enabled and therefore there is no
	       equivalent method for enabling/disabling primary filtering.

	@param	aFilters 	The secondary filters to enable/disable
	@param	aEnabled 	The boolean to specify the action. ETrue to enable and EFalse to disable.
	@return	KErrNone, if sending the SetSecondaryFiltersEnabled message to the
	                  server was successful;
	        KErrArgument, if the specified array of filters contains more than 
	                      KMaxSecondaryFiltersLimit elements, the array is empty or 
	                      number of currently set filters plus number of elements in aFilters array
	                      is greather than KMaxSecondaryFiltersLimit;
	        KErrCorrupt, if the ULogger configuration file is corrupt;	                      
            otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt SetSecondaryFiltersEnabled(const RArray<TUint32>& aFilters, TBool aEnabled);

	/**
	Gets all enabled secondary filters.

	@param	aFilters 	The array to be populated with enabled filters
	@return	KErrNone, if sending the SecondaryFiltersEnabled message to the
	                  server was successful;
	        KErrNoMemory, if there is insufficient memory available;
			KErrCorrupt, if the ULogger configuration file is corrupt;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt GetSecondaryFiltersEnabled(RArray<TUint32>& aFilters);

	/**
	Enables/disables filtering on secondary filters.
	When secondary filtering is enabled, only those trace packets containing a 
	secondary filter identifier matching an enabled secondary filter are output
	(see SetSecondaryFiltersEnabled()). Note that trace packets must also contain 
	a primary filter identifier matching an enabled primary filter in order to be output
	(see SetPrimaryFilterEnabled()).

	@param	aEnabled     The boolean to specify the action. ETrue to enable and EFalse to disable.
	@return KErrNone, if sending the SetSecondaryFilteringEnabled message to the
	                  server was successful;
	       	KErrCorrupt, if the ULogger configuration file is corrupt; 
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt SetSecondaryFilteringEnabled(TBool aEnabled);

	/**
	Gets secondary filtering enabled value.

	@param	aEnabled 	The boolean to be populated with the secondary filtering enabled value
	@return	KErrNone, if sending the SecondaryFilteringEnabled message to the
	                  server was successful;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt GetSecondaryFilteringEnabled(TBool& aEnabled);

	/**
	Sets which output plug-in is to be used for outputting trace packets. 
	The plug-in that is passed to this method needs to be one of the
	plug-ins reported by InstalledOutputPlugins();

	@param	aPluginName The name of the plug-in to be used, needs to be one of
	                    the names returned by InstalledOutputPlugins()
	@return	KErrNone, if sending the SetActiveOutputPlugin message to the server
	                  was successful;
	        KErrAlreadyExists, if aPluginName value was previously set;
	        KErrNotFound, if aPluginName does not match the name of an existing Ulogger output plug-in;
	        KErrNoMemory, if there is insufficient memory available;
	        KErrArgument, if passed descriptor is empty;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt ActivateOutputPlugin(const TDesC8& aPluginName);

	/**
	Gets the active output plug-in.

	@param	aPluginName	The 8-bit descriptor to be populated with the name of
	                    the active output plug-in (max length: KMaxPath).
	@return	KErrNone, if sending the ActiveOutputPlugin message to the
	                  server was successful;
	        KErrOverflow, if the passed-in descriptor is smaller than the length
	                      of the plug-in name;
	        KErrNotFound, if no output plug-in has been set;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt GetActiveOutputPlugin(TDes8& aPluginName);

	/**
	Gets all the installed output plug-ins. 

	@param	aPluginNames The array to be populated with the installed plug-in filename and plug-in names.
	@return	KErrNone, if sending the InstalledOutputPlugins message to the
	                  server was successful;
	        KErrNoMemory, if there is insufficient memory available;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt GetInstalledOutputPlugins(CArrayPtrFlat<HBufC8>& aPluginNames);

	/**
	Sets which input plug-in is to be used for sending messages to ULogger from a
	remote machine. This is only relevant for the remote control functionality
	of ULogger, which allows a remote machine to control the operation of
	ULogger, instead of a client on the device using RULogger.
	The plug-in that is passed to this method needs to be one of the
	plug-ins reported by GetInstalledInputPlugins.

	@param	aPluginName	The name of the input plug-in to be activated.	
	@return	KErrNone, if sending the SetActiveInputPlugin message to the server
	                  was successful;
	        KErrAlreadyExists, if aPluginName value was previously set;
	        KErrNotFound, if aPluginName does not match the name of an existing Ulogger input plug-in;
	        KErrNoMemory, if there is insufficient memory available;
	        KErrArgument, if passed descriptor is empty;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt ActivateInputPlugin(const TDesC8& aPluginName);	

	/**
	Gets the active input plug-in.

	@param	aPluginName	The 8-bit descriptor to be populated with the name of
	                    the active plug-in (max length: KMaxPath).
	@return	KErrNone, if sending the ActiveInputPlugin message to the
	                  server was successful;
	        KErrOverflow, if the passed-in descriptor is smaller than the length
	                      of the plug-in name;
	        KErrNotFound, if no input plug-in has been set;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt GetActiveInputPlugin(TDes8& aPluginName);

	/**
	Deactivates the currently active input plug-in.

	@return	KErrNone, if sending the DeActivateInputPlugin message to the server
	                  was successful;
	        KErrNotFound, if no input plug-in has been set;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt DeActivateInputPlugin();

	/**
	Gets all installed input plug-ins.

	@param	aPluginNames The array to be populated with the installed input plug-in names.
	@return	KErrNone, if sending the GetInstalledInputPlugins message to the
	                  server was successful;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt GetInstalledInputPlugins(CArrayPtrFlat<HBufC8>& aPluginNames);

	/**
	Adds a configuration to a plug-in (output or input). This is specific to the plug-in.

	Example1: aPluginName="uloggerfileplugin",   aConfiguration.SetKey("output_path"), aConfiguration.SetValue(_L("c:\\logs\\ulogger\\tracelogs.btl"))
	Example2: aPluginName="uloggerserialplugin", aConfiguration.SetKey("baudrate") , aConfiguration.SetValue("115200")

	@param aPluginName 	The plug-in name (without the .dll extension)
	@param aConfiguration TPluginConfiguration (key-value type configuration)
	@return	KErrNone, if sending the SetPluginConfigurations message to the
	                  server was successful;
	        KErrNotFound, if aPluginName does not match the name of an existing ULogger plug-in (output or input);
	        KErrArgument, if aPluginName is an empty descriptor;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt SetPluginConfigurations(const TDesC8& aPluginName, const TPluginConfiguration& aConfiguration);

	/**
	Gets all configurationas for the specified plug-in (output or input).

	@param aPluginName 	The name of the plug-in. This is mapped to the plug-in
	                   file name, it can be retrieved by calling
	                   GetInstalledOutputPlugins and GetInstalledInputPlugins
	@param aConfigurations The array that is to be populated with plug-in
	                       configuration items. Ownership of any objects
	                       appended to this array is passed to the caller.
	@return	KErrNone, if sending the PluginConfigurations message to the
	                  server was successful;
            KErrNotFound, if aPluginName does not match the name of an existing ULogger plug-in (output or input);
            KErrArgument, if aPluginName is an empty descriptor;
	        otherwise one of the other system-wide error codes.
	          
	*/
	IMPORT_C TInt GetPluginConfigurations(const TDesC8& aPluginName, RPointerArray<TPluginConfiguration>& aConfigurations);

	/**
	Removes all the configurations of the specified plug-in

	@param	aPluginName	  The plug-in name whose settings are to be removed
	@return	KErrNone, if sending the RemovePluginConfigurations message to the
	                  server was successful;
	        KErrNotFound, if aPluginName does not match the name of an existing ULogger plug-in (output or input)
	        			  or the plug-in had no settings;
            KErrArgument, if aPluginName is an empty descriptor;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt RemovePluginConfigurations(const TDesC8& aPluginName);

	/**
	Sets the size of the trace buffer, specified in KB (default: 1024KB).
	The trace buffer is the buffer that UTrace uses to hold the trace packets
	before they are output (using the currently active output plug-in).
	See also the documentation of SetNotificationSize().

	@param	aSize 		The desired new trace buffer size in KB
	@return	KErrNone, if sending the SetBufferSize message to the server was
	                  successful;
	        KErrArgument, if aSize value is not in range [1-1024];
	        KErrGeneral, if aSize value is smaller than data notification size;
            otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt SetBufferSize(TInt aSize);

	/**
	Gets the size of the trace buffer in KB.
	See SetBufferSize() for an explanation of the trace buffer size.

	@param	aSize 		The TInt to be populated with the trace buffer size in KB.
	@return	KErrNone, if sending the BufferSize message to the server was
	                  successful;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt GetBufferSize(TInt& aSize);

   /**
	Sets the data notification size, specified in KB (default: 512KB).
	The data notification size is a threshold on the trace buffer usage, above
	which the trace buffer is flushed, i.e. all the trace packets in the trace buffer
	are output using the active output plug-in.
	See also the documentation of SetBufferSize().

	@param	aSize 		The desired new data notification size in KB
	@return	KErrNone, if sending the SetNotificationSize message to the server
	                  was successful;
	        KErrArgument, if aSize value is not in range [0-1024];
	        KErrGeneral, if aSize value is greater than trace buffer size;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt SetNotificationSize(TInt aSize);

	/**
	Gets the data notification size.
	See SetNotificationSize() for an explanation of the data notification size.

	@param	aSize 		The TInt to be populated with the data notification size in KB.
	@return	KErrNone, if sending the NotificationSize message to the server was
	                  successful;
	        otherwise one of the other system-wide error codes.
	*/		
	IMPORT_C TInt GetNotificationSize(TInt& aSize);

	/**
	Sets the trace buffer mode.

	@param	aMode		The buffer mode: 1 if Straight
										 0 if Circular
	@return	KErrNone, if sending the SetBufferMode message to the server was
	                  successful;
	        KErrArgument, if the specified mode is not one of 0 or 1;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt SetBufferMode(TInt aMode);

	/**
	Gets the trace buffer mode.

	@param	aMode 		The TInt to be populated with the buffer mode: 1 if Straight
									 		                		   0 if Circular
	@return	KErrNone, if sending the BufferMode message to the server was
	                  successful;
	        otherwise one of the other system-wide error codes.
	*/
	IMPORT_C TInt GetBufferMode(TInt& aMode);

	/**
	Gets the client side version number.

	@return The client side version number.
	*/
	IMPORT_C TVersion Version();

private:
	TInt StartServer();

private:
	TUint32 iReserved1; // For future BC
	TUint32 iReserved2; // For future BC
	};

} // namespace

#endif // ULOGGERCLIENT_H
