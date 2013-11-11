// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


/**
 @file
 @internalComponent
*/

inline
COmxILCallbackManager::CCallbackCommand::CCallbackCommand(
	TInt aPriority)
	:
	iPriority(aPriority)
	{
	}

inline TInt
COmxILCallbackManager::CCallbackCommand::Priority()
	{
	return iPriority;
	}

inline
COmxILCallbackManager::CCompHandleRegistrationCommand::CCompHandleRegistrationCommand(
	OMX_HANDLETYPE aComponentHandle)
	:
	CCallbackCommand(CCallbackCommand::EPriorityVeryHigh),
	ipHandle(static_cast<OMX_COMPONENTTYPE*>(aComponentHandle))
	{
	}

inline
COmxILCallbackManager::CClientCallbacksRegistrationCommand::CClientCallbacksRegistrationCommand(
	const OMX_CALLBACKTYPE*& apCallbacks,
	const OMX_PTR& apAppData)
	:
	CCallbackCommand(CCallbackCommand::EPriorityVeryHigh),
	ipCallbacks(apCallbacks),
	ipAppData(apAppData)
	{
	}

inline
COmxILCallbackManager::CTunnelCallbackRegistrationCommand::CTunnelCallbackRegistrationCommand(
	OMX_U32 aLocalPortIndex,
	OMX_DIRTYPE aLocalPortDirection,
	OMX_HANDLETYPE aTunnelledComponentHandle,
	OMX_U32 aTunnelledPortIndex)
	:
	CCallbackCommand(CCallbackCommand::EPriorityVeryHigh),
	iTunnelInfo(aLocalPortIndex,
				aLocalPortDirection,
				aTunnelledComponentHandle,
				aTunnelledPortIndex)
	{
	}

inline
COmxILCallbackManager::CBufferMarkPropagationRegistrationCommand::
CBufferMarkPropagationRegistrationCommand(
	OMX_U32 aPortIndex,
	OMX_U32 aPropagationPortIndex)
	:
	CCallbackCommand(CCallbackCommand::EPriorityVeryHigh),
	iMarkPropagationInfo(aPortIndex,
						 aPropagationPortIndex)
	{
	}

inline
COmxILCallbackManager::CBufferRemovalCommand::CBufferRemovalCommand(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_DIRTYPE aDirection)
	:
	CCallbackCommand(CCallbackCommand::EPriorityVeryHigh),
	ipBufferHeader(apBufferHeader),
	iDirection(aDirection)
	{
	}

inline
COmxILCallbackManager::CEventCallbackCommand::CEventCallbackCommand(
	OMX_EVENTTYPE aEvent,
	TUint32 aData1,
	TUint32 aData2,
	OMX_STRING aExtraInfo)
	:
	CCallbackCommand(CCallbackCommand::EPriorityNormal),
	iEvent(aEvent),
	iData1(aData1),
	iData2(aData2),
	iExtraInfo(aExtraInfo)
	{
	}

inline
COmxILCallbackManager::CBufferDoneCallbackCommand::
CBufferDoneCallbackCommand(OMX_BUFFERHEADERTYPE* apBufferHeader,
						   OMX_U32 aLocalPortIndex,
						   OMX_DIRTYPE aLocalPortDirection,
						   TInt aPriority)
	:
	CCallbackCommand(aPriority),
	ipBufferHeader(apBufferHeader),
	iLocalPortIndex(aLocalPortIndex),
	iLocalPortDirection(aLocalPortDirection)
	{
	}

inline
COmxILCallbackManager::CPortSettingsChangeCommand::
CPortSettingsChangeCommand(OMX_U32 aLocalPortIndex,
						   TUint aPortSettingsIndex,
						   HBufC8*& apPortSettings)
	:
	CCallbackCommand(CCallbackCommand::EPriorityNormal),
	iLocalPortIndex(aLocalPortIndex),
	iPortSettingsIndex(aPortSettingsIndex),
	ipPortSettings(apPortSettings)
	{
	}

#ifdef _OMXIL_COMMON_IL516C_ON
inline
COmxILCallbackManager::CEjectBuffersRequestCommand::
CEjectBuffersRequestCommand(OMX_U32 aLocalOmxPortIndex)
	:
	CCallbackCommand(CCallbackCommand::EPriorityNormal),
	iLocalOmxPortIndex(aLocalOmxPortIndex)
	{
	}
#endif
