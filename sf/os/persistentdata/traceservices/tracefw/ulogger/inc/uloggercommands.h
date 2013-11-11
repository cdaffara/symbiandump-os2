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
// ULogger commands
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef ULOGGERCOMMANDS_H_
#define ULOGGERCOMMANDS_H_

#include <e32base.h>

/** Ulogger command line syntax description.
ULogger is used to configure and enable/disable logging of trace data on device.

SYNOPSIS: ulogger [-rqtvhioH][-edl <config_options>...][-fsSoicbmn]...

DESCRIPTION:
ULogger groups the commands into execution and configuration commands
where options -rqtvhH belong to the former group and -edlbnmfsSoic to the latter.
Execution commands: -rqtvhioH, require no other arguments.
Configuration commands are further divided into:
	-edl, these require one of the following options '-fsSoic'
	to be present, these will fail if too many or missing options are given.
	-fsSoicbmn, these will apply the arguments depending on provided option -edl.
	
OPTIONS
Execution commands:
-r                      Start logging (Run)
-q                      Stop logging (Quit)
-t                      Restart (restarT) while logging, to update configurations
-v                      Verbose mode, will list all results, or silent mode is
							assumed and only error codes will be returned
-h 						List of options.
-o						List active output plugin.
-i						List active input plugin.
-H						This man page.
-?						The version of this tool and most usefull commands.

Configuration commands:
-e <-fsSoicbmn options> ...	Enable/set either of option -fsSoicbmn
-d <-fsSoic options> ...	Disable/delete either of option -fsSoic
-l <-fsSoicbmn options> ...	List either of option -fsSoicbmn
-f [<filter> ...]       Enable/disable/list primary filter 0-255
-s [<filter> ...]       Enable/disable/list secondary filter 0-4294967295
-S 						Enable/disable/list secondary filtering.
							Enable to filter on secondary filters,
							Disable to log all traces irrespective of secondary filter.
							By default filtering is enabled.
-o [<filename>]         Enable/disable an output plugin, list all output plugins
-i [<filename>]         Enable/disable an input plugin, list all input plugins
-c [<filename> <config> ...]       Enable(set)/delete/list plugin configurations.
							First argument must be the name of the configuration
							to be changed and any following arguments are treated as values of
							this configuration. The configuration keyword can not use any white 
							characters (e.g. space or tabulation key).
-b [<size> ...]         Buffer size, 1-1024 Kb rounded up to nearest page size, 4Kb.
-n [<size> ...]         Notification size, 0-1024 Kb rounded up to nearest page size, 4Kb.
-m [<mode> ...]         Buffer mode, 'circular' or 'straight'. Default is straight.
 */


//ulogger commands
_LIT(KCmdEnable,				"e");
_LIT(KCmdDisable,				"d");
_LIT(KCmdList,					"l");
_LIT(KCmdPrimaryFilter,			"f");
_LIT(KCmdSecondaryFilter,		"s");
_LIT(KCmdSecondaryFilterToggle,	"S");
_LIT(KCmdOutputPlugin,		 	"o");
_LIT(KCmdInputPlugin,		 	"i");
_LIT(KCmdPluginConfigurations, 	"c");
_LIT(KCmdBuffer, 				"b");
_LIT(KCmdNotification,		 	"n");
_LIT(KCmdBufferMode, 			"m");
_LIT(KCmdVerboseMode,			"v");
_LIT(KCmdStart,					"r");
_LIT(KCmdStop,	  				"q");
_LIT(KCmdRestart,	  			"t");
_LIT(KCmdHelp,		  			"h");
_LIT(KCmdMan,					"H");
_LIT(KCmdVersion,				"?");
_LIT(KCmdIndicator, 			"-");
_LIT(KCmdAll,					"all");
_LIT(KCmdBufferModeCircular,	"circular");
_LIT(KCmdBufferModeStraight,	"straight");


/**
@internalTechnology
@prototype
*/
enum TCommand
 	{		
 	EUnknown = 0,
 	ERunAsService,     // function not yet implemented in server!
 	EDontRunAsService, // function not yet implemented in server!
 	EStart,
 	EStop,
 	ERestart,
 	ESetPrimaryFilter,
 	EGetPrimaryFilters,
 	ERemovePrimaryFilter,
 	ESetSecondaryFilter,
 	EGetSecondaryFilters,
 	ERemoveSecondaryFilter,
 	EGetSecondaryFiltering,
 	EEnableSecondaryFiltering,
 	EDisableSecondaryFiltering,
 	ESetActivePlugin,
 	EGetActivePlugin,
 	EGetInstalledPlugins,
 	ESetActiveInputPlugin,
 	EGetActiveInputPlugin,
 	EDeactivateInputPlugin,
 	EGetInputPlugins,
	ESetPluginSettings,
	EGetPluginSettings,
 	ERemovePluginSettings,
 	EResizeTraceBuffer,
 	EGetTraceBufferSize,
 	ESetDataNotificationSize,
 	EGetDataNotificationSize,
 	ESetBufferMode,
	EGetBufferMode,
	EHelp,
	EManPage,
	EVersion
 	};

/**
@internalTechnology
@prototype
 */
class TCommandLookup
 {
 public:
 	TCommandLookup(const TDesC* aMainCommand, const TDesC*	aSubCommand, const TCommand aCommand)
 		: iMainCommand(aMainCommand), iSubCommand(aSubCommand), iCommand(aCommand)
 		{}
 	
 	//data
 	const TDesC* 	iMainCommand;
 	const TDesC*	iSubCommand;
 	TCommand 	iCommand;
 };

 
/**
Interface to execute command. 

@internalTechnology
@prototype
 */ 
class MCommandImpl
	{
	public:
		virtual TInt DoCommandL(TCommand aCommand, const RArray<TPtrC>& aValues) = 0;
	};
 
 
/**
This is a utility class to parse command line arguments and to generate accurate event 
to execute required command. 
To use this class properly, MCommandImpl interface must be implemented and pointer to it 
must be passed during object construction.

@internalTechnology
@prototype
 */
class CCommand : public CBase
	{
	public:
		IMPORT_C static CCommand* NewL(MCommandImpl* aCommandImpl);
		IMPORT_C static CCommand* NewLC(MCommandImpl* aCommandImpl);
		IMPORT_C ~CCommand();
		IMPORT_C void ConstructL();
		IMPORT_C TInt HandleCommandL(const RArray<TPtrC>& aArgs);
			

	private:
		CCommand(MCommandImpl* aCommandImpl);
		TInt PrepareCommandL(RArray<TPtrC>& aArgs);
		
		//data
		MCommandImpl* iCommandImpl;
		TBool 		  iVerbose;
		RArray<TCommandLookup> iCommandsLookup;
	};
	
#endif /*ULOGGERCOMMANDS_H_*/
