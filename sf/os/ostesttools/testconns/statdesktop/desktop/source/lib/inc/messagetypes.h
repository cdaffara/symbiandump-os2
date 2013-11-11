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




#if !defined(__MESSAGETYPES_H__)
#define __MESSAGETYPES_H__

// messages sent by PC
enum TSTATPCMsg 
	{
		ESTATPCMsgHello,
		ESTATPCMsgStartApp,
		ESTATPCMsgOpenFile,
		ESTATPCMsgScreenCapture,
		ESTATPCMsgStopApp,
		ESTATPCMsgIconPress,
		ESTATPCMsgReady,
		ESTATPCMsgSelectDrive,
		ESTATPCMsgSelectLanguage,
		ESTATPCMsgSelectQuestion,
		ESTATPCMsgSelectOptions,
		ESTATPCMsgSelectDelDepend,
		ESTATPCMsgSelectText,
		ESTATPCMsgSelectAbortRetrySkip,
		ESTATPCMsgSelectInUse,
		ESTATPCMsgSelectSecWarning,
		ESTATPCMsgTypeText,
		ESTATPCMsgTypeSysText,
		ESTATPCMsgTypeCombinationText,
		ESTATPCMsgEShell,
		ESTATPCMsgKeyHold,
		ESTATPCMsgDeleteFile,
		ESTATPCMsgCreateFolder,
		ESTATPCMsgRemoveFolder,
		ESTATPCMsgGoodBye,
		ESTATPCMsgInvalidCommand
	};

#endif
