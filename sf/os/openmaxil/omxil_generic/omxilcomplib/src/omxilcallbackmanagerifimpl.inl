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
XOmxILCallbackManagerIfImpl::TTunnelRegistrationInfo::TTunnelRegistrationInfo(
	OMX_U32 aLocalPortIndex,
	OMX_DIRTYPE aLocalPortDirection,
	OMX_HANDLETYPE aTunnelledComponentHandle,
	OMX_U32 aTunnelledPortIndex)
	:
	iLocalPortIndex(aLocalPortIndex),
	iLocalPortDirection(aLocalPortDirection),
	iTunnelledComponentHandle(aTunnelledComponentHandle),
	iTunnelledPortIndex(aTunnelledPortIndex)
	{
	}

inline
XOmxILCallbackManagerIfImpl::TBufferMarkPropagationInfo::TBufferMarkPropagationInfo(
	OMX_U32 aPortIndex,
	OMX_U32 aPropagationPortIndex)
	:
	iPortIndex(aPortIndex),
	iPropagationPortIndex(aPropagationPortIndex)
	{
	}

// This constructor should only be used for array look-ups
inline
XOmxILCallbackManagerIfImpl::TBufferMarkPropagationInfo::TBufferMarkPropagationInfo(
	OMX_U32 aPortIndex)
	:
	iPortIndex(aPortIndex),
	iPropagationPortIndex(0)
	{
	}

inline
XOmxILCallbackManagerIfImpl::TOutputPortBufferMarkInfo::TOutputPortBufferMarkInfo(
	OMX_U32 aPortIndex,
	OMX_HANDLETYPE apMarkTargetComponent,
	OMX_PTR apMarkData)
	:
	iPortIndex(aPortIndex),
	ipMarkTargetComponent(apMarkTargetComponent),
	ipMarkData(apMarkData)
	{
	}

// This constructor should only be used for array look-ups
inline
XOmxILCallbackManagerIfImpl::TOutputPortBufferMarkInfo::TOutputPortBufferMarkInfo(
	OMX_U32 aPortIndex)
	:
	iPortIndex(aPortIndex),
	ipMarkTargetComponent(0),
	ipMarkData(0)
	{
	}
