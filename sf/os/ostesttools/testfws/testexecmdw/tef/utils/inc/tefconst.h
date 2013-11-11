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



/**
 @file TEFConst.h
*/

#ifndef __TEFCONST_H__
#define __TEFCONST_H__
#include <e32def.h>

// Non exported files access only
const TInt KTEFMaxTaskExitCategory=80;
const TInt KTEFMaxSharedArraySize=10;
const TInt KTEFSharedDataSize=1024;
const TInt KTEFMinOomParamLength=4;
const TInt KTEFMaxScriptLineLength=512;
const TInt KTEFErrInvalidRelPath=-30003;

_LIT(KTEFLoadSuiteCommand,"LOAD_SUITE");
_LIT(KTEFLoadServerCommand,"LOAD_SERVER");
_LIT(KTEFLoadSuiteSharedData,"-SharedData");
_LIT(KTEFConcurrentCommand,"CONCURRENT");
_LIT(KTEFConsecutiveCommand,"CONSECUTIVE");
_LIT(KTEFPrintCommand,"PRINT");
_LIT(KTEFRunUtilsCommand,"RUN_UTILS");
_LIT(KTEFRunUtilsMakeReadWrite,"MakeReadWrite");
_LIT(KTEFRunUtilsCopyFile,"CopyFile");
_LIT(KTEFRunUtilsMkDir,"MkDir");
_LIT(KTEFRunUtilsDeleteFile,"DeleteFile");
_LIT(KTEFRunUtilsDelete,"Delete");
_LIT(KTEFRunScriptCommand,"RUN_SCRIPT");
_LIT(KTEFCedCommand,"CED");
_LIT(KTEFDelayCommand,"DELAY");
_LIT(KTEFBreakErrorOnCommand,"BREAK_ERROR_ON");
_LIT(KTEFBreakErrorOffCommand,"BREAK_ERROR_OFF");
_LIT(KTEFPauseCommand,"PAUSE");
_LIT(KTEFPrefixCommand, "PREFIX");
_LIT(KTEFRemovePrefixCommand, "REMOVE_PREFIX");
_LIT(KTEFSharedDataCommand, "SHARED_DATA");
_LIT(KTEFStartRepeat,"START_REPEAT");
_LIT(KTEFEndRepeat,"END_REPEAT");
_LIT(KTEFColon, ":");
_LIT(KTEFSharedDataNum, "shared_data_num");
_LIT(KTEFSharedName, "shared_name_");
_LIT(KTEFErrorReadingIni,"Error reading INI file");
_LIT(KTEFSharedDataCommandRepeated,"SHARED_DATA Command is already in use");
_LIT(KTEFErrInCreatingSharedObjects, "Unable to create shared object specified");
_LIT(KTEFErrShareNameNotInIni, "The name of shared object %d is not specified in the ini");
_LIT(KTEFErrNumberOfShareNotInIni, "The number of shared objects is not specified in the ini");
_LIT(KTEFMaxNumberOfSharedObjectsReached, "Number of shared objects exceeded maximum allowable limit");
_LIT(KTEFOom, "!OOM");
_LIT(KTEFDeleteDirectory,"DeleteDirectory");
_LIT(KTEFSetUpParam, "!Setup=");
_LIT(KTEFIniExtension, ".ini");
_LIT(KTEFDotSlash, ".\\");
_LIT(KTEFOneUp,"..\\");

// Test Block specific script commands
_LIT(KTEFStartTestBlock,"START_TEST_BLOCK");
_LIT(KTEFEndTestBlock,"END_TEST_BLOCK");
_LIT(KTEFCreateObject,"CREATE_OBJECT");
_LIT(KTEFRestoreObject,"RESTORE_OBJECT");
_LIT(KTEFCommand,"COMMAND");
_LIT(KTEFStore,"STORE");
_LIT(KTEFOutstanding,"OUTSTANDING");
_LIT(KTEFDelay,"DELAY");
_LIT(KTEFAsyncDelay,"ASYNC_DELAY");
_LIT(KTEFSharedActiveScheduler,"SHARED_ACTIVE_SCHEDULER");
_LIT(KTEFStoreActiveScheduler,"STORE_ACTIVE_SCHEDULER");
_LIT(KTEFCreateActiveScheduler,"!CreateScheduler=");
_LIT(KTEFReuseActiveScheduler,"!ReuseScheduler=");
_LIT(KTEFDeleteActiveScheduler,"!DeleteScheduler=");
_LIT(KTEFAsyncError,"!AsyncError=");
_LIT(KTEFErrorResult," Error=");
_LIT(KTEFAsyncErrorResult," AsyncError=");
_LIT(KTEFComment,"//");

// Following set of literals are defined for version logging feature
_LIT(KTEFConsoleHelpMessage1, "\n\n\ntestexecute <scriptfilepath>\\<scripfilename>[.script] [-d] [-slf]\
							   \n\t: Executes the scriptfile with optional modes for JustInTime debugging or separate log files");
_LIT(KTEFConsoleHelpMessage2, "\n\ntestexecute <scripfilename>[.script] [-d] [-slf]\
							   \n\t: Script file path can be avoided when the defaultscriptdir entry is provided in testexecute.ini");
_LIT(KTEFConsoleHelpMessage3, "\n\ntestexecute [-help/-v]\
							   \n\t: Run TestExecute with optional modes to display list of help options and the version number");
_LIT(KTEFConsoleHelpMessage4, "\n\n\n\n-help   : For displaying the help options available for using TestExecute framework\
							   \n\n-v      : For displaying the version of the TestExecute framework");
_LIT(KTEFConsoleHelpMessage5, "\n\n-slf    : For printing the messages for each of the tests in separate log files\
							   \n\n-d      : For enabling just in time debug mode while execution");
_LIT(KTEFInvalidCommandSetMessage, "Invalid set of Command line arguments\
									\n\nType testexecute -help to learn about usages");

_LIT(KTEFVersionMessage, "\nTEF Version : ");
_LIT(KTEFVersionMessageXML, "TEF_VERSION");

_LIT(KTEFTestSynopsisHTM, "\nTEST SYNOPSIS:");
_LIT(KTEFTestSynopsisXML, "TEST_SYNOPSIS");

_LIT(KTEFCountMessageHTM, " COUNT : ");
_LIT(KTEFCountMessageXML, "_COUNT");

_LIT(KTEFNewLine, "\n");
_LIT(KTEFEnterKeyMessage, "\n\nEnter a key to Exit");
_LIT(KTEFEndOfLine,"\r\n");

#endif
