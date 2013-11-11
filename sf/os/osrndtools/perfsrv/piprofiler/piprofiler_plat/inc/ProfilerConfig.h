/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef PI_PROFILER_CONFIG_H
#define PI_PROFILER_CONFIG_H

// NCP release process will move the PIProfiler flags as Carbon feature flags
// where the product responsibles can choose the right settings for PI Profiler
// compilation.

	/*** NOTE!!! 
	* 	Uncomment the following definition if compiling the Profiler by your own
	***/
	#define PROFILER_SISX

	/*
	 *
	 *	Filename and path for the settings file
	 *  this file is written on application exit and
	 *	it contains the settings of the profiler application
	 *
	 */	
	 
//	#define PROFILER_SETTINGS_DRIVE		_L("C:\\")
//	#define PROFILER_SETTINGS_MAXLENGTH	32
//	#define PROFILER_SETTINGS_FILENAME	_L("ProfilerSettings.txt")
	const TInt KProfilerSettingsMaxLength = 32;
	_LIT(KProfilerSettingsFileName, "PIProfilerSettings.txt");

	// a fix for going through the different drives for settings file
	#define PROFILER_SETTINGS_DRIVE_COUNT				3
	#define	PROFILER_SETTINGS_DRIVE_ARRAY 			TBuf<4> settingsDrives[PROFILER_SETTINGS_DRIVE_COUNT];
	#define PROFILER_DEFINE_SETTINGS_DRIVE(name,number)	settingsDrives[number].Append(_L(name));										

	// change these to reflect the drive names and numbers
	// the last number must be PROFILER_SETTINGS_DRIVE_COUNT-1
	#define PROFILER_SETTINGS_DRIVES		PROFILER_DEFINE_SETTINGS_DRIVE("C:\\",0) \
		PROFILER_DEFINE_SETTINGS_DRIVE("E:\\",1) \
		PROFILER_DEFINE_SETTINGS_DRIVE("Z:\\",2)			
		
	/*
	 *
	 *	Locations of PI Profiler binaries
	 *
	 */

	#define PROFILERENGINE_EXE_PATH_PRIMARY		_L("C:\\sys\\bin\\PIProfilerEngine.exe")
	#define PROFILERENGINE_EXE_PATH_SECONDARY	_L("Z:\\sys\\bin\\PIProfilerEngine.exe")

	/*
	 *
	 *	PI Profiler tool composition definitions
	 *
	 */	

	// sampler codes and names
	#define		PROFILER_USER_MODE_SAMPLER		123
	#define		PROFILER_KERNEL_MODE_SAMPLER		321
	#define		PROFILER_DUMMY_SAMPLER			213

	// old definitions
	#define 	PROFILER_GENERALS_SAMPLER_ID		100
	#define 	PROFILER_INTERNALS_SAMPLER_ID		101
	#define		PROFILER_GPP_SAMPLER_ID			1
	#define		PROFILER_GFC_SAMPLER_ID			2
	#define		PROFILER_ITT_SAMPLER_ID			3
	#define		PROFILER_MEM_SAMPLER_ID			4
	#define		PROFILER_PRI_SAMPLER_ID			5
	#define		PROFILER_IRQ_SAMPLER_ID			6
	#define		PROFILER_BUP_SAMPLER_ID			7
	#define		PROFILER_SWI_SAMPLER_ID			8
	#define		PROFILER_TIP_SAMPLER_ID			9
	#define		PROFILER_PEC_SAMPLER_ID			10
	#define		PROFILER_PWR_SAMPLER_ID			11
	#define		PROFILER_IPC_SAMPLER_ID			12
	#define 	PROFILER_ISA_SAMPLER_ID			13
    #define     PROFILER_GPU_SAMPLER_ID         14

	// sampler IDs for external, e.g. 3rd party sampler plug-ins
	#define		PROFILER_EXT1_SAMPLER_ID			15
	#define		PROFILER_EXT2_SAMPLER_ID			16
	#define		PROFILER_EXT3_SAMPLER_ID			17
	#define		PROFILER_EXT4_SAMPLER_ID			18
	#define		PROFILER_EXT5_SAMPLER_ID			19
	
    #define     PROFILER_GPP_SAMPLER_NAME _L("GPP")
    #define     PROFILER_GFC_SAMPLER_NAME _L("GFC")
    #define     PROFILER_ITT_SAMPLER_NAME _L("ITT")
    #define     PROFILER_MEM_SAMPLER_NAME _L("MEM")
    #define     PROFILER_PRI_SAMPLER_NAME _L("PRI")
    #define     PROFILER_GPU_SAMPLER_NAME _L("GPU")

#endif
