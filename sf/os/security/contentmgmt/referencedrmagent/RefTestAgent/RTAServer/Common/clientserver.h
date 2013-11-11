/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* SisRegistry - shared client/server definitions
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#ifndef __RTACLIENTSERVER_H__
#define __RTACLIENTSERVER_H__

#include <e32std.h>
#include <e32base.h>

namespace ReferenceTestAgent
	{
	
	_LIT(KRtaServerName,"!RTAServer");
	_LIT(KRtaImg, "RTAServer");		// DLL/EXE name

	const TUid KServerUid3={0x10205EA5};

	enum TRtaMessages
		{
		// Rights based operations, require DRM capability
		EAddRights		= 1,
		EEvaluateIntent = 2,
		EExecuteIntent  = 3,
		EClearAllRights = 4,
		EDumpRightsDatabase = 5,
		EOpenFile			= 6,
		EOpenFileHandle     = 7,
		EReadData			= 8,
		ESeek				= 9,
		EDataSize			= 10,
		EDeleteRightsByKey  = 11,
		EEraseDeleteKey     = 12,
		EReadOffsetData		= 13,
		EReloadRightsDatabase = 14,
		
		EManagerDeleteFile			= 20,
		EManagerCopyFile			= 21,
		EManagerRenameFile			= 22,
		EManagerMkDir				= 23,
		EManagerMkDirAll			= 24,
		EManagerRenameDir			= 25,
		EManagerRemoveDir			= 26,
		EManagerGetDir1				= 27,
		EManagerGetDir2				= 28,
		EManagerGetDir3				= 29,
		EManagerGetAttribute		= 30,
		EManagerGetAttributeSet 	= 31,
		EManagerGetStringAttribute 	= 32,
		EManagerGetStringAttributeSet = 33,
		EManagerNotifyStatusChange = 34,
		EManagerCancelNotifyStatusChange = 35,
		EManagerCopyFileHandle		= 36,
		EManagerGetAttributeByFileHandle			= 37,
		EManagerGetAttributeSetByFileHandle			= 38,
		EManagerGetStringAttributeByFileHandle 		= 39,
		EManagerGetStringAttributeSetByFileHandle 	= 40,

		EOpenContainer 				= 50,
		ECloseContainer				= 51,
		EGetEmbeddedObjects1		= 52,
		EGetEmbeddedObjects2		= 53,
		ESearch						= 54,
		


		EStartImport				= 60,
		EWriteData					= 61,
		EWriteDataComplete			= 62,
		EContinueWithNewOutputFile	= 63,
		EOutputFileCount			= 64,
		EOutputFile					= 65,
		EGetImportStatus			= 66,
		EGetSuggestedOutputFileExtension = 67,	
		EGetSuggestedOutputFileName	= 68,

		
		ECreateRightsMessageHandler = 90,
		ECreateDataMessageHandler = 91,
		ECreateManagerMessageHandler = 92,
		ECreateContentMessageHandler = 93,		

		EContentAgentSpecificCommand = 94, 		
		EContentNotifyStatusChange = 95, 
		EContentCancelNotifyStatusChange = 96,
		EContentRequestRights = 97, 
		EContentCancelRequestRights = 98,
		EContentDisplayInfo = 99,
		EContentSetProperty = 100,

		EManagerAgentSpecificCommand = 101,		
		EManagerSetProperty = 102, 
		EManagerDisplayInfo = 103,
		EManagerDisplayInfo1 = 104,
		EManagerDisplayManagementInfo = 105,

		EDataSetProperty = 106,
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        EReadWMDRMHeaderData = 107,     
        EReadWMDRMDataPacket = 108,     
        EWMDRMGetAttribute = 109,     
        EWMDRMGetStringAttribute = 110,     
        EWMDRMGetAttributeSet = 111,     
        EWMDRMGetStringAttributeSet = 112,     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
		
		// These operations do not require DRM capability
		ESeparatorUnprotected   = 500,
		EGetAttribute			= 501,
		EGetAttributeSet		= 502,
		EGetStringAttribute 	= 503,
		EGetStringAttributeSet  = 504,

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
        ENoEnforceReadWMDRMHeaderData = 701,     
        ENoEnforceReadWMDRMDataPacket = 702,     
        ENoEnforceWMDRMGetAttribute = 703,     
        ENoEnforceWMDRMGetStringAttribute = 704,     
        ENoEnforceWMDRMGetAttributeSet = 705,     
        ENoEnforceWMDRMGetStringAttributeSet = 706,     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

		// IPC messages to use when DRM capability enforcing is disabled
		ENoEnforceAddRights		 = 1001,
		ENoEnforceEvaluateIntent = 1002,
		ENoEnforceExecuteIntent  = 1003,
		ENoEnforceClearAllRights = 1004,
		ENoEnforceDumpRightsDatabase = 1005,
		ENoEnforceOpenFile			= 1006,
		ENoEnforceOpenFileHandle     = 1007,
		ENoEnforceReadData			= 1008,
		ENoEnforceSeek				= 1009,
		ENoEnforceDataSize			= 1010,
		ENoEnforceDeleteRightsByKey = 1011,
		ENoEnforceEraseDeleteKey    = 1012,
		ENoEnforceReadOffsetData	= 1013,
		ENoEnforceReloadRightsDatabase = 1014,
		
		ENoEnforceManagerDeleteFile			= 1020,
		ENoEnforceManagerCopyFile			= 1021,
		ENoEnforceManagerRenameFile			= 1022,
		ENoEnforceManagerMkDir				= 1023,
		ENoEnforceManagerMkDirAll			= 1024,
		ENoEnforceManagerRenameDir			= 1025,
		ENoEnforceManagerRemoveDir			= 1026,
		ENoEnforceManagerGetDir1			= 1027,
		ENoEnforceManagerGetDir2			= 1028,
		ENoEnforceManagerGetDir3			= 1029,
		ENoEnforceManagerGetAttribute		= 1030,
		ENoEnforceManagerGetAttributeSet 	= 1031,
		ENoEnforceManagerGetStringAttribute 	= 1032,
		ENoEnforceManagerGetStringAttributeSet = 1033,
		ENoEnforceManagerNotifyStatusChange = 1034,
		ENoEnforceManagerCancelNotifyStatusChange = 1035,
		ENoEnforceManagerCopyFileHandle		= 1036,

		ENoEnforceOpenContainer				= 1050,
		ENoEnforceCloseContainer			= 1051,
		ENoEnforceGetEmbeddedObjects1		= 1052,
		ENoEnforceGetEmbeddedObjects2		= 1053,
		ENoEnforceSearch					= 1054,


		ENoEnforceStartImport				= 1060,
		ENoEnforceWriteData					= 1061,
		ENoEnforceWriteDataComplete			= 1062,
		ENoEnforceContinueWithNewOutputFile	= 1063,
		ENoEnforceOutputFileCount			= 1064,
		ENoEnforceOutputFile					= 1065,
		ENoEnforceGetImportStatus			= 1066,
		ENoEnforceGetSuggestedOutputFileExtension = 1067,	
		ENoEnforceGetSuggestedOutputFileName	= 1068,


		ENoEnforceCreateRightsMessageHandler = 1090,
 		ENoEnforceCreateDataMessageHandler = 1091,
 		ENoEnforceCreateManagerMessageHandler = 1092,
 		ENoEnforceCreateContentMessageHandler = 1093,

		ENoEnforceContentAgentSpecificCommand = 1094, 		
		ENoEnforceContentNotifyStatusChange = 1095, 
		ENoEnforceContentCancelNotifyStatusChange = 1096,
		ENoEnforceContentRequestRights = 1097, 
		ENoEnforceContentCancelRequestRights = 1098,
		ENoEnforceContentDisplayInfo = 1099,
		ENoEnforceContentSetProperty = 1100,

		ENoEnforceManagerAgentSpecificCommand = 1101,		
		ENoEnforceManagerSetProperty = 1102, 
		ENoEnforceManagerDisplayInfo = 1103,
		ENoEnforceManagerDisplayInfo1 = 1104,
		ENoEnforceManagerDisplayManagementInfo = 1105,

		ENoEnforceDataSetProperty = 1106
		};

	enum TRtaServerPanic 
		{
		EPanicIllegalFunction  = 1	
		};
		
	struct TAttributePckg
		{
		TInt iAttribute;
		TInt iValue;
		};
	
	struct TStringAttributePckg
		{
		TInt iAttribute;
		TBuf<100> iValue;
		};
			
	}
#endif
