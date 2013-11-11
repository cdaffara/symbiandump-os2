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
// ULogger shared
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef ULOGGERSHARED_H
#define ULOGGERSHARED_H

#include <e32base.h> 

namespace Ulogger
{

//This is enumeration used for filtering the plugins
enum TPluginFilter
	{
	EOutputPluginFilter = 0,
	EInputPluginFilter
	};

//const
_LIT(KULoggerServerSemaphore, "ULoggerServerSemaphore");
_LIT(KServerName,"uloggerserver");

//configuration file sections
_LIT8(KServerSection,          "SERVERS");
_LIT8(KActiveSection,          "ACTIVE_OUTPUT_PLUGIN");
_LIT8(KActiveControlSection,   "ACTIVE_INPUT_PLUGIN");
_LIT8(KPrimaryFilterSection,   "PRIMARY_FILTERS");
_LIT8(KSecondaryFilterSection, "SECONDARY_FILTERS");
_LIT(KSysConfigMutex,          "SYSCONFIGMUTEX");
_LIT8(KTrace,                 "TRACE");
_LIT8(KBuffer,                "buffer_size");
_LIT8(KSecondaryGlobalFilter, "secondary_global_filter");
_LIT8(KDataNotification,      "data_notification_size");
_LIT8(KBufferMode,            "buffer_mode");
_LIT8(KEnable,                "enable");
_LIT8(KDisable,               "disable");
_LIT8(KCircular, 			  "circular");
_LIT8(KStraight, 			  "straight");
_LIT(KVersion, 				  "-version");

//Formattings
_LIT8(KSeparator,";");
_LIT(KFormatString,"%S;");//';' is a separator
_LIT(KConfigFormat, "%S;%S;");

//ulogger limitations
static const TInt KMaxPluginName = 256;
static const TInt KMaxBufferSize = 1024;
static const TInt KMaxDnsSize = KMaxBufferSize;
static const TInt KMaxPrimaryFiltersLimit = 256;
static const TInt KMaxSecondaryFiltersLimit = 4096;
static const TInt KMinBufferMTPLE = 512;
static const TInt KMaxNumberOfLettersInCommand = 2;

//server binary
_LIT(KServerExe, "uloggerserver.exe");

//Paths where configuration file has to be searched
_LIT(KConfigFilename, "uloggerconfig.ini"); //configuration file name
_LIT(KPublicConfigFilePath, "\\ulogger\\"); //user path for the configuration file
_LIT(KPrivateConfigFilePath, "\\private\\10273881\\"); //ulogger server private location
_LIT(KDefaultConfigFilePath, "Z:\\private\\10273881\\uloggerconfig.ini"); //default configuration file

// server variables
const TInt KULoggerSrvMajorVersionNumber=1;
const TInt KULoggerSrvMinorVersionNumber=0;
const TInt KULoggerSrvBuildVersionNumber=1; //changed: 07/09/2007


} // namespace

#endif // ULOGGERSHARED_H
